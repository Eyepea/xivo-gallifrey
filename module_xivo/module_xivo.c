/*
 * Various Asterisk functions and applications used in XIVO.
 * Copyright (C) 2008-2010  Proformatique <technique@proformatique.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <asterisk.h>

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include <asterisk/module.h>
#include <asterisk/utils.h>
#include <asterisk/pbx.h>
#include <asterisk/app.h>
#include <string.h>
#include <stdlib.h>

#define UNUSED(x) ((void)x)

#define STRSUBST_SYNTAX		"STRSUBST(<string>,<string_to_search>,<replacement_string>)"

static void real_strsubst(char *retbuf, size_t len, const char *string, const char *search, const char *replace)
{
	const char *start;
	char *needle;
	char *dest = retbuf;
	ssize_t nb;	/* number of chars to copy, without the final \0 */
	ssize_t dest_len = ((ssize_t)len) - 1;
	size_t search_strlen = strlen(search);
	size_t replace_strlen = strlen(replace);

	for (start = string;
	     (needle = strstr(start, search)) != NULL;
	     start = needle + search_strlen) {
		nb = MIN(dest_len, needle - start);
		ast_copy_string(dest, start, (size_t)(nb + 1));
		dest_len -= nb;
		if (dest_len <= 0)
			return;
		dest += nb;
		nb = MIN(dest_len, (ssize_t)replace_strlen);
		ast_copy_string(dest, replace, (size_t)(nb + 1));
		dest_len -= nb;
		if (dest_len <= 0)
			return;
		dest += nb;
	}

	if (dest_len > 0)
		ast_copy_string(dest, start, (size_t)dest_len);

	return;
}

static char *decode(char *string)
{
	char stroct[4];
	int c;
	char *readp;
	char *writep;

	for (writep = string, readp = string; *readp; readp++, writep++) {
		if (*readp == '\\') {
			char *eoo;
			ast_copy_string(stroct, readp + 1, 4);
			if (strlen(stroct) != 3) {
				ast_log(LOG_WARNING, "Truncated coded string\n");
				*writep = '\0';
				return string;
			}
			c = strtol(stroct, &eoo, 8);
			if (*eoo != '\0' || c <= 0 || c > 255) {
				ast_log(LOG_WARNING, "Badly coded string (invalid ASCII character octal coding)\n");
				*writep = '\0';
				return string;
			}
			*writep = c;
			readp += 3;
		} else
			*writep = *readp;
	}

	*writep = '\0';
	return string;
}

static int strsubst(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len)
{
	char *string;
	char *search;
	char *replace;
	char *delim;

	ast_assert(len > 0);

	UNUSED(chan);
	UNUSED(cmd);

	delim = ast_strdupa(data);
	
	string = strsep(&delim, "|");
	search = strsep(&delim, "|");
	if (search == NULL)
		goto error_strsep;
	replace = strsep(&delim, "|");
	if (replace == NULL || delim != NULL)
		goto error_strsep;

	search = decode(search);
	replace = decode(replace);
	if (ast_strlen_zero(search)) {
		ast_log(LOG_WARNING, "Empty <string_to_search> argument in call to " STRSUBST_SYNTAX ", returning unchanged <string>\n");
		ast_copy_string(buf, string, len);
		return -1;
	}

	real_strsubst(buf, len, string, search, replace);
	return 0;

error_strsep:
	ast_log(LOG_WARNING, "Wrong number of arguments in call to " STRSUBST_SYNTAX " function\n");
	ast_copy_string(buf, "", len);
	return -1;
}

static struct ast_custom_function strsubst_function = {
	.name = "STRSUBST",
	.synopsis = "Replaces a substring inside a string",
	.syntax = STRSUBST_SYNTAX,
	.desc = 
"Returns a copy of <string> with all occurences of <string_to_search> replaced\n"
"by <replacement_string>.\n"
"Note that due to Asterisk technical limitations, <string> cannot contain pipe\n"
"characters - but, curiously, the result can (and it is in some cases useful).\n"
"Beware that there are other constraints that comes from the way the DialPlan is\n"
"parsed by Asterisk. They fall out of the scope of this description.\n"
"Both <string_to_search> and <replacement_string> can contain octal coded\n"
"ASCII characters. For example if STRSUBST receives \\073 in <string_to_search>\n"
"and \\174 in <replacement_string>, all semicolon characters of <string> will be\n"
"replaced by pipe characters in the output. Remember that Asterisk already uses\n"
"backslash characters in the DialPlan to escape special characters so you'll\n"
"have to double it there.\n",
	.read = strsubst,
};


#define GETCONF_SYNTAX		"GETCONF(<varname>)"

#define GETCONF_FUNC_TABLE(var)	{ #var , ast_config_AST_ ## var},

#define GETCONF_FUNC_DOC(var)	"\t" #var "\n"

#define GETCONF_FUNC_VAR(mode, var) GETCONF_FUNC_ ## mode (var)

#define GETCONF_FUNC_LIST(mode)				\
	GETCONF_FUNC_VAR(mode, CONFIG_DIR)		\
	GETCONF_FUNC_VAR(mode, CONFIG_FILE)		\
	GETCONF_FUNC_VAR(mode, MODULE_DIR)		\
	GETCONF_FUNC_VAR(mode, SPOOL_DIR)		\
	GETCONF_FUNC_VAR(mode, MONITOR_DIR)		\
	GETCONF_FUNC_VAR(mode, VAR_DIR)			\
	GETCONF_FUNC_VAR(mode, DATA_DIR)		\
	GETCONF_FUNC_VAR(mode, LOG_DIR)			\
	GETCONF_FUNC_VAR(mode, AGI_DIR)			\
	GETCONF_FUNC_VAR(mode, DB)			\
	GETCONF_FUNC_VAR(mode, KEY_DIR)			\
	GETCONF_FUNC_VAR(mode, PID)			\
	GETCONF_FUNC_VAR(mode, SOCKET)			\
	GETCONF_FUNC_VAR(mode, RUN_DIR)			\
	GETCONF_FUNC_VAR(mode, CTL_PERMISSIONS)		\
	GETCONF_FUNC_VAR(mode, CTL_OWNER)		\
	GETCONF_FUNC_VAR(mode, CTL_GROUP)		\
	GETCONF_FUNC_VAR(mode, CTL)			\
	GETCONF_FUNC_VAR(mode, SYSTEM_NAME)

static int getconf(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len)
{
	unsigned i;

	static const struct {
		const char *varname;
		const char *ast_config_ptr;
	} confvars[] = {
		GETCONF_FUNC_LIST(TABLE)
	};

	UNUSED(chan);
	UNUSED(cmd);

	for (i = 0; i < ARRAY_LEN(confvars); i++) {
		if (!strcasecmp(confvars[i].varname, data)) {
			ast_copy_string(buf, confvars[i].ast_config_ptr, len);
			return 0;
		}
	}

	ast_log(LOG_WARNING, "Asterisk configuration variable \"%s\" unknown\n", data);
	ast_copy_string(buf, "", len);
	return -1;
}

static struct ast_custom_function getconf_function = {
	.name = "GETCONF",
	.synopsis = "Retrieves an Asterisk configuration string",
	.syntax = GETCONF_SYNTAX,
	.desc =
"Available Asterisk configuration strings are:\n"
GETCONF_FUNC_LIST(DOC),
	.read = getconf,
};


#define APPEXISTS_SYNTAX		"APPEXISTS(<application_name>)"

static int appexists(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len)
{
	UNUSED(chan);
	UNUSED(cmd);

	ast_copy_string(buf, pbx_findapp(data) ? "1" : "0", len);
	return 0;
}

static struct ast_custom_function appexists_function = {
	.name = "APPEXISTS",
	.synopsis = "Checks if an application exists",
	.syntax = APPEXISTS_SYNTAX,
	.desc = 
"Checks the list of registered applications for <application_name>.\n"
"Returns 1 if <application_name> exists, 0 otherwise.\n",
	.read = appexists,
};


#define FUNCEXISTS_SYNTAX		"FUNCEXISTS(<function_name>)"

static int funcexists(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len)
{
	UNUSED(chan);
	UNUSED(cmd);

	ast_copy_string(buf, ast_custom_function_find(data) ? "1" : "0", len);
	return 0;
}

static struct ast_custom_function funcexists_function = {
	.name = "FUNCEXISTS",
	.synopsis = "Checks if a function exists",
	.syntax = FUNCEXISTS_SYNTAX,
	.desc = 
"Checks the list of registered functions for <function_name>.\n"
"Returns 1 if <function_name> exists, 0 otherwise.\n",
	.read = funcexists,
};


#define VALIDEXTEN_SYNTAX		"VALID_EXTEN([<context>]|<extension>[|<priority>])"

static int validexten(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len)
{
	int priority_int;

	UNUSED(cmd);

	AST_DECLARE_APP_ARGS(args,
		AST_APP_ARG(context);
		AST_APP_ARG(extension);
		AST_APP_ARG(priority);
	);

	AST_STANDARD_APP_ARGS(args, data);

	if (ast_strlen_zero(args.extension)) {
		ast_log(LOG_WARNING, "Syntax: %s - missing argument <extension>!\n", VALIDEXTEN_SYNTAX);
		ast_copy_string(buf, "", len);
		return -1;
	}

	if (ast_strlen_zero(args.priority) || (priority_int = atoi(args.priority)) < 1)
		priority_int = 1;

	ast_copy_string(buf, ast_exists_extension(chan,
						  S_OR(args.context, chan->context),
						  args.extension,
						  priority_int,
						  chan->cid.cid_num) ? "1" : "0", len);
	return 0;
}

static struct ast_custom_function validexten_function = {
	.name = "VALID_EXTEN",
	.synopsis = "Determine whether an extension exists or not",
	.syntax = VALIDEXTEN_SYNTAX,
	.desc = 
"Returns a true value if the indicated context, extension, and priority exist.\n"
"Context defaults to the current context, priority defaults to 1.\n",
	.read = validexten,
};


static int _set_parse(struct ast_channel *chan, void *data, int ifempty)
{
	char *params;
	char *scan;
	char *varname;
	char *content;
	const char *tmp = NULL;
	int paren = 0;

	if (ast_strlen_zero(data))
		goto error;

	params = ast_strdupa(data);
	
	for (scan = params; *scan && (paren || *scan != '='); scan++) {
		if (*scan == '(')
			paren++;
		else if (*scan == ')') {
			if (paren)
				paren--;
		}
	}
	if (*scan != '=')
		goto error;
	varname = params;
	*scan = '\0';
	content = scan + 1;

	if (!ifempty)
		pbx_builtin_setvar_helper(chan, varname, content);
	else {
		if (*params == '_') {
			params++;
			if (*params == '_')
				params++;
		}

		if (!(tmp = pbx_builtin_getvar_helper(chan, params)) || ast_strlen_zero(tmp))
			pbx_builtin_setvar_helper(chan, varname, content);
	}
	return 0;

error:
	ast_log(LOG_WARNING, "Set requires a variable name, an '=' sign, and a string to affect to the variable.\n");
	return 0;
}

static const char *set_one_name = "SetOne";
static const char *set_one_synopsis = "Set exactly one channel variable and allow the rvalue to contain pipes characters";
static const char *set_one_description =
"  SetOne(name=value)\n"
"This application can be used to set the value of one channel variable, with an\n"
"rvalue which is allowed to contain pipe characters.\n"
"If the variable name is prefixed with _, the variable will be inherited into\n"
"channels created from the current channel. If the variable name is prefixed\n"
"with __, the variable will be inherited into channels created from the current\n"
"channel and all children channels.\n";
static int set_one(struct ast_channel *chan, void *data)
{
	return _set_parse(chan, data, 0);
}

static const char *set_ifempty_name = "SetIfEmpty";
static const char *set_ifempty_synopsis = "Set exactly one channel variable if the variable has no length or doesn't exist";
static const char *set_ifempty_description =
"  SetIfEmpty(name=value)\n"
"This application set the value of one channel variable if the variable has no length or doesn't exist.\n"
"If the variable name is prefixed with _, the variable will be inherited into\n"
"channels created from the current channel. If the variable name is prefixed\n"
"with __, the variable will be inherited into channels created from the current\n"
"channel and all children channels.\n";
static int set_ifempty(struct ast_channel *chan, void *data)
{
	return _set_parse(chan, data, 1);
}

static int loaded;
static int load_module(void)
{
	ast_custom_function_register(&strsubst_function);
	ast_custom_function_register(&getconf_function);
	ast_custom_function_register(&appexists_function);
	ast_custom_function_register(&funcexists_function);
	ast_custom_function_register(&validexten_function);
	if (ast_register_application(
			set_one_name,
			set_one,
			set_one_synopsis,
			set_one_description) < 0
	    || ast_register_application(
	    		set_ifempty_name,
			set_ifempty,
			set_ifempty_synopsis,
			set_ifempty_description) < 0) {
		ast_unregister_application(set_one_name);
		ast_unregister_application(set_ifempty_name);
		ast_custom_function_unregister(&validexten_function);
		ast_custom_function_unregister(&funcexists_function);
		ast_custom_function_unregister(&appexists_function);
		ast_custom_function_unregister(&getconf_function);
		ast_custom_function_unregister(&strsubst_function);
		return AST_MODULE_LOAD_FAILURE;
	}
	loaded = 1;
	return AST_MODULE_LOAD_SUCCESS;
}

static int unload_module(void)
{
	if (!loaded)
		return 0;
	ast_unregister_application(set_one_name);
	ast_unregister_application(set_ifempty_name);
	ast_custom_function_unregister(&validexten_function);
	ast_custom_function_unregister(&funcexists_function);
	ast_custom_function_unregister(&appexists_function);
	ast_custom_function_unregister(&getconf_function);
	ast_custom_function_unregister(&strsubst_function);
	return 0;
}

AST_MODULE_INFO_STANDARD(ASTERISK_GPL_KEY, "XIVO specific code");/* WWW [1] */

/*
 * [1]: 'static' is not at beginning of declaration
 *      harmless
 */
