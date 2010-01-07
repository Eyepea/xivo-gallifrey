/*
 * XiVO Web-Interface
 * Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

BEGIN TRANSACTION;

DROP TABLE accesswebservice;
CREATE TABLE accesswebservice (
 id integer unsigned,
 name varchar(64) NOT NULL DEFAULT '',
 login varchar(64),
 passwd varchar(64),
 host varchar(255),
 obj longblob NOT NULL,
 disable tinyint(1) NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX accesswebservice__idx__login ON accesswebservice(login);
CREATE INDEX accesswebservice__idx__passwd ON accesswebservice(passwd);
CREATE INDEX accesswebservice__idx__host ON accesswebservice(host);
CREATE INDEX accesswebservice__idx__disable ON accesswebservice(disable);
CREATE UNIQUE INDEX accesswebservice__uidx__name ON accesswebservice(name);


DROP TABLE entity;
CREATE TABLE entity (
 id integer unsigned,
 name varchar(64) NOT NULL DEFAULT '',
 displayname varchar(128) NOT NULL DEFAULT '',
 phonenumber varchar(40) NOT NULL DEFAULT '',
 faxnumber varchar(40) NOT NULL DEFAULT '',
 email varchar(255) NOT NULL DEFAULT '',
 url varchar(255) NOT NULL DEFAULT '',
 address1 varchar(30) NOT NULL DEFAULT '',
 address2 varchar(30) NOT NULL DEFAULT '',
 city varchar(128) NOT NULL DEFAULT '',
 state varchar(128) NOT NULL DEFAULT '',
 zipcode varchar(16) NOT NULL DEFAULT '',
 country varchar(3) NOT NULL DEFAULT '',
 disable tinyint(1) NOT NULL DEFAULT 0,
 dcreate integer unsigned NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX entity__idx__displayname ON entity(displayname);
CREATE INDEX entity__idx__disable ON entity(disable);
CREATE UNIQUE INDEX entity__uidx__name ON entity(name);


DROP TABLE i18ncache;
CREATE TABLE i18ncache (
 locale varchar(7) NOT NULL DEFAULT '',
 path varchar(255) NOT NULL DEFAULT '',
 language varchar(3) NOT NULL DEFAULT '',
 dcreate integer unsigned NOT NULL DEFAULT 0,
 dupdate integer unsigned NOT NULL DEFAULT 0,
 obj longblob NOT NULL,
 PRIMARY KEY(locale,path)
);

CREATE INDEX i18ncache__idx__language ON i18ncache(language);
CREATE INDEX i18ncache__idx__dupdate ON i18ncache(dupdate);


DROP TABLE ldapserver;
CREATE TABLE ldapserver (
 id integer unsigned,
 name varchar(64) NOT NULL DEFAULT '',
 host varchar(255) NOT NULL DEFAULT '',
 port smallint unsigned NOT NULL,
 securitylayer char(3),
 protocolversion char(1) NOT NULL DEFAULT '3',
 disable tinyint(1) NOT NULL DEFAULT 0,
 dcreate integer unsigned NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX ldapserver__idx__host ON ldapserver(host);
CREATE INDEX ldapserver__idx__port ON ldapserver(port);
CREATE INDEX ldapserver__idx__disable ON ldapserver(disable);
CREATE UNIQUE INDEX ldapserver__uidx__name ON ldapserver(name);
CREATE UNIQUE INDEX ldapserver__uidx__host_port ON ldapserver(host,port);

DROP INDEX server__idx__ssl;

DROP TABLE session;
CREATE TABLE session (
 key char(32) NOT NULL DEFAULT '',
 start integer unsigned NOT NULL DEFAULT 0,
 expire integer unsigned NOT NULL DEFAULT 0,
 userid integer unsigned NOT NULL DEFAULT 0,
 data longblob NOT NULL,
 PRIMARY KEY(key)
);

CREATE INDEX session__idx__expire ON session(expire);
CREATE INDEX session__idx__userid ON session(userid);

COMMIT;
