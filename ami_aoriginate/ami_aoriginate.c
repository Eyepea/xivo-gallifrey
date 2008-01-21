/*
 * Asterisk -- An open source telephony toolkit.
 * Manager command AOriginate: Fast Originate action, implemented differently
 *
 * Copyright (C) 1999 - 2005, Digium, Inc.
 * Copyright (C) 2008  Proformatique
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 dated June, 1991.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <asterisk/lock.h>
#include <asterisk/logger.h>
#include <asterisk/channel.h>
#include <asterisk/module.h>
#include <asterisk/channel.h>
#include <asterisk/frame.h>
#include <asterisk/manager.h>
#include <asterisk/options.h>
#include <asterisk/callerid.h>
#include <asterisk/pbx.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MODULE_DESCRIPTION	"Fast Originate action, implemented differently"

const char module_date_revision[] = "ami_aoriginate $Date$ $Revision$";

struct aoriginate_data {
	char tech[AST_MAX_MANHEADER_LEN];
	char data[AST_MAX_MANHEADER_LEN];
	int timeout;
	char app[AST_MAX_APP];
	char appdata[AST_MAX_MANHEADER_LEN];
	char cid_name[AST_MAX_MANHEADER_LEN];
	char cid_num[AST_MAX_MANHEADER_LEN];
	char context[AST_MAX_CONTEXT];
	char exten[AST_MAX_EXTENSION];
	char idtext[AST_MAX_MANHEADER_LEN];
	char account[AST_MAX_ACCOUNT_CODE];
	int priority;
	struct ast_variable *vars;
};

STANDARD_LOCAL_USER;
LOCAL_USER_DECL;

#define AO_LOCAL_USER_ADD(u)

#define AO_TOEXTEN 0
#define AO_TOAPP 1
#define AO_DEST(aodata) (!ast_strlen_zero((aodata)->app))

/* evil use of not really exported function: */
int ast_pbx_outgoing_cdr_failed(void);

static void *action_aoriginate_run(void *data)
{
	struct localuser *u;
	struct ast_channel *chan;
	struct aoriginate_data *aodata = data;
	struct outgoing_helper oh;
	char *cid_num;
	char *cid_name;
	int reason = 0;
	char chan_uniqueid[32] = "<null>";

	cid_num = !ast_strlen_zero(aodata->cid_num) ? aodata->cid_num : NULL;
	cid_name = !ast_strlen_zero(aodata->cid_name) ? aodata->cid_name : NULL;

	memset(&oh, 0, sizeof(oh));
	oh.cid_num = cid_num;
	oh.cid_name = cid_name;
	oh.account = aodata->account;
	oh.vars = aodata->vars;
	if (AO_DEST(aodata) == AO_TOEXTEN) {
		oh.context = aodata->context;
		oh.exten = aodata->exten;
		oh.priority = aodata->priority;
	}

	chan = __ast_request_and_dial(aodata->tech, AST_FORMAT_SLINEAR,
	                              aodata->data, aodata->timeout, &reason,
				      cid_num, cid_name, &oh);
	if (chan == NULL)
		goto error_request_and_dial;

	/* modified LOCAL_USER_ADD(u) */ {
		if (!(u=calloc(1,sizeof(*u)))) {
			ast_log(LOG_WARNING, "Out of memory\n");
			goto error_local_user_add;
		}
		ast_mutex_lock(&localuser_lock);
		u->chan = chan;
		u->next = localusers;
		localusers = u;
		localusecnt++;
		ast_mutex_unlock(&localuser_lock);
		ast_update_use_count();
	}

	ast_mutex_lock(&chan->lock);
	if (chan->cdr) {
		ast_log(LOG_WARNING, "%s already has a call record??\n", chan->name);
	} else {
		chan->cdr = ast_cdr_alloc();
		if(!chan->cdr) {
			ast_mutex_unlock(&chan->lock);
			ast_log(LOG_WARNING, "Unable to create Call Detail Record\n");
			goto error_cdr_alloc;
		}
		ast_cdr_init(chan->cdr, chan);
		ast_cdr_start(chan->cdr);
	}
	if (chan->_state != AST_STATE_UP) {
		ast_mutex_unlock(&chan->lock);
		goto error_chan_state_not_up;
	}

	if (option_verbose > 3)
		ast_verbose(VERBOSE_PREFIX_4 "Channel %s was answered.\n", chan->name);

#define MANAGER_EVENT_AORIGINATE_SUCCESS()		\
		manager_event(EVENT_FLAG_CALL,		\
			"AOriginateSuccess",		\
			"%s"				\
			"Channel: %s/%s\r\n"		\
			"Context: %s\r\n"		\
			"Exten: %s\r\n"			\
			"Reason: %d\r\n"		\
			"Uniqueid: %s\r\n",		\
			aodata->idtext,			\
			aodata->tech,			\
			aodata->data,			\
			aodata->context,		\
			aodata->exten,			\
			reason,				\
			chan->uniqueid)

	if (AO_DEST(aodata) == AO_TOAPP) {
		struct ast_app *app;
		app = pbx_findapp(aodata->app);
		if (app == NULL) {
			ast_log(LOG_WARNING, "No such application '%s'\n", aodata->app);
			reason = AST_CONTROL_CONGESTION;
			ast_mutex_unlock(&chan->lock);
			goto error_no_application;
		}
		if (option_verbose > 3)
			ast_verbose(VERBOSE_PREFIX_4 "Launching %s(%s) on %s\n", aodata->app, aodata->appdata, chan->name);
		ast_mutex_unlock(&chan->lock);

		MANAGER_EVENT_AORIGINATE_SUCCESS();

		pbx_exec(chan, app, aodata->appdata, 1);
		LOCAL_USER_REMOVE(u);
		ast_hangup(chan);
	} else {
		ast_mutex_unlock(&chan->lock);
		
		MANAGER_EVENT_AORIGINATE_SUCCESS();

		if (ast_pbx_run(chan)) {
			ast_log(LOG_ERROR, "Unable to run PBX on %s\n", chan->name);
			ast_hangup(chan);
		}
		LOCAL_USER_REMOVE(u);
	}
	goto out;

error_chan_state_not_up:
	if (option_verbose > 3)
		ast_verbose(VERBOSE_PREFIX_4 "Channel %s was never answered.\n", chan->name);
	if (chan->cdr) {
		if (ast_cdr_disposition(chan->cdr, chan->hangupcause))
			ast_cdr_failed(chan->cdr);
	}
error_no_application:
error_cdr_alloc:
	LOCAL_USER_REMOVE(u);
error_local_user_add:
	ast_copy_string(chan_uniqueid, chan->uniqueid, sizeof(chan_uniqueid));
	ast_hangup(chan);
error_request_and_dial:
	if (reason == 0)
		ast_pbx_outgoing_cdr_failed();

	manager_event(EVENT_FLAG_CALL,
		"AOriginateFailure",
		"%s"
		"Channel: %s/%s\r\n"
		"Context: %s\r\n"
		"Exten: %s\r\n"
		"Reason: %d\r\n"
		"Uniqueid: %s\r\n",
		aodata->idtext,
		aodata->tech,
		aodata->data,
		aodata->context,
		aodata->exten,
		reason,
		chan_uniqueid);

out:
	ast_variables_destroy(aodata->vars);
	free(aodata);
	return NULL;
#undef MANAGER_EVENT_AORIGINATE_SUCCESS
}

static char name_aoriginate[] = "AOriginate";
static const char synopsis_aoriginate[] = "Fast Originate implemented differently";
static const char mandescr_aoriginate[] = 
"Description: Generates an outgoing call to a Extension/Context/Priority or\n"
"  Application/Data. When comparing this command with the builtin 'Originate'\n"
"  one, you should think of AOriginate as an Originate with the Async parameter\n"
"  of the latter set to 'true' (AOriginate will silently ignore any Async\n" 
"  parameter you could try to give to it). AOriginate technically differs from a\n"
"  fast builtin Originate in that the thread created for the continuation of the\n"
"  AOriginate action will not itself start yet another thread. The net result is\n"
"  that the dynamic behavior of the half-call to Channel more closely follows the\n"
"  behavior you can expect when using the Dial() application from within the\n"
"  DialPlan: in both cases all the function calls to the channel technology\n"
"  callbacks for the given half-call target will be made from a single thread.\n"
"  This is expected by some technologies, notably chan_agent.\n"
"Variables: (Names marked with * are required)\n"
"	*Channel: Channel name to call\n"
"	Exten: Extension to use (requires 'Context' and 'Priority')\n"
"	Context: Context to use (requires 'Exten' and 'Priority')\n"
"	Priority: Priority to use (requires 'Exten' and 'Context')\n"
"	Application: Application to use\n"
"	Data: Data to use (requires 'Application')\n"
"	Timeout: How long to wait for call to be answered (in ms)\n"
"	CallerID: Caller ID to be set on the outgoing channel\n"
"	Variable: Channel variable to set, multiple Variable: headers are allowed\n"
"	Account: Account code\n";
static int action_aoriginate(struct mansession *s, struct message *m)
{
	STANDARD_INCREMENT_USECOUNT;

	char *name = astman_get_header(m, "Channel");
	char *exten = astman_get_header(m, "Exten");
	char *context = astman_get_header(m, "Context");
	char *priority = astman_get_header(m, "Priority");
	char *timeout = astman_get_header(m, "Timeout");
	char *callerid = astman_get_header(m, "CallerID");
	char *account = astman_get_header(m, "Account");
	char *app = astman_get_header(m, "Application");
	char *appdata = astman_get_header(m, "Data");
	char *id = astman_get_header(m, "ActionID");
	struct ast_variable *vars = astman_get_variables(m);
	struct aoriginate_data *aodata;
	char *tech, *data;
	char *l=NULL, *n=NULL;
	char tmp[256];
	char tmp2[256];
	int pi = 0;
	int to = 30000;
	pthread_t th;
	pthread_attr_t attr;

	if (!name) {
		astman_send_error(s, m, "Channel not specified");
		goto out;
	}
	if (!ast_strlen_zero(priority) && (sscanf(priority, "%d", &pi) != 1)) {
		astman_send_error(s, m, "Invalid priority\n");
		goto out;
	}
	if (!ast_strlen_zero(timeout) && (sscanf(timeout, "%d", &to) != 1)) {
		astman_send_error(s, m, "Invalid timeout\n");
		goto out;
	}
	ast_copy_string(tmp, name, sizeof(tmp));
	tech = tmp;
	data = strchr(tmp, '/');
	if (!data) {
		astman_send_error(s, m, "Invalid channel\n");
		goto out;
	}
	*data = '\0';
	data++;
	ast_copy_string(tmp2, callerid, sizeof(tmp2));
	ast_callerid_parse(tmp2, &n, &l);
	if (n) {
		if (ast_strlen_zero(n))
			n = NULL;
	}
	if (l) {
		ast_shrink_phone_number(l);
		if (ast_strlen_zero(l))
			l = NULL;
	}
	aodata = malloc(sizeof(*aodata));
	if (aodata == NULL) {
		astman_send_error(s, m, "Memory allocation failure\n");
		goto out;
	}
	memset(aodata, 0, sizeof(*aodata));
	if (!ast_strlen_zero(id))
		snprintf(aodata->idtext,
		         sizeof(aodata->idtext),
		         "ActionID: %s\r\n", id);
	ast_copy_string(aodata->tech, tech, sizeof(aodata->tech));
   	ast_copy_string(aodata->data, data, sizeof(aodata->data));
	ast_copy_string(aodata->app, app, sizeof(aodata->app));
	ast_copy_string(aodata->appdata, appdata, sizeof(aodata->appdata));
	if (l)
		ast_copy_string(aodata->cid_num, l, sizeof(aodata->cid_num));
	if (n)
		ast_copy_string(aodata->cid_name, n, sizeof(aodata->cid_name));
	aodata->vars = vars;	
	ast_copy_string(aodata->context, context, sizeof(aodata->context));
	ast_copy_string(aodata->exten, exten, sizeof(aodata->exten));
	ast_copy_string(aodata->account, account, sizeof(aodata->account));
	aodata->timeout = to;
	aodata->priority = pi;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (ast_pthread_create(&th, &attr, action_aoriginate_run, aodata)) {
		free(aodata);
		astman_send_error(s, m, "AOriginate failed");
	} else
		astman_send_ack(s, m, "AOriginate successfully queued");
	pthread_attr_destroy(&attr);
out:
	STANDARD_DECREMENT_USECOUNT;
	return 0;
}

static int loaded = 0;
int load_module(void)
{
	if (ast_manager_register2(name_aoriginate,
	                          EVENT_FLAG_CALL,
	                          action_aoriginate,
	                          synopsis_aoriginate,
	                          mandescr_aoriginate))
		return -1;
	loaded = 1;
	return 0;
}

int unload_module(void)
{
	if (!loaded)
		return 0;
	ast_manager_unregister(name_aoriginate);
	return 0;
}

char *description(void)
{
	return MODULE_DESCRIPTION;
}

/* uses Asterisk's unreliable module reference counting */
int usecount(void)
{
	int res;
	STANDARD_USECOUNT(res);
	return res;
}

char *key(void)
{
	return ASTERISK_GPL_KEY;
}
