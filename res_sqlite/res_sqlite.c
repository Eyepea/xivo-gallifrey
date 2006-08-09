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

static int enabled;
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
static char sql_create_cdr_table[] =
"CREATE TABLE %s ("
"       AcctId          INTEGER PRIMARY KEY,"
"       clid            VARCHAR(80),"
"       src             VARCHAR(80),"
"       dst             VARCHAR(80),"
"       dcontext        VARCHAR(80),"
"       channel         VARCHAR(80),"
"       dstchannel      VARCHAR(80),"
"       lastapp         VARCHAR(80),"
"       lastdata        VARCHAR(80),"
"       start           CHAR(19),"
"       answer          CHAR(19),"
"       end             CHAR(19),"
"       duration        INTEGER,"
"       billsec         INTEGER,"
"       disposition     INTEGER,"
"       amaflags        INTEGER,"
"       accountcode     VARCHAR(20),"
"       uniqueid        VARCHAR(32),"
"       userfield       VARCHAR(255)"
");";

static int
set_var(char **var, char *name, char *value)
{
  if (*var != NULL)
    free(*var);

  *var = strdup(value);

  if (*var == NULL)
    {
      ast_log(LOG_ERROR, "Unable to allocate variable %s\n", name);
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
        ast_log(LOG_WARNING, "Unknown parameter : %s\n", var->name);
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
  ast_verbose(VERBOSE_PREFIX_3 "cdr_handler() called\n");
  return 0;
}

int
load_module(void)
{
  char *errormsg;
  int error;

  enabled = 1;
  cdr_registered = 0;
  error = load_config();

  if (error)
    {
      ast_verbose(VERBOSE_PREFIX_3 "Unable to load " RES_SQLITE_CONF_FILE
                  ", " RES_SQLITE_NAME " disabled\n");
      unload_module();
      return 0;
    }

  db = sqlite_open(dbfile, 0660, &errormsg);

  if (db == NULL)
    {
      ast_log(LOG_ERROR, "Unable to open SQLite database: %s\n", errormsg);
      unload_module();
      return 0;
    }

  error = ast_cdr_register(RES_SQLITE_NAME, RES_SQLITE_DESCRIPTION,
                           cdr_handler);

  if (error)
    {
      unload_module();
      return 0;
    }

  cdr_registered = 1;
  error = sqlite_exec_printf(db, "SELECT COUNT(AcctId) FROM %s;", NULL, NULL,
                             &errormsg, cdr_table);

  if (error)
    {
      /*
       * Unexpected error.
       */
      if (error != SQLITE_ERROR)
        {
          ast_log(LOG_ERROR, "Unable to access CDR table, error: %s\n",
                  errormsg);
          unload_module();
          return 0;
        }

      else
        {
          error = sqlite_exec_printf(db, sql_create_cdr_table, NULL, NULL,
                  &errormsg, cdr_table);

          if (error)
            {
              ast_log(LOG_ERROR, "Unable to create CDR table, error: %s\n",
                      errormsg);
              unload_module();
              return 0;
            }
        }
    }

  return 0;
}

int
unload_module(void)
{
  if (!enabled)
    return 0;

  unload_config();

  if (db != NULL)
    sqlite_close(db);

  if (cdr_registered)
    ast_cdr_unregister(RES_SQLITE_NAME);

  cdr_registered = 0;
  enabled = 0;
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
