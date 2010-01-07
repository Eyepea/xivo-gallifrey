/*
 * Asterisk -- A telephony toolkit for Linux.
 *
 * Resource to make watchdogs happy
 *
 * Copyright (C) 2005, Junghanns.NET GmbH
 * Klaus-Peter Junghanns <kpj@junghanns.net>
 *
 * Copyright (C) 2007-2010 Proformatique
 * Guillaume Knispel <gknispel@proformatique.com>
 * 	(mask signals causing the watchdog thread to abort,
 * 	 configurate the serial interface)
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 */

#include <asterisk.h>

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <pthread.h>
#include <signal.h>

#include <asterisk/logger.h>
#include <asterisk/options.h>
#include <asterisk/module.h>
#include <asterisk/config.h>
#include <asterisk/utils.h>

#define MAXERR_REPORT			250
#define REDUCE_RATE_AFTER_OVER_MAXERR	75
/* After MAXERR_REPORT errors, report only one error on 
   REDUCE_RATE_AFTER_OVER_MAXERR */

/* In ms: */
#define INTERVAL_MIN			1
#define INTERVAL_MAX			60000

static struct watchdog_pvt *watchdogs = NULL;

typedef struct watchdog_pvt {
    char device[80];
    int fd;
    int type;
    int interval;
    int errnum;
    int reduce_error_report_rate;
    pthread_t watchdog_thread;
    struct watchdog_pvt *next;
} watchdog_pvt;

static void *do_watchdog_thread(void *data)
{
    struct watchdog_pvt *woof = (struct watchdog_pvt *)data;
    sigset_t sig_to_block;

    /* block every signals */
    sigfillset(&sig_to_block);
    pthread_sigmask(SIG_SETMASK, &sig_to_block, NULL);

    /* the watchdog itself */
    for (;;) {
	if (woof->fd >= 0) {
	    if (write(woof->fd, "P", 1) <= 0) {
	    	woof->errnum++;
		if (woof->errnum < MAXERR_REPORT
			|| woof->reduce_error_report_rate
				>= REDUCE_RATE_AFTER_OVER_MAXERR) {

	            ast_log(LOG_ERROR, "An error occured while trying to "
		            "write to the file descriptor %d in %s.\n",
			    woof->fd, __FUNCTION__);
		    if (woof->errnum >= MAXERR_REPORT)
		    	woof->reduce_error_report_rate = 0;

		} else if (woof->errnum >= MAXERR_REPORT
				&& woof->reduce_error_report_rate
					< REDUCE_RATE_AFTER_OVER_MAXERR) {

			woof->reduce_error_report_rate++;
		}
	    }
	} else {
	    ast_log(LOG_ERROR, "Bad file descriptor detected in %s. "
	    	    "Exiting watchdog thread\n", __FUNCTION__);
	    return NULL;
	}
	usleep((useconds_t)(woof->interval * 1000));
	pthread_testcancel();
    }
    return NULL;
}

static int serial_config(int fd, const char *dev_name,
			 const char *ubitrate, const char *uparity,
			 const char *ubits, const char *ustop,
			 const char *uflow)
{
	struct termios newtio;
	unsigned int baudrate;

	if (!ubitrate)
		ubitrate = "9600";
	if (!ubits)
		ubits = "8";
	if (!uparity)
		uparity = "N";
	if (!ustop)
		ustop = "1";
	if (!uflow)
		uflow = "None";

	memset(&newtio, 0, sizeof(newtio));

	if (!strcmp(ubitrate, "50"))
		baudrate = B50;
	else if (!strcmp(ubitrate, "75"))
		baudrate = B75;
	else if (!strcmp(ubitrate, "110"))
		baudrate = B110;
	else if (!strcmp(ubitrate, "134"))
		baudrate = B134;
	else if (!strcmp(ubitrate, "150"))
		baudrate = B150;
	else if (!strcmp(ubitrate, "200"))
		baudrate = B200;
	else if (!strcmp(ubitrate, "300"))
		baudrate = B300;
	else if (!strcmp(ubitrate, "600"))
		baudrate = B600;
	else if (!strcmp(ubitrate, "1200"))
		baudrate = B1200;
	else if (!strcmp(ubitrate, "1800"))
		baudrate = B1800;
	else if (!strcmp(ubitrate, "2400"))
		baudrate = B2400;
	else if (!strcmp(ubitrate, "4800"))
		baudrate = B4800;
	else if (!strcmp(ubitrate, "9600"))
		baudrate = B9600;
	else if (!strcmp(ubitrate, "19200"))
		baudrate = B19200;
	else if (!strcmp(ubitrate, "38400"))
		baudrate = B38400;
	else if (!strcmp(ubitrate, "57600"))
		baudrate = B57600;
	else if (!strcmp(ubitrate, "115200"))
		baudrate = B115200;
	else if (!strcmp(ubitrate, "230400"))
		baudrate = B230400;
	else {
		ast_log(LOG_ERROR, "Invalid configured bitrate for device %s\n",
			dev_name);
		return -1;
	}

	newtio.c_cflag = baudrate | CLOCAL;
	newtio.c_iflag = IGNPAR | IGNBRK;
	newtio.c_oflag = 0;
	newtio.c_lflag = ICANON;

	if (!strcmp(ubits, "8"))
		newtio.c_cflag |= CS8;
	else if (!strcmp(ubits, "7"))
		newtio.c_cflag |= CS7;
	else if (!strcmp(ubits, "6"))
		newtio.c_cflag |= CS6;
	else if (!strcmp(ubits, "5"))
		newtio.c_cflag |= CS5;
	else {
		ast_log(LOG_ERROR, "Invalid configured bits number for "
			"device %s\n", dev_name);
		return -1;
	}

	if (!strcmp(ustop, "2"))
		newtio.c_cflag |= CSTOPB;
	else if (strcmp(ustop, "1")) {
		ast_log(LOG_ERROR, "Invalid configured stop bits for "
			"device %s\n", dev_name);
		return -1;
	}

	if (!strcasecmp(uparity, "O") || !strcasecmp(uparity, "Odd"))
		newtio.c_cflag |= PARENB | PARODD;
	else if (!strcasecmp(uparity, "E") || !strcasecmp(uparity, "Even"))
		newtio.c_cflag |= PARENB;
	else if (strcasecmp(uparity, "N") && strcasecmp(uparity, "None")) {
		ast_log(LOG_ERROR, "Invalid configured parity for device %s\n",
			dev_name);
		return -1;
	}

	if (!strcasecmp(uflow, "RtsCts"))
		newtio.c_cflag |= CRTSCTS;
	else if (!strcasecmp(uflow, "XonXoff"))
		newtio.c_iflag |= IXON | IXOFF;
	else if (strcasecmp(uflow, "None") && strcasecmp(uflow, "N")) {
		ast_log(LOG_ERROR, "Invalid configured flow control for "
			"device %s\n", dev_name);
		return -1;
	}
		
	newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
	newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
	newtio.c_cc[VERASE]   = 0;     /* del */
	newtio.c_cc[VKILL]    = 0;     /* @ */
	newtio.c_cc[VEOF]     = 0;     /* Ctrl-d */
	newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
	newtio.c_cc[VSWTC]    = 0;     /* '\0' */
	newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
	newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
	newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
	newtio.c_cc[VEOL]     = 0;     /* '\0' */
	newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
	newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
	newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
	newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
	newtio.c_cc[VEOL2]    = 0;     /* '\0' */

	if (tcflush(fd, TCIOFLUSH) < 0) {
		ast_log(LOG_ERROR, "tcflush(fd, TCIOFLUSH) failed for device %s",
			dev_name);
		return -1;
	}
	if (tcsetattr(fd, TCSANOW, &newtio) < 0) {
		ast_log(LOG_ERROR, "tcsetattr(fd,TCSANOW,&newtio) failed "
			"for device %s", dev_name);
		return -1;
	}

	return 0;
}

static int load_module(void)
{
	const char *cat, *utype, *udevice, *uinterval,
	     *userial, *ubitrate, *uparity, *ustop, *uflow, *ubits;
	struct ast_config *cfg;
	struct watchdog_pvt *woof = NULL;

	cfg = ast_config_load("watchdog.conf");
	if (cfg) {
	
	    cat = ast_category_browse(cfg, NULL);

	    while(cat) {

		utype = ast_variable_retrieve(cfg, cat, "type");
		udevice = ast_variable_retrieve(cfg, cat, "device");
		uinterval = ast_variable_retrieve(cfg, cat, "interval");
		userial = ast_variable_retrieve(cfg, cat, "serial");
		ubitrate = ast_variable_retrieve(cfg, cat, "bitrate");
		ubits = ast_variable_retrieve(cfg, cat, "bits");
		uparity = ast_variable_retrieve(cfg, cat, "parity");
		ustop = ast_variable_retrieve(cfg, cat, "stop");
		uflow = ast_variable_retrieve(cfg, cat, "flow");

		ast_log(LOG_DEBUG, "Loaded watchdog configuration => "
			"type: %s -- device: %s -- interval: %s -- "
			"serial: %s -- bitrate: %s -- bits: %s -- "
			"parity: %s -- stop: %s -- flow: %s\n",
			utype, udevice, uinterval, userial, ubitrate,
			ubits, uparity, ustop, uflow);

		if (uinterval && udevice && utype) {
		    woof = malloc(sizeof(struct watchdog_pvt));
		    if (!woof) {
			ast_log(LOG_ERROR, "unable to malloc!\n");
			return AST_MODULE_LOAD_FAILURE;
		    }
		    memset(woof, 0x0, sizeof(struct watchdog_pvt));

		    strncpy(woof->device, udevice, sizeof(woof->device) - 1);
		    woof->device[sizeof(woof->device)-1] = 0;
		    
		    woof->interval = atoi(uinterval);
		    if (woof->interval < INTERVAL_MIN
		        || woof->interval > INTERVAL_MAX) {
			free(woof);
			ast_log(LOG_ERROR, "interval out of range: %i ms\n", woof->interval);
			return AST_MODULE_LOAD_FAILURE;
		    }
		    ast_log(LOG_DEBUG, "interval: %i ms\n", woof->interval);
		    woof->next = watchdogs;
		    watchdogs = woof;

		    woof->fd = open(woof->device, O_WRONLY | O_SYNC | O_NOCTTY);
		    if (woof->fd >= 0) {
			if (userial && !strncasecmp("yes", userial, 3)) {
			    if (serial_config(woof->fd, woof->device,
			    		      ubitrate, uparity, ubits,
					      ustop, uflow) < 0)
			    	ast_log(LOG_ERROR, "Bad serial port parameters "
					"or unable to configurate device %s.\n",
					woof->device);
			}
			if (!strncmp(utype, "isdnguard", strlen(utype))) {
			    woof->type = 1;
			    write(woof->fd, "START\n", 6);
			}
			ast_pthread_create(&woof->watchdog_thread, NULL, do_watchdog_thread, woof);
		    } else {
			ast_log(LOG_WARNING, "error opening watchdog "
				"device %s !\n", woof->device);
		    }
		}
		cat = ast_category_browse(cfg, cat);
	    }
    	    ast_config_destroy(cfg);
	}
	return AST_MODULE_LOAD_SUCCESS;
}

static int unload_module(void)
{
	struct watchdog_pvt *dogs, *woof;

	dogs = watchdogs;
	while (dogs) {
	    pthread_cancel(dogs->watchdog_thread);
	    close(dogs->fd);
	    woof = dogs->next;
	    free(dogs);
	    dogs = woof;
	}
	return 0;
}

AST_MODULE_INFO_STANDARD(ASTERISK_GPL_KEY, "Watchdog API");/* WWW [1] */

/*
 * [1]: 'static' is not at beginning of declaration
 *      harmless
 */
