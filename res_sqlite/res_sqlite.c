/*
 * Copyright (C) 2006 Richard Braun <rbraun@proformatique.com>
 * Resource module for SQLite 2
 * 
 * Based on res_sqlite3 by Anthony Minessale II, res_config_mysql by
 * by Matthew Boehm and app_addon_sql_mysql by Constantine Filin and
 * Christos Ricudis.
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

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite.h>
#include <asterisk/pbx.h>
#include <asterisk/cdr.h>
#include <asterisk/lock.h>
#include <asterisk/config.h>
#include <asterisk/logger.h>
#include <asterisk/module.h>
#include <asterisk/options.h>
#include <asterisk/linkedlists.h>

#define RES_SQLITE_NAME "res_sqlite"
#define RES_SQLITE_DRIVER "sqlite"
#define RES_SQLITE_APP_DRIVER "SQLITE"
#define RES_SQLITE_DESCRIPTION "Resource Module for SQLite 2"
#define RES_SQLITE_CONF_FILE "res_sqlite.conf"
#define RES_SQLITE_APP_SYNOPSIS "Dialplan access to SQLite 2"
#define RES_SQLITE_APP_DESCRIPTION \
"SQLITE(): " RES_SQLITE_APP_SYNOPSIS "\n"

#define RES_SQLITE_CONFIG_COLUMNS 6
#define RES_SQLITE_CONFIG_ID 0
#define RES_SQLITE_CONFIG_COMMENTED 1
#define RES_SQLITE_CONFIG_FILENAME 2
#define RES_SQLITE_CONFIG_CATEGORY 3
#define RES_SQLITE_CONFIG_VAR_NAME 4
#define RES_SQLITE_CONFIG_VAR_VAL 5

/*
 * Limit the number of maximum simultaneous registered SQLite VMs to avoid
 * a denial of service attack.
 */
#define RES_SQLITE_VM_MAX 1024

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

struct rt_multi_cfg_entry_args
{
  struct ast_config *cfg;
  char *initfield;
};

struct vm_entry
{
  int vmid;
  sqlite_vm *vm;
  AST_LIST_ENTRY(vm_entry) list;
};

static int set_var(char **var, char *name, char *value);
static int load_config(void);
static void unload_config(void);
static int check_vars(void);
static int cdr_handler(struct ast_cdr *cdr);
static int add_cfg_entry(void *arg, int argc, char **argv, char **columnNames);
static struct ast_config * config_handler(const char *database,
                                          const char *table, const char *file,
                                          struct ast_config *cfg);
static size_t get_params(va_list ap, const char ***params_ptr,
                         const char ***vals_ptr);
static int add_rt_cfg_entry(void *arg, int argc, char **argv,
                            char **columnNames);
static struct ast_variable * realtime_handler(const char *database,
                                              const char *table, va_list ap);
static int add_rt_multi_cfg_entry(void *arg, int argc, char **argv,
                                  char **columnNames);
static struct ast_config * realtime_multi_handler(const char *database,
                                                  const char *table,
                                                  va_list ap);
static int realtime_update_handler(const char *database, const char *table,
                                   const char *keyfield, const char *entity,
                                   va_list ap);
static sqlite_vm * app_alloc_vm(const char *query);
static int app_free_vm(sqlite_vm *vm);
static int app_register_vm(sqlite_vm *vm);
static int app_unregister_vm(int vmid);
static sqlite_vm * app_find_vm(int vmid);
static int app_set_vm(int vmid, sqlite_vm *vm);
static char * app_itoa(int i);
static int app_atoi(char *s, int *i);
static int app_query(struct ast_channel *chan, char *data);
static int app_fetch(struct ast_channel *chan, char *data);
static int app_clear(struct ast_channel *chan, char *data);
static int app_exec(struct ast_channel *chan, void *data_ptr);

static sqlite *db;
static int use_cdr;
static int use_app;
static int cdr_registered;
static int app_registered;
static char *dbfile;
static char *config_table;
static char *cdr_table;
static char *app_enable;
static int vm_count = 0;

static struct ast_config_engine sqlite_engine =
{
  .name = RES_SQLITE_DRIVER,
  .load_func = config_handler,
  .realtime_func = realtime_handler,
  .realtime_multi_func = realtime_multi_handler,
  .update_func = realtime_update_handler
};

AST_MUTEX_DEFINE_STATIC(mutex);
static AST_LIST_HEAD_STATIC(vm_list_head, vm_entry);
static struct vm_list_head *vm_list = &vm_list_head;

/*
 * Taken from Asterisk 1.2 cdr_sqlite.so.
 */

static char *sql_create_cdr_table =
"CREATE TABLE '%q' ("
"	id		INTEGER PRIMARY KEY,"
"	clid		VARCHAR(80) NOT NULL DEFAULT '',"
"	src		VARCHAR(80) NOT NULL DEFAULT '',"
"	dst		VARCHAR(80) NOT NULL DEFAULT '',"
"	dcontext	VARCHAR(80) NOT NULL DEFAULT '',"
"	channel		VARCHAR(80) NOT NULL DEFAULT '',"
"	dstchannel	VARCHAR(80) NOT NULL DEFAULT '',"
"	lastapp		VARCHAR(80) NOT NULL DEFAULT '',"
"	lastdata	VARCHAR(80) NOT NULL DEFAULT '',"
"	start		CHAR(19) NOT NULL DEFAULT '0000-00-00 00:00:00',"
"	answer		CHAR(19) NOT NULL DEFAULT '0000-00-00 00:00:00',"
"	end		CHAR(19) NOT NULL DEFAULT '0000-00-00 00:00:00',"
"	duration	INT(11) NOT NULL DEFAULT '0',"
"	billsec		INT(11) NOT NULL DEFAULT '0',"
"	disposition	INT(11) NOT NULL DEFAULT '0',"
"	amaflags	INT(11) NOT NULL DEFAULT '0',"
"	accountcode	VARCHAR(20) NOT NULL DEFAULT '',"
"	uniqueid	VARCHAR(32) NOT NULL DEFAULT '',"
"	userfield	VARCHAR(255) NOT NULL DEFAULT ''"
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
"	datetime(%d,'unixepoch'),"
"	datetime(%d,'unixepoch'),"
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
    {
      ast_log(LOG_ERROR, "Unable to load " RES_SQLITE_CONF_FILE "\n");
      return 1;
    }

  for (var = ast_variable_browse(config, "general");
       var != NULL;
       var = var->next)
    {
      if (strcasecmp(var->name, "dbfile") == 0)
        SET_VAR(config, dbfile, var)

      else if (strcasecmp(var->name, "config_table") == 0)
        SET_VAR(config, config_table, var)

      else if (strcasecmp(var->name, "cdr_table") == 0)
        SET_VAR(config, cdr_table, var)

      else if (strcasecmp(var->name, "app_enable") == 0)
        SET_VAR(config, app_enable, var)

      else
        ast_log(LOG_WARNING, "Unknown parameter : %s\n", var->name);
    }

  ast_config_destroy(config);
  error = check_vars();

  if (error)
    {
      unload_config();
      return 1;
    }

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
  free(app_enable);
  app_enable = NULL;
}

static int
check_vars(void)
{
  if (dbfile == NULL)
    {
      ast_log(LOG_ERROR, "Undefined parameter %s\n", dbfile);
      return 1;
    }

  use_cdr = (cdr_table != NULL);
  use_app = (app_enable != NULL && (strcasecmp(app_enable, "yes") == 0));
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
      ast_log(LOG_ERROR, "%s\n", errormsg);
      free(errormsg);
      return 1;
    }

  return 0;
}

/*
 * This callback relies on SQL entries being sorted by category.
 */
static int
add_cfg_entry(void *arg, int argc, char **argv, char **columnNames)
{
  struct cfg_entry_args *args;
  struct ast_variable *var;

  if (argc != RES_SQLITE_CONFIG_COLUMNS)
    {
      ast_log(LOG_WARNING, "Corrupt table\n");
      return 1;
    }

  args = (struct cfg_entry_args *)arg;

  if (args->cat_name == NULL
      || (strcmp(args->cat_name, argv[RES_SQLITE_CONFIG_CATEGORY]) != 0))
    {
      args->cat = ast_category_new(argv[RES_SQLITE_CONFIG_CATEGORY]);

      if (args->cat == NULL)
        {
          ast_log(LOG_ERROR, "Unable to allocate category\n");
          return 1;
        }

      free(args->cat_name);
      args->cat_name = strdup(argv[RES_SQLITE_CONFIG_CATEGORY]);

      if (args->cat_name == NULL)
        {
          ast_log(LOG_ERROR, "Unable to allocate category name\n");
          ast_category_destroy(args->cat);
          return 1;
        }

      ast_category_append(args->cfg, args->cat);
    }

  var = ast_variable_new(argv[RES_SQLITE_CONFIG_VAR_NAME],
                         argv[RES_SQLITE_CONFIG_VAR_VAL]);

  if (var == NULL)
    {
      ast_log(LOG_ERROR, "Unable to allocate variable");
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

  if (config_table == NULL)
    {
      if (table == NULL)
        {
          ast_log(LOG_ERROR, "Table name unspecified\n");
          return NULL;
        }
    }

  else
    table = config_table;

  args.cfg = cfg;
  args.cat = NULL;
  args.cat_name = NULL;

  ast_mutex_lock(&mutex);

  RES_SQLITE_BEGIN
    error = sqlite_exec_printf(db, sql_get_config_table, add_cfg_entry,
                               &args, &errormsg, table, file);
  RES_SQLITE_END(error)

  ast_mutex_unlock(&mutex);

  free(args.cat_name);

  if (error)
    {
      ast_log(LOG_ERROR, "%s\n", errormsg);
      free(errormsg);
      return NULL;
    }

  return cfg;
}

/*
 * Returns params_count, or 0 if an error occured (if ap doesn't contain
 * at least 2 elements, it's considered as an error). params_ptr and
 * vals_ptr are set if params_count is greater than 0.
 */
static size_t
get_params(va_list ap, const char ***params_ptr, const char ***vals_ptr)
{
  const char **tmp, *param, *val, **params, **vals;
  size_t params_count;

  params = NULL;
  vals = NULL;
  params_count = 0;

  while ((param = va_arg(ap, const char *)) != NULL
         && (val = va_arg(ap, const char *)) != NULL)
    {
      tmp = realloc(params, (params_count + 1) * sizeof(char *));

      if (tmp == NULL)
        {
          ast_log(LOG_WARNING, "Unable to allocate params\n");
          free(params);
          free(vals);
          return 0;
        }

      params = tmp;
      tmp = realloc(vals, (params_count + 1) * sizeof(char *));

      if (tmp == NULL)
        {
          ast_log(LOG_WARNING, "Unable to allocate vars\n");
          free(params);
          free(vals);
          return 0;
        }

      vals = tmp;
      params[params_count] = param;
      vals[params_count] = val;
      params_count++;
    }

  if (params_count == 0)
    ast_log(LOG_WARNING, "1 parameter and 1 value at least required\n");

  else
    {
      *params_ptr = params;
      *vals_ptr = vals;
    }

  return params_count;
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
          ast_log(LOG_WARNING, "Unable to allocate variable\n");
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
  char *query, *errormsg, *op, *tmp_str;
  struct rt_cfg_entry_args args;
  const char **params, **vals;
  size_t params_count;
  int error;

  if (table == NULL)
    {
      ast_log(LOG_WARNING, "Table name unspecified\n");
      return NULL;
    }

  params_count = get_params(ap, &params, &vals);

  if (params_count == 0)
    return NULL;

  op = (strchr(params[0], ' ') == NULL) ? " =" : "";

#undef QUERY
#define QUERY "SELECT * FROM '%q' WHERE commented = 0 AND %q%s '%q'"
  query = sqlite_mprintf(QUERY, table, params[0], op, vals[0]);

  if (query == NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate SQL query\n");
      free(params);
      free(vals);
      return NULL;
    }

  if (params_count > 1)
    {
      size_t i;

      for (i = 1; i < params_count; i++)
        {
          op = (strchr(params[i], ' ') == NULL) ? " =" : "";
          tmp_str = sqlite_mprintf("%s AND %q%s '%q'", query, params[i], op,
                                   vals[i]);
          sqlite_freemem(query);

          if (tmp_str == NULL)
            {
              ast_log(LOG_WARNING, "Unable to reallocate SQL query\n");
              free(params);
              free(vals);
              return NULL;
            }

          query = tmp_str;
        }
    }

  free(params);
  free(vals);

  tmp_str = sqlite_mprintf("%s LIMIT 1;", query);
  sqlite_freemem(query);

  if (tmp_str == NULL)
    {
      ast_log(LOG_WARNING, "Unable to reallocate SQL query\n");
      return NULL;
    }

  query = tmp_str;
  ast_log(LOG_DEBUG, "SQL query: %s\n", query);
  args.var = NULL;
  args.last = NULL;

  ast_mutex_lock(&mutex);

  RES_SQLITE_BEGIN
    error = sqlite_exec(db, query, add_rt_cfg_entry, &args, &errormsg);
  RES_SQLITE_END(error)

  ast_mutex_unlock(&mutex);

  sqlite_freemem(query);

  if (error)
    {
      ast_log(LOG_WARNING, "%s\n", errormsg);
      free(errormsg);
      ast_variables_destroy(args.var);
      return NULL;
    }

  return args.var;
}

static int
add_rt_multi_cfg_entry(void *arg, int argc, char **argv, char **columnNames)
{
  struct rt_multi_cfg_entry_args *args;
  struct ast_category *cat;
  struct ast_variable *var;
  char *cat_name;
  size_t i;

  args = (struct rt_multi_cfg_entry_args *)arg;

  /*
   * cat_name should always be set here, since initfield is forged from
   * params[0] in realtime_multi_handler(), which is a search parameter
   * of the SQL query.
   */
  for (i = 0; i < argc; i++)
    if (strcmp(args->initfield, columnNames[i]) == 0)
      cat_name = argv[i];

  cat = ast_category_new(cat_name);

  if (cat == NULL)
    {
      ast_log(LOG_ERROR, "Unable to allocate category\n");
      return 1;
    }

  ast_category_append(args->cfg, cat);

  for (i = 0; i < argc; i++)
    {
      if (argv[i] == NULL || (strcmp(args->initfield, columnNames[i]) == 0))
        continue;

      var = ast_variable_new(columnNames[i], argv[i]);

      if (var == NULL)
        {
          ast_log(LOG_WARNING, "Unable to allocate variable\n");
          return 1;
        }

      ast_variable_append(cat, var);
    }

  return 0;
}

static struct ast_config *
realtime_multi_handler(const char *database, const char *table, va_list ap)
{
  char *query, *errormsg, *op, *tmp_str, *initfield;
  struct rt_multi_cfg_entry_args args;
  const char **params, **vals;
  struct ast_config *cfg;
  size_t params_count;
  int error;

  if (table == NULL)
    {
      ast_log(LOG_WARNING, "Table name unspecified\n");
      return NULL;
    }

  cfg = ast_config_new();

  if (cfg == NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate configuration structure\n");
      return NULL;
    }

  params_count = get_params(ap, &params, &vals);

  if (params_count == 0)
    {
      ast_config_destroy(cfg);
      return NULL;
    }

  initfield = strdup(params[0]);

  if (initfield == NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate initfield\n");
      ast_config_destroy(cfg);
      free(params);
      free(vals);
      return NULL;
    }

  tmp_str = strchr(initfield, ' ');

  if (tmp_str != NULL)
    *tmp_str = '\0';

  op = (strchr(params[0], ' ') == NULL) ? " =" : "";

  /*
   * Asterisk sends us an already escaped string when searching for
   * "exten LIKE" (uh!). Handle it separately.
   */
  tmp_str = (strcmp(vals[0], "\\_%") == 0) ? "_%" : (char *)vals[0];

#undef QUERY
#define QUERY "SELECT * FROM '%q' WHERE commented = 0 AND %q%s '%q'"
  query = sqlite_mprintf(QUERY, table, params[0], op, tmp_str);

  if (query == NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate SQL query\n");
      ast_config_destroy(cfg);
      free(params);
      free(vals);
      free(initfield);
      return NULL;
    }

  if (params_count > 1)
    {
      size_t i;

      for (i = 1; i < params_count; i++)
        {
          op = (strchr(params[i], ' ') == NULL) ? " =" : "";
          tmp_str = sqlite_mprintf("%s AND %q%s '%q'", query, params[i], op,
                                   vals[i]);
          sqlite_freemem(query);

          if (tmp_str == NULL)
            {
              ast_log(LOG_WARNING, "Unable to reallocate SQL query\n");
              ast_config_destroy(cfg);
              free(params);
              free(vals);
              free(initfield);
              return NULL;
            }

          query = tmp_str;
        }
    }

  free(params);
  free(vals);

  tmp_str = sqlite_mprintf("%s ORDER BY %q;", query, initfield);
  sqlite_freemem(query);

  if (tmp_str == NULL)
    {
      ast_log(LOG_WARNING, "Unable to reallocate SQL query\n");
      ast_config_destroy(cfg);
      free(initfield);
      return NULL;
    }

  query = tmp_str;
  ast_log(LOG_DEBUG, "SQL query: %s\n", query);
  args.cfg = cfg;
  args.initfield = initfield;

  ast_mutex_lock(&mutex);

  RES_SQLITE_BEGIN
    error = sqlite_exec(db, query, add_rt_multi_cfg_entry, &args, &errormsg);
  RES_SQLITE_END(error)

  ast_mutex_unlock(&mutex);

  sqlite_freemem(query);
  free(initfield);

  if (error)
    {
      ast_log(LOG_WARNING, "%s\n", errormsg);
      free(errormsg);
      ast_config_destroy(cfg);
      return NULL;
    }

  return cfg;
}

static int
realtime_update_handler(const char *database, const char *table,
                        const char *keyfield, const char *entity,
                        va_list ap)
{
  char *query, *errormsg, *tmp_str;
  const char **params, **vals;
  size_t params_count;
  int error, rows_num;

  if (table == NULL)
    {
      ast_log(LOG_WARNING, "Table name unspecified\n");
      return -1;
    }

  params_count = get_params(ap, &params, &vals);

  if (params_count == 0)
    return -1;

#undef QUERY
#define QUERY "UPDATE '%q' SET %q = '%q'"
  query = sqlite_mprintf(QUERY, table, params[0], vals[0]);

  if (query == NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate SQL query\n");
      free(params);
      free(vals);
      return -1;
    }

  if (params_count > 1)
    {
      size_t i;

      for (i = 1; i < params_count; i++)
        {
          tmp_str = sqlite_mprintf("%s, %q = '%q'", query, params[i],
                                   vals[i]);
          sqlite_freemem(query);

          if (tmp_str == NULL)
            {
              ast_log(LOG_WARNING, "Unable to reallocate SQL query\n");
              free(params);
              free(vals);
              return -1;
            }

          query = tmp_str;
        }
    }

  free(params);
  free(vals);

  tmp_str = sqlite_mprintf("%s WHERE %q = '%q';", query, keyfield, entity);
  sqlite_freemem(query);

  if (tmp_str == NULL)
    {
      ast_log(LOG_WARNING, "Unable to reallocate SQL query\n");
      return -1;
    }

  query = tmp_str;
  ast_log(LOG_DEBUG, "SQL query: %s\n", query);

  ast_mutex_lock(&mutex);

  RES_SQLITE_BEGIN
    error = sqlite_exec(db, query, NULL, NULL, &errormsg);
  RES_SQLITE_END(error)

  if (!error)
    rows_num = sqlite_changes(db);

  ast_mutex_unlock(&mutex);

  sqlite_freemem(query);

  if (error)
    {
      ast_log(LOG_WARNING, "%s\n", errormsg);
      free(errormsg);
      return -1;
    }

  return rows_num;
}

static sqlite_vm *
app_alloc_vm(const char *query)
{
  char *errormsg;
  sqlite_vm *vm;
  int error;

  ast_mutex_lock(&mutex);

  /*
   * XXX Can sqlite_compile() fail because of locking ?
   */
  RES_SQLITE_BEGIN
    error = sqlite_compile(db, query, NULL, &vm, &errormsg);
  RES_SQLITE_END(error)

  ast_mutex_unlock(&mutex);

  if (error)
    {
      ast_log(LOG_WARNING, "%s\n", errormsg);
      sqlite_freemem(errormsg);
      return NULL;
    }

  return vm;
}

static int
app_free_vm(sqlite_vm *vm)
{
  char *errormsg;
  int error;

  error = sqlite_finalize(vm, &errormsg);

  if (error)
    {
      ast_log(LOG_WARNING, "%s\n", errormsg);
      sqlite_freemem(errormsg);
      return -1;
    }

  return 0;
}

/*
 * Register a SQLite VM and associate it to an index. This index is the
 * value returned in the VMID variable. Return -1 if an error occurred,
 * the index associated to this VM otherwise.
 */
static int
app_register_vm(sqlite_vm *vm)
{
  struct vm_entry *entry, *i, *next;
  int vmid;

  entry = malloc(sizeof(struct vm_entry));

  if (entry == NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate VM entry\n");
      return -1;
    }

  AST_LIST_LOCK(vm_list);

  vm_count++;

  if (vm_count >= RES_SQLITE_VM_MAX)
    {
      vm_count--;
      AST_LIST_UNLOCK(vm_list);
      free(entry);
      ast_log(LOG_WARNING, "Maximum number of simultaneous SQLite VMs "
                           "reached, can't create new VM\n");
      return -1;
    }

  if (AST_LIST_EMPTY(vm_list))
    {
      vmid = 0;
      AST_LIST_INSERT_HEAD(vm_list, entry, list);
    }

  else
    {
      AST_LIST_TRAVERSE(vm_list, i, list)
        {
          next = AST_LIST_NEXT(i, list);

          if (next == NULL || ((next->vmid - i->vmid) > 1))
            break;
        }

      vmid = i->vmid + 1;
      AST_LIST_INSERT_AFTER(vm_list, i, entry, list);
    }

  entry->vmid = vmid;
  entry->vm = vm;

  AST_LIST_UNLOCK(vm_list);

  return vmid;
}

static int
app_unregister_vm(int vmid)
{
  struct vm_entry *i;
  int found;

  found = 0;

  AST_LIST_LOCK(vm_list);

  AST_LIST_TRAVERSE(vm_list, i, list)
    if (i->vmid == vmid)
      {
        found = 1;
        break;
      }

  if (!found)
    {
      AST_LIST_UNLOCK(vm_list);
      ast_log(LOG_WARNING, "VMID %d not found in VMs list\n", vmid);
      return -1;
    }

  AST_LIST_REMOVE(vm_list, i, list);
  vm_count--;

  AST_LIST_UNLOCK(vm_list);

  free(i);
  return 0;
}

static sqlite_vm *
app_find_vm(int vmid)
{
  struct vm_entry *i;
  sqlite_vm *vm;

  vm = NULL;

  AST_LIST_LOCK(vm_list);

  AST_LIST_TRAVERSE(vm_list, i, list)
    if (i->vmid == vmid)
      {
        vm = i->vm;
        break;
      }

  AST_LIST_UNLOCK(vm_list);

  if (vm == NULL)
    {
      ast_log(LOG_WARNING, "VMID %d not found in VMs list\n", vmid);
      return NULL;
    }

  return vm;
}

static int
app_set_vm(int vmid, sqlite_vm *vm)
{
  struct vm_entry *i;
  int found;

  found = 0;

  AST_LIST_LOCK(vm_list);

  AST_LIST_TRAVERSE(vm_list, i, list)
    if (i->vmid == vmid)
      {
        i->vm = vm;
        found = 1;
        break;
      }

  AST_LIST_UNLOCK(vm_list);

  if (!found)
    {
      ast_log(LOG_WARNING, "VMID %d not found in VMs list\n", vmid);
      return -1;
    }

  return 0;
}

static char *
app_itoa(int i)
{
  int size;
  char *s;

  size = snprintf(NULL, 0, "%d", i);

  s = malloc((size + 1) * sizeof(char));

  if (s == NULL)
    return NULL;

  sprintf(s, "%d", i);
  return s;
}

static int
app_atoi(char *s, int *i)
{
  char *endptr;

  if (s == NULL || *s == '\0')
    return -1;

  *i = strtol(s, &endptr, 10);

  if (*endptr != '\0')
    return -1;

  return 0;
}

static int
app_query(struct ast_channel *chan, char *data)
{
  char *vmid_var, *vmid_val;
  sqlite_vm *vm;
  int vmid;

  if (data == NULL)
    {
      ast_log(LOG_WARNING, "No VMID variable name\n");
      return -1;
    }

  vmid_var = strsep(&data, " ");

  if (data == NULL)
    {
      ast_log(LOG_WARNING, "No SQL query\n");
      return -1;
    }

  vm = app_alloc_vm(data);

  if (vm == NULL)
    return -1;

  vmid = app_register_vm(vm);

  if (vmid == -1)
    {
      app_free_vm(vm);
      return -1;
    }

  vmid_val = app_itoa(vmid);

  if (vmid_val == NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate VMID\n");
      app_unregister_vm(vmid);
      app_free_vm(vm);
      return -1;
    }

  pbx_builtin_setvar_helper(chan, vmid_var, vmid_val);
  free(vmid_val);
  return 0;
}

static int
app_fetch(struct ast_channel *chan, char *data)
{
  char *errormsg, *fetchid, *vmid_str, *var;
  int i, error, vmid, cols_count;
  const char **cols, **values;
  sqlite_vm *vm;

  if (data == NULL)
    {
      ast_log(LOG_WARNING, "No fetch ID\n");
      return -1;
    }

  fetchid = strsep(&data, " ");

  if (data == NULL)
    {
      ast_log(LOG_WARNING, "No VMID\n");
      return -1;
    }

  vmid_str = strsep(&data, " ");

  if (data == NULL)
    {
      ast_log(LOG_WARNING, "Fetch CMD requires at least one variable name\n");
      return -1;
    }

  error = app_atoi(vmid_str, &vmid);

  if (error)
    {
      ast_log(LOG_WARNING, "Unable to convert VMID %s to an integer\n",
              vmid_str);
      return -1;
    }

  vm = app_find_vm(vmid);

  if (vm == NULL)
    return -1;

  ast_mutex_lock(&mutex);

  RES_SQLITE_BEGIN
    error = sqlite_step(vm, &cols_count, &values, &cols);
  RES_SQLITE_END(error)

  ast_mutex_unlock(&mutex);

  if (error != SQLITE_ROW)
    {
      error = app_set_vm(vmid, NULL);

      /*
       * Quick and dirty error handling since app_set_vm() is very unlikely
       * to fail, as app_find_vm() returned a valid pointer.
       */
      if (error)
        return -1;

      error = app_free_vm(vm);

      if (error)
        return -1;

      if (error != SQLITE_DONE)
        {
          ast_log(LOG_WARNING, "%s\n", errormsg);
          sqlite_freemem(errormsg);
          return -1;
        }

      else
        {
          pbx_builtin_setvar_helper(chan, fetchid, "0");
          return 0;
        }
    }

  for (i = 0; i < cols_count; i++)
    {
      var = strsep(&data, " ");

      if (var == NULL)
        {
          ast_log(LOG_WARNING, "More fields than variables\n");
          break;
        }

      pbx_builtin_setvar_helper(chan, var,
                                (values[i] != NULL) ? values[i] : "NULL");
    }

  pbx_builtin_setvar_helper(chan, fetchid, "1");
  return 0;
}

static int
app_clear(struct ast_channel *chan, char *data)
{
  int error, vmid;
  char *vmid_str;
  sqlite_vm *vm;

  if (data == NULL)
    {
      ast_log(LOG_WARNING, "No VMID\n");
      return -1;
    }

  vmid_str = strsep(&data, " ");
  error = app_atoi(vmid_str, &vmid);

  if (error)
    {
      ast_log(LOG_WARNING, "Unable to convert VMID %s to an integer\n",
              vmid_str);
      return -1;
    }

  vm = app_find_vm(vmid);

  if (vm == NULL)
    return -1;

  error = app_unregister_vm(vmid);

  if (error)
    return -1;

  error = app_free_vm(vm);

  if (error)
    return -1;

  return 0;
}

static int
app_exec(struct ast_channel *chan, void *data_ptr)
{
  int (*cmd_func)(struct ast_channel *, char *);
  char *data, *cmd;
  int error;

  if (data_ptr == NULL)
    {
      ast_log(LOG_WARNING, RES_SQLITE_APP_DRIVER "() requires an argument\n");
      return -1;
    }

  data = strdup(data_ptr);

  if (data == NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate copy of argument\n");
      return -1;
    }

  /*
   * data is going to be modified (both address and content). Save the
   * original address for deallocation.
   */
  data_ptr = data;
  cmd = strsep(&data, " ");

  if (strcasecmp(cmd, "query") == 0)
    cmd_func = app_query;

  else if (strcasecmp(cmd, "fetch") == 0)
    cmd_func = app_fetch;

  else if (strcasecmp(cmd, "clear") == 0)
    cmd_func = app_clear;

  error = cmd_func(chan, data);
  free(data_ptr);
  return error;
}

int
load_module(void)
{
  char *errormsg;
  int error;

  cdr_registered = 0;
  app_registered = 0;
  error = load_config();

  if (error)
    return 1;

  db = sqlite_open(dbfile, 0660, &errormsg);

  if (db == NULL)
    {
      ast_log(LOG_ERROR, "%s\n", errormsg);
      free(errormsg);
      unload_module();
      return 1;
    }

  ast_config_engine_register(&sqlite_engine);

  if (use_cdr)
    {
      RES_SQLITE_BEGIN
        error = sqlite_exec_printf(db, "SELECT COUNT(id) FROM %Q;", NULL, NULL,
                                   &errormsg, cdr_table);
      RES_SQLITE_END(error)

      if (error)
        {
          /*
           * Unexpected error.
           */
          if (error != SQLITE_ERROR)
            {
              ast_log(LOG_ERROR, "%s\n", errormsg);
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
              ast_log(LOG_ERROR, "%s\n", errormsg);
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
    }

  if (use_app)
    {
      error = ast_register_application(RES_SQLITE_APP_DRIVER, app_exec,
                                       RES_SQLITE_APP_SYNOPSIS,
                                       RES_SQLITE_APP_DESCRIPTION);

      if (error)
        {
          unload_module();
          return 1;
        }

      app_registered = 1;
    }

  return 0;
}

int
unload_module(void)
{
  if (app_registered)
    ast_unregister_application(RES_SQLITE_APP_DRIVER);

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
