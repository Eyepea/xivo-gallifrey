/*
 * Devstate application
 * 
 * Since we like the snom leds so much, a little app to
 * light the lights on the snom on demand ....
 *
 * Copyright (C) 2005, Druid Software
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <asterisk/lock.h>
#include <asterisk/file.h>
#include <asterisk/logger.h>
#include <asterisk/channel.h>
#include <asterisk/pbx.h>
#include <asterisk/module.h>
#include <asterisk/astdb.h>
#include <asterisk/utils.h>
#include <asterisk/cli.h>
#include <asterisk/manager.h>
#include <asterisk/devicestate.h>

static char type[] = "DS";
static char tdesc[] = "Application for sending device state messages";

static char app[] = "Devstate";

static char synopsis[] = "Generate a device state change event given the input parameters";

static char descrip[] = " Devstate(device|state):  Generate a device state change event given the input parameters. Returns 0. State values match the asterisk device states. They are 0 = unknown, 1 = not inuse, 2 = inuse, 3 = busy, 4 = invalid, 5 = unavailable, 6 = ringing\n";

static char devstate_cli_usage[] = 
"Usage: devstate device state\n" 
"       Generate a device state change event given the input parameters.\n Mainly used for lighting the LEDs on the snoms.\n";

static int devstate_cli(int fd, int argc, char *argv[]);
static struct ast_cli_entry  cli_dev_state =
        { { "devstate", NULL }, devstate_cli, "Set the device state on one of the \"pseudo devices\".", devstate_cli_usage };

STANDARD_LOCAL_USER;

LOCAL_USER_DECL;


static int devstate_cli(int fd, int argc, char *argv[])
{
    char devName[128];
    if ((argc != 3) && (argc != 4) && (argc != 5))
        return RESULT_SHOWUSAGE;

    if (ast_db_put("DEVSTATES", argv[1], argv[2]))
    {
        ast_log(LOG_DEBUG, "ast_db_put failed\n");
    }
    snprintf(devName, sizeof(devName), "DS/%s", argv[1]);
    if (argc == 4) {
        ast_log(LOG_NOTICE, "devname %s cid %s\n", devName, argv[3]);
	ast_device_state_changed_literal(devName);
    } else if (argc == 5) {
        ast_log(LOG_NOTICE, "devname %s cid %s cidname %s\n", devName, argv[3], argv[4]);
	ast_device_state_changed_literal(devName);
    } else {
	ast_device_state_changed_literal(devName);
    }
    return RESULT_SUCCESS;
}

static int devstate_exec(struct ast_channel *chan, void *data)
{
    struct localuser *u;
    char *device, *state, *info;
    char devName[128];
    if (!(info = ast_strdupa(data))) {
            ast_log(LOG_WARNING, "Unable to dupe data :(\n");
            return -1;
    }
    LOCAL_USER_ADD(u);
    
    device = info;
    state = strchr(info, '|');
    if (state) {
        *state = '\0';
        state++;
    }
    else
    {
        ast_log(LOG_DEBUG, "No state argument supplied\n");
        return -1;
    }

    if (ast_db_put("DEVSTATES", device, state))
    {
        ast_log(LOG_DEBUG, "ast_db_put failed\n");
    }

    snprintf(devName, sizeof(devName), "DS/%s", device);
    ast_device_state_changed_literal(devName);

    LOCAL_USER_REMOVE(u);
    return 0;
}


static int ds_devicestate(void *data)
{
    char *dest = data;
    char stateStr[16];
    if (ast_db_get("DEVSTATES", dest, stateStr, sizeof(stateStr)))
    {
        ast_log(LOG_DEBUG, "ds_devicestate couldnt get state in astdb\n");
        return 0;
    }
    else
    {
        ast_log(LOG_DEBUG, "ds_devicestate dev=%s returning state %d\n",
               dest, atoi(stateStr));
        return (atoi(stateStr));
    }
}

static struct ast_channel_tech devstate_tech = {
	.type = type,
	.description = tdesc,
	.capabilities = ((AST_FORMAT_MAX_AUDIO << 1) - 1),
	.devicestate = ds_devicestate,
	.requester = NULL,
	.send_digit = NULL,
	.send_text = NULL,
	.call = NULL,
	.hangup = NULL,
	.answer = NULL,
	.read = NULL,
	.write = NULL,
	.bridge = NULL,
	.exception = NULL,
	.indicate = NULL,
	.fixup = NULL,
	.setoption = NULL,
};

static char mandescr_devstate[] = 
"Description: Put a value into astdb\n"
"Variables: \n"
"	Family: ...\n"
"	Key: ...\n"
"	Value: ...\n";

static int action_devstate(struct mansession *s, struct message *m)
{
        char *devstate = astman_get_header(m, "Devstate");
        char *value = astman_get_header(m, "Value");
	char *id = astman_get_header(m,"ActionID");
	char devName[128];

	if (!strlen(devstate)) {
		astman_send_error(s, m, "No Devstate specified");
		return 0;
	}
	if (!strlen(value)) {
		astman_send_error(s, m, "No Value specified");
		return 0;
	}

        if (!ast_db_put("DEVSTATES", devstate, value)) {
	    snprintf(devName, sizeof(devName), "DS/%s", devstate);
	    ast_device_state_changed_literal(devName);
	    ast_cli(s->fd, "Response: Success\r\n");
	} else {
	    ast_log(LOG_DEBUG, "ast_db_put failed\n");
	    ast_cli(s->fd, "Response: Failed\r\n");
	}
	if (id && !ast_strlen_zero(id))
		ast_cli(s->fd, "ActionID: %s\r\n",id);
	ast_cli(s->fd, "\r\n");
	return 0;
}

int load_module(void)
{
    if (ast_channel_register(&devstate_tech)) {
        ast_log(LOG_DEBUG, "Unable to register channel class %s\n", type);
        return -1;
    }
    ast_cli_register(&cli_dev_state);  
    ast_manager_register2( "Devstate", EVENT_FLAG_CALL, action_devstate, "Change a device state", mandescr_devstate );
    return ast_register_application(app, devstate_exec, synopsis, descrip);
}

int unload_module(void)
{
    int res = 0;
    STANDARD_HANGUP_LOCALUSERS;
    ast_manager_unregister( "Devstate");
    ast_cli_unregister(&cli_dev_state);
    res = ast_unregister_application(app);
    ast_channel_unregister(&devstate_tech);    
    return res;
}

char *description(void)
{
    return tdesc;
}

int usecount(void)
{
    int res;
    STANDARD_USECOUNT(res);
    return res;
}

char *key()
{
    return ASTERISK_GPL_KEY;
}
