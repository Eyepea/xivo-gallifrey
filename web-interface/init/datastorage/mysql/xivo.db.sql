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

START TRANSACTION;

GRANT ALL PRIVILEGES ON `xivo`.* TO `xivo`@`localhost` IDENTIFIED BY PASSWORD '*DBA86DFECE903EB25FE460A66BDCDA790A1CA4A4';
CREATE DATABASE IF NOT EXISTS `xivo` DEFAULT CHARACTER SET utf8;

USE `xivo`;

DROP TABLE IF EXISTS `accesswebservice`;
CREATE TABLE `accesswebservice` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(64) NOT NULL DEFAULT '',
 `login` varchar(64),
 `passwd` varchar(64),
 `host` varchar(255),
 `obj` longblob NOT NULL,
 `disable` tinyint(1) NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `accesswebservice__idx__login` ON `accesswebservice`(`login`);
CREATE INDEX `accesswebservice__idx__passwd` ON `accesswebservice`(`passwd`);
CREATE INDEX `accesswebservice__idx__host` ON `accesswebservice`(`host`);
CREATE INDEX `accesswebservice__idx__disable` ON `accesswebservice`(`disable`);
CREATE UNIQUE INDEX `accesswebservice__uidx__name` ON `accesswebservice`(`name`);


DROP TABLE IF EXISTS `directories`;
CREATE TABLE `directories` (
 `id` int(10) unsigned auto_increment,
 `uri` varchar(255),
 `dirtype` varchar(20),
 `name` varchar(255),
 `tablename` varchar(255),
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS `entity`;
CREATE TABLE `entity` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(64) NOT NULL DEFAULT '',
 `displayname` varchar(128) NOT NULL DEFAULT '',
 `phonenumber` varchar(40) NOT NULL DEFAULT '',
 `faxnumber` varchar(40) NOT NULL DEFAULT '',
 `email` varchar(255) NOT NULL DEFAULT '',
 `url` varchar(255) NOT NULL DEFAULT '',
 `address1` varchar(30) NOT NULL DEFAULT '',
 `address2` varchar(30) NOT NULL DEFAULT '',
 `city` varchar(128) NOT NULL DEFAULT '',
 `state` varchar(128) NOT NULL DEFAULT '',
 `zipcode` varchar(16) NOT NULL DEFAULT '',
 `country` varchar(3) NOT NULL DEFAULT '',
 `disable` tinyint(1) NOT NULL DEFAULT 0,
 `dcreate` int(10) unsigned NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `entity__idx__displayname` ON `entity`(`displayname`);
CREATE INDEX `entity__idx__disable` ON `entity`(`disable`);
CREATE UNIQUE INDEX `entity__uidx__name` ON `entity`(`name`);


DROP TABLE IF EXISTS `i18ncache`;
CREATE TABLE `i18ncache` (
 `locale` varchar(7) NOT NULL DEFAULT '',
 `path` varchar(255) NOT NULL DEFAULT '',
 `language` varchar(3) NOT NULL DEFAULT '',
 `dcreate` int(10) unsigned NOT NULL DEFAULT 0,
 `dupdate` int(10) unsigned NOT NULL DEFAULT 0,
 `obj` longblob NOT NULL,
 PRIMARY KEY(`locale`,`path`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `i18ncache__idx__language` ON `i18ncache`(`language`);
CREATE INDEX `i18ncache__idx__dupdate` ON `i18ncache`(`dupdate`);


DROP TABLE IF EXISTS `iproute`;
CREATE TABLE `iproute` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(64) NOT NULL DEFAULT '',
 `iface` varchar(64) NOT NULL DEFAULT '',
 `destination` varchar(39) NOT NULL,
 `netmask` varchar(39) NOT NULL,
 `gateway` varchar(39) NOT NULL,
 `disable` tinyint(1) NOT NULL DEFAULT 0,
 `dcreate` int(10) unsigned NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `iproute__idx__iface` ON `iproute`(`iface`);
CREATE UNIQUE INDEX `iproute__uidx__name` ON `iproute`(`name`);
CREATE UNIQUE INDEX `iproute__uidx__destination_netmask_gateway` ON `iproute`(`destination`,`netmask`,`gateway`);


DROP TABLE IF EXISTS `ldapserver`;
CREATE TABLE `ldapserver` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(64) NOT NULL DEFAULT '',
 `host` varchar(255) NOT NULL DEFAULT '',
 `port` smallint unsigned NOT NULL,
 `securitylayer` enum('tls','ssl'),
 `protocolversion` enum('2','3') NOT NULL DEFAULT '3',
 `disable` tinyint(1) NOT NULL DEFAULT 0,
 `dcreate` int(10) unsigned NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `ldapserver__idx__host` ON `ldapserver`(`host`);
CREATE INDEX `ldapserver__idx__port` ON `ldapserver`(`port`);
CREATE INDEX `ldapserver__idx__disable` ON `ldapserver`(`disable`);
CREATE UNIQUE INDEX `ldapserver__uidx__name` ON `ldapserver`(`name`);
CREATE UNIQUE INDEX `ldapserver__uidx__host_port` ON `ldapserver`(`host`,`port`);


DROP TABLE IF EXISTS `netiface`;
CREATE TABLE `netiface` (
 `name` varchar(64) NOT NULL DEFAULT '',
 `ifname` varchar(64) NOT NULL DEFAULT '',
 `hwtypeid` smallint unsigned NOT NULL DEFAULT 65534,
 `networktype` enum('data','voip') NOT NULL,
 `type` enum('iface') NOT NULL,
 `family` enum('inet','inet6') NOT NULL,
 `method` enum('static','dhcp') NOT NULL,
 `address` varchar(39),
 `netmask` varchar(39),
 `broadcast` varchar(15),
 `gateway` varchar(39),
 `mtu` smallint(4) unsigned,
 `vlanrawdevice` varchar(64),
 `vlanid` smallint(4) unsigned,
 `options` text NOT NULL,
 `disable` tinyint(1) NOT NULL DEFAULT 0,
 `dcreate` int(10) unsigned NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `netiface__idx__hwtypeid` ON `netiface`(`hwtypeid`);
CREATE INDEX `netiface__idx__networktype` ON `netiface`(`networktype`);
CREATE INDEX `netiface__idx__type` ON `netiface`(`type`);
CREATE INDEX `netiface__idx__family` ON `netiface`(`family`);
CREATE INDEX `netiface__idx__method` ON `netiface`(`method`);
CREATE INDEX `netiface__idx__address` ON `netiface`(`address`);
CREATE INDEX `netiface__idx__netmask` ON `netiface`(`netmask`);
CREATE INDEX `netiface__idx__broadcast` ON `netiface`(`broadcast`);
CREATE INDEX `netiface__idx__gateway` ON `netiface`(`gateway`);
CREATE INDEX `netiface__idx__mtu` ON `netiface`(`mtu`);
CREATE INDEX `netiface__idx__vlanrawdevice` ON `netiface`(`vlanrawdevice`);
CREATE INDEX `netiface__idx__vlanid` ON `netiface`(`vlanid`);
CREATE INDEX `netiface__idx__disable` ON `netiface`(`disable`);
CREATE UNIQUE INDEX `netiface__uidx__ifname` ON `netiface`(`ifname`);


DROP TABLE IF EXISTS `resolvconf`;
CREATE TABLE `resolvconf` (
 `id` tinyint(1) auto_increment,
 `hostname` varchar(63) NOT NULL DEFAULT 'xivo',
 `nameserver1` varchar(255),
 `nameserver2` varchar(255),
 `nameserver3` varchar(255),
 `search` varchar(255),
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE UNIQUE INDEX `resolvconf__uidx__hostname` ON `resolvconf`(`hostname`);


DROP TABLE IF EXISTS `server`;
CREATE TABLE `server` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(64) NOT NULL DEFAULT '',
 `host` varchar(255) NOT NULL DEFAULT '',
 `port` smallint unsigned NOT NULL,
 `ssl` tinyint(1) NOT NULL DEFAULT 0,
 `disable` tinyint(1) NOT NULL DEFAULT 0,
 `dcreate` int(10) unsigned NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 `webi` varchar(255) NOT NULL DEFAULT '',
 `ami_port` smallint unsigned NOT NULL,
 `ami_login` varchar(64) NOT NULL DEFAULT '',
 `ami_pass` varchar(64) NOT NULL DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `server__idx__host` ON `server`(`host`);
CREATE INDEX `server__idx__port` ON `server`(`port`);
CREATE INDEX `server__idx__disable` ON `server`(`disable`);
CREATE UNIQUE INDEX `server__uidx__name` ON `server`(`name`);
CREATE UNIQUE INDEX `server__uidx__host_port` ON `server`(`host`,`port`);


DROP TABLE IF EXISTS `session`;
CREATE TABLE `session` (
 `sessid` char(32) binary NOT NULL,
 `start` int(10) unsigned NOT NULL,
 `expire` int(10) unsigned NOT NULL,
 `identifier` varchar(255) NOT NULL,
 `data` longblob NOT NULL,
 PRIMARY KEY(`sessid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `session__idx__expire` ON `session`(`expire`);
CREATE INDEX `session__idx__identifier` ON `session`(`identifier`);


DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
 `id` int(10) unsigned auto_increment,
 `login` varchar(64) NOT NULL DEFAULT '',
 `passwd` varchar(64) NOT NULL DEFAULT '',
 `meta` enum('user','admin','root') NOT NULL DEFAULT 'user',
 `valid` tinyint(1) NOT NULL DEFAULT 1,
 `time` int(10) unsigned NOT NULL DEFAULT 0,
 `dcreate` int(10) unsigned NOT NULL DEFAULT 0,
 `dupdate` int(10) unsigned NOT NULL DEFAULT 0,
 `obj` longblob NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `user__idx__login` ON `user`(`login`);
CREATE INDEX `user__idx__passwd` ON `user`(`passwd`);
CREATE INDEX `user__idx__meta` ON `user`(`meta`);
CREATE INDEX `user__idx__valid` ON `user`(`valid`);
CREATE INDEX `user__idx__time` ON `user`(`time`);
CREATE UNIQUE INDEX `user__uidx__login_meta` ON `user`(`login`,`meta`);

INSERT INTO `user` VALUES (1,'root','proformatique','root',1,0,UNIX_TIMESTAMP(UTC_TIMESTAMP()),0,'');
INSERT INTO `user` VALUES (2,'admin','proformatique','admin',1,0,UNIX_TIMESTAMP(UTC_TIMESTAMP()),0,'');

COMMIT;
