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

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite.h>
#include <pthread.h>
#include <asterisk/cdr.h>
#include <asterisk/lock.h>
#include <asterisk/config.h>
#include <asterisk/logger.h>
#include <asterisk/module.h>
#include <asterisk/options.h>

#define RES_SQLITE_NAME "res_sqlite"
#define RES_SQLITE_DRIVER "sqlite"
#define RES_SQLITE_DESCRIPTION "Resource Module for SQLite 2"
#define RES_SQLITE_CONF_FILE "res_sqlite.conf"

#define SET_VAR(config, to, from) \
{ \
  int __error; \
  __error = set_var(&to, #to, from->value); \
  if (__error) \
    { \
      ast_config_destroy(config); \
      unload_config(); \
      return 1; \
    } \
}

#define CHECK_PARAMETER(name) \
{ \
  if (name == NULL) \
    { \
      ast_log(LOG_ERROR, "Undefined parameter %s\n", #name); \
      return 1; \
    } \
}

#define RES_SQLITE_BEGIN \
{ \
  int __i; \
  for (__i = 0; __i < 10; __i++) \
    {

#define RES_SQLITE_END(error) \
      if (error != SQLITE_BUSY && error != SQLITE_LOCKED) \
        break; \
      usleep(1000); \
    } \
}

struct cfg_entry_args
{
  struct ast_config *cfg;
  struct ast_category *cat;
  char *cat_name;
};

struct rt_cfg_entry_args
{
  struct ast_variable *var;
  struct ast_variable *last;
};

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
static int add_cfg_entry(void *arg, int argc, char **argv, char **columnNames);
static struct ast_config * config_handler(const char *database,
                                          const char *table, const char *file,
                                          struct ast_config *cfg);
static int add_rt_cfg_entry(void *arg, int argc, char **argv,
                            char **columnNames);
static struct ast_variable * realtime_handler(const char *database,
                                              const char *table, va_list ap);
static struct ast_config * realtime_multi_handler(const char *database,
                                                  const char *table,
                                                  va_list ap);
static int realtime_update_handler(const char *database, const char *table,
                                   const char *keyfield, const char *entity,
                                   va_list ap);

AST_MUTEX_DEFINE_STATIC(mutex);

static struct ast_config_engine sqlite_engine =
{
  .name = RES_SQLITE_DRIVER,
  .load_func = config_handler,
  .realtime_func = realtime_handler,
  .realtime_multi_func = realtime_multi_handler,
  .update_func = realtime_update_handler
};

/*
 * Taken from Asterisk 1.2 cdr_sqlite.so.
 */

static char *sql_create_cdr_table =
"CREATE TABLE '%q' ("
"	id		INTEGER PRIMARY KEY,"
"	clid		VARCHAR(80),"
"	src		VARCHAR(80),"
"	dst		VARCHAR(80),"
"	dcontext	VARCHAR(80),"
"	channel		VARCHAR(80),"
"	dstchannel	VARCHAR(80),"
"	lastapp		VARCHAR(80),"
"	lastdata	VARCHAR(80),"
"	start		VARCHAR(80),"
"	answer		VARCHAR(80),"
"	end		VARCHAR(80),"
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

static char *sql_get_config_table =
"SELECT *"
"	FROM '%q'"
"	WHERE filename = '%q' AND commented = 0"
"	ORDER BY category;";

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
  /*
   * Don't check config_table, as it can be given in extconfig.conf.
   */
  CHECK_PARAMETER(dbfile)
  CHECK_PARAMETER(cdr_table)
  return 0;
}

static int
cdr_handler(struct ast_cdr *cdr)
{
  char *errormsg;
  int error;

  ast_mutex_lock(&mutex);

  RES_SQLITE_BEGIN
    error = sqlite_exec_printf(db, sql_add_cdr_entry, NULL, NULL, &errormsg,
                               cdr_table, cdr->clid, cdr->src, cdr->dst,
                               cdr->dcontext, cdr->channel, cdr->dstchannel,
                               cdr->lastapp, cdr->lastdata, cdr->start.tv_sec,
                               cdr->answer.tv_sec, cdr->end.tv_sec,
                               cdr->duration, cdr->billsec, cdr->disposition,
                               cdr->amaflags, cdr->accountcode, cdr->uniqueid,
                               cdr->userfield);
  RES_SQLITE_END(error)

  ast_mutex_unlock(&mutex);

  if (error)
    {
      ast_log(LOG_ERROR, RES_SQLITE_NAME ": %s\n", errormsg);
      free(errormsg);
      return 1;
    }

  return 0;
}

/*
 * Columns are :
 *  0 id
 *  1 filename
 *  2 category
 *  3 commented
 *  4 var_name
 *  5 var_val
 */
static int
add_cfg_entry(void *arg, int argc, char **argv, char **columnNames)
{
  struct cfg_entry_args *args;
  struct ast_variable *var;

  if (argc != 6)
    {
      ast_log(LOG_ERROR, RES_SQLITE_NAME ": Corrupt table\n");
      return 1;
    }

  args = (struct cfg_entry_args *)arg;

  if (args->cat_name == NULL || strcmp(args->cat_name, argv[2]))
    {
      args->cat = ast_category_new(argv[2]);

      if (args->cat == NULL)
        {
          ast_log(LOG_ERROR, RES_SQLITE_NAME ": Unable to allocate category\n");
          return 1;
        }

      free(args->cat_name);
      args->cat_name = strdup(argv[2]);

      if (args->cat_name == NULL)
        {
          ast_log(LOG_ERROR, RES_SQLITE_NAME ": Unable to allocate category "
                  "name\n");
          return 1;
        }

      ast_category_append(args->cfg, args->cat);
    }

  var = ast_variable_new(argv[4], argv[5]);

  if (var == NULL)
    {
      ast_log(LOG_ERROR, RES_SQLITE_NAME ": Unable to allocate variable");
      return 1;
    }

  ast_variable_append(args->cat, var);
  return 0;
}

static struct ast_config *
config_handler(const char *database, const char *table, const char *file,
               struct ast_config *cfg)
{
  struct cfg_entry_args args;
  char *errormsg;
  int error;

  if (table == NULL)
    {
      if (config_table == NULL)
        {
          ast_log(LOG_ERROR, RES_SQLITE_NAME ": Table name unspecified\n");
          return NULL;
        }

      else
        table = config_table;
    }

  args.cfg = cfg;
  args.cat = NULL;
  args.cat_name = NULL;

  /*
   * This handler is normally called only by a single thread, so there should
   * be no need for locking.
   */
  RES_SQLITE_BEGIN
    error = sqlite_exec_printf(db, sql_get_config_table, add_cfg_entry,
                               &args, &errormsg, table, file);
  RES_SQLITE_END(error)

  free(args.cat_name);

  if (error)
    {
      ast_log(LOG_ERROR, RES_SQLITE_NAME ": %s\n", errormsg);
      free(errormsg);
      ast_config_destroy(cfg);
      return NULL;
    }

  return cfg;
}

static int
add_rt_cfg_entry(void *arg, int argc, char **argv, char **columnNames)
{
  struct rt_cfg_entry_args *args;
  struct ast_variable *var;
  int i;

  args = (struct rt_cfg_entry_args *)arg;

  for (i = 0; i < argc; i++)
    {
      if (argv[i] == NULL)
        continue;

      var = ast_variable_new(columnNames[i], argv[i]);

      if (var == NULL)
        {
          ast_log(LOG_ERROR, RES_SQLITE_NAME ": Unable to allocate variable\n");
          return 1;
        }

      if (args->var == NULL)
        args->var = var;

      if (args->last == NULL)
        args->last = var;

      else
        {
          args->last->next = var;
          args->last = var;
        }
    }

  return 0;
}

static struct ast_variable *
realtime_handler(const char *database, const char *table, va_list ap)
{
  const char **tmp, *param, *val, **params, **vals;
  struct rt_cfg_entry_args args;
  char *query, *errormsg;
  size_t params_count;
  int error;

  if (table == NULL)
    {
      ast_log(LOG_ERROR, RES_SQLITE_NAME ": Table name unspecified\n");
      return NULL;
    }

  params = NULL;
  vals = NULL;
  params_count = 0;

  while ((param = va_arg(ap, const char *)) != NULL
         && (val = va_arg(ap, const char *)) != NULL)
    {
      tmp = realloc(params, (params_count + 1) * sizeof(char *));

      if (tmp == NULL)
        {
          ast_log(LOG_ERROR, RES_SQLITE_NAME ": Unable to allocate params\n");
          free(params);
          free(vals);
          return NULL;
        }

      params = tmp;
      tmp = realloc(vals, (params_count + 1) * sizeof(char *));

      if (tmp == NULL)
        {
          ast_log(LOG_ERROR, RES_SQLITE_NAME ": Unable to allocate vars\n");
          free(params);
          free(vals);
          return NULL;
        }

      vals = tmp;
      params[params_count] = param;
      vals[params_count] = val;
      params_count++;
    }

  if (params_count == 0)
    {
      ast_log(LOG_ERROR, RES_SQLITE_NAME ": 1 parameter and 1 value at least "
              "required\n");
      return NULL;
    }

#define QUERY "SELECT * FROM '%q' WHERE commented = 0 AND %q = '%q'"
  query = sqlite_mprintf(QUERY, table, params[0], vals[0]);

  if (query == NULL)
    {
      ast_log(LOG_ERROR, RES_SQLITE_NAME ": Unable to allocate SQL query\n");
      free(params);
      free(vals);
      return NULL;
    }

  if (params_count > 1)
    {
      char *tmp_str;
      size_t i;

      for (i = 1; i < params_count; i++)
        {
          tmp_str = sqlite_mprintf("%s AND %q = '%q'", query, params[i],
                                   vals[i]);

          if (tmp_str == NULL)
            {
              ast_log(LOG_ERROR, RES_SQLITE_NAME ": Unable to reallocate SQL "
                      "query\n");
              free(params);
              free(vals);
              sqlite_freemem(query);
              return NULL;
            }

          sqlite_freemem(query);
          query = tmp_str;
        }
    }

  free(params);
  free(vals);

  args.var = NULL;
  args.last = NULL;

  ast_mutex_lock(&mutex);

  RES_SQLITE_BEGIN
    error = sqlite_exec_printf(db, "%s LIMIT 1;", add_rt_cfg_entry, &args,
                               &errormsg, query);
  RES_SQLITE_END(error)

  ast_mutex_unlock(&mutex);

  sqlite_freemem(query);

  if (error)
    {
      ast_log(LOG_ERROR, RES_SQLITE_NAME ": %s\n", errormsg);
      ast_variables_destroy(args.var);
      return NULL;
    }

  return args.var;
}

static struct ast_config *
realtime_multi_handler(const char *database, const char *table, va_list ap)
{
  const char *var;

  ast_verbose(VERBOSE_PREFIX_3 "%s() called: database = %s, table = %s\n",
              __PRETTY_FUNCTION__, database, table);

  while ((var = va_arg(ap, const char *)) != NULL)
    {
      ast_verbose(VERBOSE_PREFIX_3 "%s\n", var);
    }

  ast_verbose(VERBOSE_PREFIX_3 "%s() returns\n", __PRETTY_FUNCTION__);

  return NULL;
}

static int
realtime_update_handler(const char *database, const char *table,
                        const char *keyfield, const char *entity,
                        va_list ap)
{
  const char *var;

  ast_verbose(VERBOSE_PREFIX_3 "%s() called: database = %s, table = %s\n",
              __PRETTY_FUNCTION__, database, table);
  ast_verbose(VERBOSE_PREFIX_3 "%s(): keyfield = %s, entity = %s\n",
              __PRETTY_FUNCTION__, keyfield, entity);

  while ((var = va_arg(ap, const char *)) != NULL)
    {
      ast_verbose(VERBOSE_PREFIX_3 "%s\n", var);
    }

  ast_verbose(VERBOSE_PREFIX_3 "%s() returns\n", __PRETTY_FUNCTION__);

  return -1;
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

  RES_SQLITE_BEGIN
    error = sqlite_exec_printf(db, "SELECT COUNT(id) FROM %s;", NULL, NULL,
                               &errormsg, cdr_table);
  RES_SQLITE_END(error)

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

      RES_SQLITE_BEGIN
        error = sqlite_exec_printf(db, sql_create_cdr_table, NULL, NULL,
                &errormsg, cdr_table);
      RES_SQLITE_END(error)

      if (error)
        {
          ast_log(LOG_ERROR, RES_SQLITE_NAME ": %s\n", errormsg);
          free(errormsg);
          unload_module();
          return 1;
        }
    }

  ast_config_engine_register(&sqlite_engine);
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

  ast_config_engine_deregister(&sqlite_engine);

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
