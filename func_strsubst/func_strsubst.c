/*
 * String Substitution Function for Asterisk
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

#include <asterisk/module.h>
#include <asterisk/pbx.h>
#include <string.h>
#include <stdlib.h>

#define UNUSED(x) ((void)x)

#define MODULE_DESCRIPTION	"Function to replace substrings and application to set a channel variable with a string containing pipe characters"
#define STRSUBST_SYNTAX		"STRSUBST(<string>,<string_to_search>,<replacement_string>)"

const char module_date_revision[] = "func_strsubst $Date$ $Revision$ $HeadURL$";

static void real_strsubst(char *retbuf, size_t len, const char *string, const char *search, const char *replace)
{
	const char *start;
	char *needle;
	char *dest = retbuf;
	ssize_t nb;	/* number of chars to copy, without the final \0 */
	ssize_t dest_len = len - 1;
	size_t search_strlen = strlen(search);
	size_t replace_strlen = strlen(replace);

	for (start = string;
	     (needle = strstr(start, search)) != NULL;
	     start = needle + search_strlen) {
		nb = MIN(dest_len, needle - start);
		ast_copy_string(dest, start, nb + 1);
		dest_len -= nb;
		if (dest_len <= 0)
			return;
		dest += nb;
		nb = MIN(dest_len, (ssize_t)replace_strlen);
		ast_copy_string(dest, replace, nb + 1);
		dest_len -= nb;
		if (dest_len <= 0)
			return;
		dest += nb;
	}

	ast_copy_string(dest, start, dest_len);

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

static char *strsubst(struct ast_channel *chan, char *cmd, char *data, char *buf, size_t len) 
{
	char *string;
	char *search;
	char *replace;
	char *delim;

	UNUSED(chan);
	UNUSED(cmd);

	if (len == 0)
		return buf;

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
		return buf;
	}

	real_strsubst(buf, len, string, search, replace);
	return buf;

error_strsep:
	ast_log(LOG_WARNING, "Wrong number of arguments in call to " STRSUBST_SYNTAX " function\n");
	ast_copy_string(buf, "", len);
	return buf;
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
	char *params;
	char *scan;
	char *varname;
	char *content;
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
	
	pbx_builtin_setvar_helper(chan, varname, content);
	return 0;

error:
	ast_log(LOG_WARNING, "Set requires a variable name, an '=' sign, and a string to affect to the variable.\n");
	return 0;
}

static int loaded;
int load_module(void)
{
	ast_custom_function_register(&strsubst_function);
	if (ast_register_application(
			set_one_name, set_one,
			set_one_synopsis, set_one_description) < 0) {
		ast_custom_function_unregister(&strsubst_function);
	}
	loaded = 1;
	return 0;
}

int unload_module(void)
{
	if (!loaded)
		return 0;
	ast_unregister_application(set_one_name);
	ast_custom_function_unregister(&strsubst_function);
	return 0;
}

char *description(void)
{
	return MODULE_DESCRIPTION;
}

int usecount(void)
{
	return 0;
}

char *key(void)
{
	return ASTERISK_GPL_KEY;
}
