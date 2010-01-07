/*
 * Copyright (C) 2006-2010 Proformatique <technique@proformatique.com>
 * Written by Richard Braun <rbraun@proformatique.com>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \mainpage res_config_sqlite
 *
 * \section intro_sec Presentation
 *
 * res_config_sqlite is a module for the Asterisk Open Source PBX to support SQLite 2
 * databases. It can be used to fetch configuration from a database (static
 * configuration files and/or using the Asterisk RealTime Architecture - ARA).
 * It can also be used to log CDR entries. Finally, it can be used for simple
 * queries in the Dialplan. Note that Asterisk already comes with a module
 * named cdr_sqlite. There are two reasons for including it in res_config_sqlite:
 * the first is that rewriting it was a training to learn how to write a
 * simple module for Asterisk, the other is to have the same database open for
 * all kinds of operations, which improves reliability and performance.
 *
 * There is already a module for SQLite 3 (named res_sqlite3) in the Asterisk
 * addons. res_config_sqlite was developed because we, at Proformatique, are using
 * PHP 4 in our embedded systems, and PHP 4 has no stable support for SQLite 3
 * at this time. We also needed RealTime support.
 *
 * \section build_install_sec Building and installing
 *
 * To build res_config_sqlite, simply enter <code>make</code>. To install it,
 * enter make install. The Makefile has been slightly designed for
 * cross compilation and installation in non standard locations, to ease
 * the work of packagers. Read it for more details.
 *
 * \section conf_sec Configuration
 *
 * The main configuration file is res_sqlite.conf. It must be readable or
 * res_config_sqlite will fail to start. It is suggested to use the sample file
 * in this package as a starting point. The file has only one section
 * named <code>general</code>. Here are the supported parameters :
 *
 * <dl>
 *  <dt><code>dbfile</code></dt>
 *  <dd>The absolute path to the SQLite database (the file can be non existent,
 *      res_config_sqlite will create it if is has the appropriate rights)</dd>
 *  <dt><code>config_table</code></dt>
 *  <dd>The table used for static configuration</dd>
 *  <dt><code>cdr_table</code></dt>
 *  <dd>The table used to store CDR entries (if ommitted, CDR support is
 *      disabled)</dd>
 *  <dt><code>app_enable</code></dt>
 *  <dd>If set to <code>yes</code>, the SQLITE() application will be usable in
 *      the Dialplan</dd>
 * </dl>
 *
 * To use res_config_sqlite for static and/or RealTime configuration, refer to the
 * Asterisk documentation. The file tables.sql can be used to create the
 * needed tables.
 *
 * The SQLITE() application is very similar to the MYSQL() application. You
 * can find more details at
 * <a href="http://voip-info.org/wiki/view/Asterisk+cmd+MYSQL">http://voip-info.org/wiki/view/Asterisk+cmd+MYSQL</a>.
 * The main difference is that you cannot choose your database - it's the
 * file set in the <code>dbfile</code> parameter. As a result, there is no
 * Connect or Disconnect command, and there is no connid variable.
 *
 * \section status_sec Driver status
 *
 * The CLI command <code>show sqlite status</code> returns status information
 * about the running driver. One information is more important than others:
 * the number of registered virtual machines. A SQLite virtual machine is
 * created each time a SQLITE() query command is used. If the number of
 * registered virtual machines isn't 0 (or near 0, since one or more SQLITE()
 * commands can be running when requesting the module status) and increases
 * over time, this probably means that you're badly using the application
 * and you're creating resource leaks. You should check your Dialplan and
 * reload res_config_sqlite (by unloading and then loading again - reloading isn't
 * supported)
 *
 * \section credits_sec Credits
 *
 * res_config_sqlite was developed by Richard Braun at the Proformatique company.
 */

/**
 * \file res_config_sqlite.c
 * \brief res_config_sqlite module.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite.h>
#include <asterisk/pbx.h>
#include <asterisk/cdr.h>
#include <asterisk/cli.h>
#include <asterisk/lock.h>
#include <asterisk/config.h>
#include <asterisk/logger.h>
#include <asterisk/module.h>
#include <asterisk/options.h>
#include <asterisk/linkedlists.h>

#define __unused __attribute__((unused))

#define RES_CONFIG_SQLITE_NAME "res_config_sqlite"
#define RES_CONFIG_SQLITE_DRIVER "sqlite"
#define RES_CONFIG_SQLITE_APP_DRIVER "SQLITE"
#define RES_CONFIG_SQLITE_DESCRIPTION "Resource Module for SQLite 2"
#define RES_CONFIG_SQLITE_CONF_FILE "res_sqlite.conf"
#define RES_CONFIG_SQLITE_APP_SYNOPSIS "Dialplan access to SQLite 2"
#define RES_CONFIG_SQLITE_APP_DESCRIPTION \
"SQLITE(): " RES_CONFIG_SQLITE_APP_SYNOPSIS "\n"
#define RES_CONFIG_SQLITE_STATUS_SUMMARY \
"Show status information about the SQLite 2 driver"
#define RES_CONFIG_SQLITE_STATUS_USAGE \
"Usage: show sqlite status\n" \
"	" RES_CONFIG_SQLITE_STATUS_SUMMARY "\n"

#define RES_CONFIG_SQLITE_CONFIG_COLUMNS 8
#define RES_CONFIG_SQLITE_CONFIG_ID 0
#define RES_CONFIG_SQLITE_CONFIG_CAT_METRIC 1
#define RES_CONFIG_SQLITE_CONFIG_VAR_METRIC 2
#define RES_CONFIG_SQLITE_CONFIG_COMMENTED 3
#define RES_CONFIG_SQLITE_CONFIG_FILENAME 4
#define RES_CONFIG_SQLITE_CONFIG_CATEGORY 5
#define RES_CONFIG_SQLITE_CONFIG_VAR_NAME 6
#define RES_CONFIG_SQLITE_CONFIG_VAR_VAL 7

#define MACRO_BEGIN	do {
#define MACRO_END	} while (0)

/**
 * Limit the number of maximum simultaneous registered SQLite VMs to avoid
 * a denial of service attack.
 */
#define RES_CONFIG_SQLITE_VM_MAX 1024

#define SET_VAR(config, to, from)		\
MACRO_BEGIN					\
  int __error;					\
						\
  __error = set_var(&to, #to, from->value);	\
						\
  if (__error)					\
    {						\
      ast_config_destroy(config);		\
      unload_config();				\
      return 1;					\
    }						\
MACRO_END

/**
 * Maximum number of loops before giving up executing a query. Calls to
 * sqlite_xxx() functions which can return SQLITE_BUSY or SQLITE_LOCKED
 * are enclosed by RES_CONFIG_SQLITE_BEGIN and RES_CONFIG_SQLITE_END, e.g.
 * <pre>
 * char *errormsg;
 * int error;
 *
 * RES_CONFIG_SQLITE_BEGIN
 *   error = sqlite_exec(db, query, NULL, NULL, &errormsg);
 * RES_CONFIG_SQLITE_END(error)
 *
 * if (error)
 *   ...;
 * </pre>
 */
#define RES_CONFIG_SQLITE_MAX_LOOPS 10

/**
 * Macro used before executing a query.
 *
 * @see RES_CONFIG_SQLITE_MAX_LOOPS.
 */
#define RES_CONFIG_SQLITE_BEGIN					\
MACRO_BEGIN							\
  int __i;							\
								\
  for (__i = 0; __i < RES_CONFIG_SQLITE_MAX_LOOPS; __i++)		\
    {

/**
 * Macro used after executing a query.
 *
 * @see RES_CONFIG_SQLITE_MAX_LOOPS.
 */
#define RES_CONFIG_SQLITE_END(error)					\
      if ((error != SQLITE_BUSY) && (error != SQLITE_LOCKED))	\
        break;							\
      usleep(1000);						\
    }								\
MACRO_END;

/**
 * Structure sent to the SQLite callback function for static configuration.
 *
 * @see add_cfg_entry()
 */
struct cfg_entry_args
{
  struct ast_config *cfg;
  struct ast_category *cat;
  char *cat_name;
};

/**
 * Structure sent to the SQLite callback function for RealTime configuration.
 *
 * @see add_rt_cfg_entry()
 */
struct rt_cfg_entry_args
{
  struct ast_variable *var;
  struct ast_variable *last;
};

/**
 * Structure sent to the SQLite callback function for RealTime configuration
 * (realtime_multi_handler()).
 *
 * @see add_rt_multi_cfg_entry()
 */
struct rt_multi_cfg_entry_args
{
  struct ast_config *cfg;
  char *initfield;
};

/**
 * Entry in the linked list of registered SQLite virtual machines.
 *
 * @see app_alloc_vm()
 * @see app_free_vm()
 * @see app_register_vm()
 * @see app_unregister_vm()
 * @see app_find_vm()
 * @see app_set_vm()
 */
struct vm_entry
{
  int vmid;
  sqlite_vm *vm;
  AST_LIST_ENTRY(vm_entry) list;
};

/**
 * Prototypes required for the asterisk-1.4 compilation.
 *
 */
int load_module(void);
int unload_module(void);
char * description(void);
int usecount(void);
char * key(void);

/**
 * Allocate a variable.
 *
 * @param var   the address of the variable to set (it will be allocated)
 * @param name  the name of the variable (for error handling)
 * @param value the value to store in var
 * @return 1 if an allocation error occurred, 0 otherwise
 */
static int set_var(char **var, const char *name, char *value);

/**
 * Load the configuration file.
 *
 * This function sets dbfile, config_table, cdr_table and app_enable. It calls
 * check_vars() before returning, and unload_config() if an error occurred.
 *
 * @return 1 if an error occurred, 0 otherwise
 * @see unload_config()
 */
static int load_config(void);

/**
 * Free resources related to configuration.
 *
 * @see load_config()
 */
static void unload_config(void);

/**
 * Check that required parameters have been set in the configuration file,
 * and set use_cdr and use_app to enable/disable CDR/APP support.
 *
 * @return 1 if a required parameter was not set, 0 otherwise
 */
static int check_vars(void);

/**
 * Asterisk callback function for CDR support.
 *
 * Asterisk will call this function each time a CDR entry must be logged if
 * CDR support is enabled.
 *
 * @param cdr the CDR entry Asterisk sends us
 * @return 1 if an error occurred, 0 otherwise
 */
static int cdr_handler(struct ast_cdr *cdr);

/**
 * SQLite callback function for static configuration.
 *
 * This function is passed to the SQLite engine as a callback function to
 * parse a row and store it in a struct ast_config object. It relies on
 * resulting rows being sorted by category.
 *
 * @param arg         a pointer to a struct cfg_entry_args object
 * @param argc        number of columns
 * @param argv        values in the row
 * @param columnNames names and types of the columns
 * @return 1 if an error occurred, 0 otherwise
 * @see cfg_entry_args
 * @see sql_get_config_table
 * @see config_handler()
 */
static int add_cfg_entry(void *arg, int argc, char **argv, char **columnNames);

/**
 * Asterisk callback function for static configuration.
 *
 * Asterisk will call this function when it loads its static configuration,
 * which usually happens at startup and reload.
 *
 * @param database the database to use (ignored)
 * @param table    the table to use
 * @param file     the file to load from the database
 * @param cfg      the struct ast_config object to use when storing variables
 * @return NULL if an error occurred, cfg otherwise
 * @see add_cfg_entry()
 */
static struct ast_config * config_handler(const char *database,
                                          const char *table, const char *file,
                                          struct ast_config *cfg,
                                          int withcomments);

/**
 * Helper function to parse a va_list object into 2 dynamic arrays of
 * strings, parameters and values.
 *
 * ap must have the following format : param1 val1 param2 val2 param3 val3 ...
 * arguments will be extracted to create 2 arrays:
 *
 * <ul>
 *  <li>params : param1 param2 param3 ...</li>
 *  <li>vals : val1 val2 val3 ...</li>
 * </ul>
 *
 * The address of these arrays are stored in params_ptr and vals_ptr. It
 * is the responsibility of the caller to release the memory of these arrays.
 * It is considered an error that va_list has a null or odd number of strings.
 *
 * @param ap         the va_list object to parse
 * @param params_ptr where the address of the params array is stored
 * @param vals_ptr   where the address of the vals array is stored
 * @return 0 if an error occurred, the number of elements in the arrays (which
 *         have the same size) otherwise
 */
static size_t get_params(va_list ap, const char ***params_ptr,
                         const char ***vals_ptr);

/**
 * SQLite callback function for RealTime configuration.
 *
 * This function is passed to the SQLite engine as a callback function to
 * parse a row and store it in a linked list of struct ast_variable objects.
 *
 * @param arg         a pointer to a struct rt_cfg_entry_args object
 * @param argc        number of columns
 * @param argv        values in the row
 * @param columnNames names and types of the columns
 * @return 1 if an error occurred, 0 otherwise
 * @see rt_cfg_entry_args
 * @see realtime_handler()
 */
static int add_rt_cfg_entry(void *arg, int argc, char **argv,
                            char **columnNames);

/**
 * Asterisk callback function for RealTime configuration.
 *
 * Asterisk will call this function each time it requires a variable
 * through the RealTime architecture. ap is a list of parameters and
 * values used to find a specific row, e.g one parameter "name" and
 * one value "123" so that the SQL query becomes <code>SELECT * FROM
 * table WHERE name = '123';</code>.
 *
 * @param database the database to use (ignored)
 * @param table    the table to use
 * @param ap       list of parameters and values to match
 * @return NULL if an error occurred, a linked list of struct ast_variable
 *         objects otherwise
 * @see add_rt_cfg_entry()
 */
static struct ast_variable * realtime_handler(const char *database,
                                              const char *table, va_list ap);

/**
 * SQLite callback function for RealTime configuration.
 *
 * This function performs the same actions as add_rt_cfg_entry() except
 * that the rt_multi_cfg_entry_args structure is designed to store
 * categories in addition of variables.
 *
 * @param arg         a pointer to a struct rt_multi_cfg_entry_args object
 * @param argc        number of columns
 * @param argv        values in the row
 * @param columnNames names and types of the columns
 * @return 1 if an error occurred, 0 otherwise
 * @see rt_multi_cfg_entry_args
 * @see realtime_multi_handler()
 */
static int add_rt_multi_cfg_entry(void *arg, int argc, char **argv,
                                  char **columnNames);

/**
 * Asterisk callback function for RealTime configuration.
 *
 * This function performs the same actions as realtime_handler() except
 * that it can store variables per category, and can return several
 * categories.
 *
 * @param database the database to use (ignored)
 * @param table    the table to use
 * @param ap       list of parameters and values to match
 * @return NULL if an error occurred, a struct ast_config object storing
 *         categories and variables
 * @see add_rt_multi_cfg_entry()
 */
static struct ast_config * realtime_multi_handler(const char *database,
                                                  const char *table,
                                                  va_list ap);

/**
 * Asterisk callback function for RealTime configuration (variable
 * update).
 *
 * Asterisk will call this function each time a variable has been modified
 * internally and must be updated in the backend engine. keyfield and entity
 * are used to find the row to update, e.g. <code>UPDATE table SET ... WHERE
 * keyfield = 'entity';</code>. ap is a list of parameters and values with the
 * same format as the other realtime functions.
 *
 * @param database the database to use (ignored)
 * @param table    the table to use
 * @param keyfield the column of the matching cell
 * @param entity   the value of the matching cell
 * @param ap       list of parameters and new values to update in the database
 * @return -1 if an error occurred, the number of affected rows otherwise
 */
static int realtime_update_handler(const char *database, const char *table,
                                   const char *keyfield, const char *entity,
                                   va_list ap);

/**
 * Compile a SQL query into a SQLite virtual machine.
 *
 * @param query the query to compile
 * @return NULL if an error occurred, the virtual machine executing the query
 *         otherwise
 * @see vm_entry
 */
static sqlite_vm * app_alloc_vm(const char *query);

/**
 * Finalize a SQLite virtual machine to release it resources.
 *
 * @param vm the virtual machine
 * @return 1 if an error occurred, 0 otherwise
 * @see vm_entry
 */
static int app_free_vm(sqlite_vm *vm);

/**
 * Insert a virtual machine in the linked list of registered virtual machines.
 *
 * This function is also responsible of finding an unused VMID.
 *
 * @param vm the virtual machine to register
 * @return 1 if an error occurred, the VMID associated to the given VM
 *         otherwise
 * @see vm_entry
 */
static int app_register_vm(sqlite_vm *vm);

/**
 * Remove a virtual machine from the linked list of registered virtual
 * machines.
 *
 * @param vmid the VMID of the virtual machine to remove
 * @return 1 if an error occurred, 0 otherwise
 * @see vm_entry
 */
static int app_unregister_vm(int vmid);

/**
 * Return the virtual machine associated with the given VMID.
 *
 * @param vmid VMID of the virtual machine to find
 * @return NULL if the given VMID didn't match any registered virtual machine,
 *         the virtual machine associated with the given VMID otherwise
 * @see vm_entry
 */
static sqlite_vm * app_find_vm(int vmid);

/**
 * Set the virtual machine to associate with the given VMID.
 *
 * This function doesn't release the previous virtual machine, this is left
 * to the caller.
 *
 * @param vmid the VMID of the virtual machine to update
 * @param vm   the new virtual machine
 * @return 1 if the given VMID didn't match any registered virtual machine,
 * @see vm_entry
 */
static int app_set_vm(int vmid, sqlite_vm *vm);

/**
 * Helper function to convert an integer to a string.
 *
 * The string is dynamically allocated. It is the responsibility of the
 * caller to release it.
 *
 * @param i the integer to convert
 * @return NULL if an allocation error occurred, the given integer as a
 *         dynamically allocated string otherwise
 */
static char * app_itoa(int i);

/**
 * Helper function to convert a string to an integer.
 *
 * @param s the string to convert
 * @param i where to store the converted string
 * @return 1 if an error occurred, 0 otherwise
 */
static int app_atoi(char *s, int *i);

/**
 * Handle the query command.
 *
 * Extract the VMID variable name from the arguments, compile the query,
 * register the resulting virtual machine and set the VMID variable.
 *
 * @param chan the Asterisk channel associated to the command
 * @param data the arguments of the command
 * @return -1 if an error occurred, 0 otherwise
 */
static int app_query(struct ast_channel *chan, char *data);

/**
 * Handle the fetch command.
 *
 * Extract the fetchid variable name from the arguments and the VMID, retreive
 * the associated virtual machine, fetch a row from it, set variables with
 * values from the row, and set the fetchid variable to 1 or 0.
 *
 * @param chan the Asterisk channel associated to the command
 * @param data the arguments of the command
 * @return -1 if an error occurred, 0 otherwise
 */
static int app_fetch(struct ast_channel *chan, char *data);

/**
 * Handle the clear command.
 *
 * Extract the VMID variable from the arguments, retreive the associated virtual
 * machine, unregister it, and release its resources.
 *
 * @param chan the Asterisk channel associated to the command
 * @param data the arguments of the command
 * @return -1 if an error occurred, 0 otherwise
 */
static int app_clear(struct ast_channel *chan, char *data);

/**
 * Asterisk callback function for the SQLITE() application.
 *
 * Asterisk will call this function if support for the SQLITE() application
 * is enabled (app_enable variable in the configuration file) and when the
 * SQLITE() application is used in the Dialplan. It extracts the command and
 * executes the appropriate function.
 *
 * @param chan     the Asterisk channel associated to the command
 * @param data_ptr the string passed to the SQLITE() application
 * @return -1 if an error occurred, 0 otherwise
 */
static int app_exec(struct ast_channel *chan, void *data_ptr);

/**
 * Asterisk callback function for the CLI status command.
 *
 * @param fd   file descriptor provided by Asterisk to use with ast_cli()
 * @param argc number of arguments
 * @param argv arguments list
 * @return RESULT_SUCCESS
 */
static int cli_status(int fd, int argc, char *argv[]);

/**
 * The SQLite database object.
 */
static sqlite *db;

/**
 * Set to 1 if CDR support is enabled.
 */
static int use_cdr;

/**
 * Set to 1 if the SQLITE() application is enabled.
 */
static int use_app;

/**
 * Set to 1 if the CDR callback function was registered.
 */
static int cdr_registered;

/**
 * Set to 1 if the SQLITE() application callback function was registered.
 */
static int app_registered;

/**
 * Set to 1 if the CLI status command callback function was registered.
 */
static int cli_status_registered;

/**
 * The path of the database file.
 */
static char *dbfile;

/**
 * The name of the static configuration table.
 */
static char *config_table;

/**
 * The name of the table used to store CDR entries.
 */
static char *cdr_table;

/**
 * The value of the app_enable parameter in the configuration file.
 */
static char *app_enable;

/**
 * The number of registered virtual machines.
 */
static int vm_count;

/**
 * The structure specifying all callback functions used by Asterisk for static
 * and RealTime configuration.
 */
static struct ast_config_engine sqlite_engine =
{
  .name = RES_CONFIG_SQLITE_DRIVER,	/* WWW [1] */
  .load_func = config_handler,
  .realtime_func = realtime_handler,
  .realtime_multi_func = realtime_multi_handler,
  .update_func = realtime_update_handler
};

/**
 * The mutex used to prevent simultaneous access to the SQLite database.
 * SQLite isn't always compiled with thread safety.
 */
AST_MUTEX_DEFINE_STATIC(mutex);

/**
 * The linked list of registered SQLite virtual machines.
 * This list is kept ordered so that VMID never gets a value which could
 * overflow.
 */
static AST_LIST_HEAD_STATIC(vm_list_head, vm_entry);

/**
 * Pointer to the linked list of registered SQLite virtual machines.
 * The purpose of this pointer is to use the Asterisk linked lists macros
 * conveniently.
 */
static struct vm_list_head *vm_list = &vm_list_head;

/**
 * Structure containing details and callback functions for the CLI status
 * command.
 */
static struct ast_cli_entry cli_status_cmd =
{
  .cmda = {"show", "sqlite", "status", NULL},	/* WWW [1] */
  .handler = cli_status,
  .summary = RES_CONFIG_SQLITE_STATUS_SUMMARY,
  .usage = RES_CONFIG_SQLITE_STATUS_USAGE
};

/*
 * Taken from Asterisk 1.2 cdr_sqlite.so.
 */

/**
 * SQL query format to create the CDR table if non existent.
 */
static char *sql_create_cdr_table =
"CREATE TABLE '%q'\n"
"(\n"
" id          INTEGER,\n"
" calldate    DATETIME     NOT NULL DEFAULT '0000-00-00 00:00:00',\n"
" clid        VARCHAR(80)  NOT NULL DEFAULT '',\n"
" src         VARCHAR(80)  NOT NULL DEFAULT '',\n"
" dst         VARCHAR(80)  NOT NULL DEFAULT '',\n"
" dcontext    VARCHAR(80)  NOT NULL DEFAULT '',\n"
" channel     VARCHAR(80)  NOT NULL DEFAULT '',\n"
" dstchannel  VARCHAR(80)  NOT NULL DEFAULT '',\n"
" lastapp     VARCHAR(80)  NOT NULL DEFAULT '',\n"
" lastdata    VARCHAR(80)  NOT NULL DEFAULT '',\n"
" answer      DATETIME     NOT NULL DEFAULT '0000-00-00 00:00:00',\n"
" end         DATETIME     NOT NULL DEFAULT '0000-00-00 00:00:00',\n"
" duration    INT(11)      NOT NULL DEFAULT 0,\n"
" billsec     INT(11)      NOT NULL DEFAULT 0,\n"
" disposition VARCHAR(45)  NOT NULL DEFAULT '',\n"
" amaflags    INT(11)      NOT NULL DEFAULT 0,\n"
" accountcode VARCHAR(20)  NOT NULL DEFAULT '',\n"
" uniqueid    VARCHAR(32)  NOT NULL DEFAULT '',\n"
" userfield   VARCHAR(255) NOT NULL DEFAULT '',\n"
" PRIMARY KEY (id)\n"
");";	/* WWW [1] */

/**
 * SQL query format to insert a CDR entry.
 */
static char *sql_add_cdr_entry =
"INSERT INTO '%q' ("
"	calldate,"
"       clid,"
"	src,"
"	dst,"
"	dcontext,"
"	channel,"
"	dstchannel,"
"	lastapp,"
"	lastdata,"
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
"	datetime(%d,'unixepoch','localtime'),"
"	'%q',"
"	'%q',"
"	'%q',"
"	'%q',"
"	'%q',"
"	'%q',"
"	'%q',"
"	'%q',"
"	datetime(%d,'unixepoch','localtime'),"
"	datetime(%d,'unixepoch','localtime'),"
"	'%ld',"
"	'%ld',"
"	'%q',"
"	'%ld',"
"	'%q',"
"	'%q',"
"	'%q'"
");";	/* WWW [1] */

/**
 * SQL query format to fetch the static configuration of a file.
 *
 * @see add_cfg_entry()
 */
static char *sql_get_config_table =
"SELECT *"
"	FROM '%q'"
"	WHERE filename = '%q' AND commented = 0"
"	ORDER BY cat_metric ASC, var_metric ASC;";	/* WWW [1] */

static int
set_var(char **var, const char *name, char *value)
{
  if (*var != NULL)
    free(*var);

  *var = strdup(value);

  if (*var == NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate variable %s\n", name);
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

  config = ast_config_load(RES_CONFIG_SQLITE_CONF_FILE);

  if (config == NULL)
    {
      ast_log(LOG_ERROR, "Unable to load " RES_CONFIG_SQLITE_CONF_FILE "\n");
      return 1;
    }

  for (var = ast_variable_browse(config, "general");
       var != NULL;
       var = var->next)
    {
      if (strcasecmp(var->name, "dbfile") == 0)
        SET_VAR(config, dbfile, var);

      else if (strcasecmp(var->name, "config_table") == 0)
        SET_VAR(config, config_table, var);

      else if (strcasecmp(var->name, "cdr_table") == 0)
        SET_VAR(config, cdr_table, var);

      else if (strcasecmp(var->name, "app_enable") == 0)
        SET_VAR(config, app_enable, var);

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
      ast_log(LOG_ERROR, "Required parameter undefined : dbfile\n");
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

  RES_CONFIG_SQLITE_BEGIN
    error = sqlite_exec_printf(db, sql_add_cdr_entry, NULL, NULL, &errormsg,
                               cdr_table, cdr->start.tv_sec, cdr->clid,
			       cdr->src, cdr->dst, cdr->dcontext,
			       cdr->channel, cdr->dstchannel, cdr->lastapp,
			       cdr->lastdata, cdr->answer.tv_sec,
			       cdr->end.tv_sec, cdr->duration, cdr->billsec,
			       ast_cdr_disp2str(cdr->disposition),
			       cdr->amaflags, cdr->accountcode, cdr->uniqueid,
			       cdr->userfield);
  RES_CONFIG_SQLITE_END(error)

  ast_mutex_unlock(&mutex);

  if (error)
    {
      ast_log(LOG_ERROR, "%s\n", errormsg);
      sqlite_freemem(errormsg);
      return 1;
    }

  return 0;
}

static int
add_cfg_entry(void *arg, int argc, char **argv, char __unused **columnNames)
{
  struct cfg_entry_args *args;
  struct ast_variable *var;

  if (argc != RES_CONFIG_SQLITE_CONFIG_COLUMNS)
    {
      ast_log(LOG_WARNING, "Corrupt table\n");
      return 1;
    }

  args = arg;

  if (strcmp(argv[RES_CONFIG_SQLITE_CONFIG_VAR_NAME], "#include") == 0)
    {
      struct ast_config *cfg;
      char *val;

      val = argv[RES_CONFIG_SQLITE_CONFIG_VAR_VAL];
      cfg = ast_config_internal_load(val, args->cfg, 0);

      if (cfg == NULL)
        {
          ast_log(LOG_WARNING, "Unable to include %s\n", val);
          return 1;
        }

      else
        {
          args->cfg = cfg;
          return 0;
        }
    }

  if (args->cat_name == NULL
      || (strcmp(args->cat_name, argv[RES_CONFIG_SQLITE_CONFIG_CATEGORY]) != 0))
    {
      args->cat = ast_category_new(argv[RES_CONFIG_SQLITE_CONFIG_CATEGORY]);

      if (args->cat == NULL)
        {
          ast_log(LOG_WARNING, "Unable to allocate category\n");
          return 1;
        }

      free(args->cat_name);
      args->cat_name = strdup(argv[RES_CONFIG_SQLITE_CONFIG_CATEGORY]);

      if (args->cat_name == NULL)
        {
          ast_log(LOG_WARNING, "Unable to allocate category name\n");
          ast_category_destroy(args->cat);
          return 1;
        }

      ast_category_append(args->cfg, args->cat);
    }

  var = ast_variable_new(argv[RES_CONFIG_SQLITE_CONFIG_VAR_NAME],
                         argv[RES_CONFIG_SQLITE_CONFIG_VAR_VAL]);

  if (var == NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate variable");
      return 1;
    }

  ast_variable_append(args->cat, var);
  return 0;
}

static struct ast_config *
config_handler(const char __unused *database, const char *table,
               const char *file, struct ast_config *cfg,
               int __unused withcomments)
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

  RES_CONFIG_SQLITE_BEGIN
    error = sqlite_exec_printf(db, sql_get_config_table, add_cfg_entry,
                               &args, &errormsg, table, file);
  RES_CONFIG_SQLITE_END(error)

  ast_mutex_unlock(&mutex);

  free(args.cat_name);

  if (error)
    {
      ast_log(LOG_ERROR, "%s\n", errormsg);
      sqlite_freemem(errormsg);
      return NULL;
    }

  return cfg;
}

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

  args = arg;

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
realtime_handler(const char __unused *database,
                 const char *table, va_list ap)
{
  const char *op;
  char *query, *errormsg, *tmp_str;
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

/* @cond DOXYGEN_CAN_PARSE_THIS */
#undef QUERY
#define QUERY "SELECT * FROM '%q' WHERE commented = 0 AND %q%s '%q'"
/* @endcond */

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

  RES_CONFIG_SQLITE_BEGIN
    error = sqlite_exec(db, query, add_rt_cfg_entry, &args, &errormsg);
  RES_CONFIG_SQLITE_END(error)

  ast_mutex_unlock(&mutex);

  sqlite_freemem(query);

  if (error)
    {
      ast_log(LOG_WARNING, "%s\n", errormsg);
      sqlite_freemem(errormsg);
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
  int i;

  args = arg;
  cat_name = NULL;

  /*
   * cat_name should always be set here, since initfield is forged from
   * params[0] in realtime_multi_handler(), which is a search parameter
   * of the SQL query.
   */
  for (i = 0; i < argc; i++)
    if (strcmp(args->initfield, columnNames[i]) == 0)
      cat_name = argv[i];

  if (cat_name == NULL)
    {
      ast_log(LOG_ERROR, "Bogus SQL results, cat_name is NULL !\n");
      return 1;
    }

  cat = ast_category_new(cat_name);

  if (cat == NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate category\n");
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
realtime_multi_handler(const char __unused *database, const char *table,
                       va_list ap)
{
  const char *op;
  char *query, *errormsg, *tmp_str, *initfield, *initfield_space;
  struct rt_multi_cfg_entry_args args;
  const char **params, **vals;
  const char *subst_val0;
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

  initfield_space = strchr(initfield, ' ');
  if (initfield_space != NULL)
    *initfield_space = '\0';

  op = (strchr(params[0], ' ') == NULL) ? " =" : "";

  /*
   * Asterisk sends us an already escaped string when searching for
   * "exten LIKE" (uh!). Handle it separately.
   */
  if (strcmp(vals[0], "\\_%") == 0) {
          if (strcmp(params[0], "exten LIKE") == 0) {
                  params[0] = "SUBSTR(exten,0,1)";
                  op = " =";
                  subst_val0 = "_";
          } else
                  subst_val0 = "_%";
  } else
          subst_val0 = vals[0];

/* @cond DOXYGEN_CAN_PARSE_THIS */
#undef QUERY
#define QUERY "SELECT * FROM '%q' WHERE commented = 0 AND %q%s '%q'"
/* @endcond */

  query = sqlite_mprintf(QUERY, table, params[0], op, subst_val0);

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

  RES_CONFIG_SQLITE_BEGIN
    error = sqlite_exec(db, query, add_rt_multi_cfg_entry, &args, &errormsg);
  RES_CONFIG_SQLITE_END(error)

  ast_mutex_unlock(&mutex);

  sqlite_freemem(query);
  free(initfield);

  if (error)
    {
      ast_log(LOG_WARNING, "%s\n", errormsg);
      sqlite_freemem(errormsg);
      ast_config_destroy(cfg);
      return NULL;
    }

  return cfg;
}

static int
realtime_update_handler(const char __unused *database,
                        const char *table, const char *keyfield,
                        const char *entity, va_list ap)
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

/* @cond DOXYGEN_CAN_PARSE_THIS */
#undef QUERY
#define QUERY "UPDATE '%q' SET %q = '%q'"
/* @endcond */

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

  RES_CONFIG_SQLITE_BEGIN
    error = sqlite_exec(db, query, NULL, NULL, &errormsg);
  RES_CONFIG_SQLITE_END(error)

  if (!error)
    rows_num = sqlite_changes(db);

  else
    rows_num = -1;

  ast_mutex_unlock(&mutex);

  sqlite_freemem(query);

  if (error)
    {
      ast_log(LOG_WARNING, "%s\n", errormsg);
      sqlite_freemem(errormsg);
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
  RES_CONFIG_SQLITE_BEGIN
    error = sqlite_compile(db, query, NULL, &vm, &errormsg);
  RES_CONFIG_SQLITE_END(error)

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
      return 1;
    }

  return 0;
}

static int
app_register_vm(sqlite_vm *vm)
{
  struct vm_entry *entry, *i, *next;
  int vmid;

  entry = malloc(sizeof(struct vm_entry));

  if (entry == NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate VM entry\n");
      return 1;
    }

  AST_LIST_LOCK(vm_list);

  vm_count++;

  if (vm_count >= RES_CONFIG_SQLITE_VM_MAX)
    {
      vm_count--;
      AST_LIST_UNLOCK(vm_list);
      free(entry);
      ast_log(LOG_WARNING, "Maximum number of simultaneous SQLite VMs "
                           "reached, can't create new VM\n");
      return 1;
    }

  if (AST_LIST_EMPTY(vm_list) || AST_LIST_FIRST(vm_list)->vmid != 0)
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
      return 1;
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
      return 1;
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
    return 1;

  *i = strtol(s, &endptr, 10);

  if (*endptr != '\0')
    return 1;

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
  char *fetchid, *vmid_str, *var;
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

  RES_CONFIG_SQLITE_BEGIN
    error = sqlite_step(vm, &cols_count, &values, &cols);
  RES_CONFIG_SQLITE_END(error)

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

      /*
       * There can be two cases here : if sqlite_step() returned SQLITE_DONE,
       * app_free_vm() won't fail, and we can safely assume that the virtual
       * machine has completed execution, so we set fetchid; if sqlite_step()
       * returned an error, app_free_vm() will tell what this error was, and
       * we stop execution here, without setting fetchid.
       */
      error = app_free_vm(vm);

      if (error)
        return -1;

      pbx_builtin_setvar_helper(chan, fetchid, "0");
    }

  else
    {
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
    }

  return 0;
}

static int
app_clear(struct ast_channel __unused *chan, char *data)
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
      ast_log(LOG_WARNING, RES_CONFIG_SQLITE_APP_DRIVER "() requires an argument\n");
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
  cmd_func = NULL;

  if (strcasecmp(cmd, "query") == 0)
    cmd_func = app_query;

  else if (strcasecmp(cmd, "fetch") == 0)
    cmd_func = app_fetch;

  else if (strcasecmp(cmd, "clear") == 0)
    cmd_func = app_clear;

  if (cmd_func == NULL)
    {
      ast_log(LOG_WARNING, "Unknown command: %s\n", cmd);
      free(data_ptr);
      return -1;
    }

  error = cmd_func(chan, data);
  free(data_ptr);
  return error;
}

static int
cli_status(int fd, int __unused argc, char __unused *argv[])
{
  ast_cli(fd, "SQLite database path: %s\n", dbfile);	/* WWW [1] */
  ast_cli(fd, "config_table: ");			/* WWW [1] */

  if (config_table == NULL)
    ast_cli(fd, "unspecified, must be present in extconfig.conf\n");	/* WWW [1] */

  else
    ast_cli(fd, "%s\n", config_table);			/* WWW [1] */

  ast_cli(fd, "cdr_table: ");				/* WWW [1] */

  if (cdr_table == NULL)
    ast_cli(fd, "unspecified, CDR support disabled\n");	/* WWW [1] */

  else
    ast_cli(fd, "%s\n", cdr_table);			/* WWW [1] */

  ast_cli(fd, "app_enable: %s, SQLITE() application %s\n", app_enable,
          use_app ? "enabled" : "disabled");		/* WWW [1] */

  if (use_app)
    {
      int vm_count_tmp;

      AST_LIST_LOCK(vm_list);
      vm_count_tmp = vm_count;
      AST_LIST_UNLOCK(vm_list);

      ast_cli(fd, "Number of registered SQLite virtual machines : %d\n",
              vm_count_tmp);				/* WWW [1] */
    }

  return RESULT_SUCCESS;
}

int
load_module(void)
{
  char *errormsg;
  int error;

  db = NULL;
  cdr_registered = 0;
  app_registered = 0;
  cli_status_registered = 0;
  dbfile = NULL;
  config_table = NULL;
  cdr_table = NULL;
  app_enable = NULL;
  vm_count = 0;
  error = load_config();

  if (error)
    return 1;

  db = sqlite_open(dbfile, 0660, &errormsg);

  if (db == NULL)
    {
      ast_log(LOG_ERROR, "%s\n", errormsg);
      sqlite_freemem(errormsg);
      unload_module();
      return 1;
    }

  ast_config_engine_register(&sqlite_engine);

  if (use_cdr)
    {
      RES_CONFIG_SQLITE_BEGIN
        error = sqlite_exec_printf(db, "SELECT COUNT(id) FROM %Q;", NULL, NULL,
                                   &errormsg, cdr_table);
      RES_CONFIG_SQLITE_END(error)

      if (error)
        {
          /*
           * Unexpected error.
           */
          if (error != SQLITE_ERROR)
            {
              ast_log(LOG_ERROR, "%s\n", errormsg);
              sqlite_freemem(errormsg);
              unload_module();
              return 1;
            }

          RES_CONFIG_SQLITE_BEGIN
            error = sqlite_exec_printf(db, sql_create_cdr_table, NULL, NULL,
                    &errormsg, cdr_table);
          RES_CONFIG_SQLITE_END(error)

          if (error)
            {
              ast_log(LOG_ERROR, "%s\n", errormsg);
              sqlite_freemem(errormsg);
              unload_module();
              return 1;
            }
        }

      error = ast_cdr_register(RES_CONFIG_SQLITE_NAME, RES_CONFIG_SQLITE_DESCRIPTION,
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
      error = ast_register_application(RES_CONFIG_SQLITE_APP_DRIVER, app_exec,
                                       RES_CONFIG_SQLITE_APP_SYNOPSIS,
                                       RES_CONFIG_SQLITE_APP_DESCRIPTION);

      if (error)
        {
          unload_module();
          return 1;
        }

      app_registered = 1;
    }

  error = ast_cli_register(&cli_status_cmd);

  if (error)
    {
      unload_module();
      return 1;
    }

  cli_status_registered = 1;
  return 0;
}

int
unload_module(void)
{
  if (cli_status_registered)
    ast_cli_unregister(&cli_status_cmd);

  if (app_registered)
    ast_unregister_application(RES_CONFIG_SQLITE_APP_DRIVER);

  if (cdr_registered)
    ast_cdr_unregister(RES_CONFIG_SQLITE_NAME);

  ast_config_engine_deregister(&sqlite_engine);

  if (db != NULL)
    sqlite_close(db);

  unload_config();
  return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_GLOBAL_SYMBOLS, "Realtime SQLite configuration",/* WWW [2] */
		.load = load_module,
		.unload = unload_module,
		);

/*
 * [1]: discards qualifiers from pointer target type
 *	Compiler warning due to poor Asterisk prototypes
 *
 * [2]: 'static' is not at beginning of declaration
 *      harmless
 */
