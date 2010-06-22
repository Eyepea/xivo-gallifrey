/*
 * XiVO Base-Config
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

GRANT ALL PRIVILEGES ON `asterisk`.* TO `asterisk`@`localhost` IDENTIFIED BY PASSWORD '*DBA86DFECE903EB25FE460A66BDCDA790A1CA4A4';
CREATE DATABASE IF NOT EXISTS `asterisk` DEFAULT CHARACTER SET utf8;

USE `asterisk`;

DROP TABLE IF EXISTS `accessfeatures`;
CREATE TABLE `accessfeatures` (
 `id` int(10) unsigned auto_increment,
 `host` varchar(255) NOT NULL DEFAULT '',
 `feature` enum('phonebook') NOT NULL,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `accessfeatures__idx__host` ON `accessfeatures`(`host`);
CREATE INDEX `accessfeatures__idx__feature` ON `accessfeatures`(`feature`);
CREATE INDEX `accessfeatures__idx__commented` ON `accessfeatures`(`commented`);
CREATE UNIQUE INDEX `accessfeatures__uidx__host_feature` ON `accessfeatures`(`host`,`feature`);


DROP TABLE IF EXISTS `agentfeatures`;
CREATE TABLE `agentfeatures` (
 `id` int(10) unsigned auto_increment,
 `agentid` int(10) unsigned NOT NULL,
 `numgroup` tinyint(2) unsigned NOT NULL,
 `firstname` varchar(128) NOT NULL DEFAULT '',
 `lastname` varchar(128) NOT NULL DEFAULT '',
 `number` varchar(40) NOT NULL,
 `passwd` varchar(128) NOT NULL,
 `context` varchar(39) NOT NULL,
 `language` varchar(20) NOT NULL,
 `silent` tinyint(1) NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `agentfeatures__idx__commented` ON `agentfeatures`(`commented`);
CREATE UNIQUE INDEX `agentfeatures__uidx__agentid` ON `agentfeatures`(`agentid`);
CREATE UNIQUE INDEX `agentfeatures__uidx__number` ON `agentfeatures`(`number`);


DROP TABLE IF EXISTS `agentgroup`;
CREATE TABLE `agentgroup` (
 `id` tinyint(2) unsigned auto_increment,
 `groupid` int(10) unsigned NOT NULL,
 `name` varchar(128) NOT NULL DEFAULT '',
 `groups` varchar(255) NOT NULL DEFAULT '',
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `deleted` tinyint(1) NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `agentgroup__idx__groupid` ON `agentgroup`(`groupid`);
CREATE INDEX `agentgroup__idx__name` ON `agentgroup`(`name`);
CREATE INDEX `agentgroup__idx__commented` ON `agentgroup`(`commented`);
CREATE INDEX `agentgroup__idx__deleted` ON `agentgroup`(`deleted`);

INSERT INTO `agentgroup` VALUES (1,3,'default','',0,0,'');

-- agent queueskills
DROP TABLE IF EXISTS `agentqueueskill`;
CREATE TABLE `agentqueueskill` (
 `agentid` int(10) unsigned,
 `skillid` int(10) unsigned,
 `weight` int(3) unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(`agentid`, `skillid`)
);

CREATE INDEX `agentqueueskill__idx__agentid` ON `agentqueueskill`(`agentid`);


DROP TABLE IF EXISTS `callerid`;
CREATE TABLE `callerid` (
 `mode` enum('prepend','overwrite','append'),
 `callerdisplay` varchar(80) NOT NULL DEFAULT '',
 `type` enum('callfilter','incall','group','queue') NOT NULL,
 `typeval` int(10) unsigned NOT NULL,
 PRIMARY KEY(`type`,`typeval`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS `callfilter`;
CREATE TABLE `callfilter` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(128) NOT NULL DEFAULT '',
 `context` varchar(39) NOT NULL,
 `type` enum('bosssecretary') NOT NULL,
 `bosssecretary` enum('bossfirst-serial','bossfirst-simult','secretary-serial','secretary-simult','all'),
 `callfrom` enum('internal','external','all') NOT NULL,
 `ringseconds` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `callfilter__idx__context` ON `callfilter`(`context`);
CREATE INDEX `callfilter__idx__type` ON `callfilter`(`type`);
CREATE INDEX `callfilter__idx__bosssecretary` ON `callfilter`(`bosssecretary`);
CREATE INDEX `callfilter__idx__callfrom` ON `callfilter`(`callfrom`);
CREATE INDEX `callfilter__idx__commented` ON `callfilter`(`commented`);
CREATE UNIQUE INDEX `callfilter__uidx__name` ON `callfilter`(`name`);


DROP TABLE IF EXISTS `callfiltermember`;
CREATE TABLE `callfiltermember` (
 `id` int(10) unsigned auto_increment,
 `callfilterid` int(10) unsigned NOT NULL DEFAULT 0,
 `type` enum('user') NOT NULL,
 `typeval` varchar(128) NOT NULL DEFAULT 0,
 `ringseconds` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `priority` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `bstype` enum('boss','secretary'),
 `active` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `callfiltermember__idx__priority` ON `callfiltermember`(`priority`);
CREATE INDEX `callfiltermember__idx__bstype` ON `callfiltermember`(`bstype`);
CREATE INDEX `callfiltermember__idx__active` ON `callfiltermember`(`active`);
CREATE UNIQUE INDEX `callfiltermember__uidx__callfilterid_type_typeval` ON `callfiltermember`(`callfilterid`,`type`,`typeval`);


DROP TABLE IF EXISTS `cdr`;
CREATE TABLE `cdr` (
 `id` int(10) unsigned auto_increment,
 `calldate` timestamp DEFAULT 0,
 `clid` varchar(80) NOT NULL DEFAULT '',
 `src` varchar(80) NOT NULL DEFAULT '',
 `dst` varchar(80) NOT NULL DEFAULT '',
 `dcontext` varchar(39) NOT NULL DEFAULT '',
 `channel` varchar(80) NOT NULL DEFAULT '',
 `dstchannel` varchar(80) NOT NULL DEFAULT '',
 `lastapp` varchar(80) NOT NULL DEFAULT '',
 `lastdata` varchar(80) NOT NULL DEFAULT '',
 `answer` timestamp DEFAULT 0,
 `end` timestamp DEFAULT 0,
 `duration` int(10) unsigned NOT NULL DEFAULT 0,
 `billsec` int(10) unsigned NOT NULL DEFAULT 0,
 `disposition` varchar(9) NOT NULL DEFAULT '',
 `amaflags` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `accountcode` varchar(20) NOT NULL DEFAULT '',
 `uniqueid` varchar(32) NOT NULL DEFAULT '',
 `userfield` varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `cdr__idx__calldate` ON `cdr`(`calldate`);
CREATE INDEX `cdr__idx__clid` ON `cdr`(`clid`);
CREATE INDEX `cdr__idx__src` ON `cdr`(`src`);
CREATE INDEX `cdr__idx__dst` ON `cdr`(`dst`);
CREATE INDEX `cdr__idx__channel` ON `cdr`(`channel`);
CREATE INDEX `cdr__idx__dstchannel` ON `cdr`(`dstchannel`);
CREATE INDEX `cdr__idx__duration` ON `cdr`(`duration`);
CREATE INDEX `cdr__idx__disposition` ON `cdr`(`disposition`);
CREATE INDEX `cdr__idx__amaflags` ON `cdr`(`amaflags`);
CREATE INDEX `cdr__idx__accountcode` ON `cdr`(`accountcode`);
CREATE INDEX `cdr__idx__userfield` ON `cdr`(`userfield`);


DROP TABLE IF EXISTS `context`;
CREATE TABLE `context` (
 `name` varchar(39) NOT NULL,
 `displayname` varchar(128) NOT NULL DEFAULT '',
 `entity` varchar(64),
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `context__idx__displayname` ON `context`(`displayname`);
CREATE INDEX `context__idx__entity` ON `context`(`entity`);
CREATE INDEX `context__idx__commented` ON `context`(`commented`);


DROP TABLE IF EXISTS `contextinclude`;
CREATE TABLE `contextinclude` (
 `context` varchar(39) NOT NULL,
 `include` varchar(39) NOT NULL,
 `priority` tinyint(2) unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(`context`,`include`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `contextinclude__idx__context` ON `contextinclude`(`context`);
CREATE INDEX `contextinclude__idx__include` ON `contextinclude`(`include`);
CREATE INDEX `contextinclude__idx__priority` ON `contextinclude`(`priority`);


DROP TABLE IF EXISTS `contextmember`;
CREATE TABLE `contextmember` (
 `context` varchar(39) NOT NULL,
 `type` varchar(32) NOT NULL,
 `typeval` varchar(128) NOT NULL DEFAULT '',
 `varname` varchar(128) NOT NULL DEFAULT '',
 PRIMARY KEY(`context`,`type`,`typeval`,`varname`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `contextmember__idx__context` ON `contextmember`(`context`);
CREATE INDEX `contextmember__idx__context_type` ON `contextmember`(`context`,`type`);


DROP TABLE IF EXISTS `contextnumbers`;
CREATE TABLE `contextnumbers` (
 `context` varchar(39) NOT NULL,
 `type` enum('user','group','queue','meetme','incall') NOT NULL,
 `numberbeg` varchar(16) NOT NULL DEFAULT '',
 `numberend` varchar(16) NOT NULL DEFAULT '',
 `didlength` tinyint(2) unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(`context`,`type`,`numberbeg`,`numberend`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `contextnumbers__idx__context_type` ON `contextnumbers`(`context`,`type`);


DROP TABLE IF EXISTS `contextnummember`;
CREATE TABLE `contextnummember` (
 `context` varchar(39) NOT NULL,
 `type` enum('user','group','queue','meetme','incall') NOT NULL,
 `typeval` varchar(128) NOT NULL DEFAULT 0,
 `number` varchar(40) NOT NULL DEFAULT '',
 PRIMARY KEY(`context`,`type`,`typeval`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `contextnummember__idx__context` ON `contextnummember`(`context`);
CREATE INDEX `contextnummember__idx__context_type` ON `contextnummember`(`context`,`type`);
CREATE INDEX `contextnummember__idx__number` ON `contextnummember`(`number`);


DROP TABLE IF EXISTS `cticontexts`;
CREATE TABLE `cticontexts` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(50),
 `directories` text NOT NULL,
 `display` text NOT NULL,
 `description` text NOT NULL,
 `deletable` tinyint(1),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO `cticontexts` VALUES(3,'default','xivodir,internal','Display','Contexte par défaut',1);


DROP TABLE IF EXISTS `ctidirectories`;
CREATE TABLE `ctidirectories` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(255),
 `uri` varchar(255),
 `delimiter` varchar(20),
 `match_direct` text NOT NULL,
 `match_reverse` text NOT NULL,
 `field_phone` text NOT NULL,
 `field_fullname` varchar(255),
 `field_company` varchar(255),
 `field_mail` varchar(255),
 `field_firstname` varchar(255),
 `field_lastname` varchar(255),
 `display_reverse` varchar(255),
 `description` varchar(255),
 `deletable` tinyint(1),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO `ctidirectories` VALUES(4,'xivodir','phonebook','phonebook','["phonebook.firstname","phonebook.lastname","phonebook.displayname","phonebook.society","phonebooknumber.office.number"]','["phonebooknumber.office.number","phonebooknumber.mobile.number"]','["phonebooknumber.office.number"]','["phonebook.fullname"]','["phonebook.society"]','["phonebook.email"]','["phonebook.firstname"]','["phonebook.lastname"]','["{db-fullname}"]','Répertoire XiVO Externe',1);
INSERT INTO `ctidirectories` VALUES(5,'internal','internal','','','','','["{internal-fullname}"]','','','','','','Répertoire XiVO Interne',1);


DROP TABLE IF EXISTS `ctidisplays`;
CREATE TABLE `ctidisplays` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(50),
 `data` text NOT NULL,
 `deletable` tinyint(1),
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO `ctidisplays` VALUES(4,'Display','{"10": [ "Numéro","phone","","{db-phone}" ],"20": [ "Nom","","","{db-fullname}" ],"30": [ "Entreprise","","Inconnue","{db-company}" ],"40": [ "E-mail","","","{db-mail}({xivo-directory})" ]}',1,'Affichage par défaut');


DROP TABLE IF EXISTS `ctimain`;
CREATE TABLE `ctimain` (
 `id` int(10) unsigned auto_increment,
 `commandset` varchar(20),
 `fagi_ip` varchar(255),
 `fagi_port` int(10) unsigned,
 `cti_ip` varchar(255),
 `cti_port` int(10) unsigned,
 `webi_ip` varchar(255),
 `webi_port` int(10) unsigned,
 `info_ip` varchar(255),
 `info_port` int(10) unsigned,
 `announce_ip` varchar(255),
 `announce_port` int(10) unsigned,
 `asterisklist` varchar(255),
 `updates_period` int(10) unsigned,
 `socket_timeout` int(10) unsigned,
 `login_timeout` int(10) unsigned,
 `parting_astid_context` varchar(255),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO `ctimain` VALUES(1, 'xivocti', '0.0.0.0', 5002, '0.0.0.0', 5003, '127.0.0.1', 5004, '127.0.0.1', 5005, '127.0.0.1', 5006, 1, 60, 10, 5, 'context');


DROP TABLE IF EXISTS `ctiphonehints`;
CREATE TABLE `ctiphonehints` (
 `id` int(10) unsigned auto_increment,
 `number` integer,
 `name` varchar(255),
 `color` varchar(128),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO `ctiphonehints` VALUES(1,-2,'Inexistant','#030303');
INSERT INTO `ctiphonehints` VALUES(2,-1,'Désactivé','#000000');
INSERT INTO `ctiphonehints` VALUES(3,0,'Disponible','#0DFF25');
INSERT INTO `ctiphonehints` VALUES(4,1,'En ligne OU appelle','#FF032D');
INSERT INTO `ctiphonehints` VALUES(5,2,'Occupé','#FF0008');
INSERT INTO `ctiphonehints` VALUES(6,4,'Indisponible','#FFFFFF');
INSERT INTO `ctiphonehints` VALUES(7,8,'Sonne','#1B0AFF');
INSERT INTO `ctiphonehints` VALUES(8,9,'(En Ligne OU Appelle) ET Sonne','#FF0526');
INSERT INTO `ctiphonehints` VALUES(9,16,'En Attente','#F7FF05');


DROP TABLE IF EXISTS `ctipresences`;
CREATE TABLE `ctipresences` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(255),
 `description` varchar(255),
 `deletable` tinyint(1),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO `ctipresences` VALUES(1,'xivo','De base non supprimable',0);


DROP TABLE IF EXISTS `ctiprofiles`;
CREATE TABLE `ctiprofiles` (
 `id` int(10) unsigned auto_increment,
 `xlets` text,
 `funcs` varchar(255),
 `maxgui` integer,
 `appliname` varchar(255),
 `name` varchar(40) unique,
 `presence` varchar(255),
 `services` varchar(255),
 `preferences` varchar(2048),
 `deletable` tinyint(1),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO `ctiprofiles` VALUES(9,'[[ "queues", "dock", "fms", "N/A" ],[ "queuedetails", "dock", "fms", "N/A" ],[ "queueentrydetails", "dock", "fcms", "N/A" ],[ "agents", "dock", "fcms", "N/A" ],[ "agentdetails", "dock", "fcms", "N/A" ],[ "identity", "grid", "fcms", "0" ],[ "conference", "dock", "fcm", "N/A" ]]','agents,presence,switchboard',-1,'Superviseur','agentsup','xivo','','',1);
INSERT INTO `ctiprofiles` VALUES(10,'[[ "queues", "dock", "ms", "N/A" ],[ "identity", "grid", "fcms", "0" ],[ "customerinfo", "dock", "cms", "N/A" ],[ "agentdetails", "dock", "cms", "N/A" ]]','presence',-1,'Agent','agent','xivo','','',1);
INSERT INTO `ctiprofiles` VALUES(11,'[[ "tabber", "grid", "fcms", "N/A" ],[ "dial", "grid", "fcms", "2" ],[ "search", "tab", "fcms", "0" ],[ "customerinfo", "tab", "fcms", "4" ],[ "identity", "grid", "fcms", "0" ],[ "fax", "tab", "fcms", "N/A" ],[ "history", "tab", "fcms", "N/A" ],[ "directory", "tab", "fcms", "N/A" ],[ "features", "tab", "fcms", "N/A" ],[ "mylocaldir", "tab", "fcms", "N/A" ],[ "conference", "tab", "fcms", "N/A" ]]','presence,customerinfo',-1,'Client','client','xivo','','',1);
INSERT INTO `ctiprofiles` VALUES(12,'[[ "tabber", "grid", "fcms", "N/A" ],[ "dial", "grid", "fcms", "2" ],[ "search", "tab", "fcms", "0" ],[ "customerinfo", "tab", "fcms", "4" ],[ "identity", "grid", "fcms", "0" ],[ "fax", "tab", "fcms", "N/A" ],[ "history", "tab", "fcms", "N/A" ],[ "directory", "tab", "fcms", "N/A" ],[ "features", "tab", "fcms", "N/A" ],[ "mylocaldir", "tab", "fcms", "N/A" ],[ "conference", "tab", "fcms", "N/A" ],[ "outlook", "tab", "fcms", "N/A" ]]','presence,customerinfo',-1,'Client+Outlook','clientoutlook','xivo','','',1);
INSERT INTO `ctiprofiles` VALUES(13,'[[ "datetime", "dock", "fm", "N/A" ]]','',-1,'Horloge','clock','xivo','','',1);
INSERT INTO `ctiprofiles` VALUES(14,'[[ "dial", "dock", "fm", "N/A" ],[ "operator", "dock", "fcm", "N/A" ],[ "datetime", "dock", "fcm", "N/A" ],[ "identity", "grid", "fcms", "0" ],[ "calls", "dock", "fcm", "N/A" ],[ "parking", "dock", "fcm", "N/A" ],[ "calls", "dock", "fcm", "N/A" ]]','presence,switchboard,search,dial',-1,'Opérateur','oper','xivo','','',1);
INSERT INTO `ctiprofiles` VALUES(15,'[[ "parking", "dock", "fcms", "N/A" ],[ "search", "dock", "fcms", "N/A" ],[ "calls", "dock", "fcms", "N/A" ],[ "switchboard", "dock", "fcms", "N/A" ],[ "customerinfo", "dock", "fcms", "N/A" ],[ "datetime", "dock", "fcms", "N/A" ],[ "dial", "dock", "fcms", "N/A" ],[ "identity", "grid", "fcms", "0" ],[ "messages", "dock", "fcms", "N/A" ],[ "operator", "dock", "fcms", "N/A" ]]','switchboard,dial,presence,customerinfo,search,agents,conference,directory,features,history,fax,chitchat,database','','Switchboard','switchboard','xivo','','',1);


DROP TABLE IF EXISTS `ctireversedirectories`;
CREATE TABLE `ctireversedirectories` (
 `id` int(10) unsigned auto_increment,
 `number` varchar(50),
 `directories` text NOT NULL,
 `description` text NOT NULL,
 `deletable` tinyint(1),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO `ctireversedirectories` VALUES(1,'default','["xivodir","internal"]','Répertoires XiVO',1);


DROP TABLE IF EXISTS `ctisheetactions`;
CREATE TABLE `ctisheetactions` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(50),
 `description` text NOT NULL,
 `context` varchar(50),
 `whom` varchar(50),
 `capaids` text NOT NULL,
 `sheet_info` varchar(50),
 `systray_info` varchar(50),
 `sheet_qtui` varchar(50),
 `action_info` varchar(50),
 `focus` tinyint(1),
 `deletable` tinyint(1),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO `ctisheetactions` VALUES(6,'dial','sheet_action_dial','["default"]','dest','["agentsup","agent","client"]','{"10": [ "","text","Inconnu","Appel {xivo-direction} de {xivo-calleridnum}" ],"20": [ "Numéro entrant","phone","Inconnu","{xivo-calleridnum}" ],"30": [ "Nom","text","Inconnu","{db-fullname}" ],"40": [ "Numéro appelé","phone","Inconnu","{xivo-calledidnum}" ]}','{"10": [ "","title","","Appel {xivo-direction}" ],"20": [ "","body","Inconnu","appel de {xivo-calleridnum} pour {xivo-calledidnum}" ],"30": [ "","body","Inconnu","{db-fullname} (selon {xivo-directory})" ],"40": [ "","body","","le {xivo-date}, il est {xivo-time}" ]}','','{"10": [ "","urlauto","","http://www.google.fr/search?q={xivo-calleridnum}" ]}','',1);
INSERT INTO `ctisheetactions` VALUES(7,'queue','sheet_action_queue','["default"]','dest','["agentsup","agent","client"]','{"10": [ "","text","Inconnu","Appel {xivo-direction} de la File {xivo-queuename}" ],"20": [ "Numéro entrant","phone","Inconnu","{xivo-calleridnum}" ],"30": [ "Nom","text","Inconnu","{db-fullname}" ]}','{"10": [ "","title","","Appel {xivo-direction} de la File {xivo-queuename}" ],"20": [ "","body","Inconnu","appel de {xivo-calleridnum} pour {xivo-calledidnum}" ],"30": [ "","body","Inconnu","{db-fullname} (selon {xivo-directory})" ],"40": [ "","body","","le {xivo-date}, il est {xivo-time}" ]}','file:///etc/pf-xivo/ctiservers/form.ui','{}','',1);
INSERT INTO `ctisheetactions` VALUES(8,'custom1','sheet_action_custom1','["default"]','all','["agentsup","agent","client"]','{"10": [ "","text","Inconnu","Appel {xivo-direction} (Custom)" ],"20": [ "Numéro entrant","phone","Inconnu","{xivo-calleridnum}" ],"30": [ "Nom","text","Inconnu","{db-fullname}" ]}','{"10": [ "","title","","Appel {xivo-direction} (Custom)" ],"20": [ "","body","Inconnu","appel de {xivo-calleridnum} pour {xivo-calledidnum}" ],"30": [ "","body","Inconnu","{db-fullname} (selon {xivo-directory})" ],"40": [ "","body","","le {xivo-date}, il est {xivo-time}" ]}','','{}','',1);


DROP TABLE IF EXISTS `ctisheetevents`;
CREATE TABLE `ctisheetevents` (
 `id` int(10) unsigned auto_increment,
 `agentlinked` varchar(50),
 `agentunlinked` varchar(50),
 `faxreceived` varchar(50),
 `incomingqueue` varchar(50),
 `incominggroup` varchar(50),
 `incomingdid` varchar(50),
 `dial` varchar(50),
 `link` varchar(50),
 `unlink` varchar(50),
 `custom` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO `ctisheetevents` VALUES(1,'','','','','','','dial','','','{"custom-example1": "custom1"}');


DROP TABLE IF EXISTS `ctistatus`;
CREATE TABLE `ctistatus` (
 `id` int(10) unsigned auto_increment,
 `presence_id` int(10) unsigned,
 `name` varchar(255),
 `display_name` varchar(255),
 `actions` varchar(255),
 `color` varchar(20),
 `access_status` varchar(255),
 `deletable` tinyint(1),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO `ctistatus` VALUES(1,1,'available','Disponible','enablednd(false)','#08FD20','1,2,6',1);
INSERT INTO `ctistatus` VALUES(2,1,'away','Parti loin','enablednd(true)','#FDE50A','1,6',1);
INSERT INTO `ctistatus` VALUES(6,1,'outtolunch','Parti Manger','enablednd(true)','#001AFF','1,2,6',1);
INSERT INTO `ctistatus` VALUES(7,2,'sdsds','dsd','','#FFFFFF','',1);
INSERT INTO `ctistatus` VALUES(8,2,'outtolunch','sdf','','#FFFFFF','',1);
INSERT INTO `ctistatus` VALUES(9,3,'outtolunch','Miam','','#FFB163','',1);
INSERT INTO `ctistatus` VALUES(10,1,'donotdisturb','Ne pas déranger','enablednd(true)','#FF032D','1,2,6',1);
INSERT INTO `ctistatus` VALUES(11,1,'berightback','Bientôt de retour','enablednd(true)','#FFB545','1,2,6,10',1);


DROP TABLE IF EXISTS `dialaction`;
CREATE TABLE `dialaction` (
 `event` enum('answer',
              'noanswer',
              'congestion',
              'busy',
              'chanunavail',
              'inschedule',
              'outschedule') NOT NULL,
 `category` enum('callfilter','group','incall','queue','schedule','user') NOT NULL,
 `categoryval` varchar(128) NOT NULL DEFAULT '',
 `action` enum('none',
               'endcall:busy',
               'endcall:congestion',
               'endcall:hangup',
               'user',
               'group',
               'queue',
               'meetme',
               'voicemail',
               'schedule',
               'voicemenu',
               'extension',
               'application:callbackdisa',
               'application:disa',
               'application:directory',
               'application:faxtomail',
               'application:voicemailmain',
               'sound',
               'custom') NOT NULL,
 `actionarg1` varchar(255) NOT NULL DEFAULT '',
 `actionarg2` varchar(255) NOT NULL DEFAULT '',
 `linked` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`event`,`category`,`categoryval`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `dialaction__idx__action_actionarg1` ON `dialaction`(`action`,`actionarg1`);
CREATE INDEX `dialaction__idx__actionarg2` ON `dialaction`(`actionarg2`);
CREATE INDEX `dialaction__idx__linked` ON `dialaction`(`linked`);


DROP TABLE IF EXISTS `extensions`;
CREATE TABLE `extensions` (
 `id` int(10) unsigned auto_increment,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `context` varchar(39) NOT NULL DEFAULT '',
 `exten` varchar(40) binary NOT NULL DEFAULT '',
 `priority` tinyint unsigned NOT NULL DEFAULT 0,
 `app` varchar(128) NOT NULL DEFAULT '',
 `appdata` varchar(128) NOT NULL DEFAULT '',
 `name` varchar(128) NOT NULL DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `extensions__idx__commented` ON `extensions`(`commented`);
CREATE INDEX `extensions__idx__context_exten_priority` ON `extensions`(`context`,`exten`,`priority`);
CREATE INDEX `extensions__idx__name` ON `extensions`(`name`);

INSERT INTO `extensions` VALUES (NULL,1,'xivo-features','_*33.',1,'Macro','agentdynamiclogin|${EXTEN:3}','agentdynamiclogin');
INSERT INTO `extensions` VALUES (NULL,1,'xivo-features','_*31.',1,'Macro','agentstaticlogin|${EXTEN:3}','agentstaticlogin');
INSERT INTO `extensions` VALUES (NULL,1,'xivo-features','_*32.',1,'Macro','agentstaticlogoff|${EXTEN:3}','agentstaticlogoff');
INSERT INTO `extensions` VALUES (NULL,1,'xivo-features','_*30.',1,'Macro','agentstaticlogtoggle|${EXTEN:3}','agentstaticlogtoggle');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*37.',1,'Macro','bsfilter|${EXTEN:3}','bsfilter');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*664.',1,'Macro','group|${EXTEN:4}|','callgroup');
INSERT INTO `extensions` VALUES (NULL,1,'xivo-features','*34',1,'Macro','calllistening','calllistening');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*667.',1,'Macro','meetme|${EXTEN:4}|','callmeetme');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*665.',1,'Macro','queue|${EXTEN:4}|','callqueue');
INSERT INTO `extensions` VALUES (NULL,1,'xivo-features','*26',1,'Macro','callrecord','callrecord');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*666.',1,'Macro','user|${EXTEN:4}|','calluser');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','*36',1,'Directory','${CONTEXT}','directoryaccess');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','*25',1,'Macro','enablednd','enablednd');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','*90',1,'Macro','enablevm','enablevm');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','*91',1,'Macro','enablevmbox','enablevmbox');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*91.',1,'Macro','enablevmbox|${EXTEN:3}','enablevmboxslt');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*90.',1,'Macro','enablevm|${EXTEN:3}','enablevmslt');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*23.',1,'Macro','feature_forward|busy|${EXTEN:3}','fwdbusy');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*22.',1,'Macro','feature_forward|rna|${EXTEN:3}','fwdrna');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*21.',1,'Macro','feature_forward|unc|${EXTEN:3}','fwdunc');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','*20',1,'Macro','fwdundoall','fwdundoall');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*51.',1,'Macro','groupmember|group|add|${EXTEN:3}','groupaddmember');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*52.',1,'Macro','groupmember|group|remove|${EXTEN:3}','groupremovemember');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*50.',1,'Macro','groupmember|group|toggle|${EXTEN:3}','grouptogglemember');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','*48378',1,'Macro','guestprov','guestprov');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','*27',1,'Macro','incallfilter','incallfilter');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','*10',1,'Macro','phonestatus','phonestatus');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*735.',1,'Macro','phoneprogfunckey|${EXTEN:0:4}|${EXTEN:4}','phoneprogfunckey');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*8.',1,'Pickup','${EXTEN:2}%${CONTEXT}@PICKUPMARK','pickup');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*56.',1,'Macro','groupmember|queue|add|${EXTEN:3}','queueaddmember');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*57.',1,'Macro','groupmember|queue|remove|${EXTEN:3}','queueremovemember');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*55.',1,'Macro','groupmember|queue|toggle|${EXTEN:3}','queuetogglemember');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','*9',1,'Macro','recsnd|wav','recsnd');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*99.',1,'Macro','vmboxmsg|${EXTEN:3}','vmboxmsgslt');
INSERT INTO `extensions` VALUES (NULL,1,'xivo-features','_*93.',1,'Macro','vmboxpurge|${EXTEN:3}','vmboxpurgeslt');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*97.',1,'Macro','vmbox|${EXTEN:3}','vmboxslt');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','*98',1,'Macro','vmusermsg','vmusermsg');
INSERT INTO `extensions` VALUES (NULL,1,'xivo-features','*92',1,'Macro','vmuserpurge','vmuserpurge');
INSERT INTO `extensions` VALUES (NULL,1,'xivo-features','_*92.',1,'Macro','vmuserpurge|${EXTEN:3}','vmuserpurgeslt');
INSERT INTO `extensions` VALUES (NULL,0,'xivo-features','_*96.',1,'Macro','vmuser|${EXTEN:3}','vmuserslt');


DROP TABLE IF EXISTS `extenumbers`;
CREATE TABLE `extenumbers` (
 `id` int(10) unsigned auto_increment,
 `exten` varchar(40) NOT NULL DEFAULT '',
 `extenhash` char(40) NOT NULL DEFAULT '',
 `context` varchar(39) NOT NULL,
 `type` enum('extenfeatures',
             'featuremap',
             'generalfeatures',
             'group',
             'handynumbers',
             'incall',
             'meetme',
             'outcall',
             'queue',
             'user',
             'voicemenu') NOT NULL,
 `typeval` varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `extenumbers__idx__exten` ON `extenumbers`(`exten`);
CREATE INDEX `extenumbers__idx__extenhash` ON `extenumbers`(`extenhash`);
CREATE INDEX `extenumbers__idx__context` ON `extenumbers`(`context`);
CREATE INDEX `extenumbers__idx__type` ON `extenumbers`(`type`);
CREATE INDEX `extenumbers__idx__typeval` ON `extenumbers`(`typeval`);

INSERT INTO `extenumbers` VALUES (NULL,'*8','aa5820277564fac26df0e3dc72f796407597721d','','generalfeatures','pickupexten');
INSERT INTO `extenumbers` VALUES (NULL,'700','d8e4bbea3af2e4861ad5a445aaec573e02f9aca2','','generalfeatures','parkext');
INSERT INTO `extenumbers` VALUES (NULL,'*0','e914c907ff6d7a8ffefae72fe47363726b39d112','','featuremap','disconnect');
INSERT INTO `extenumbers` VALUES (NULL,'*1','8e04e82b8798d3979eded4ca2afdda3bebcb963d','','featuremap','blindxfer');
INSERT INTO `extenumbers` VALUES (NULL,'*2','6951109c8ca021277336cc2c8f6ac7f47d3b30e9','','featuremap','atxfer');
INSERT INTO `extenumbers` VALUES (NULL,'*3','68631b4b53ba2a27a969ca63bdcdc00805c2c258','','featuremap','automon');
INSERT INTO `extenumbers` VALUES (NULL,'_*33.','269371911e5bac9176919fa42e66814882c496e1','','extenfeatures','agentdynamiclogin');
INSERT INTO `extenumbers` VALUES (NULL,'_*31.','678fe23ee0d6aa64460584bebbed210e270d662f','','extenfeatures','agentstaticlogin');
INSERT INTO `extenumbers` VALUES (NULL,'_*32.','3ae0f1ff0ef4907faa2dad5da7bb891c9dbf45ad','','extenfeatures','agentstaticlogoff');
INSERT INTO `extenumbers` VALUES (NULL,'_*30.','7758898081b262cc0e42aed23cf601fba8969b08','','extenfeatures','agentstaticlogtoggle');
INSERT INTO `extenumbers` VALUES (NULL,'_*37.','249b00b17a5983bbb2af8ed0af2ab1a74abab342','','extenfeatures','bsfilter');
INSERT INTO `extenumbers` VALUES (NULL,'_*664.','9dfe780f1dc7fccbfc841b41a38933d4dab56369','','extenfeatures','callgroup');
INSERT INTO `extenumbers` VALUES (NULL,'*34','668a8d2d8fe980b663e2cdcecb977860e1b272f3','','extenfeatures','calllistening');
INSERT INTO `extenumbers` VALUES (NULL,'_*667.','666f6f18439eb7f205b5932d7f9aef6d2e5ba9a3','','extenfeatures','callmeetme');
INSERT INTO `extenumbers` VALUES (NULL,'_*665.','7e2df45aedebded219eaa5fb84d6db7e8e24fc66','','extenfeatures','callqueue');
INSERT INTO `extenumbers` VALUES (NULL,'*26','f8aeb70618cc87f1143c7dff23cdc0d3d0a48a0c','','extenfeatures','callrecord');
INSERT INTO `extenumbers` VALUES (NULL,'_*666.','d7b68f456ddb50215670c5bfca921176a21c4270','','extenfeatures','calluser');
INSERT INTO `extenumbers` VALUES (NULL,'*36','f9b69fe3c361ddfc2ae49e048460ea197ea850c8','','extenfeatures','directoryaccess');
INSERT INTO `extenumbers` VALUES (NULL,'*25','c0d236c38bf8d5d84a2e154203cd2a18b86c6b2a','','extenfeatures','enablednd');
INSERT INTO `extenumbers` VALUES (NULL,'*90','2fc9fcda52bd8293da1bfa68cbdb8974fafd409e','','extenfeatures','enablevm');
INSERT INTO `extenumbers` VALUES (NULL,'*91','880d3330b465056ede825e1fbc8ceb50fd816e1d','','extenfeatures','enablevmbox');
INSERT INTO `extenumbers` VALUES (NULL,'_*91.','936ec7abe6019d9d47d8be047ef6fc0ebc334c00','','extenfeatures','enablevmboxslt');
INSERT INTO `extenumbers` VALUES (NULL,'_*90.','9fdaa61ea338dcccf1450949cbf6f7f99f1ccc54','','extenfeatures','enablevmslt');
INSERT INTO `extenumbers` VALUES (NULL,'_*23.','a1968a70f1d265b8aa263e73c79259961c4f7bbb','','extenfeatures','fwdbusy');
INSERT INTO `extenumbers` VALUES (NULL,'_*22.','00638af9e028d4cd454c00f43caf5626baa7d84c','','extenfeatures','fwdrna');
INSERT INTO `extenumbers` VALUES (NULL,'_*21.','52c97d56ebcca524ccf882590e94c52f6db24649','','extenfeatures','fwdunc');
INSERT INTO `extenumbers` VALUES (NULL,'*20','934aca632679075488681be0e9904cf9102f8766','','extenfeatures','fwdundoall');
INSERT INTO `extenumbers` VALUES (NULL,'_*51.','fd3d50358d246ab2fbc32e14056e2f559d054792','','extenfeatures','groupaddmember');
INSERT INTO `extenumbers` VALUES (NULL,'_*52.','069a278d266d0cf2aa7abf42a732fc5ad109a3e6','','extenfeatures','groupremovemember');
INSERT INTO `extenumbers` VALUES (NULL,'_*50.','53f7e7fa7fbbabb1245ed8dedba78da442a8659f','','extenfeatures','grouptogglemember');
INSERT INTO `extenumbers` VALUES (NULL,'*48378','e27276ceefcc71a5d2def28c9b59a6410959eb43','','extenfeatures','guestprov');
INSERT INTO `extenumbers` VALUES (NULL,'*27','663b9615ba92c21f80acac52d60b28a8d1fb1c58','','extenfeatures','incallfilter');
INSERT INTO `extenumbers` VALUES (NULL,'_*735.','32e9b3597f8b9cd2661f0c3d3025168baafca7e6','','extenfeatures','phoneprogfunckey');
INSERT INTO `extenumbers` VALUES (NULL,'*10','eecefbd85899915e6fc2ff5a8ea44c2c83597cd6','','extenfeatures','phonestatus');
INSERT INTO `extenumbers` VALUES (NULL,'_*8.','b349d094036a97a7e0631ba60de759a9597c1c3a','','extenfeatures','pickup');
INSERT INTO `extenumbers` VALUES (NULL,'_*56.','95d84232b10af6f6905dcd22f4261a4550461c7d','','extenfeatures','queueaddmember');
INSERT INTO `extenumbers` VALUES (NULL,'_*57.','3ad1e945e85735f6417e5a0aba7fde3bc9d2ffec','','extenfeatures','queuetogglemember');
INSERT INTO `extenumbers` VALUES (NULL,'_*55.','f8085e23f56e5433006483dee5fe3db8c94a0a06','','extenfeatures','queueremovemember');
INSERT INTO `extenumbers` VALUES (NULL,'*9','e28d0f359da60dcf86340435478b19388b1b1d05','','extenfeatures','recsnd');
INSERT INTO `extenumbers` VALUES (NULL,'_*99.','6c92223f2ea0cfd9fad3db2f288ebdc9c64dc8f5','','extenfeatures','vmboxmsgslt');
INSERT INTO `extenumbers` VALUES (NULL,'_*93.','7d891f90799fd6cb5bc85c4bd227a3357096be8f','','extenfeatures','vmboxpurgeslt');
INSERT INTO `extenumbers` VALUES (NULL,'_*97.','8bdbf6703cf5225aad457422afdda738b9bd628c','','extenfeatures','vmboxslt');
INSERT INTO `extenumbers` VALUES (NULL,'*98','6fb653e9eaf6f4d9c8d2cb48d1a6e3f4d4085710','','extenfeatures','vmusermsg');
INSERT INTO `extenumbers` VALUES (NULL,'*92','97f991a4ffd7fa843bc0ca3bdc730851382c5cdf','','extenfeatures','vmuserpurge');
INSERT INTO `extenumbers` VALUES (NULL,'_*92.','36711086667cbfc27488236e0e0fdd2d7f896f6b','','extenfeatures','vmuserpurgeslt');
INSERT INTO `extenumbers` VALUES (NULL,'_*96.','ac6c7ac899867fe0120fe20120fae163012615f2','','extenfeatures','vmuserslt');


DROP TABLE IF EXISTS `features`;
CREATE TABLE `features` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(255),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `features__idx__commented` ON `features`(`commented`);
CREATE INDEX `features__idx__filename` ON `features`(`filename`);
CREATE INDEX `features__idx__category` ON `features`(`category`);
CREATE INDEX `features__idx__var_name` ON `features`(`var_name`);

INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','parkext','700');
INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','context','parkedcalls');
INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','parkingtime','45');
INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','parkpos','701-750');
INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','parkfindnext','no');
INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','adsipark','no');
INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','transferdigittimeout','3');
INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','featuredigittimeout','500');
INSERT INTO `features` VALUES (NULL,0,0,1,'features.conf','general','courtesytone',NULL);
INSERT INTO `features` VALUES (NULL,0,0,1,'features.conf','general','xfersound',NULL);
INSERT INTO `features` VALUES (NULL,0,0,1,'features.conf','general','xferfailsound',NULL);
INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','pickupexten','*8');
INSERT INTO `features` VALUES (NULL,1,0,0,'features.conf','featuremap','blindxfer','*1');
INSERT INTO `features` VALUES (NULL,1,0,0,'features.conf','featuremap','atxfer','*2');
INSERT INTO `features` VALUES (NULL,1,0,0,'features.conf','featuremap','automon','*3');
INSERT INTO `features` VALUES (NULL,1,0,0,'features.conf','featuremap','disconnect','*0');


DROP TABLE IF EXISTS `groupfeatures`;
CREATE TABLE `groupfeatures` (
 `id` tinyint(2) unsigned auto_increment,
 `name` varchar(128) NOT NULL,
 `number` varchar(40) NOT NULL DEFAULT '',
 `context` varchar(39) NOT NULL,
 `transfer_user` tinyint(1) NOT NULL DEFAULT 0,
 `transfer_call` tinyint(1) NOT NULL DEFAULT 0,
 `write_caller` tinyint(1) NOT NULL DEFAULT 0,
 `write_calling` tinyint(1) NOT NULL DEFAULT 0,
 `timeout` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `preprocess_subroutine` varchar(39),
 `deleted` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `groupfeatures__idx__name` ON `groupfeatures`(`name`);
CREATE INDEX `groupfeatures__idx__number` ON `groupfeatures`(`number`);
CREATE INDEX `groupfeatures__idx__context` ON `groupfeatures`(`context`);
CREATE INDEX `groupfeatures__idx__deleted` ON `groupfeatures`(`deleted`);


DROP TABLE IF EXISTS `handynumbers`;
CREATE TABLE `handynumbers` (
 `id` int(10) unsigned auto_increment,
 `exten` varchar(40) NOT NULL DEFAULT '',
 `trunkfeaturesid` int(10) unsigned NOT NULL DEFAULT 0,
 `type` enum('emergency','special') NOT NULL,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `handynumbers__idx__trunkfeaturesid` ON `handynumbers`(`trunkfeaturesid`);
CREATE INDEX `handynumbers__idx__type` ON `handynumbers`(`type`);
CREATE INDEX `handynumbers__idx__commented` ON `handynumbers`(`commented`);
CREATE UNIQUE INDEX `handynumbers__uidx__exten` ON `handynumbers`(`exten`);


DROP TABLE IF EXISTS `incall`;
CREATE TABLE `incall` (
 `id` int(10) unsigned auto_increment,
 `exten` varchar(40) NOT NULL,
 `context` varchar(39) NOT NULL,
 `preprocess_subroutine` varchar(39),
 `faxdetectenable` tinyint(1) NOT NULL DEFAULT 0,
 `faxdetecttimeout` tinyint(2) unsigned NOT NULL DEFAULT 4,
 `faxdetectemail` varchar(255) NOT NULL DEFAULT '',
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `incall__idx__exten` ON `incall`(`exten`);
CREATE INDEX `incall__idx__context` ON `incall`(`context`);
CREATE INDEX `incall__idx__commented` ON `incall`(`commented`);
CREATE UNIQUE INDEX `incall__uidx__exten_context` ON `incall`(`exten`,`context`);


DROP TABLE IF EXISTS `ldapfilter`;
CREATE TABLE `ldapfilter` (
 `id` int(10) unsigned auto_increment,
 `ldapserverid` int(10) unsigned NOT NULL,
 `name` varchar(128) NOT NULL DEFAULT '',
 `user` varchar(255),
 `passwd` varchar(255),
 `basedn` varchar(255) NOT NULL DEFAULT '',
 `filter` varchar(255) NOT NULL DEFAULT '',
 `attrdisplayname` varchar(255) NOT NULL DEFAULT '',
 `attrphonenumber` varchar(255) NOT NULL DEFAULT '',
 `additionaltype` enum('office','home','mobile','fax','other','custom') NOT NULL,
 `additionaltext` varchar(16) NOT NULL DEFAULT '',
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `ldapfilter__idx__ldapserverid` ON `ldapfilter`(`ldapserverid`);
CREATE INDEX `ldapfilter__idx__commented` ON `ldapfilter`(`commented`);
CREATE UNIQUE INDEX `ldapfilter__uidx__name` ON `ldapfilter`(`name`);


DROP TABLE IF EXISTS `meetmefeatures`;
CREATE TABLE `meetmefeatures` (
 `id` int(10) unsigned auto_increment,
 `meetmeid` int(10) unsigned NOT NULL,
 `name` varchar(80) NOT NULL,
 `number` varchar(40) NOT NULL,
 `context` varchar(39) NOT NULL,
 `admin_typefrom` enum('none','internal','external','undefined'),
 `admin_internalid` int(10) unsigned,
 `admin_externalid` varchar(40),
 `admin_identification` enum('calleridnum','pin','all') NOT NULL,
 `admin_mode` enum('listen','talk','all') NOT NULL,
 `admin_announceusercount` tinyint(1) NOT NULL DEFAULT 0,
 `admin_announcejoinleave` enum('no','yes','noreview') NOT NULL,
 `admin_moderationmode` tinyint(1) NOT NULL DEFAULT 0,
 `admin_initiallymuted` tinyint(1) NOT NULL DEFAULT 0,
 `admin_musiconhold` varchar(128),
 `admin_poundexit` tinyint(1) NOT NULL DEFAULT 0,
 `admin_quiet` tinyint(1) NOT NULL DEFAULT 0,
 `admin_starmenu` tinyint(1) NOT NULL DEFAULT 0,
 `admin_closeconflastmarkedexit` tinyint(1) NOT NULL DEFAULT 0,
 `admin_enableexitcontext` tinyint(1) NOT NULL DEFAULT 0,
 `admin_exitcontext` varchar(39),
 `user_mode` enum('listen','talk','all') NOT NULL,
 `user_announceusercount` tinyint(1) NOT NULL DEFAULT 0,
 `user_hiddencalls` tinyint(1) NOT NULL DEFAULT 0,
 `user_announcejoinleave` enum('no','yes','noreview') NOT NULL,
 `user_initiallymuted` tinyint(1) NOT NULL DEFAULT 0,
 `user_musiconhold` varchar(128),
 `user_poundexit` tinyint(1) NOT NULL DEFAULT 0,
 `user_quiet` tinyint(1) NOT NULL DEFAULT 0,
 `user_starmenu` tinyint(1) NOT NULL DEFAULT 0,
 `user_enableexitcontext` tinyint(1) NOT NULL DEFAULT 0,
 `user_exitcontext` varchar(39),
 `talkeroptimization` tinyint(1) NOT NULL DEFAULT 0,
 `record` tinyint(1) NOT NULL DEFAULT 0,
 `talkerdetection` tinyint(1) NOT NULL DEFAULT 0,
 `noplaymsgfirstenter` tinyint(1) NOT NULL DEFAULT 0,
 `durationm` smallint(3) unsigned,
 `closeconfdurationexceeded` tinyint(1) NOT NULL DEFAULT 0,
 `nbuserstartdeductduration` tinyint(2) unsigned,
 `timeannounceclose` smallint(3) unsigned,
 `maxuser` tinyint(2) unsigned,
 `startdate` datetime,
 `emailfrom` varchar(255),
 `emailfromname` varchar(255),
 `emailsubject` varchar(255),
 `emailbody` text NOT NULL,
 `preprocess_subroutine` varchar(39),
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `meetmefeatures__idx__number` ON `meetmefeatures`(`number`);
CREATE INDEX `meetmefeatures__idx__context` ON `meetmefeatures`(`context`);
CREATE UNIQUE INDEX `meetmefeatures__uidx__meetmeid` ON `meetmefeatures`(`meetmeid`);
CREATE UNIQUE INDEX `meetmefeatures__uidx__name` ON `meetmefeatures`(`name`);


DROP TABLE IF EXISTS `meetmeguest`;
CREATE TABLE `meetmeguest` (
 `id` int(10) unsigned auto_increment,
 `meetmefeaturesid` int(10) unsigned NOT NULL,
 `fullname` varchar(255) NOT NULL,
 `telephonenumber` varchar(40),
 `email` varchar(320),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `meetmeguest__idx__meetmefeaturesid` ON `meetmeguest`(`meetmefeaturesid`);
CREATE INDEX `meetmeguest__idx__fullname` ON `meetmeguest`(`fullname`);
CREATE INDEX `meetmeguest__idx__email` ON `meetmeguest`(`email`);


DROP TABLE IF EXISTS `musiconhold`;
CREATE TABLE `musiconhold` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(128),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `musiconhold__idx__commented` ON `musiconhold`(`commented`);
CREATE UNIQUE INDEX `musiconhold__uidx__filename_category_var_name` ON `musiconhold`(`filename`(64),`category`,`var_name`);

INSERT INTO `musiconhold` VALUES (1,0,0,0,'musiconhold.conf','default','mode','custom');
INSERT INTO `musiconhold` VALUES (2,0,0,0,'musiconhold.conf','default','application','/usr/bin/madplay --mono -a -10 -R 8000 --output=raw:-');
INSERT INTO `musiconhold` VALUES (3,0,0,0,'musiconhold.conf','default','random','no');
INSERT INTO `musiconhold` VALUES (4,0,0,0,'musiconhold.conf','default','directory','/var/lib/pf-xivo/moh/default');


DROP TABLE IF EXISTS `outcall`;
CREATE TABLE `outcall` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(128) NOT NULL,
 `exten` varchar(40) NOT NULL,
 `context` varchar(39) NOT NULL,
 `externprefix` varchar(20) NOT NULL DEFAULT '',
 `stripnum` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `setcallerid` tinyint(1) NOT NULL DEFAULT 0,
 `callerid` varchar(80) NOT NULL DEFAULT '',
 `useenum` tinyint(1) NOT NULL DEFAULT 0,
 `internal` tinyint(1) NOT NULL DEFAULT 0,
 `preprocess_subroutine` varchar(39),
 `hangupringtime` smallint(3) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `outcall__idx__exten` ON `outcall`(`exten`);
CREATE INDEX `outcall__idx__commented` ON `outcall`(`commented`);
CREATE UNIQUE INDEX `outcall__uidx__name` ON `outcall`(`name`);
CREATE UNIQUE INDEX `outcall__uidx__exten_context` ON `outcall`(`exten`,`context`);


DROP TABLE IF EXISTS `outcalltrunk`;
CREATE TABLE `outcalltrunk` (
 `outcallid` int(10) unsigned NOT NULL DEFAULT 0,
 `trunkfeaturesid` int(10) unsigned NOT NULL DEFAULT 0,
 `priority` tinyint(2) unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(`outcallid`,`trunkfeaturesid`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `outcalltrunk__idx__priority` ON `outcalltrunk`(`priority`);


DROP TABLE IF EXISTS `phone`;
CREATE TABLE `phone` (
 `macaddr` char(17) NOT NULL,
 `vendor` varchar(16) NOT NULL,
 `model` varchar(16) NOT NULL,
 `proto` varchar(50) NOT NULL,
 `iduserfeatures` int(10) unsigned NOT NULL,
 `isinalan` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`macaddr`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `phone__idx__proto_iduserfeatures` ON `phone`(`proto`,`iduserfeatures`);


DROP TABLE IF EXISTS `phonebook`;
CREATE TABLE `phonebook` (
 `id` int(10) unsigned auto_increment,
 `title` enum('mr','mrs','ms') NOT NULL,
 `firstname` varchar(128) NOT NULL DEFAULT '',
 `lastname` varchar(128) NOT NULL DEFAULT '',
 `displayname` varchar(64) NOT NULL DEFAULT '',
 `society` varchar(128) NOT NULL DEFAULT '',
 `email` varchar(255) NOT NULL DEFAULT '',
 `url` varchar(255) NOT NULL DEFAULT '',
 `image` blob,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `phonebook__idx__title` ON `phonebook`(`title`);
CREATE INDEX `phonebook__idx__firstname` ON `phonebook`(`firstname`);
CREATE INDEX `phonebook__idx__lastname` ON `phonebook`(`lastname`);
CREATE INDEX `phonebook__idx__displayname` ON `phonebook`(`displayname`);
CREATE INDEX `phonebook__idx__society` ON `phonebook`(`society`);
CREATE INDEX `phonebook__idx__email` ON `phonebook`(`email`);


DROP TABLE IF EXISTS `phonebookaddress`;
CREATE TABLE `phonebookaddress` (
 `id` int(10) unsigned auto_increment,
 `phonebookid` int(10) unsigned NOT NULL,
 `address1` varchar(30) NOT NULL DEFAULT '',
 `address2` varchar(30) NOT NULL DEFAULT '',
 `city` varchar(128) NOT NULL DEFAULT '',
 `state` varchar(128) NOT NULL DEFAULT '',
 `zipcode` varchar(16) NOT NULL DEFAULT '',
 `country` varchar(3) NOT NULL DEFAULT '',
 `type` enum('home','office','other') NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `phonebookaddress__idx__address1` ON `phonebookaddress`(`address1`);
CREATE INDEX `phonebookaddress__idx__address2` ON `phonebookaddress`(`address2`);
CREATE INDEX `phonebookaddress__idx__city` ON `phonebookaddress`(`city`);
CREATE INDEX `phonebookaddress__idx__state` ON `phonebookaddress`(`state`);
CREATE INDEX `phonebookaddress__idx__zipcode` ON `phonebookaddress`(`zipcode`);
CREATE INDEX `phonebookaddress__idx__country` ON `phonebookaddress`(`country`);
CREATE INDEX `phonebookaddress__idx__type` ON `phonebookaddress`(`type`);
CREATE UNIQUE INDEX `phonebookaddress__uidx__phonebookid_type` ON `phonebookaddress`(`phonebookid`,`type`);


DROP TABLE IF EXISTS `phonebooknumber`;
CREATE TABLE `phonebooknumber` (
 `id` int(10) unsigned auto_increment,
 `phonebookid` int(10) unsigned NOT NULL,
 `number` varchar(40) NOT NULL DEFAULT '',
 `type` enum('home','office','mobile','fax','other') NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `phonebooknumber__idx__number` ON `phonebooknumber`(`number`);
CREATE INDEX `phonebooknumber__idx__type` ON `phonebooknumber`(`type`);
CREATE UNIQUE INDEX `phonebooknumber__uidx__phonebookid_type` ON `phonebooknumber`(`phonebookid`,`type`);


DROP TABLE IF EXISTS `phonefunckey`;
CREATE TABLE `phonefunckey` (
 `iduserfeatures` int(10) unsigned NOT NULL,
 `fknum` smallint unsigned NOT NULL,
 `exten` varchar(40),
 `typeextenumbers` enum('extenfeatures','featuremap','generalfeatures'),
 `typevalextenumbers` varchar(255),
 `typeextenumbersright` enum('agent',
                             'group',
                             'meetme',
                             'queue',
                             'user'),
 `typevalextenumbersright` varchar(255),
 `label` varchar(32),
 `supervision` tinyint(1) NOT NULL DEFAULT 0,
 `progfunckey` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`iduserfeatures`,`fknum`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `phonefunckey__idx__exten` ON `phonefunckey`(`exten`);
CREATE INDEX `phonefunckey__idx__progfunckey` ON `phonefunckey`(`progfunckey`);
CREATE INDEX `phonefunckey__idx__typeextenumbers_typevalextenumbers` ON `phonefunckey`(`typeextenumbers`,`typevalextenumbers`);
CREATE INDEX `phonefunckey__idx__typeextenumbersright_typevalextenumbersright` ON `phonefunckey`(`typeextenumbersright`,`typevalextenumbersright`);


DROP TABLE IF EXISTS `queue`;
CREATE TABLE `queue` (
 `name` varchar(128) NOT NULL,
 `musiconhold` varchar(128),
 `announce` varchar(128),
 `context` varchar(39),
 `timeout` tinyint(2) unsigned DEFAULT 0,
 `monitor-type` enum('no','mixmonitor'),
 `monitor-format` varchar(128),
 `queue-youarenext` varchar(128),
 `queue-thereare` varchar(128),
 `queue-callswaiting` varchar(128),
 `queue-holdtime` varchar(128),
 `queue-minutes` varchar(128),
 `queue-seconds` varchar(128),
 `queue-lessthan` varchar(128),
 `queue-thankyou` varchar(128),
 `queue-reporthold` varchar(128),
 `periodic-announce` text,
 `announce-frequency` int(10) unsigned,
 `periodic-announce-frequency` int(10) unsigned,
 `announce-round-seconds` tinyint(2) unsigned,
 `announce-holdtime` varchar(4),
 `retry` tinyint(2) unsigned,
 `wrapuptime` tinyint(2) unsigned,
 `maxlen` int(10) unsigned,
 `servicelevel` int(11),
 `strategy` varchar(11),
 `joinempty` varchar(6),
 `leavewhenempty` varchar(6),
 `eventmemberstatus` tinyint(1) NOT NULL DEFAULT 0,
 `eventwhencalled` tinyint(1) NOT NULL DEFAULT 0,
 `ringinuse` tinyint(1) NOT NULL DEFAULT 0,
 `reportholdtime` tinyint(1) NOT NULL DEFAULT 0,
 `memberdelay` int(10) unsigned,
 `weight` int(10) unsigned,
 `timeoutrestart` tinyint(1) NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `category` enum('group','queue') NOT NULL,
 `autopause` tinyint(1) unsigned NOT NULL DEFAULT 0,
 `setinterfacevar` tinyint(1) unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `queue__idx__commented` ON `queue`(`commented`);
CREATE INDEX `queue__idx__category` ON `queue`(`category`);


DROP TABLE IF EXISTS `queuefeatures`;
CREATE TABLE `queuefeatures` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(128) NOT NULL,
 `number` varchar(40) NOT NULL DEFAULT '',
 `context` varchar(39),
 `data_quality` tinyint(1) NOT NULL DEFAULT 0,
 `hitting_callee` tinyint(1) NOT NULL DEFAULT 0,
 `hitting_caller` tinyint(1) NOT NULL DEFAULT 0,
 `retries` tinyint(1) NOT NULL DEFAULT 0,
 `ring` tinyint(1) NOT NULL DEFAULT 0,
 `transfer_user` tinyint(1) NOT NULL DEFAULT 0,
 `transfer_call` tinyint(1) NOT NULL DEFAULT 0,
 `write_caller` tinyint(1) NOT NULL DEFAULT 0,
 `write_calling` tinyint(1) NOT NULL DEFAULT 0,
 `url` varchar(255) NOT NULL DEFAULT '',
 `announceoverride` varchar(128) NOT NULL DEFAULT '',
 `timeout` smallint(4) unsigned NOT NULL DEFAULT 0,
 `preprocess_subroutine` varchar(39),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `queuefeatures__idx__number` ON `queuefeatures`(`number`);
CREATE INDEX `queuefeatures__idx__context` ON `queuefeatures`(`context`);
CREATE UNIQUE INDEX `queuefeatures__uidx__name` ON `queuefeatures`(`name`);


DROP TABLE IF EXISTS `queuemember`;
CREATE TABLE `queuemember` (
 `queue_name` varchar(128) NOT NULL,
 `interface` varchar(128) NOT NULL,
 `penalty` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `call-limit` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `paused` tinyint(1),
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `usertype` enum('agent','user') NOT NULL,
 `userid` int(10) unsigned NOT NULL,
 `channel` varchar(25) NOT NULL,
 `category` enum('group','queue') NOT NULL,
 `skills` varchar(64) NOT NULL DEFAULT '',
 `state_interface` varchar(128) NOT NULL DEFAULT '',
 PRIMARY KEY(`queue_name`,`interface`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `queuemember__idx__commented` ON `queuemember`(`commented`);
CREATE INDEX `queuemember__idx__usertype` ON `queuemember`(`usertype`);
CREATE INDEX `queuemember__idx__userid` ON `queuemember`(`userid`);
CREATE INDEX `queuemember__idx__channel` ON `queuemember`(`channel`);
CREATE INDEX `queuemember__idx__category` ON `queuemember`(`category`);
CREATE UNIQUE INDEX `queuemember__uidx__queue_name_channel_usertype_userid_category` ON `queuemember`(`queue_name`,`channel`,`usertype`,`userid`,`category`);


DROP TABLE IF EXISTS `rightcall`;
CREATE TABLE `rightcall` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(128) NOT NULL DEFAULT '',
 `context` varchar(39) NOT NULL,
 `passwd` varchar(40) NOT NULL DEFAULT '',
 `authorization` tinyint(1) NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `rightcall__idx__context` ON `rightcall`(`context`);
CREATE INDEX `rightcall__idx__passwd` ON `rightcall`(`passwd`);
CREATE INDEX `rightcall__idx__authorization` ON `rightcall`(`authorization`);
CREATE INDEX `rightcall__idx__commented` ON `rightcall`(`commented`);
CREATE UNIQUE INDEX `rightcall__uidx__name` ON `rightcall`(`name`);


DROP TABLE IF EXISTS `rightcallexten`;
CREATE TABLE `rightcallexten` (
 `id` int(10) unsigned auto_increment,
 `rightcallid` int(10) unsigned NOT NULL DEFAULT 0,
 `exten` varchar(40) NOT NULL DEFAULT '',
 `extenhash` char(40) NOT NULL DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE UNIQUE INDEX `rightcallexten__uidx__rightcallid_extenhash` ON `rightcallexten`(`rightcallid`,`extenhash`);


DROP TABLE IF EXISTS `rightcallmember`;
CREATE TABLE `rightcallmember` (
 `id` int(10) unsigned auto_increment,
 `rightcallid` int(10) unsigned NOT NULL DEFAULT 0,
 `type` enum('user','group','incall','outcall') NOT NULL,
 `typeval` varchar(128) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE UNIQUE INDEX `rightcallmember__uidx__rightcallid_type_typeval` ON `rightcallmember`(`rightcallid`,`type`,`typeval`);


DROP TABLE IF EXISTS `schedule`;
CREATE TABLE `schedule` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(128) NOT NULL DEFAULT '',
 `context` varchar(39) NOT NULL,
 `timebeg` varchar(5) NOT NULL DEFAULT '*',
 `timeend` varchar(5),
 `daynamebeg` enum('*','sun','mon','tue','wed','thu','fri','sat') NOT NULL DEFAULT '*',
 `daynameend` enum('sun','mon','tue','wed','thu','fri','sat'),
 `daynumbeg` varchar(2) NOT NULL DEFAULT '*',
 `daynumend` varchar(2),
 `monthbeg` enum('*',
                 'jan',
                 'feb',
                 'mar',
                 'apr',
                 'may',
                 'jun',
                 'jul',
                 'aug',
                 'sep',
                 'oct',
                 'nov',
                 'dec') NOT NULL DEFAULT '*',
 `monthend` enum('jan',
                 'feb',
                 'mar',
                 'apr',
                 'may',
                 'jun',
                 'jul',
                 'aug',
                 'sep',
                 'oct',
                 'nov',
                 'dec'),
 `publicholiday` tinyint(1) NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `schedule__idx__context` ON `schedule`(`context`);
CREATE INDEX `schedule__idx__publicholiday` ON `schedule`(`publicholiday`);
CREATE INDEX `schedule__idx__commented` ON `schedule`(`commented`);
CREATE UNIQUE INDEX `schedule__uidx__name` ON `schedule`(`name`);


DROP TABLE IF EXISTS `serverfeatures`;
CREATE TABLE `serverfeatures` (
 `id` int(10) unsigned auto_increment,
 `serverid` int(10) unsigned NOT NULL,
 `feature` enum('phonebook') NOT NULL,
 `type` enum('xivo','ldap') NOT NULL,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `serverfeatures__idx__serverid` ON `serverfeatures`(`serverid`);
CREATE INDEX `serverfeatures__idx__feature` ON `serverfeatures`(`feature`);
CREATE INDEX `serverfeatures__idx__type` ON `serverfeatures`(`type`);
CREATE INDEX `serverfeatures__idx__commented` ON `serverfeatures`(`commented`);
CREATE UNIQUE INDEX `serverfeatures__uidx__serverid_feature_type` ON `serverfeatures`(`serverid`,`feature`,`type`);


DROP TABLE IF EXISTS `staticagent`;
CREATE TABLE `staticagent` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(255),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `staticagent__idx__cat_metric` ON `staticagent`(`cat_metric`);
CREATE INDEX `staticagent__idx__var_metric` ON `staticagent`(`var_metric`);
CREATE INDEX `staticagent__idx__commented` ON `staticagent`(`commented`);
CREATE INDEX `staticagent__idx__filename` ON `staticagent`(`filename`);
CREATE INDEX `staticagent__idx__category` ON `staticagent`(`category`);
CREATE INDEX `staticagent__idx__var_name` ON `staticagent`(`var_name`);
CREATE INDEX `staticagent__idx__var_val` ON `staticagent`(`var_val`);

INSERT INTO `staticagent` VALUES (1,0,0,0,'agents.conf','general','persistentagents','yes');
INSERT INTO `staticagent` VALUES (2,0,0,0,'agents.conf','general','multiplelogin','yes');
INSERT INTO `staticagent` VALUES (3,1,1000000,0,'agents.conf','agents','group',1);


DROP TABLE IF EXISTS `staticiax`;
CREATE TABLE `staticiax` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(255),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `staticiax__idx__commented` ON `staticiax`(`commented`);
CREATE INDEX `staticiax__idx__filename` ON `staticiax`(`filename`);
CREATE INDEX `staticiax__idx__category` ON `staticiax`(`category`);
CREATE INDEX `staticiax__idx__var_name` ON `staticiax`(`var_name`);

INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','bindport',4569);
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','bindaddr','0.0.0.0');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','iaxthreadcount',10);
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','iaxmaxthreadcount',100);
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','iaxcompat','no');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','authdebug','yes');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','delayreject','no');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','trunkfreq',20);
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','trunktimestamps','yes');
INSERT INTO `staticiax` VALUES (NULL,0,0,1,'iax.conf','general','regcontext',NULL);
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','minregexpire',60);
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','maxregexpire',60);
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','bandwidth','high');
INSERT INTO `staticiax` VALUES (NULL,0,0,1,'iax.conf','general','tos',NULL);
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','jitterbuffer','no');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','forcejitterbuffer','no');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','maxjitterbuffer',1000);
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','maxjitterinterps',10);
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','resyncthreshold',1000);
INSERT INTO `staticiax` VALUES (NULL,0,0,1,'iax.conf','general','accountcode',NULL);
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','amaflags','default');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','adsi','no');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','transfer','yes');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','language','fr_FR');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','mohinterpret','default');
INSERT INTO `staticiax` VALUES (NULL,0,0,1,'iax.conf','general','mohsuggest',NULL);
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','encryption','no');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','maxauthreq',3);
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','codecpriority','host');
INSERT INTO `staticiax` VALUES (NULL,0,0,1,'iax.conf','general','disallow',NULL);
INSERT INTO `staticiax` VALUES (NULL,0,0,1,'iax.conf','general','allow',NULL);
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','rtcachefriends','yes');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','rtupdate','yes');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','rtignoreregexpire','yes');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','rtautoclear','no');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','pingtime',20);
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','lagrqtime',10);
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','nochecksums','no');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','autokill','yes');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','requirecalltoken','no');
INSERT INTO `staticiax` VALUES (NULL,0,0,0,'iax.conf','general','calltokenoptional','0.0.0.0');

DROP TABLE IF EXISTS `staticmeetme`;
CREATE TABLE `staticmeetme` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(128),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `staticmeetme__idx__commented` ON `staticmeetme`(`commented`);
CREATE INDEX `staticmeetme__idx__filename` ON `staticmeetme`(`filename`);
CREATE INDEX `staticmeetme__idx__category` ON `staticmeetme`(`category`);
CREATE INDEX `staticmeetme__idx__var_name` ON `staticmeetme`(`var_name`);

INSERT INTO `staticmeetme` VALUES (NULL,0,0,0,'meetme.conf','general','audiobuffers',32);


DROP TABLE IF EXISTS `staticqueue`;
CREATE TABLE `staticqueue` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(128),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `staticqueue__idx__commented` ON `staticqueue`(`commented`);
CREATE INDEX `staticqueue__idx__filename` ON `staticqueue`(`filename`);
CREATE INDEX `staticqueue__idx__category` ON `staticqueue`(`category`);
CREATE INDEX `staticqueue__idx__var_name` ON `staticqueue`(`var_name`);

INSERT INTO `staticqueue` VALUES (NULL,0,0,0,'queues.conf','general','persistentmembers','yes');
INSERT INTO `staticqueue` VALUES (NULL,0,0,0,'queues.conf','general','autofill','no');
INSERT INTO `staticqueue` VALUES (NULL,0,0,0,'queues.conf','general','monitor-type','no');


DROP TABLE IF EXISTS `staticsip`;
CREATE TABLE `staticsip` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(255),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `staticsip__idx__commented` ON `staticsip`(`commented`);
CREATE INDEX `staticsip__idx__filename` ON `staticsip`(`filename`);
CREATE INDEX `staticsip__idx__category` ON `staticsip`(`category`);
CREATE INDEX `staticsip__idx__var_name` ON `staticsip`(`var_name`);

INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','bindport',5060);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','bindaddr','0.0.0.0');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','videosupport','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','autocreatepeer','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','allowguest','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','allowsubscribe','yes');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','allowoverlap','yes');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','promiscredir','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','autodomain','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,1,'sip.conf','general','domain',NULL);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','allowexternaldomains','yes');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','usereqphone','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','realm','xivo');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','alwaysauthreject','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','limitonpeer','yes');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','useragent','XiVO PBX');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','checkmwi',10);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','buggymwi','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,1,'sip.conf','general','regcontext',NULL);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','callerid','xivo');
INSERT INTO `staticsip` VALUES (NULL,0,0,1,'sip.conf','general','fromdomain',NULL);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','sipdebug','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','dumphistory','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','recordhistory','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','callevents','yes');
INSERT INTO `staticsip` VALUES (NULL,0,0,1,'sip.conf','general','tos_sip',NULL);
INSERT INTO `staticsip` VALUES (NULL,0,0,1,'sip.conf','general','tos_audio',NULL);
INSERT INTO `staticsip` VALUES (NULL,0,0,1,'sip.conf','general','tos_video',NULL);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','t38pt_udptl','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','t38pt_rtp','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','t38pt_tcp','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','t38pt_usertpsource','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,1,'sip.conf','general','localnet',NULL);
INSERT INTO `staticsip` VALUES (NULL,0,0,1,'sip.conf','general','externip',NULL);
INSERT INTO `staticsip` VALUES (NULL,0,0,1,'sip.conf','general','externhost',NULL);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','externrefresh',10);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','matchexterniplocally','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,1,'sip.conf','general','outboundproxy',NULL);
INSERT INTO `staticsip` VALUES (NULL,0,0,1,'sip.conf','general','outboundproxyport',NULL);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','g726nonstandard','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,1,'sip.conf','general','disallow',NULL);
INSERT INTO `staticsip` VALUES (NULL,0,0,1,'sip.conf','general','allow',NULL);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','t1min',100);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','relaxdtmf','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','rfc2833compensate','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','compactheaders','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','rtptimeout',0);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','rtpholdtimeout',0);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','rtpkeepalive',0);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','directrtpsetup','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','notifymimetype','application/simple-message-summary');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','srvlookup','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','pedantic','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','minexpiry',60);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','maxexpiry',3600);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','defaultexpiry',120);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','registertimeout',20);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','registerattempts',0);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','notifyringing','yes');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','notifyhold','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','allowtransfer','yes');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','maxcallbitrate',384);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','autoframing','yes');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','jbenable','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','jbforce','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','jbmaxsize',200);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','jbresyncthreshold',1000);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','jbimpl','fixed');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','jblog','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,1,'sip.conf','general','context',NULL);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','nat','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','dtmfmode','info');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','qualify','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','useclientcode','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','progressinband','never');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','language','fr_FR');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','mohinterpret','default');
INSERT INTO `staticsip` VALUES (NULL,0,0,1,'sip.conf','general','mohsuggest',NULL);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','vmexten','*98');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','trustrpid','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','sendrpid','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','canreinvite','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','insecure','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','rtcachefriends','yes');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','rtupdate','yes');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','ignoreregexpire','yes');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','rtsavesysname','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','rtautoclear','no');
INSERT INTO `staticsip` VALUES (NULL,0,0,1,'sip.conf','general','subscribecontext',NULL);
INSERT INTO `staticsip` VALUES (NULL,0,0,0,'sip.conf','general','assertedidentity','no');


DROP TABLE IF EXISTS `staticvoicemail`;
CREATE TABLE `staticvoicemail` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` text,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `staticvoicemail__idx__commented` ON `staticvoicemail`(`commented`);
CREATE INDEX `staticvoicemail__idx__filename` ON `staticvoicemail`(`filename`);
CREATE INDEX `staticvoicemail__idx__category` ON `staticvoicemail`(`category`);
CREATE INDEX `staticvoicemail__idx__var_name` ON `staticvoicemail`(`var_name`);

INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','maxmsg',100);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','silencethreshold',256);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','minmessage',0);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','maxmessage',0);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','maxsilence',15);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','review','yes');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','operator','yes');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','format','wav');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','maxlogins',3);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','envelope','yes');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','saycid','no');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','cidinternalcontexts',NULL);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','sayduration','yes');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','saydurationm',2);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','forcename','no');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','forcegreetings','no');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','tempgreetwarn','yes');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','maxgreet',0);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','skipms',3000);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','sendvoicemail','no');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','usedirectory','yes');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','nextaftercmd','yes');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','dialout',NULL);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','callback',NULL);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','exitcontext',NULL);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','attach','yes');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','volgain',0);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','mailcmd','/usr/sbin/sendmail -t');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','serveremail','xivo');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','charset','UTF-8');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','fromstring','XiVO PBX');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','emaildateformat','%A %d %B %Y à %H:%M:%S %Z');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','emaildatelocale','fr_FR.UTF-8');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','pbxskip','no');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','emailsubject','Messagerie XiVO');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','emailbody','Bonjour ${VM_NAME} !

Vous avez reçu un message d\'une durée de ${VM_DUR} minute(s), il vous reste actuellement ${VM_MSGNUM} message(s) non lu(s) sur votre messagerie vocale : ${VM_MAILBOX}.

Le dernier a été envoyé par ${VM_CALLERID}, le ${VM_DATE}. Si vous le souhaitez vous pouvez l\'écouter ou le consulter en tapant le *98 sur votre téléphone.

Merci.

-- Messagerie XiVO --');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','pagerfromstring','XiVO PBX');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','pagersubject',NULL);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','pagerbody',NULL);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','adsifdn','0000000F');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','adsisec','9BDBF7AC');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','adsiver',1);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','searchcontexts','no');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','externpass','/usr/share/asterisk/bin/change-pass-vm');
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','externnotify',NULL);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','smdiport',NULL);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','odbcstorage',NULL);
INSERT INTO `staticvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','odbctable',NULL);
INSERT INTO `staticvoicemail` VALUES (NULL,1,0,0,'voicemail.conf','zonemessages','eu-fr','Europe/Paris|\'vm-received\' q \'digits/at\' kM');


DROP TABLE IF EXISTS `staticsccp`;
CREATE TABLE `staticsccp` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` text,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `staticsccp__idx__commented` ON `staticvoicemail`(`commented`);
CREATE INDEX `staticsccp__idx__filename` ON `staticvoicemail`(`filename`);
CREATE INDEX `staticsccp__idx__category` ON `staticvoicemail`(`category`);
CREATE INDEX `staticsccp__idx__var_name` ON `staticvoicemail`(`var_name`);

INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','servername','Asterisk');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','keepalive',60);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','debug','');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','dateFormat','D.M.Y');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','port',2000);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','firstdigittimeout',16);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','digittimeout',8);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','autoanswer_ring_time',0);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','autoanswer_tone','0x32');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','remotehangup_tone','0x32');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','transfer_tone',0);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','callwaiting_tone','0x2d');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','musicclass','default');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','dnd','on');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','sccp_tos','0x68');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','sccp_cos',4);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','audio_tos','0xB8');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','audio_cos',6);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','video_tos','0x88');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','video_cos',5);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','echocancel','on');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','silencesuppression','off');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','trustphoneip','no');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','private','on');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','protocolversion',11);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','disallow','all');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','language','fr_FR');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','hotline_enabled','yes');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','hotline_context','xivo-initconfig');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','hotline_extension','sccp');


DROP TABLE IF EXISTS `trunkfeatures`;
CREATE TABLE `trunkfeatures` (
 `id` int(10) unsigned auto_increment,
 `protocol` varchar(50) NOT NULL,
 `protocolid` int(10) unsigned NOT NULL,
 `registerid` int(10) unsigned NOT NULL DEFAULT 0,
 `registercommented` tinyint(1) NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `trunkfeatures__idx__registerid` ON `trunkfeatures`(`registerid`);
CREATE INDEX `trunkfeatures__idx__registercommented` ON `trunkfeatures`(`registercommented`);
CREATE UNIQUE INDEX `trunkfeatures__uidx__protocol_protocolid` ON `trunkfeatures`(`protocol`,`protocolid`);


DROP TABLE IF EXISTS `usercustom`;
CREATE TABLE `usercustom` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(40),
 `context` varchar(39),
 `interface` varchar(128) NOT NULL,
 `intfsuffix` varchar(32) NOT NULL DEFAULT '',
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `protocol` enum('custom') NOT NULL DEFAULT 'custom',
 `category` enum('user','trunk') NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `usercustom__idx__name` ON `usercustom`(`name`);
CREATE INDEX `usercustom__idx__context` ON `usercustom`(`context`);
CREATE INDEX `usercustom__idx__commented` ON `usercustom`(`commented`);
CREATE INDEX `usercustom__idx__protocol` ON `usercustom`(`protocol`);
CREATE INDEX `usercustom__idx__category` ON `usercustom`(`category`);
CREATE UNIQUE INDEX `usercustom__uidx__interface_intfsuffix_category` ON `usercustom`(`interface`,`intfsuffix`,`category`);


DROP TABLE IF EXISTS `userfeatures`;
CREATE TABLE `userfeatures` (
 `id` int(10) unsigned auto_increment,
 `protocol` varchar(50) NOT NULL,
 `protocolid` int(10) unsigned NOT NULL,
 `firstname` varchar(128) NOT NULL DEFAULT '',
 `lastname` varchar(128) NOT NULL DEFAULT '',
 `name` varchar(128) NOT NULL,
 `number` varchar(40) NOT NULL,
 `context` varchar(39),
 `voicemailid` int(10) unsigned,
 `agentid` int(10) unsigned,
 `provisioningid` mediumint(8) unsigned,
 `ringseconds` tinyint(2) unsigned NOT NULL DEFAULT 30,
 `simultcalls` tinyint(2) unsigned NOT NULL DEFAULT 5,
 `enableclient` tinyint(1) NOT NULL DEFAULT 1,
 `loginclient` varchar(64) NOT NULL DEFAULT '',
 `passwdclient` varchar(64) NOT NULL DEFAULT '',
 `profileclient` varchar(64) NOT NULL DEFAULT '',
 `enablehint` tinyint(1) NOT NULL DEFAULT 1,
 `enablevoicemail` tinyint(1) NOT NULL DEFAULT 0,
 `enablexfer` tinyint(1) NOT NULL DEFAULT 0,
 `enableautomon` tinyint(1) NOT NULL DEFAULT 0,
 `callrecord` tinyint(1) NOT NULL DEFAULT 0,
 `incallfilter` tinyint(1) NOT NULL DEFAULT 0,
 `enablednd` tinyint(1) NOT NULL DEFAULT 0,
 `enableunc` tinyint(1) NOT NULL DEFAULT 0,
 `destunc` varchar(128) NOT NULL DEFAULT '',
 `enablerna` tinyint(1) NOT NULL DEFAULT 0,
 `destrna` varchar(128) NOT NULL DEFAULT '',
 `enablebusy` tinyint(1) NOT NULL DEFAULT 0,
 `destbusy` varchar(128) NOT NULL DEFAULT '',
 `musiconhold` varchar(128) NOT NULL DEFAULT '',
 `outcallerid` varchar(80) NOT NULL DEFAULT '',
 `mobilephonenumber` varchar(128) NOT NULL DEFAULT '',
 `bsfilter` enum('no','boss','secretary') NOT NULL DEFAULT 'no',
 `preprocess_subroutine` varchar(39),
 `internal` tinyint(1) NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `userfeatures__idx__firstname` ON `userfeatures`(`firstname`);
CREATE INDEX `userfeatures__idx__lastname` ON `userfeatures`(`lastname`);
CREATE INDEX `userfeatures__idx__number` ON `userfeatures`(`number`);
CREATE INDEX `userfeatures__idx__context` ON `userfeatures`(`context`);
CREATE INDEX `userfeatures__idx__voicemailid` ON `userfeatures`(`voicemailid`);
CREATE INDEX `userfeatures__idx__agentid` ON `userfeatures`(`agentid`);
CREATE INDEX `userfeatures__idx__provisioningid` ON `userfeatures`(`provisioningid`);
CREATE INDEX `userfeatures__idx__loginclient` ON `userfeatures`(`loginclient`);
CREATE INDEX `userfeatures__idx__musiconhold` ON `userfeatures`(`musiconhold`);
CREATE INDEX `userfeatures__idx__internal` ON `userfeatures`(`internal`);
CREATE INDEX `userfeatures__idx__commented` ON `userfeatures`(`commented`);
CREATE UNIQUE INDEX `userfeatures__uidx__protocol_name` ON `userfeatures`(`protocol`,`name`);
CREATE UNIQUE INDEX `userfeatures__uidx__protocol_protocolid` ON `userfeatures`(`protocol`,`protocolid`);

INSERT INTO `userfeatures` VALUES (1,'sip',1,'Guest','','guest','','xivo-initconfig',NULL,NULL,148378,
                                   30,5,0,'','','',0,0,0,0,0,0,0,0,'',0,'',0,'','','','','no',NULL,1,0,'');


DROP TABLE IF EXISTS `useriax`;
CREATE TABLE `useriax` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(40) NOT NULL, -- user / peer --
 `type` enum('friend','peer','user') NOT NULL, -- user / peer --
 `username` varchar(80), -- peer --
 `secret` varchar(80) NOT NULL DEFAULT '', -- peer / user --
 `dbsecret` varchar(255) NOT NULL DEFAULT '', -- peer / user --
 `context` varchar(39), -- peer / user --
 `language` varchar(20), -- general / user --
 `accountcode` varchar(20), -- general / user --
 `amaflags` enum('default',
                 'omit',
                 'billing',
                 'documentation') DEFAULT 'default', -- general / user --
 `mailbox` varchar(80), -- peer --
 `callerid` varchar(160), -- user / peer --
 `fullname` varchar(80), -- user / peer --
 `cid_number` varchar(80), -- user / peer --
 `trunk` tinyint(1) NOT NULL DEFAULT 0, -- user / peer --
 `auth` enum('plaintext',
             'md5',
             'rsa',
             'plaintext,md5',
             'plaintext,rsa',
             'md5,rsa',
             'plaintext,md5,rsa') NOT NULL DEFAULT 'plaintext,md5', -- user / peer --
 `encryption` enum('no','yes','aes128'), -- user / peer --
 `maxauthreq` tinyint(2) unsigned, -- general / user --
 `inkeys` varchar(80), -- user / peer --
 `outkey` varchar(80), -- peer --
 `adsi` tinyint(1), -- general / user / peer --
 `transfer` enum('no','yes','mediaonly'), -- general / user / peer --
 `codecpriority` enum('disabled','host','caller','reqonly'), -- general / user --
 `jitterbuffer` tinyint(1), -- general / user / peer --
 `forcejitterbuffer` tinyint(1), -- general / user / peer --
 `sendani` tinyint(1) NOT NULL DEFAULT 0, -- peer --
 `qualify` varchar(4) NOT NULL DEFAULT 'no', -- peer --
 `qualifysmoothing` tinyint(1) NOT NULL DEFAULT 0, -- peer --
 `qualifyfreqok` int(10) unsigned NOT NULL DEFAULT 60000, -- peer --
 `qualifyfreqnotok` int(10) unsigned NOT NULL DEFAULT 10000, -- peer --
 `timezone` varchar(80), -- peer --
 `disallow` varchar(100), -- general / user / peer --
 `allow` varchar(100), -- general / user / peer --
 `mohinterpret` varchar(80), -- general / user / peer --
 `mohsuggest` varchar(80), -- general / user / peer --
 `deny` varchar(31), -- user / peer --
 `permit` varchar(31), -- user / peer --
 `defaultip` varchar(255), -- peer --
 `sourceaddress` varchar(255), -- peer --
 `setvar` varchar(100) NOT NULL DEFAULT '', -- user --
 `host` varchar(255) NOT NULL DEFAULT 'dynamic', -- peer --
 `port` smallint unsigned, -- peer --
 `mask` varchar(15), -- peer --
 `regexten` varchar(80), -- peer --
 `peercontext` varchar(80), -- peer --
 `ipaddr` varchar(255) NOT NULL DEFAULT '',
 `regseconds` int(10) unsigned NOT NULL DEFAULT 0,
 `protocol` enum('iax') NOT NULL DEFAULT 'iax',
 `category` enum('user','trunk') NOT NULL,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `requirecalltoken` varchar(4) NOT NULL DEFAULT 'no', -- peer--
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `useriax__idx__mailbox` ON `useriax`(`mailbox`);
CREATE INDEX `useriax__idx__protocol` ON `useriax`(`protocol`);
CREATE INDEX `useriax__idx__category` ON `useriax`(`category`);
CREATE INDEX `useriax__idx__commented` ON `useriax`(`commented`);
CREATE INDEX `useriax__idx__name_host` ON `useriax`(`name`,`host`);
CREATE INDEX `useriax__idx__name_ipaddr_port` ON `useriax`(`name`,`ipaddr`,`port`);
CREATE INDEX `useriax__idx__ipaddr_port` ON `useriax`(`ipaddr`,`port`);
CREATE INDEX `useriax__idx__host_port` ON `useriax`(`host`,`port`);
CREATE UNIQUE INDEX `useriax__uidx__name` ON `useriax`(`name`);


DROP TABLE IF EXISTS `usersip`;
CREATE TABLE `usersip` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(40) NOT NULL, -- user / peer --
 `type` enum('friend','peer','user') NOT NULL, -- user / peer --
 `username` varchar(80), -- peer --
 `secret` varchar(80) NOT NULL DEFAULT '', -- user / peer --
 `md5secret` varchar(32) NOT NULL DEFAULT '', -- user / peer --
 `context` varchar(39), -- general / user / peer --
 `language` varchar(20), -- general / user / peer --
 `accountcode` varchar(20), -- user / peer --
 `amaflags` enum('default',
                 'omit',
                 'billing',
                 'documentation') NOT NULL DEFAULT 'default', -- user / peer --
 `allowtransfer` tinyint(1), -- general / user / peer --
 `fromuser` varchar(80), -- peer --
 `fromdomain` varchar(255), -- general / peer --
 `mailbox` varchar(80), -- peer --
 `subscribemwi` tinyint(1) NOT NULL DEFAULT 1, -- peer --
 `buggymwi` tinyint(1), -- general / user / peer --
 `call-limit` tinyint(2) unsigned NOT NULL DEFAULT 0, -- user / peer --
 `callerid` varchar(160), -- general / user / peer --
 `fullname` varchar(80), -- user / peer --
 `cid_number` varchar(80), -- user / peer --
 `maxcallbitrate` smallint(4) unsigned, -- general / user / peer --
 `insecure` enum('port','invite','port,invite'), -- general / user / peer --
 `nat` enum('no','yes','never','route'), -- general / user / peer --
 `canreinvite` enum('no','yes','update','nonat','update,nonat'), -- general / user / peer --
 `promiscredir` tinyint(1), -- general / user / peer --
 `usereqphone` tinyint(1), -- general / peer --
 `videosupport` tinyint(1), -- general / user / peer --
 `trustrpid` tinyint(1), -- general / user / peer --
 `sendrpid` tinyint(1), -- general / user / peer --
 `allowsubscribe` tinyint(1), -- general / user / peer --
 `allowoverlap` tinyint(1), -- general / user / peer --
 `dtmfmode` enum('rfc2833','inband','info','auto'), -- general / user / peer --
 `rfc2833compensate` tinyint(1), -- general / user / peer --
 `qualify` varchar(4), -- general / peer --
 `g726nonstandard` tinyint(1), -- general / user / peer --
 `disallow` varchar(100), -- general / user / peer --
 `allow` varchar(100), -- general / user / peer --
 `autoframing` tinyint(1), -- general / user / peer --
 `mohinterpret` varchar(80), -- general / user / peer --
 `mohsuggest` varchar(80), -- general / user / peer --
 `useclientcode` tinyint(1), -- general / user / peer --
 `progressinband` enum('no','yes','never'), -- general / user / peer --
 `t38pt_udptl` tinyint(1), -- general / user / peer --
 `t38pt_rtp` tinyint(1), -- general / user / peer --
 `t38pt_tcp` tinyint(1), -- general / user / peer --
 `t38pt_usertpsource` tinyint(1), -- general / user / peer --
 `rtptimeout` tinyint unsigned, -- general / peer --
 `rtpholdtimeout` tinyint unsigned, -- general / peer --
 `rtpkeepalive` tinyint unsigned, -- general / peer --
 `deny` varchar(31), -- user / peer --
 `permit` varchar(31), -- user / peer --
 `defaultip` varchar(255), -- peer --
 `callgroup` varchar(180), -- user / peer --
 `pickupgroup` varchar(180), -- user / peer --
 `setvar` varchar(100) NOT NULL DEFAULT '', -- user / peer --
 `host` varchar(255) NOT NULL DEFAULT 'dynamic', -- peer --
 `port` smallint unsigned, -- peer --
 `regexten` varchar(80), -- peer --
 `subscribecontext` varchar(80), -- general / user / peer --
 `fullcontact` varchar(255), -- peer --
 `vmexten` varchar(40), -- general / peer --
 `callingpres` tinyint(1), -- user / peer --
 `ipaddr` varchar(255) NOT NULL DEFAULT '',
 `regseconds` int(10) unsigned NOT NULL DEFAULT 0,
 `regserver` varchar(20),
 `lastms` varchar(15) NOT NULL DEFAULT '',
 `protocol` enum('sip') NOT NULL DEFAULT 'sip',
 `category` enum('user','trunk') NOT NULL,
 `commented` tinyint(1) NOT NULL DEFAULT 0, -- user / peer --
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `usersip__idx__mailbox` ON `usersip`(`mailbox`);
CREATE INDEX `usersip__idx__protocol` ON `usersip`(`protocol`);
CREATE INDEX `usersip__idx__category` ON `usersip`(`category`);
CREATE INDEX `usersip__idx__commented` ON `usersip`(`commented`);
CREATE INDEX `usersip__idx__host_port` ON `usersip`(`host`,`port`);
CREATE INDEX `usersip__idx__ipaddr_port` ON `usersip`(`ipaddr`,`port`);
CREATE INDEX `usersip__idx__lastms` ON `usersip`(`lastms`);
CREATE UNIQUE INDEX `usersip__uidx__name` ON `usersip`(`name`);

INSERT INTO `usersip` VALUES (1,'guest','friend','guest','guest','','xivo-initconfig',NULL,
                              NULL,'default',NULL,NULL,NULL,NULL,0,NULL,0,'Guest',
                              NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                              NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                              NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'XIVO_USERID=1',
                              'dynamic',NULL,NULL,NULL,NULL,NULL,NULL,'',0,NULL,'','sip','user',0);


DROP TABLE IF EXISTS `voicemail`;
CREATE TABLE `voicemail` (
 `uniqueid` int(10) unsigned auto_increment,
 `context` varchar(39) NOT NULL,
 `mailbox` varchar(40) NOT NULL,
 `password` varchar(80) NOT NULL DEFAULT '',
 `fullname` varchar(80) NOT NULL DEFAULT '',
 `email` varchar(80),
 `pager` varchar(80),
 `dialout` varchar(39),
 `callback` varchar(39),
 `exitcontext` varchar(39),
 `language` varchar(20),
 `tz` varchar(80),
 `attach` tinyint(1),
 `saycid` tinyint(1),
 `review` tinyint(1),
 `operator` tinyint(1),
 `envelope` tinyint(1),
 `sayduration` tinyint(1),
 `saydurationm` tinyint(2) unsigned,
 `sendvoicemail` tinyint(1),
 `deletevoicemail` tinyint(1) NOT NULL DEFAULT 0,
 `forcename` tinyint(1),
 `forcegreetings` tinyint(1),
 `hidefromdir` enum('yes','no') NOT NULL DEFAULT 'no',
 `maxmsg` smallint(4) unsigned,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`uniqueid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `voicemail__idx__context` ON `voicemail`(`context`);
CREATE INDEX `voicemail__idx__commented` ON `voicemail`(`commented`);
CREATE UNIQUE INDEX `voicemail__uidx__mailbox_context` ON `voicemail`(`mailbox`,`context`);


DROP TABLE IF EXISTS `voicemailfeatures`;
CREATE TABLE `voicemailfeatures` (
 `id` int(10) unsigned auto_increment,
 `voicemailid` int(10) unsigned,
 `skipcheckpass` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE UNIQUE INDEX `voicemailfeatures__uidx__voicemailid` ON `voicemailfeatures`(`voicemailid`);


DROP TABLE IF EXISTS `voicemenu`;
CREATE TABLE `voicemenu` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(29) NOT NULL DEFAULT '',
 `number` varchar(40) NOT NULL,
 `context` varchar(39) NOT NULL,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `voicemenu__idx__number` ON `voicemenu`(`number`);
CREATE INDEX `voicemenu__idx__context` ON `voicemenu`(`context`);
CREATE INDEX `voicemenu__idx__commented` ON `voicemenu`(`commented`);
CREATE UNIQUE INDEX `voicemenu__uidx__name` ON `voicemenu`(`name`);


-- queueskill categories
DROP TABLE IF EXISTS `queueskillcat`;
CREATE TABLE `queueskillcat` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(64) NOT NULL DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE UNIQUE INDEX `queueskillcat__uidx__name` ON `queueskillcat`(`name`);

-- queueskill values
DROP TABLE IF EXISTS `queueskill`;
CREATE TABLE `queueskill` (
 `id` int(10) unsigned auto_increment,
 `catid` int(10) unsigned NOT NULL DEFAULT 1,
 `name` varchar(64) NOT NULL DEFAULT '',
 `description` text,
 `printscreen` varchar(5),
 PRIMARY KEY(`id`)
);

CREATE INDEX `queueskill__idx__catid` ON `queueskill`(`catid`);
CREATE UNIQUE INDEX `queueskill__uidx__name` ON `queueskill`(`name`);

-- queueskill rules;
DROP TABLE IF EXISTS `queueskillrule`;
CREATE TABLE `queueskillrule` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(64) NOT NULL DEFAULT '',
 `rule` text,
 PRIMARY KEY(`id`)
);

-- user queueskills
DROP TABLE IF EXISTS `userqueueskill`;
CREATE TABLE `userqueueskill` (
 `userid` int(10) unsigned,
 `skillid` int(10) unsigned,
 `weight` int(3) unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(`userid`, `skillid`)
);

CREATE INDEX `userqueueskill__idx__userid` ON `userqueueskill`(`userid`);

DROP TABLE IF EXISTS `usersccp`;
CREATE TABLE `usersccp`
(
 `id` int(10) unsigned auto_increment,
 `name` varchar(128),
 `devicetype` varchar(64),            -- phone model, ie 7960
 `keepalive` int(3) unsigned,        -- i.e 60
 `tzoffset` varchar(3),               -- ie: +1 == Europe/Paris
 `dtmfmode` varchar(16),               -- outofband, inband
 `transfer` varchar(3),                -- on, off, NULL
 `park` varchar(3),                    -- on, off, NULL
 `cfwdall` varchar(3),                 -- on, off, NULL
 `cfwdbusy` varchar(3),                -- on, off, NULL
 `cfwdnoanswer` varchar(3),            -- on, off, NULL
 `mwilamp` varchar(3),                 -- on, off, NULL
 `mwioncall` varchar(3),               -- on, off, NULL
 `dnd` varchar(6),                     -- on, off, NULL
 `pickupexten` varchar(3),             -- on, off, NULL
 `pickupcontext` varchar(64),          -- pickup context name
 `pickupmodeanswer` varchar(3),        -- on, off, NULL
 `permit` varchar(31),                 -- 192.168.0.0/255.255.255.0
 `deny` varchar(31),                   -- 0.0.0.0/0.0.0.0
 `addons` varchar(24),                 -- comma separated addons list. i.e 7914,7914
 `imageversion` varchar(64),           -- i.e P00405000700
 `trustphoneip` varchar(3),            -- yes, no, NULL
 `nat` varchar(3),                     -- on, off, NULL
 `directrtp` varchar(3),               -- on, off, NULL
 `earlyrtp` varchar(3),                -- on, off, NULL
 `private` varchar(3),                 -- on, off, NULL
 `privacy` varchar(4),                 -- on, off, full, NULL
 `protocol` varchar(4) NOT NULL DEFAULT 'sccp', -- required for join with userfeatures

 -- softkeys
 `softkey_onhook`      varchar(1024),
 `softkey_connected`   varchar(1024),
 `softkey_onhold`      varchar(1024),
 `softkey_ringin`      varchar(1024),
 `softkey_offhook`     varchar(1024),
 `softkey_conntrans`   varchar(1024),
 `softkey_digitsfoll`  varchar(1024),
 `softkey_connconf`    varchar(1024),
 `softkey_ringout`     varchar(1024),
 `softkey_offhookfeat` varchar(1024),
 `softkey_onhint`      varchar(1024),
           
 `defaultline` int(10) unsigned,
 `commented` int(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
);

CREATE UNIQUE INDEX `usersccp__uidx__name` ON `usersccp`(`name`);


DROP TABLE IF EXISTS `sccpline`;
CREATE TABLE `sccpline`
(
 `id` int(10) unsigned auto_increment,
 `name` varchar(80) NOT NULL,
 `pin` varchar(8) NOT NULL DEFAULT '',
 `label` varchar(128) NOT NULL DEFAULT '',
 `description` text,
 `context` varchar(64),
 `incominglimit` int(3) unsigned,
 `transfer` varchar(3) DEFAULT 'on',                    -- on, off, NULL
 `mailbox` varchar(64) DEFAULT NULL,
 `vmnum` varchar(64) DEFAULT NULL,
 `meetmenum` varchar(64) DEFAULT NULL,
 `cid_name` varchar(64) NOT NULL DEFAULT '',
 `cid_num` varchar(64) NOT NULL DEFAULT '',
 `trnsfvm` varchar(64),
 `secondary_dialtone_digits` varchar(10),
 `secondary_dialtone_tone` int(5) unsigned,
 `musicclass` varchar(32),
 `language` varchar(32),                                 -- en, fr, ...
 `accountcode` varchar(32),
 `audio_tos` varchar(8),
 `audio_cos` int(3) unsigned,
 `video_tos` varchar(8),
 `video_cos` int(3) unsigned,
 `echocancel` varchar(3) DEFAULT 'on',                   -- on, off, NULL
 `silencesuppression` varchar(3) DEFAULT 'on',           -- on, off, NULL
 `callgroup` varchar(64) DEFAULT '',                     -- i.e: 1,4-9
 `pickupgroup` varchar(64) DEFAULT '',                   -- i.e: 1,3-9
 `amaflags` varchar(16) DEFAULT '',                      -- default, omit, billing, documentation
 `adhocnumber` varchar(64),
 `setvar` varchar(512) DEFAULT '',
 `commented` int(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
);

CREATE UNIQUE INDEX `sccpline__uidx__name` ON `sccpline`(`name`);

COMMIT;
