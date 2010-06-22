/*
 * Asterisk -- An open source telephony toolkit.
 * Manager command AOriginate: Fast Originate action, implemented differently
 *
 * Copyright (C) 1999 - 2005, Digium, Inc.
 * Copyright (C) 2008-2010  Proformatique.
 *
 * This program is free software; you can redistribute it and/or modify
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

#include <asterisk.h>

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

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

struct aoriginate_data {
	char tech[AST_MAX_EXTENSION];
	char data[AST_MAX_EXTENSION];
	int timeout;
	char app[AST_MAX_APP];
	char appdata[AST_MAX_EXTENSION];
	char cid_name[AST_MAX_EXTENSION];
	char cid_num[AST_MAX_EXTENSION];
	char context[AST_MAX_CONTEXT];
	char exten[AST_MAX_EXTENSION];
	char idtext[AST_MAX_EXTENSION];
	char account[AST_MAX_ACCOUNT_CODE];
	int priority;
	struct ast_variable *vars;
};

#define AO_TOEXTEN 0
#define AO_TOAPP 1
#define AO_DEST(aodata) (!ast_strlen_zero((aodata)->app))

/*! Function to post an empty cdr after a spool call fails.
 *
 *  This function posts an empty cdr for a failed spool call
 *
 *  The same function exists in ast_pbx_outgoing_cdr_failed() in main/pbx.c
 *  This one should be updated if ast_pbx_outgoing_cdr_failed() changes.
 */
static int my_pbx_outgoing_cdr_failed(void)
{
	/* allocate a channel */
	struct ast_channel *chan = ast_channel_alloc(0, AST_STATE_DOWN, 0, 0, "", "", "", 0, 0);

	if (!chan)
		return -1;  /* failure */

	if (!chan->cdr) {
		/* allocation of the cdr failed */
		ast_channel_free(chan);   /* free the channel */
		return -1;                /* return failure */
	}

	/* allocation of the cdr was successful */
	ast_cdr_init(chan->cdr, chan);  /* initilize our channel's cdr */
	ast_cdr_start(chan->cdr);       /* record the start and stop time */
	ast_cdr_end(chan->cdr);
	ast_cdr_failed(chan->cdr);      /* set the status to failed */
	ast_cdr_detach(chan->cdr);      /* post and free the record */
	ast_channel_free(chan);         /* free the channel */

	return 0;  /* success */
}

static void *action_aoriginate_run(void *data)
{
	struct ast_module_user *u;
	struct ast_channel *chan;
	struct aoriginate_data *aodata = data;
	struct outgoing_helper oh;
	char *cid_num;
	char *cid_name;
	int reason = 0;
	char chan_uniqueid[32] = "<null>";

	cid_num = S_OR(aodata->cid_num, NULL);
	cid_name = S_OR(aodata->cid_name, NULL);

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

	u = ast_module_user_add(chan);
	if(u == NULL)
		goto error_local_user_add;

	ast_mutex_lock(&chan->lock);
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
			"Uniqueid: %s\r\n"		\
			"CallerID: %s\r\n"		/* This parameter is deprecated and will be removed post-1.4 */	\
			"CallerIDNum: %s\r\n"		\
			"CallerIDName: %s\r\n",		\
			aodata->idtext,			\
			aodata->tech,			\
			aodata->data,			\
			aodata->context,		\
			aodata->exten,			\
			reason,				\
			chan->uniqueid,			\
			S_OR(aodata->cid_num, "<unknown>"),	\
			S_OR(aodata->cid_num, "<unknown>"),	\
			S_OR(aodata->cid_name, "<unknown>"))

	if (AO_DEST(aodata) == AO_TOAPP) {
		struct ast_app *app;
		/* NOTE that there is a race condition with (other) module
		   unloading here, but Asterisk does the same. */
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

		pbx_exec(chan, app, S_OR(aodata->appdata, NULL));
		ast_module_user_remove(u);
		ast_hangup(chan);
	} else {
		ast_mutex_unlock(&chan->lock);
		
		MANAGER_EVENT_AORIGINATE_SUCCESS();

		if (ast_pbx_run(chan)) {
			ast_log(LOG_ERROR, "Unable to run PBX on %s\n", chan->name);
			/* should remain before ast_hangup so there is no risk
			   that a race with ast_module_user_hangup_all() occurs: */
			ast_module_user_remove(u);
			ast_hangup(chan);
		} else {
			/* BUGBUG if we come here because of ast_module_user_hangup_all(),
			   there will be a double free.
			   This module (as any Asterisk module, because this is
			   a general design bug) should be unloaded with extra care,
			   and shall _NOT_ be unloaded if it is being used.
			   I don't think there is any way to check whether
			   ast_module_user_hangup_all() occured. Avoiding the bug with
			   such a scheme would be a very lame way of fixing it anyway.
			   Proper synchronization is needed to release resources on module
			   unload (but anyway without proper module reference counting,
			   some small race conditions will _always_ exists so module
			   unloading should NEVER be used at all in Asterisk.
			   An other improper way of bugfixing would be to check that u has
			   been removed in __ast_module_user_remove(). This would be a
			   little better but will not fix all potential issues anyway.
			   If somebody ever reads this text and wants to know how to
			   properly design a module unloading function, just go read a
			   recent Linux. */
			ast_module_user_remove(u);
		}
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
	ast_module_user_remove(u);
error_local_user_add:
	ast_copy_string(chan_uniqueid, chan->uniqueid, sizeof(chan_uniqueid));
	ast_hangup(chan);
error_request_and_dial:
	if (reason == 0)
		my_pbx_outgoing_cdr_failed();

	manager_event(EVENT_FLAG_CALL,
		"AOriginateFailure",
		"%s"
		"Channel: %s/%s\r\n"
		"Context: %s\r\n"
		"Exten: %s\r\n"
		"Reason: %d\r\n"
		"Uniqueid: %s\r\n"
		"CallerID: %s\r\n"		/* This parameter is deprecated and will be removed post-1.4 */
		"CallerIDNum: %s\r\n"
		"CallerIDName: %s\r\n",
		aodata->idtext,
		aodata->tech,
		aodata->data,
		aodata->context,
		aodata->exten,
		reason,
		chan_uniqueid,
		S_OR(aodata->cid_num, "<unknown>"),
		S_OR(aodata->cid_num, "<unknown>"),
		S_OR(aodata->cid_name, "<unknown>"));

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
static int action_aoriginate(struct mansession *s, const struct message *m)
{
	ast_module_ref(ast_module_info->self);

	const char *name = astman_get_header(m, "Channel");       /* WWW [1] */
	const char *exten = astman_get_header(m, "Exten");        /* WWW [1] */
	const char *context = astman_get_header(m, "Context");    /* WWW [1] */
	const char *priority = astman_get_header(m, "Priority");  /* WWW [1] */
	const char *timeout = astman_get_header(m, "Timeout");    /* WWW [1] */
	const char *callerid = astman_get_header(m, "CallerID");  /* WWW [1] */
	const char *account = astman_get_header(m, "Account");    /* WWW [1] */
	const char *app = astman_get_header(m, "Application");    /* WWW [1] */
	const char *appdata = astman_get_header(m, "Data");       /* WWW [1] */
	const char *id = astman_get_header(m, "ActionID");        /* WWW [1] */
	struct ast_variable *vars = astman_get_variables(m);
	struct aoriginate_data *aodata;
	char *tech, *data;
	char *l = NULL, *n = NULL;
	char tmp[256];
	char tmp2[256];
	int pi = 0;
	int to = 30000;
	pthread_t th;
	pthread_attr_t attr;

	if (!name) {
		astman_send_error(s, m, "Channel not specified"); /* WWW [1] */
		goto clear_vars;
	}
	if (!ast_strlen_zero(priority) && (sscanf(priority, "%d", &pi) != 1)) {
		if ((pi = ast_findlabel_extension(NULL, context, exten, priority, NULL)) < 1) {
			astman_send_error(s, m, "Invalid priority"); /* WWW [1] */
			goto clear_vars;
		}
	}
	if (!ast_strlen_zero(timeout) && (sscanf(timeout, "%d", &to) != 1)) {
		astman_send_error(s, m, "Invalid timeout");       /* WWW [1] */
		goto clear_vars;
	}
	ast_copy_string(tmp, name, sizeof(tmp));
	tech = tmp;
	data = strchr(tmp, '/');
	if (!data) {
		astman_send_error(s, m, "Invalid channel");       /* WWW [1] */
		goto clear_vars;
	}
	*data++ = '\0';
	ast_copy_string(tmp2, callerid, sizeof(tmp2));
	ast_callerid_parse(tmp2, &n, &l);
	n = S_OR(n, NULL);
	if (l) {
		ast_shrink_phone_number(l);
		if (ast_strlen_zero(l))
			l = NULL;
	}
	aodata = ast_calloc(1, sizeof(*aodata));
	if (aodata == NULL) {
		astman_send_error(s, m, "Memory allocation failure");/* WWW [1] */
		goto clear_vars;
	}
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
		ast_variables_destroy(vars);
		free(aodata);
		astman_send_error(s, m, "AOriginate failed");     /* WWW [1] */
	} else
		astman_send_ack(s, m, "AOriginate successfully queued");/* WWW [1] */
	pthread_attr_destroy(&attr);

	ast_module_unref(ast_module_info->self);
	return 0;

clear_vars:
	ast_variables_destroy(vars);
	ast_module_unref(ast_module_info->self);
	return 0;
}


static int loaded = 0;
static int load_module(void)
{
	if (ast_manager_register2(name_aoriginate,
	                          EVENT_FLAG_CALL,
	                          action_aoriginate,
	                          synopsis_aoriginate,
	                          mandescr_aoriginate))
		return AST_MODULE_LOAD_FAILURE;
	loaded = 1;
	return AST_MODULE_LOAD_SUCCESS;
}

static int unload_module(void)
{
	if (!loaded)
		return 0;
	ast_manager_unregister(name_aoriginate);
	ast_module_user_hangup_all();
	return 0;
}

AST_MODULE_INFO_STANDARD(ASTERISK_GPL_KEY, "Asynchronous Originate");/* WWW [2] */

/*
 * [1]: discards qualifiers from pointer target type
 *	Compiler warning due to poor Asterisk prototypes
 *
 * [2]: 'static' is not at beginning of declaration
 *      harmless
 */
