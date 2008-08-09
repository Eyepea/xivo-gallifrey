/*
 * Application to receive a TIFF FAX file
 * based on app_rxfax.c from: Copyright (C) 2003, Steve Underwood <steveu@coppice.org>
 * based on app_rxfax.c from www.callweaver.org
 * PATCHED BY (C) 20007 by Antonio Gallo <agx@linux.it>
 * - added ECM support
 * - added more env variables
 * - added logging to external file
 * PATCHED BY (C) 2008 Proformatique <technique@proformatique.com>
 * - removed useless logging to external file
 * - cleaned up all the mess
 */

/*** MODULEINFO
Depends: libspandsp
Desciption: Receive a FAX to a file
DisplayName: RxFAX
 ***/

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision:$")

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <pthread.h>
#include <errno.h>
#include <tiffio.h>

#include <spandsp.h>
#include <spandsp/version.h>

#include "asterisk/lock.h"
#include "asterisk/file.h"
#include "asterisk/logger.h"
#include "asterisk/channel.h"
#include "asterisk/pbx.h"
#include "asterisk/module.h"
#include "asterisk/manager.h"

#ifndef AST_MODULE
#define AST_MODULE "app_rxfax"
#endif

static char *app = "RxFAX";

static char *synopsis = "Receive a FAX to a file";

static char *descrip = 
	"  RxFAX(filename[|caller][|debug]): Receives a FAX from the channel into the\n"
	"given filename. If the file exists it will be overwritten. The file\n"
	"should be in TIFF/F format.\n"
	"The \"caller\" option makes the application behave as a calling machine,\n"
	"rather than the answering machine. The default behaviour is to behave as\n"
	"an answering machine.\n"
	"The \"ecm\" option enables ECM.\n"
	"Uses LOCALSTATIONID to identify itself to the remote end.\n"
	"     LOCALHEADERINFO to generate a header line on each page.\n"
	"Sets REMOTESTATIONID to the sender CSID.\n"
	"     FAXPAGES to the number of pages received.\n"
	"     FAXBITRATE to the transmition rate.\n"
	"     FAXRESOLUTION to the resolution.\n"
	"     PHASEESTATUS to the phase E result status.\n"
	"     PHASEESTRING to the phase E result string.\n"
	"Note that PHASEESTATUS=0 means that the fax was handled correctly. But that doesn't\n"
	"imply that any pages were sent. Actually you should also check FAXPAGES to be\n"
	"greater than zero.\n"
	"Returns -1 when the user hangs up.\n"
	"Returns 0 otherwise.\n";

#define MAX_BLOCK_SIZE 240

static void span_message(int level, const char *msg)
{
	if (msg==NULL) return;
	int ast_level;
	if (level == SPAN_LOG_ERROR)
		ast_level = __LOG_ERROR;
	else if (level == SPAN_LOG_WARNING)
		ast_level = __LOG_WARNING;
	else
		ast_level = __LOG_DEBUG;
	ast_log(ast_level, _A_, "%s", msg);
}

/*- End of function --------------------------------------------------------*/

static void phase_b_handler(t30_state_t *s, void *user_data, int result)
{
	/* nothing */
}

/*- End of function --------------------------------------------------------*/
static void phase_e_handler(t30_state_t *s, void *user_data, int result)
{
	struct ast_channel *chan;
	char local_ident[21];
	char far_ident[21];
	char buf[128];
	t30_stats_t t;

	chan = (struct ast_channel *) user_data;
	t30_get_transfer_statistics(s, &t);

	t30_get_local_ident(s, local_ident);
	t30_get_far_ident(s, far_ident);
	pbx_builtin_setvar_helper(chan, "REMOTESTATIONID", far_ident);
	snprintf(buf, sizeof(buf), "%d", t.pages_transferred);
	pbx_builtin_setvar_helper(chan, "FAXPAGES", buf);
	snprintf(buf, sizeof(buf), "%d", t.y_resolution);
	pbx_builtin_setvar_helper(chan, "FAXRESOLUTION", buf);
	snprintf(buf, sizeof(buf), "%d", t.bit_rate);
	pbx_builtin_setvar_helper(chan, "FAXBITRATE", buf);
	snprintf(buf, sizeof(buf), "%d", result);
	pbx_builtin_setvar_helper(chan, "PHASEESTATUS", buf);
	snprintf(buf, sizeof(buf), "%s", t30_completion_code_to_str(result));
	pbx_builtin_setvar_helper(chan, "PHASEESTRING", buf);

	ast_log(LOG_DEBUG, "==============================================================================\n");
	if (result == T30_ERR_OK)
	{
		ast_log(LOG_DEBUG, "Fax successfully received.\n");
		ast_log(LOG_DEBUG, "Remote station id: %s\n", far_ident);
		ast_log(LOG_DEBUG, "Local station id:  %s\n", local_ident);
		ast_log(LOG_DEBUG, "Pages transferred: %i\n", t.pages_transferred);
		ast_log(LOG_DEBUG, "Image resolution:  %i x %i\n", t.x_resolution, t.y_resolution);
		ast_log(LOG_DEBUG, "Transfer Rate:     %i\n", t.bit_rate);
		manager_event(EVENT_FLAG_CALL,
				"FaxReceived", "Channel: %s\nExten: %s\nCallerID: %s\nRemoteStationID: %s\nLocalStationID: %s\nPagesTransferred: %i\nResolution: %i\nTransferRate: %i\nFileName: %s\n",
				chan->name,
				chan->exten,
				(chan->cid.cid_num)  ?  chan->cid.cid_num  :  "",
				far_ident,
				local_ident,
				t.pages_transferred,
				t.y_resolution,
				t.bit_rate,
				s->rx_file);
	}
	else
		ast_log(LOG_DEBUG, "Fax receive not successful - result (%d) %s.\n", result, t30_completion_code_to_str(result));
	ast_log(LOG_DEBUG, "==============================================================================\n");
}
/*- End of function --------------------------------------------------------*/

static void phase_d_handler(t30_state_t *s, void *user_data, int result)
{
	struct ast_channel *chan;
	t30_stats_t t;

	chan = (struct ast_channel *) user_data;
	if (result)
	{
		t30_get_transfer_statistics(s, &t);
		ast_log(LOG_DEBUG, "==============================================================================\n");
		ast_log(LOG_DEBUG, "Pages transferred:  %i\n", t.pages_transferred);
		ast_log(LOG_DEBUG, "Image size:         %i x %i\n", t.width, t.length);
		ast_log(LOG_DEBUG, "Image resolution    %i x %i\n", t.x_resolution, t.y_resolution);
		ast_log(LOG_DEBUG, "Transfer Rate:      %i\n", t.bit_rate);
		ast_log(LOG_DEBUG, "Bad rows            %i\n", t.bad_rows);
		ast_log(LOG_DEBUG, "Longest bad row run %i\n", t.longest_bad_row_run);
		ast_log(LOG_DEBUG, "Compression type    %s\n", t4_encoding_to_str(t.encoding));
		ast_log(LOG_DEBUG, "Image size (bytes)  %i\n", t.image_size);
		ast_log(LOG_DEBUG, "==============================================================================\n");
	}
}
/*- End of function --------------------------------------------------------*/

static int rxfax_exec(struct ast_channel *chan, void *data)
{
	int res = 0;
	char target_file[256];
	char template_file[256];
	int samples;
	char *s;
	char *t;
	char *v;
	const char *x;
	int option;
	int len;
	int i;
	fax_state_t fax;
	struct ast_frame *inf = NULL;
	struct ast_frame outf;
	int calling_party;
	int verbose;
	int ecm = FALSE;

	struct ast_module_user *u;

	int original_read_fmt;
	int original_write_fmt;


	/* Basic initial checkings */

	if (chan == NULL) {
		ast_log(LOG_WARNING, "Fax receive channel is NULL. Giving up.\n");
		return -1;
	}

	span_set_message_handler(span_message);
	/* make sure they are initialized to zero */
	memset( &fax, 0, sizeof(fax));

	/* Resetting channel variables related to T38 */
	pbx_builtin_setvar_helper(chan, "REMOTESTATIONID", "");
	pbx_builtin_setvar_helper(chan, "FAXPAGES", "");
	pbx_builtin_setvar_helper(chan, "FAXRESOLUTION", "");
	pbx_builtin_setvar_helper(chan, "FAXBITRATE", "");
	pbx_builtin_setvar_helper(chan, "PHASEESTATUS", "");
	pbx_builtin_setvar_helper(chan, "PHASEESTRING", "");

	/* Parsig parameters */

	/* The next few lines of code parse out the filename and header from the input string */
	if (data == NULL)
	{
		/* No data implies no filename or anything is present */
		ast_log(LOG_WARNING, "Rxfax requires an argument (filename)\n");
		return -1;
	}

	calling_party = FALSE;
	verbose = FALSE;
	target_file[0] = '\0';

	char tbuf[256];
	for (option = 0, v = s = data;  v;  option++, s++) {
		t = s;
		v = strchr(s, '|');
		s = (v)  ?  v  :  s + strlen(s);
		strncpy((char *) tbuf, t, s - t);
		tbuf[s - t] = '\0';
		if (option == 0) {
			/* The first option is always the file name */
			len = s - t;
			if (len > 255)
				len = 255;
			strncpy(target_file, t, len);
			target_file[len] = '\0';
			/* Allow the use of %d in the file name for a wild card of sorts, to
			   create a new file with the specified name scheme */
			if ((x = strchr(target_file, '%'))  &&  x[1] == 'd') {
				strcpy(template_file, target_file);
				i = 0;
				do {
					snprintf(target_file, 256, template_file, 1);
					i++;
				} while (ast_fileexists(target_file, "", chan->language) != -1);
			}
		} else if (strncmp("caller", t, s - t) == 0) {
			calling_party = TRUE;
		} else if (strncmp("debug", t, s - t) == 0) {
			verbose = TRUE;
		} else if (strncmp("ecm", t, s - t) == 0) {
			ecm = TRUE;
		}
	}

	/* Done parsing */

	u = ast_module_user_add(chan);

	if (chan->_state != AST_STATE_UP)
	{
		/* Shouldn't need this, but checking to see if channel is already answered
		 * Theoretically asterisk should already have answered before running the app */
		res = ast_answer(chan);
	}

	/* Setting read and write formats */

	original_read_fmt = chan->readformat;
	if (original_read_fmt != AST_FORMAT_SLINEAR)
	{
		res = ast_set_read_format(chan, AST_FORMAT_SLINEAR);
		if (res < 0)
		{
			ast_log(LOG_WARNING, "Unable to set to linear read mode, giving up\n");
			ast_module_user_remove(u);
			return -1;
		}
	}

	original_write_fmt = chan->writeformat;
	if (original_write_fmt != AST_FORMAT_SLINEAR)
	{
		res = ast_set_write_format(chan, AST_FORMAT_SLINEAR);
		if (res < 0)
		{
			ast_log(LOG_WARNING, "Unable to set to linear write mode, giving up\n");
			res = ast_set_read_format(chan, original_read_fmt);
			if (res)
				ast_log(LOG_WARNING, "Unable to restore read format on '%s'\n", chan->name);
			ast_module_user_remove(u);
			return -1;
		}
	}

	/* Remove any app level gain adjustments and disable echo cancel. */
	signed char sc;
	sc = 0;
	ast_channel_setoption(chan, AST_OPTION_RXGAIN, &sc, sizeof(sc), 0);
	ast_channel_setoption(chan, AST_OPTION_TXGAIN, &sc, sizeof(sc), 0);
	ast_channel_setoption(chan, AST_OPTION_ECHOCAN, &sc, sizeof(sc), 0);

	/* This is the main loop */

	uint8_t __buf[sizeof(uint16_t)*MAX_BLOCK_SIZE + 2*AST_FRIENDLY_OFFSET];
	uint8_t *buf = __buf + AST_FRIENDLY_OFFSET;

	memset(&fax, 0, sizeof(fax));

	if (fax_init(&fax, calling_party) == NULL)
	{
		ast_log(LOG_WARNING, "Unable to set to start fax_init\n");
		ast_module_user_remove(u);
		return -1;
	}
	fax_set_transmit_on_idle(&fax, TRUE);
	span_log_set_message_handler(&fax.logging, span_message);
	span_log_set_message_handler(&fax.t30_state.logging, span_message);
	if (verbose)
	{
		span_log_set_level(&fax.logging, SPAN_LOG_SHOW_SEVERITY | SPAN_LOG_SHOW_PROTOCOL | SPAN_LOG_FLOW);
		span_log_set_level(&fax.t30_state.logging, SPAN_LOG_SHOW_SEVERITY | SPAN_LOG_SHOW_PROTOCOL | SPAN_LOG_FLOW);
	}
	x = pbx_builtin_getvar_helper(chan, "LOCALSTATIONID");
	if (x  &&  x[0])
		t30_set_local_ident(&fax.t30_state, x);
	x = pbx_builtin_getvar_helper(chan, "LOCALHEADERINFO");
	if (x  &&  x[0])
		t30_set_header_info(&fax.t30_state, x);
	t30_set_rx_file(&fax.t30_state, target_file, -1);
	t30_set_phase_b_handler(&fax.t30_state, phase_b_handler, chan);
	t30_set_phase_d_handler(&fax.t30_state, phase_d_handler, chan);
	t30_set_phase_e_handler(&fax.t30_state, phase_e_handler, chan);

	x = pbx_builtin_getvar_helper(chan, "FAX_DISABLE_V17");
	if (x  &&  x[0])
		t30_set_supported_modems(&(fax.t30_state), T30_SUPPORT_V29 | T30_SUPPORT_V27TER);
	else
		t30_set_supported_modems(&(fax.t30_state), T30_SUPPORT_V29 | T30_SUPPORT_V27TER | T30_SUPPORT_V17 );

	/* Support for different image sizes && resolutions*/
	t30_set_supported_image_sizes(&fax.t30_state, T30_SUPPORT_US_LETTER_LENGTH | T30_SUPPORT_US_LEGAL_LENGTH | T30_SUPPORT_UNLIMITED_LENGTH
			| T30_SUPPORT_215MM_WIDTH | T30_SUPPORT_255MM_WIDTH | T30_SUPPORT_303MM_WIDTH);
	t30_set_supported_resolutions(&fax.t30_state, T30_SUPPORT_STANDARD_RESOLUTION | T30_SUPPORT_FINE_RESOLUTION | T30_SUPPORT_SUPERFINE_RESOLUTION
			| T30_SUPPORT_R8_RESOLUTION | T30_SUPPORT_R16_RESOLUTION);
	if (ecm) {
		t30_set_ecm_capability(&(fax.t30_state), TRUE);
		t30_set_supported_compressions(&(fax.t30_state), T30_SUPPORT_T4_1D_COMPRESSION | T30_SUPPORT_T4_2D_COMPRESSION | T30_SUPPORT_T6_COMPRESSION);
		ast_log(LOG_DEBUG, "Enabling ECM mode for app_rxfax\n"  );
	}


	/* This is the main loop */

	res = 0;
	while ( chan )
	{
		if (ast_check_hangup(chan)) {
			ast_log(LOG_WARNING, "Channel has been hanged at fax.\n");
			res = 0;
			break;
		}

		if ((res = ast_waitfor(chan, 20)) < 0) {
			ast_log(LOG_WARNING, "Channel ast_waitfor < 0.\n");
			res = 0;
			break;
		}

		if ((fax.current_rx_type == T30_MODEM_DONE)  ||  (fax.current_tx_type == T30_MODEM_DONE)) {
			ast_log(LOG_WARNING, "Channel T30 DONE < 0.\n");
/*JUST WARNING:		res = 0;  
			break;*/ /* ??? WTF ??? FIXME */
		}

		inf = ast_read(chan);
		if (inf == NULL)
		{
			ast_log(LOG_WARNING, "Channel INF is NULL.\n");

			/* While trasmitting i got: Received a DCN from remote after sending a page
			   at last page */
			continue;
#ifdef AGX_DEBUGGING
			res = 0;
			break;
#endif
		}

		/* We got a frame */
		/* Check the frame type. Format also must be checked because there is a chance
		   that a frame in old format was already queued before we set chanel format
		   to slinear so it will still be received by ast_read */
		if (inf->frametype == AST_FRAME_VOICE && inf->subclass == AST_FORMAT_SLINEAR) {
			if (fax_rx(&fax, inf->data, inf->samples)) {
				ast_log(LOG_WARNING, "RXFAX: fax_rx returned error\n");
				res = -1;
				break;
			}

			samples = (inf->samples <= MAX_BLOCK_SIZE) ? inf->samples : MAX_BLOCK_SIZE;
			len = fax_tx(&fax, (int16_t *) &buf[AST_FRIENDLY_OFFSET], samples);
			if (len > 0) {
				memset(&outf, 0, sizeof(outf));
				outf.frametype = AST_FRAME_VOICE;
				outf.subclass = AST_FORMAT_SLINEAR;
				outf.datalen = len*sizeof(int16_t);
				outf.samples = len;
				outf.data = &buf[AST_FRIENDLY_OFFSET];
				outf.offset = AST_FRIENDLY_OFFSET;
				outf.src = "RxFAX";
				if (ast_write(chan, &outf) < 0)
				{
					ast_log(LOG_WARNING, "Unable to write frame to channel; %s\n", strerror(errno));
					res = -1;
					break;
				}
			}
		}
		ast_frfree(inf);
		inf = NULL;
		/* TODO put a Watchdog here */
	}

	if (inf != NULL)
	{
		ast_frfree(inf);
		inf = NULL;
	}

	t30_terminate(&fax.t30_state);
	fax_release(&fax);

	/* Restoring initial channel formats. */

	if (original_read_fmt != AST_FORMAT_SLINEAR)
	{
		res = ast_set_read_format(chan, original_read_fmt);
		if (res)
			ast_log(LOG_WARNING, "Unable to restore read format on '%s'\n", chan->name);
	}
	if (original_write_fmt != AST_FORMAT_SLINEAR)
	{
		res = ast_set_write_format(chan, original_write_fmt);
		if (res)
			ast_log(LOG_WARNING, "Unable to restore write format on '%s'\n", chan->name);
	}
	ast_module_user_remove(u);
	return res;
}
/*- End of function --------------------------------------------------------*/

static int unload_module(void)
{
	int res;
	ast_module_user_hangup_all();
	res = ast_unregister_application(app);	
	return res;
}
/*- End of function --------------------------------------------------------*/

static int load_module(void)
{
	ast_log(LOG_NOTICE, "RxFax using spandsp %i %i\n", SPANDSP_RELEASE_DATE, SPANDSP_RELEASE_TIME );
	return ast_register_application(app, rxfax_exec, synopsis, descrip);
}
/*- End of function --------------------------------------------------------*/

AST_MODULE_INFO_STANDARD(ASTERISK_GPL_KEY, "Trivial FAX Receive Application");

/*- End of file ------------------------------------------------------------*/
