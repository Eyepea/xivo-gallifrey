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


DROP TABLE directories;
CREATE TABLE directories (
 id integer unsigned,
 uri varchar(255),
 dirtype varchar(20),
 name varchar(255),
 tablename varchar(255),
 description text NOT NULL,
 PRIMARY KEY(id)
);


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


DROP TABLE iproute;
CREATE TABLE iproute (
 id integer unsigned,
 name varchar(64) NOT NULL DEFAULT '',
 iface varchar(64) NOT NULL DEFAULT '',
 destination varchar(39),
 netmask varchar(39),
 gateway varchar(39),
 disable tinyint(1) NOT NULL DEFAULT 0,
 dcreate integer unsigned NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX iproute__idx__iface ON iproute(iface);
CREATE UNIQUE INDEX iproute__uidx__name ON iproute(name);
CREATE UNIQUE INDEX iproute__uidx__destination_netmask_gateway ON iproute(destination,netmask,gateway);


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


DROP TABLE netiface;
CREATE TABLE netiface (
 name varchar(64) NOT NULL DEFAULT '',
 ifname varchar(64) NOT NULL DEFAULT '',
 networktype char(4) NOT NULL,
 hwtypeid smallint unsigned NOT NULL DEFAULT 65534,
 type char(5) NOT NULL,
 family varchar(5) NOT NULL,
 method varchar(6) NOT NULL,
 address varchar(39),
 netmask varchar(39),
 broadcast varchar(39),
 gateway varchar(39),
 mtu smallint unsigned,
 vlanrawdevice varchar(64),
 vlanid smallint unsigned,
 options text NOT NULL,
 disable tinyint(1) NOT NULL DEFAULT 0,
 dcreate integer unsigned NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(name)
);

CREATE INDEX netiface__idx__hwtypeid ON netiface(hwtypeid);
CREATE INDEX netiface__idx__networktype ON netiface(networktype);
CREATE INDEX netiface__idx__type ON netiface(type);
CREATE INDEX netiface__idx__family ON netiface(family);
CREATE INDEX netiface__idx__method ON netiface(method);
CREATE INDEX netiface__idx__address ON netiface(address);
CREATE INDEX netiface__idx__netmask ON netiface(netmask);
CREATE INDEX netiface__idx__broadcast ON netiface(broadcast);
CREATE INDEX netiface__idx__gateway ON netiface(gateway);
CREATE INDEX netiface__idx__mtu ON netiface(mtu);
CREATE INDEX netiface__idx__vlanrawdevice ON netiface(vlanrawdevice);
CREATE INDEX netiface__idx__vlanid ON netiface(vlanid);
CREATE INDEX netiface__idx__disable ON netiface(disable);
CREATE UNIQUE INDEX netiface__uidx__ifname ON netiface(ifname);


DROP TABLE resolvconf;
CREATE TABLE resolvconf (
 id tinyint(1),
 hostname varchar(63) NOT NULL DEFAULT 'xivo',
 domain varchar(255) NOT NULL DEFAULT '',
 nameserver1 varchar(255),
 nameserver2 varchar(255),
 nameserver3 varchar(255),
 search varchar(255),
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE UNIQUE INDEX resolvconf__uidx__hostname ON resolvconf(hostname);


DROP TABLE server;
CREATE TABLE server (
 id integer unsigned,
 name varchar(64) NOT NULL DEFAULT '',
 host varchar(255) NOT NULL DEFAULT '',
 port smallint unsigned NOT NULL,
 ssl tinyint(1) NOT NULL DEFAULT 0,
 disable tinyint(1) NOT NULL DEFAULT 0,
 dcreate integer unsigned NOT NULL DEFAULT 0,
 description text NOT NULL,
 webi varchar(255) NOT NULL DEFAULT '',
 ami_port smallint unsigned NOT NULL,
 ami_login varchar(64) NOT NULL DEFAULT '',
 ami_pass varchar(64) NOT NULL DEFAULT '',
 PRIMARY KEY(id)
);

CREATE INDEX server__idx__host ON server(host);
CREATE INDEX server__idx__port ON server(port);
CREATE INDEX server__idx__disable ON server(disable);
CREATE UNIQUE INDEX server__uidx__name ON server(name);
CREATE UNIQUE INDEX server__uidx__host_port ON server(host,port);


DROP TABLE session;
CREATE TABLE session (
 sessid char(32) NOT NULL,
 start integer unsigned NOT NULL,
 expire integer unsigned NOT NULL,
 identifier varchar(255) NOT NULL,
 data longblob NOT NULL,
 PRIMARY KEY(sessid)
);

CREATE INDEX session__idx__expire ON session(expire);
CREATE INDEX session__idx__identifier ON session(identifier);


DROP TABLE user;
CREATE TABLE user (
 id integer unsigned,
 login varchar(64) NOT NULL DEFAULT '',
 passwd varchar(64) NOT NULL DEFAULT '',
 meta varchar(5) NOT NULL DEFAULT 'user',
 valid tinyint(1) NOT NULL DEFAULT 1,
 time integer unsigned NOT NULL DEFAULT 0,
 dcreate integer unsigned NOT NULL DEFAULT 0,
 dupdate integer unsigned NOT NULL DEFAULT 0,
 obj longblob NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX user__idx__login ON user(login);
CREATE INDEX user__idx__passwd ON user(passwd);
CREATE INDEX user__idx__meta ON user(meta);
CREATE INDEX user__idx__valid ON user(valid);
CREATE INDEX user__idx__time ON user(time);
CREATE UNIQUE INDEX user__uidx__login_meta ON user(login,meta);


DROP TABLE dhcp;
CREATE TABLE dhcp (
 id integer unsigned,
 active tinyint(1) NOT NULL DEFAULT 0,
 pool_start varchar(64) NOT NULL DEFAULT '',
 pool_end varchar(64) NOT NULL DEFAULT '',
 extra_ifaces varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY(id)
);

INSERT INTO dhcp VALUES (1,0,'','','');


DROP TABLE mail;
CREATE TABLE mail (
 id integer unsigned,
 mydomain varchar(255) NOT NULL DEFAULT 0,
 origin varchar(255) NOT NULL DEFAULT 'xivo-clients.proformatique.com',
 relayhost varchar(255) NOT NULL DEFAULT '',
 fallback_relayhost varchar(255) NOT NULL DEFAULT '',
 canonical varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY(id)
);

INSERT INTO mail VALUES (1,'', 'xivo-clients.proformatique.com', '', '', '');


DROP TABLE monitoring;
CREATE TABLE monitoring (
 id integer unsigned,
 maintenance tinyint(1) NOT NULL DEFAULT 0,
 alert_emails varchar(4096) DEFAULT NULL,
 dahdi_monitor_ports varchar(255) DEFAULT NULL,
 max_call_duration integer DEFAULT NULL,
 PRIMARY KEY(id)
);

INSERT INTO monitoring VALUES (1,0,NULL,NULL,NULL);


-- HA
DROP TABLE ha;
CREATE TABLE ha (
 id integer unsigned,
 apache2 tinyint(1) NOT NULL DEFAULT 0,
 asterisk tinyint(1) NOT NULL DEFAULT 0,
 dhcp tinyint(1) NOT NULL DEFAULT 0,
 monit tinyint(1) NOT NULL DEFAULT 0,
 mysql tinyint(1) NOT NULL DEFAULT 0,
 ntp tinyint(1) NOT NULL DEFAULT 0,
 rsync tinyint(1) NOT NULL DEFAULT 0,
 smokeping tinyint(1) NOT NULL DEFAULT 0,
 mailto tinyint(1) NOT NULL DEFAULT 0,
 alert_emails varchar(1024) DEFAULT NULL,
 serial varchar(16) NOT NULL DEFAULT '',
 authkeys varchar(128) NOT NULL DEFAULT '',
 com_mode varchar(8) NOT NULL DEFAULT 'ucast',
 user varchar(16) NOT NULL DEFAULT 'pf-replication',
 password varchar(16) NOT NULL DEFAULT 'proformatique',
 dest_user varchar(16) NOT NULL DEFAULT 'pf-replication',
 dest_password varchar(16) NOT NULL DEFAULT 'proformatique',
 PRIMARY KEY(id)
);

INSERT INTO ha VALUES (1,0,0,0,0,0,0,0,0,0,NULL,'','','ucast','pf-replication','proformatique','pf-replication','proformatique');

DROP TABLE ha_uname_node;
CREATE TABLE ha_uname_node (
 uname_node varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY (uname_node)
);

DROP TABLE ha_ping_ipaddr;
CREATE TABLE ha_ping_ipaddr (
 ping_ipaddr varchar(39) NOT NULL DEFAULT '',
 PRIMARY KEY (ping_ipaddr)
);

DROP TABLE ha_virtual_network;
CREATE TABLE ha_virtual_network(
 ipaddr varchar(39) NOT NULL DEFAULT '',
 netmask varchar(39) NOT NULL DEFAULT '',
 broadcast varchar(39) NOT NULL DEFAULT '',
 PRIMARY KEY (ipaddr)
);

DROP TABLE ha_peer;
CREATE TABLE ha_peer (
 iface varchar(64) NOT NULL DEFAULT '',
 host varchar(128) NOT NULL DEFAULT '',
 transfer tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY (iface, host)
);


COMMIT;
