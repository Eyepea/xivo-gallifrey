/*
 * Copyright (C) 2006 Richard Braun <rbraun@proformatique.com>
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

#ifndef _RES_SQLITE_H
#define _RES_SQLITE_H

#define RES_SQLITE_NAME "res_sqlite"
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

#endif /* _RES_SQLITE_H */
