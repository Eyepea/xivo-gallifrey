
BEGIN;

-- migrate server table
ALTER TABLE `server` ADD COLUMN `ami_pass`  varchar(64) NOT NULL DEFAULT ''  AFTER `description`;
ALTER TABLE `server` ADD COLUMN `ami_login` varchar(64) NOT NULL DEFAULT ''  AFTER `description`;
ALTER TABLE `server` ADD COLUMN `ami_port`  smallint unsigned NOT NULL       AFTER `description`;
ALTER TABLE `server` ADD COLUMN `webi`      varchar(255) NOT NULL DEFAULT '' AFTER `description`;

UPDATE `server` SET
	`webi`      = '127.0.0.1',
	`ami_port`  = 5038,
	`ami_login` = 'xivouser',
	`ami_pass`  = 'xivouser';

INSERT IGNORE INTO `server` VALUES(1,'xivo','localhost',443,1,0,1271070538,'','127.0.0.1',5038,'xivouser','xivouser');

-- migrate session table
DROP TABLE `session`;
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


-- other tables are fully new

CREATE TABLE `directories` (
 `id` int(10) unsigned auto_increment,
 `uri` varchar(255),
 `dirtype` varchar(20),
 `name` varchar(255),
 `tablename` varchar(255),
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO `directories` VALUES (1,'internal' , NULL, 'internal' , '', 'XiVO internal users');
INSERT INTO `directories` VALUES (2,'phonebook', NULL, 'phonebook', '', 'XiVO phonebook');

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


CREATE TABLE `resolvconf` (
 `id` tinyint(1) auto_increment,
 `hostname` varchar(63) NOT NULL DEFAULT 'xivo',
 `domain` varchar(255) NOT NULL DEFAULT '',
 `nameserver1` varchar(255),
 `nameserver2` varchar(255),
 `nameserver3` varchar(255),
 `search` varchar(255),
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE UNIQUE INDEX `resolvconf__uidx__hostname` ON `resolvconf`(`hostname`);
INSERT INTO `resolvconf` VALUES(1, '', '', NULL, NULL, NULL, NULL, '');

CREATE TABLE `dhcp` (
 `id` int(10) unsigned auto_increment,
 `active` int(1) unsigned NOT NULL DEFAULT 0,
 `pool_start` varchar(64) NOT NULL DEFAULT '',
 `pool_end` varchar(64) NOT NULL DEFAULT '',
 `extra_ifaces` varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO `dhcp` VALUES (1,0,'','','');

CREATE TABLE `mail` (
 `id` int(10) unsigned auto_increment,
 `mydomain` varchar(255) NOT NULL DEFAULT 0,
 `origin` varchar(255) NOT NULL DEFAULT 'xivo-clients.proformatique.com',
 `relayhost` varchar(255),
 `fallback_relayhost` varchar(255),
 `canonical` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE UNIQUE INDEX `mail__uidx__origin` ON `mail`(`origin`);
INSERT INTO `mail` VALUES (1,'','xivo-clients.proformatique.com','','','');

CREATE TABLE `monitoring` (
 `id` int(10) unsigned auto_increment,
 `maintenance` int(1) unsigned NOT NULL DEFAULT 0,
 `alert_emails` varchar(4096) DEFAULT NULL,
 `dahdi_monitor_ports` varchar(255) DEFAULT NULL,
 `max_call_duration` int(5) DEFAULT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO monitoring VALUES (1,0,NULL,NULL,NULL);

-- HA
DROP TABLE IF EXISTS `ha`;
CREATE TABLE `ha` (
 `id` int(10) unsigned auto_increment,
 `apache2` int(1) unsigned NOT NULL DEFAULT 0,
 `asterisk` int(1) unsigned NOT NULL DEFAULT 0,
 `dhcp` int(1) unsigned NOT NULL DEFAULT 0,
 `monit` int(1) unsigned NOT NULL DEFAULT 0,
 `mysql` int(1) unsigned NOT NULL DEFAULT 0,
 `ntp` int(1) unsigned NOT NULL DEFAULT 0,
 `rsync` int(1) unsigned NOT NULL DEFAULT 0,
 `smokeping` int(1) unsigned NOT NULL DEFAULT 0,
 `mailto` int(1) unsigned NOT NULL DEFAULT 0,
 `alert_emails` varchar(1024) DEFAULT NULL,
 `serial` varchar(16) NOT NULL DEFAULT '',
 `authkeys` varchar(128) NOT NULL DEFAULT '',
 `com_mode` varchar(8) NOT NULL DEFAULT 'ucast',
 `user` varchar(16) NOT NULL DEFAULT 'pf-replication',
 `password` varchar(16) NOT NULL DEFAULT 'proformatique',
 `dest_user` varchar(16) NOT NULL DEFAULT 'pf-replication',
 `dest_password` varchar(16) NOT NULL DEFAULT 'proformatique',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO `ha` VALUES (1,0,0,0,0,0,0,0,0,0,NULL,'','','ucast','pf-replication','proformatique','pf-replication','proformatique');

CREATE TABLE `ha_uname_node` (
 `uname_node` varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY (`uname_node`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `ha_ping_ipaddr` (
 `ping_ipaddr` varchar(39) NOT NULL DEFAULT '',
 PRIMARY KEY (`ping_ipaddr`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `ha_virtual_network` (
 `ipaddr` varchar(39) NOT NULL DEFAULT '',
 `netmask` varchar(39) NOT NULL DEFAULT '',
 `broadcast` varchar(39) NOT NULL DEFAULT '',
 PRIMARY KEY (`ipaddr`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `ha_peer` (
 `iface` varchar(64) NOT NULL DEFAULT '',
 `host` varchar(128) NOT NULL DEFAULT '',
 `transfer` int(1) unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY (`iface`, `host`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- provisioning
CREATE TABLE `provisioning` (
 `id` tinyint(1) auto_increment,
 `registrar_main`   varchar(255) NOT NULL DEFAULT '',
 `registrar_backup` varchar(255) NOT NULL DEFAULT '',
 `proxy_main`       varchar(255) NOT NULL DEFAULT '',
 `proxy_backup`     varchar(255) NOT NULL DEFAULT '',
 `vlan_id`          integer,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM;


INSERT INTO `provisioning` VALUES(1, '', '', '', '', NULL);

COMMIT;
