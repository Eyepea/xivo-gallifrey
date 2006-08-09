/*
 * Copyright (C) 2006 Richard Braun <rbraun@proformatique.com>
 * Resource module for SQLite 2,
 * based on res_sqlite3 by Anthony Minessale II.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <string.h>
#include <sqlite.h>
#include <pthread.h>
#include <asterisk/cdr.h>
#include <asterisk/config.h>
#include <asterisk/logger.h>
#include <asterisk/module.h>
#include <asterisk/options.h>

#include "res_sqlite.h"

static int cdr_registered;
static sqlite *db;
static char *dbfile;
static char *config_table;
static char *cdr_table;

static int set_var(char **var, char *name, char *value);
static int load_config(void);
static void unload_config(void);
static int check_vars(void);
static int cdr_handler(struct ast_cdr *cdr);

/*
 * Taken from Asterisk 1.2 cdr_sqlite.so.
 */

static char *sql_create_cdr_table =
"CREATE TABLE '%q' ("
"	AcctId		INTEGER PRIMARY KEY,"
"	clid		VARCHAR(80),"
"	src		VARCHAR(80),"
"	dst		VARCHAR(80),"
"	dcontext	VARCHAR(80),"
"	channel		VARCHAR(80),"
"	dstchannel	VARCHAR(80),"
"	lastapp		VARCHAR(80),"
"	lastdata	VARCHAR(80),"
"	start		CHAR(19),"
"	answer		CHAR(19),"
"	end		CHAR(19),"
"	duration	INTEGER,"
"	billsec		INTEGER,"
"	disposition	INTEGER,"
"	amaflags	INTEGER,"
"	accountcode	VARCHAR(20),"
"	uniqueid	VARCHAR(32),"
"	userfield	VARCHAR(255)"
");";

static char *sql_add_cdr_entry =
"INSERT INTO '%q' ("
"       clid,"
"	src,"
"	dst,"
"	dcontext,"
"	channel,"
"	dstchannel,"
"	lastapp,"
"	lastdata,"
"	start,"
"	answer,"
"	end,"
"	duration,"
"	billsec,"
"	disposition,"
"	amaflags,"
"	accountcode,"
"	uniqueid,"
"	userfield"
") VALUES ("
"	'%q',"
"	'%q',"
"	'%q',"
"	'%q',"
"	'%q',"
"	'%q',"
"	'%q',"
"	'%q',"
"	datetime(%d,'unixepoch')"
"	,datetime(%d,'unixepoch'),"
"	datetime(%d,'unixepoch'),"
"	'%ld',"
"	'%ld',"
"	'%ld',"
"	'%ld',"
"	'%q',"
"	'%q',"
"	'%q'"
");";

static int
set_var(char **var, char *name, char *value)
{
  if (*var != NULL)
    free(*var);

  *var = strdup(value);

  if (*var == NULL)
    {
      ast_log(LOG_ERROR, RES_SQLITE_NAME ": Unable to allocate variable %s\n",
              name);
      return 1;
    }

  return 0;
}

static int
load_config(void)
{
  struct ast_config *config;
  struct ast_variable *var;
  int error;

  config = ast_config_load(RES_SQLITE_CONF_FILE);

  if (config == NULL)
    return 1;

  for (var = ast_variable_browse(config, "general");
       var != NULL;
       var = var->next)
    {
      if (strcmp(var->name, "dbfile") == 0)
        SET_VAR(config, dbfile, var)

      else if (strcmp(var->name, "config_table") == 0)
        SET_VAR(config, config_table, var)

      else if (strcmp(var->name, "cdr_table") == 0)
        SET_VAR(config, cdr_table, var)

      else
        ast_log(LOG_WARNING, RES_SQLITE_NAME ": Unknown parameter : %s\n",
                var->name);
    }

  ast_config_destroy(config);
  error = check_vars();

  if (error)
    return 1;

  return 0;
}

static void
unload_config(void)
{
  free(dbfile);
  dbfile = NULL;
  free(config_table);
  config_table = NULL;
  free(cdr_table);
  cdr_table = NULL;
}

static int
check_vars(void)
{
  CHECK_PARAMETER(dbfile)
  CHECK_PARAMETER(config_table)
  CHECK_PARAMETER(cdr_table)
  return 0;
}

static int
cdr_handler(struct ast_cdr *cdr)
{
  char *errormsg;
  int i, error;

  for (i = 0; i < 3; i++)
    {
      error = sqlite_exec_printf(db, sql_add_cdr_entry, NULL, NULL, &errormsg,
                                 cdr_table, cdr->clid, cdr->src, cdr->dst,
                                 cdr->dcontext, cdr->channel, cdr->dstchannel,
                                 cdr->lastapp, cdr->lastdata, cdr->start.tv_sec,
                                 cdr->answer.tv_sec, cdr->end.tv_sec,
                                 cdr->duration, cdr->billsec, cdr->disposition,
                                 cdr->amaflags, cdr->accountcode, cdr->uniqueid,
                                 cdr->userfield);

      if (!error || (error && (error != SQLITE_BUSY && error != SQLITE_LOCKED)))
        break;
    }

  if (error)
    {
      ast_log(LOG_ERROR, RES_SQLITE_NAME ": %s\n", errormsg);
      free(errormsg);
    }

  return 0;
}

int
load_module(void)
{
  char *errormsg;
  int error;

  cdr_registered = 0;
  error = load_config();

  if (error)
    {
      unload_module();
      return 1;
    }

  db = sqlite_open(dbfile, 0660, &errormsg);

  if (db == NULL)
    {
      ast_log(LOG_ERROR, RES_SQLITE_NAME ": %s\n", errormsg);
      free(errormsg);
      unload_module();
      return 1;
    }

  error = sqlite_exec_printf(db, "SELECT COUNT(AcctId) FROM %s;", NULL, NULL,
                             &errormsg, cdr_table);

  if (error)
    {
      /*
       * Unexpected error.
       */
      if (error != SQLITE_ERROR)
        {
          ast_log(LOG_ERROR, RES_SQLITE_NAME ": %s\n", errormsg);
          free(errormsg);
          unload_module();
          return 1;
        }

      error = sqlite_exec_printf(db, sql_create_cdr_table, NULL, NULL,
              &errormsg, cdr_table);

      if (error)
        {
          ast_log(LOG_ERROR, RES_SQLITE_NAME ": %s\n", errormsg);
          free(errormsg);
          unload_module();
          return 1;
        }
    }

  error = ast_cdr_register(RES_SQLITE_NAME, RES_SQLITE_DESCRIPTION,
                           cdr_handler);

  if (error)
    {
      unload_module();
      return 1;
    }

  cdr_registered = 1;
  return 0;
}

int
unload_module(void)
{
  if (cdr_registered)
    ast_cdr_unregister(RES_SQLITE_NAME);

  if (db != NULL)
    sqlite_close(db);

  unload_config();
  return 0;
}

char *
description(void)
{
  return RES_SQLITE_DESCRIPTION;
}

int
usecount(void)
{
  return 0;
}

char *
key(void)
{
  return ASTERISK_GPL_KEY;
}
