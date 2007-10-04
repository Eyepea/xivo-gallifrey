GRANT ALL PRIVILEGES ON `asterisk`.* TO `asterisk`@localhost IDENTIFIED BY PASSWORD '7560ba9c16089426';
CREATE DATABASE IF NOT EXISTS `asterisk` DEFAULT CHARACTER SET utf8;
USE `asterisk`;

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
CREATE INDEX `cdr__idx__duration` ON `cdr`(`duration`);
CREATE INDEX `cdr__idx__disposition` ON `cdr`(`disposition`);
CREATE INDEX `cdr__idx__amaflags` ON `cdr`(`amaflags`);
CREATE INDEX `cdr__idx__accountcode` ON `cdr`(`accountcode`);
CREATE INDEX `cdr__idx__userfield` ON `cdr`(`userfield`);


DROP TABLE IF EXISTS `incall`;
CREATE TABLE `incall` (
 `id` int(10) unsigned auto_increment,
 `type` varchar(64) NOT NULL DEFAULT '',
 `typeval` varchar(255) NOT NULL DEFAULT '',
 `extenumid` int(10) unsigned NOT NULL,
 `linked` tinyint(1) NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `incall__idx__type_typeval` ON `incall`(`type`,`typeval`);
CREATE INDEX `incall__idx__linked` ON `incall`(`linked`);
CREATE INDEX `incall__idx__commented` ON `incall`(`commented`);
CREATE UNIQUE INDEX `incall__uidx__extenumid` ON `incall`(`extenumid`);


DROP TABLE IF EXISTS `extenumbers`;
CREATE TABLE `extenumbers` (
 `id` int(10) unsigned auto_increment,
 `exten` varchar(40) NOT NULL DEFAULT '',
 `extenhash` char(40) NOT NULL DEFAULT '',
 `context` varchar(39) NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `extenumbers__idx__exten` ON `extenumbers`(`exten`);
CREATE INDEX `extenumbers__idx__extenhash` ON `extenumbers`(`extenhash`);
CREATE INDEX `extenumbers__idx__context` ON `extenumbers`(`context`);


DROP TABLE IF EXISTS `groupfeatures`;
CREATE TABLE `groupfeatures` (
 `id` tinyint(2) unsigned auto_increment,
 `name` varchar(128) NOT NULL,
 `number` varchar(40) DEFAULT '',
 `context` varchar(39),
 `deleted` tinyint(1) DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `groupfeatures__idx__name` ON `groupfeatures`(`name`);
CREATE INDEX `groupfeatures__idx__deleted` ON `groupfeatures`(`deleted`);


DROP TABLE IF EXISTS `meetme`;
CREATE TABLE `meetme` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned DEFAULT 0,
 `var_metric` int(10) unsigned DEFAULT 0,
 `commented` tinyint(1) DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(128),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `meetme__idx__commented` ON `meetme`(`commented`);
CREATE INDEX `meetme__idx__filename` ON `meetme`(`filename`);
CREATE INDEX `meetme__idx__category` ON `meetme`(`category`);
CREATE INDEX `meetme__idx__var_name` ON `meetme`(`var_name`);


DROP TABLE IF EXISTS `meetmefeatures`;
CREATE TABLE `meetmefeatures` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(128) NOT NULL,
 `number` varchar(40) NOT NULL,
 `meetmeid` int(10) unsigned NOT NULL,
 `mode` varchar(6) NOT NULL DEFAULT 'all',
 `musiconhold` varchar(128) NOT NULL DEFAULT '',
 `context` varchar(39),
 `exit` tinyint(1) DEFAULT 0,
 `quiet` tinyint(1) DEFAULT 0,
 `record` tinyint(1) DEFAULT 0,
 `video` tinyint(1) DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE UNIQUE INDEX `meetmefeatures__uidx__meetmeid` ON `meetmefeatures`(`meetmeid`);
CREATE UNIQUE INDEX `meetmefeatures__uidx__name` ON `meetmefeatures`(`name`);
CREATE UNIQUE INDEX `meetmefeatures__uidx__number` ON `meetmefeatures`(`number`);


DROP TABLE IF EXISTS `phone`;
CREATE TABLE `phone` (
 `macaddr` char(17) NOT NULL,
 `vendor` varchar(16) NOT NULL,
 `model` varchar(16) NOT NULL,
 `proto` varchar(50) NOT NULL,
 `iduserfeatures` int(10) unsigned,
 `isinalan` tinyint(1) DEFAULT 0,
 PRIMARY KEY(`macaddr`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `phone__idx__proto_iduserfeatures` ON `phone`(`proto`,`iduserfeatures`);


DROP TABLE IF EXISTS `queue`;
CREATE TABLE `queue` (
 `name` varchar(128) NOT NULL,
 `musiconhold` varchar(128),
 `announce` varchar(128),
 `context` varchar(39),
 `timeout` tinyint(2) unsigned,
 `monitor-join` tinyint(1) DEFAULT 0,
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
 `periodic-announce` varchar(128),
 `announce-frequency` int(10) unsigned,
 `periodic-announce-frequency` int(10) unsigned,
 `announce-round-seconds` tinyint(2) unsigned,
 `announce-holdtime` varchar(4),
 `retry` tinyint(2) unsigned,
 `wrapuptime` int(10) unsigned,
 `maxlen` int(10) unsigned,
 `servicelevel` int(11),
 `strategy` varchar(11),
 `joinempty` varchar(6),
 `leavewhenempty` varchar(6),
 `eventmemberstatus` tinyint(1) DEFAULT 0,
 `eventwhencalled` tinyint(1) DEFAULT 0,
 `reportholdtime` tinyint(1) DEFAULT 0,
 `memberdelay` int(10) unsigned,
 `weight` int(10) unsigned,
 `timeoutrestart` tinyint(1) DEFAULT 0,
 `commented` tinyint(1) DEFAULT 0,
 `category` enum('group','queue') NOT NULL,
 PRIMARY KEY(`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `queue__idx__commented` ON `queue`(`commented`);
CREATE INDEX `queue__idx__category` ON `queue`(`category`);


DROP TABLE IF EXISTS `queuefeatures`;
CREATE TABLE `queuefeatures` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(128) NOT NULL,
 `number` varchar(40) DEFAULT '',
 `context` varchar(39),
 `data_quality` tinyint(1) DEFAULT 0,
 `hitting_callee` tinyint(1) DEFAULT 0,
 `hitting_caller` tinyint(1) DEFAULT 0,
 `retries` tinyint(1) DEFAULT 0,
 `ring` tinyint(1) DEFAULT 0,
 `transfer_user` tinyint(1) DEFAULT 0,
 `transfer_call` tinyint(1) DEFAULT 0,
 `write_caller` tinyint(1) DEFAULT 0,
 `write_calling` tinyint(1) DEFAULT 0,
 `url` varchar(255) DEFAULT '',
 `announceoverride` varchar(128) DEFAULT '',
 `timeout` tinyint(2) unsigned,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE UNIQUE INDEX `queuefeatures__uidx__name` ON `queuefeatures`(`name`);


DROP TABLE IF EXISTS `queuemember`;
CREATE TABLE `queuemember` (
 `queue_name` varchar(128) NOT NULL,
 `interface` varchar(255) NOT NULL,
 `penalty` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `call-limit` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) DEFAULT 0,
 `usertype` enum('agent','user') NOT NULL,
 `userid` int(10) unsigned NOT NULL,
 `channel` varchar(25) NOT NULL,
 `category` enum('group','queue') NOT NULL,
 PRIMARY KEY(`queue_name`,`interface`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `queuemember__idx__commented` ON `queuemember`(`commented`);
CREATE INDEX `queuemember__idx__usertype` ON `queuemember`(`usertype`);
CREATE INDEX `queuemember__idx__userid` ON `queuemember`(`userid`);
CREATE INDEX `queuemember__idx__channel` ON `queuemember`(`channel`);
CREATE INDEX `queuemember__idx__category` ON `queuemember`(`category`);
CREATE UNIQUE INDEX `queuemember__uidx__queue_name_channel_usertype_userid_category` ON `queuemember`(`queue_name`,`channel`,`usertype`,`userid`,`category`);


DROP TABLE IF EXISTS `trunkfeatures`;
CREATE TABLE `trunkfeatures` (
 `id` int(10) unsigned auto_increment,
 `trunk` varchar(50) NOT NULL,
 `trunkid` int(10) unsigned NOT NULL,
 `registerid` int(10) unsigned DEFAULT 0,
 `registercommented` tinyint(1) DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `trunkfeatures__idx__registerid` ON `trunkfeatures`(`registerid`);
CREATE INDEX `trunkfeatures__idx__registercommented` ON `trunkfeatures`(`registercommented`);
CREATE UNIQUE INDEX `trunkfeatures__uidx__trunk_trunkid` ON `trunkfeatures`(`trunk`,`trunkid`);


DROP TABLE IF EXISTS `trunkcustom`;
CREATE TABLE `trunkcustom` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(40) NOT NULL,
 `interface` varchar(255) NOT NULL,
 `commented` tinyint(1) DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE UNIQUE INDEX `trunkcustom__uidx__name` ON `trunkcustom`(`name`);
CREATE UNIQUE INDEX `trunkcustom__uidx__interface` ON `trunkcustom`(`interface`);


DROP TABLE IF EXISTS `usergroup`;
CREATE TABLE `usergroup` (
 `id` int(10) unsigned auto_increment,
 `userid` int(10) unsigned NOT NULL,
 `groupid` tinyint(2) unsigned NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE UNIQUE INDEX `usergroup__uidx__userid_groupid` ON `usergroup`(`userid`,`groupid`);


DROP TABLE IF EXISTS `useriax`;
CREATE TABLE `useriax` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(40) NOT NULL,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `username` varchar(80) NOT NULL,
 `type` varchar(6) NOT NULL DEFAULT 'friend',
 `secret` varchar(80),
 `md5secret` varchar(32),
 `dbsecret` varchar(100),
 `notransfer` char(3),
 `trunk` char(3),
 `inkeys` varchar(100),
 `outkey` varchar(100),
 `auth` varchar(100),
 `accountcode` varchar(100),
 `amaflags` varchar(13),
 `callerid` varchar(160),
 `callgroup` varchar(180),
 `context` varchar(39),
 `defaultip` varchar(15),
 `host` varchar(31) NOT NULL DEFAULT 'dynamic',
 `language` char(2),
 `mailbox` varchar(80),
 `deny` varchar(95),
 `permit` varchar(95),
 `qualify` char(3),
 `disallow` varchar(100),
 `allow` varchar(100),
 `ipaddr` varchar(15) NOT NULL,
 `port` smallint unsigned,
 `regseconds` int(10) unsigned DEFAULT 0,
 `call-limit` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `category` varchar(50) NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `useriax__idx__commented` ON `useriax`(`commented`);
CREATE INDEX `useriax__idx__category` ON `useriax`(`category`);
CREATE UNIQUE INDEX `useriax__uidx__name` ON `useriax`(`name`);


DROP TABLE IF EXISTS `uservoicemail`;
CREATE TABLE `uservoicemail` (
 `id` int(10) unsigned auto_increment,
 `customer_id` varchar(11) NOT NULL DEFAULT '0',
 `context` varchar(39) NOT NULL DEFAULT '',
 `mailbox` varchar(40) NOT NULL DEFAULT '0',
 `password` varchar(5) NOT NULL DEFAULT '0',
 `fullname` varchar(150) NOT NULL DEFAULT '',
 `email` varchar(50) NOT NULL DEFAULT '',
 `pager` varchar(50) NOT NULL DEFAULT '',
 `tz` varchar(10) NOT NULL DEFAULT 'central',
 `attach` varchar(4) NOT NULL DEFAULT 'yes',
 `saycid` varchar(4) NOT NULL DEFAULT 'yes',
 `dialout` varchar(10) NOT NULL DEFAULT '',
 `callback` varchar(10) NOT NULL DEFAULT '',
 `review` varchar(4) NOT NULL DEFAULT 'no',
 `operator` varchar(4) NOT NULL DEFAULT 'no',
 `envelope` varchar(4) NOT NULL DEFAULT 'no',
 `sayduration` varchar(4) NOT NULL DEFAULT 'no',
 `saydurationm` tinyint(2) unsigned NOT NULL DEFAULT 2,
 `sendvoicemail` varchar(4) NOT NULL DEFAULT 'no',
 `delete` varchar(4) NOT NULL DEFAULT 'no',
 `nextaftercmd` varchar(5) NOT NULL DEFAULT 'yes',
 `forcename` varchar(4) NOT NULL DEFAULT 'no',
 `forcegreetings` varchar(4) NOT NULL DEFAULT 'no',
 `hidefromdir` varchar(4) NOT NULL DEFAULT 'yes',
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `uservoicemail__idx__commented` ON `uservoicemail`(`commented`);
CREATE INDEX `uservoicemail__idx__context` ON `uservoicemail`(`context`);
CREATE UNIQUE INDEX `uservoicemail__uidx__mailbox_context` ON `uservoicemail`(`mailbox`,`context`);


DROP TABLE IF EXISTS `userfeatures`;
CREATE TABLE `userfeatures` (
 `id` int(10) unsigned auto_increment,
 `protocol` varchar(50) NOT NULL,
 `protocolid` int(10) unsigned NOT NULL,
 `firstname` varchar(128) NOT NULL DEFAULT '',
 `lastname` varchar(128) NOT NULL DEFAULT '',
 `name` varchar(40) NOT NULL,
 `number` varchar(40) NOT NULL,
 `context` varchar(39),
 `provisioningid` mediumint(8) unsigned NOT NULL,
 `ringseconds` tinyint(2) unsigned DEFAULT 15,
 `ringgroup` tinyint(1) DEFAULT 0,
 `simultcalls` tinyint(2) unsigned DEFAULT 1,
 `popupwidget` tinyint(1) DEFAULT 0,
 `musiconhold` varchar(128) NOT NULL DEFAULT '',
 `outcallerid` varchar(80) NOT NULL DEFAULT '',
 `description` text DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `userfeatures__idx__firstname` ON `userfeatures`(`firstname`);
CREATE INDEX `userfeatures__idx__lastname` ON `userfeatures`(`lastname`);
CREATE INDEX `userfeatures__idx__number` ON `userfeatures`(`number`);
CREATE INDEX `userfeatures__idx__context` ON `userfeatures`(`context`);
CREATE INDEX `userfeatures__idx__musiconhold` ON `userfeatures`(`musiconhold`);
CREATE UNIQUE INDEX `userfeatures__uidx__protocol_name` ON `userfeatures`(`protocol`,`name`);
CREATE UNIQUE INDEX `userfeatures__uidx__protocol_protocolid` ON `userfeatures`(`protocol`,`protocolid`);
CREATE UNIQUE INDEX `userfeatures__uidx__provisioningid` ON `userfeatures`(`provisioningid`);

INSERT INTO `userfeatures` VALUES(1,'sip',1,'Guest','','guest','','initconfig',148378,30,0,5,0,'','');
INSERT INTO `userfeatures` VALUES(2,'sip',2,'XivoSB','','xivosb','','local-extensions',194867,30,0,5,0,'','');


DROP TABLE IF EXISTS `extensions`;
CREATE TABLE `extensions` (
 `id` int(10) unsigned auto_increment,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `context` varchar(39) NOT NULL DEFAULT '',
 `exten` varchar(40) NOT NULL DEFAULT '',
 `priority` tinyint(3) unsigned NOT NULL DEFAULT 0,
 `app` varchar(128) NOT NULL DEFAULT '',
 `appdata` varchar(128) NOT NULL DEFAULT '',
 `name` varchar(128) DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `extensions__idx__commented` ON `extensions`(`commented`);
CREATE INDEX `extensions__idx__context_exten_priority` ON `extensions`(`context`,`exten`,`priority`);
CREATE INDEX `extensions__idx__name` ON `extensions`(`name`);

INSERT INTO `extensions` VALUES(NULL,0,'features','*98',1,'VoiceMailMain','${CALLERID(num)}@${CONTEXT}|s','voicemsg');
INSERT INTO `extensions` VALUES(NULL,0,'features','*98',2,'Hangup','','voicemsg');
INSERT INTO `extensions` VALUES(NULL,0,'features','*20',1,'Macro','features|FWD/Unc/Status|||1','fwdundoall');
INSERT INTO `extensions` VALUES(NULL,0,'features','*20',2,'Macro','features|FWD/RNA/Status|||1','fwdundoall');
INSERT INTO `extensions` VALUES(NULL,0,'features','*20',3,'Macro','features|FWD/Busy/Status|all-forward-off|all-forward-off|1','fwdundoall');
INSERT INTO `extensions` VALUES(NULL,0,'features','*21',1,'Macro','features|FWD/Unc/Status|forward-on|forward-off|1','fwdundounc');
INSERT INTO `extensions` VALUES(NULL,0,'features','*22',1,'Macro','features|FWD/RNA/Status|forward-on|forward-off|1','fwdundorna');
INSERT INTO `extensions` VALUES(NULL,0,'features','*23',1,'Macro','features|FWD/Busy/Status|forward-on|forward-off|1','fwdundobusy');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*21.',1,'Answer','','fwdunc');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*21.',2,'Wait',0.5,'fwdunc');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*21.',3,'Set','USER=${CALLERID(num)}','fwdunc');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*21.',4,'Macro','antiloops','fwdunc');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*21.',5,'Set','DB(${CONTEXT}/users/${USER}/FWD/Unc/Number)=${EXTEN:3}','fwdunc');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*21.',6,'Set','DB(${CONTEXT}/users/${USER}/FWD/Unc/Status)=1','fwdunc');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*21.',7,'Playback','forward-on','fwdunc');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*21.',8,'Hangup','','fwdunc');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*22.',1,'Answer','','fwdrna');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*22.',2,'Wait',0.5,'fwdrna');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*22.',3,'Set','USER=${CALLERID(num)}','fwdrna');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*22.',4,'Macro','antiloops','fwdrna');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*22.',5,'Set','DB(${CONTEXT}/users/${USER}/FWD/RNA/Number)=${EXTEN:3}','fwdrna');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*22.',6,'Set','DB(${CONTEXT}/users/${USER}/FWD/RNA/Status)=1','fwdrna');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*22.',7,'Playback','forward-on','fwdrna');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*22.',8,'Hangup','','fwdrna');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*23.',1,'Answer','','fwdbusy');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*23.',2,'Wait',0.5,'fwdbusy');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*23.',3,'Set','USER=${CALLERID(num)}','fwdbusy');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*23.',4,'Macro','antiloops','fwdbusy');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*23.',5,'Set','DB(${CONTEXT}/users/${USER}/FWD/Busy/Number)=${EXTEN:3}','fwdbusy');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*23.',6,'Set','DB(${CONTEXT}/users/${USER}/FWD/Busy/Status)=1','fwdbusy');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*23.',7,'Playback','forward-on','fwdbusy');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*23.',8,'Hangup','','fwdbusy');
INSERT INTO `extensions` VALUES(NULL,0,'features','*9',1,'Playback','record-message-after-beep','recsnd');
INSERT INTO `extensions` VALUES(NULL,0,'features','*9',2,'Set','ROOT=/usr/share/asterisk/sounds/web-interface/recordings','recsnd');
INSERT INTO `extensions` VALUES(NULL,0,'features','*9',3,'Set','RECORDFILE=${ROOT}/${CALLERID(num)}-${EPOCH}','recsnd');
INSERT INTO `extensions` VALUES(NULL,0,'features','*9',4,'Record','${RECORDFILE}:wav','recsnd');
INSERT INTO `extensions` VALUES(NULL,0,'features','*9',5,'Wait',1,'recsnd');
INSERT INTO `extensions` VALUES(NULL,0,'features','*9',6,'Playback','${RECORDFILE}','recsnd');
INSERT INTO `extensions` VALUES(NULL,0,'features','*9',7,'Hangup','','recsnd');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',1,'Answer','','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',2,'Playback','status-phone','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',3,'Playback','forward-inc','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',4,'Macro','checkdbfeatures|FWD/Unc/Status|1|FWD/Unc/Number','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',5,'Playback','forward-busy','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',6,'Macro','checkdbfeatures|FWD/Busy/Status|1|FWD/Busy/Number','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',7,'Playback','forward-rna','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',8,'Macro','checkdbfeatures|FWD/RNA/Status|1|FWD/RNA/Number','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',9,'Playback','vm-status','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',10,'Macro','checkdbfeatures|VM','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',11,'Playback','screening-status','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',12,'Macro','checkdbfeatures|Screen','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',13,'Playback','record-status','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',14,'Macro','checkdbfeatures|Record','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',15,'Playback','dnd-status','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',16,'Macro','checkdbfeatures|DND','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',17,'Playback','bye','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*10',18,'Hangup','','phonestatus');
INSERT INTO `extensions` VALUES(NULL,0,'features','*24',1,'Macro','features|VM|vm-on|vm-off','enablevm');
INSERT INTO `extensions` VALUES(NULL,0,'features','*25',1,'Macro','features|DND|dnd-on|dnd-off','enablednd');
INSERT INTO `extensions` VALUES(NULL,0,'features','*26',1,'Macro','features|Record|record-call-on|record-call-off','incallrec');
INSERT INTO `extensions` VALUES(NULL,0,'features','*27',1,'Macro','features|Screen|screening-on|screening-off','incallfilter');
INSERT INTO `extensions` VALUES(NULL,0,'features','_*8.',1,'Pickup','${EXTEN:2}','pickup');


DROP TABLE IF EXISTS `generalsip`;
CREATE TABLE `generalsip` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned DEFAULT 0,
 `var_metric` int(10) unsigned DEFAULT 0,
 `commented` tinyint(1) DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(255),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `generalsip__idx__commented` ON `generalsip`(`commented`);
CREATE INDEX `generalsip__idx__filename` ON `generalsip`(`filename`);
CREATE INDEX `generalsip__idx__category` ON `generalsip`(`category`);
CREATE INDEX `generalsip__idx__var_name` ON `generalsip`(`var_name`);

INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','bindport',5060);
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','bindaddr','0.0.0.0');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','videosupport','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','autocreatepeer','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','allowguest','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','promiscredir','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','autodomain','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,1,'sip.conf','general','domain',NULL);
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','allowexternaldomains','yes');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','usereqphone','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','realm','xivo');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','alwaysauthreject','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','useragent','Xivo PBX');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','checkmwi',10);
INSERT INTO `generalsip` VALUES(NULL,0,0,1,'sip.conf','general','regcontext',NULL);
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','callerid','xivo');
INSERT INTO `generalsip` VALUES(NULL,0,0,1,'sip.conf','general','fromdomain',NULL);
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','sipdebug','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','dumphistory','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','recordhistory','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','callevents','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','tos','lowdelay');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','ospauth','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,1,'sip.conf','general','localnet',NULL);
INSERT INTO `generalsip` VALUES(NULL,0,0,1,'sip.conf','general','externip',NULL);
INSERT INTO `generalsip` VALUES(NULL,0,0,1,'sip.conf','general','externhost',NULL);
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','externrefresh',10);
INSERT INTO `generalsip` VALUES(NULL,0,0,1,'sip.conf','general','outboundproxy',NULL);
INSERT INTO `generalsip` VALUES(NULL,0,0,1,'sip.conf','general','outboundproxyport',NULL);
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','disallow','all');
INSERT INTO `generalsip` VALUES(NULL,0,0,1,'sip.conf','general','allow',NULL);
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','relaxdtmf','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','compactheaders','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','rtptimeout',0);
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','rtpholdtimeout',0);
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','rtpkeepalive',0);
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','notifymimetype','application/simple-message-summary');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','srvlookup','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','pedantic','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','maxexpiry',3600);
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','defaultexpiry',120);
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','registertimeout',20);
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','registerattempts',0);
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','notifyringing','yes');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','context','default');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','nat','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','dtmfmode','info');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','qualify','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','useclientcode','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','progressinband','never');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','language','fr');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','musiconhold','default');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','vmexten','*98');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','trustrpid','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','sendrpid','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','canreinvite','yes');
INSERT INTO `generalsip` VALUES(NULL,0,0,1,'sip.conf','general','insecure',NULL);
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','rtcachefriends','yes');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','rtupdate','yes');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','ignoreregexpire','no');
INSERT INTO `generalsip` VALUES(NULL,0,0,0,'sip.conf','general','rtautoclear','yes');


DROP TABLE IF EXISTS `generaliax`;
CREATE TABLE `generaliax` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned DEFAULT 0,
 `var_metric` int(10) unsigned DEFAULT 0,
 `commented` tinyint(1) DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(255),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `generaliax__idx__commented` ON `generaliax`(`commented`);
CREATE INDEX `generaliax__idx__filename` ON `generaliax`(`filename`);
CREATE INDEX `generaliax__idx__category` ON `generaliax`(`category`);
CREATE INDEX `generaliax__idx__var_name` ON `generaliax`(`var_name`);

INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','bindport','4569');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','bindaddr','0.0.0.0');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','iaxcompat','no');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','authdebug','yes');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','delayreject','no');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','trunkfreq','20');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','trunktimestamps','yes');
INSERT INTO `generaliax` VALUES(NULL,0,0,1,'iax.conf','general','regcontext',NULL);
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','minregexpire','60');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','maxregexpire','60');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','bandwidth','high');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','tos','lowdelay');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','jitterbuffer','no');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','forcejitterbuffer','no');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','dropcount','3');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','maxjitterbuffer','1000');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','maxjitterinterps','10');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','resyncthreshold','1000');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','minexcessbuffer','10');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','maxexcessbuffer','50');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','jittershrinkrate','2');
INSERT INTO `generaliax` VALUES(NULL,0,0,1,'iax.conf','general','accountcode',NULL);
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','amaflags','default');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','mailboxdetail','yes');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','notransfer','no');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','language','fr');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','encryption','no');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','maxauthreq','0');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','codecpriority','host');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','disallow','all');
INSERT INTO `generaliax` VALUES(NULL,0,0,1,'iax.conf','general','allow',NULL);
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','rtcachefriends','yes');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','rtupdate','yes');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','rtignoreregexpire','no');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','rtautoclear','yes');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','pingtime','20');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','lagrqtime','10');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','nochecksums','no');
INSERT INTO `generaliax` VALUES(NULL,0,0,0,'iax.conf','general','autokill','yes');


DROP TABLE IF EXISTS `generalvoicemail`;
CREATE TABLE `generalvoicemail` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned DEFAULT 0,
 `var_metric` int(10) unsigned DEFAULT 0,
 `commented` tinyint(1) DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` text,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `generalvoicemail__idx__commented` ON `generalvoicemail`(`commented`);
CREATE INDEX `generalvoicemail__idx__filename` ON `generalvoicemail`(`filename`);
CREATE INDEX `generalvoicemail__idx__category` ON `generalvoicemail`(`category`);
CREATE INDEX `generalvoicemail__idx__var_name` ON `generalvoicemail`(`var_name`);

INSERT INTO `generalvoicemail` VALUES(1,0,0,0,'voicemail.conf','general','maxmessage',180);
INSERT INTO `generalvoicemail` VALUES(2,0,0,0,'voicemail.conf','general','minmessage',2);
INSERT INTO `generalvoicemail` VALUES(3,0,0,0,'voicemail.conf','general','maxsilence',1);
INSERT INTO `generalvoicemail` VALUES(4,0,0,0,'voicemail.conf','general','review','yes');
INSERT INTO `generalvoicemail` VALUES(5,0,0,0,'voicemail.conf','general','serveremail','voicemail@xivo');
INSERT INTO `generalvoicemail` VALUES(6,0,0,0,'voicemail.conf','general','fromstring','XIVO PBX');
INSERT INTO `generalvoicemail` VALUES(7,0,0,0,'voicemail.conf','general','maxmsg',100);
INSERT INTO `generalvoicemail` VALUES(8,0,0,0,'voicemail.conf','general','emailsubject','Messagerie XIVO');
INSERT INTO `generalvoicemail` VALUES(9,0,0,0,'voicemail.conf','general','emailbody','Bonjour ${VM_NAME} !

Vous avez reçu un message d\'une durée de ${VM_DUR} minute(s), il vous reste actuellement ${VM_MSGNUM} message(s) non lu(s) sur votre messagerie vocale : ${VM_MAILBOX}.
Le dernier a été envoyé par ${VM_CALLERID}, le ${VM_DATE}. Si vous le souhaitez vous pouvez l\'écouter ou le consulter en tapant le *98 sur votre téléphone. Merci !

-- Messagerie XIVO --');
INSERT INTO `generalvoicemail` VALUES(10,0,0,0,'voicemail.conf','general','charset','UTF-8');
INSERT INTO `generalvoicemail` VALUES(11,0,0,0,'voicemail.conf','zonemessages','eu-fr','Europe/Paris|\'vm-received\' q \'digits/at\' kM');
INSERT INTO `generalvoicemail` VALUES(12,0,0,0,'voicemail.conf','general','tz','eu-fr');
INSERT INTO `generalvoicemail` VALUES(13,0,0,0,'voicemail.conf','general','externpass','/usr/share/asterisk/bin/change-pass-vm');


DROP TABLE IF EXISTS `generaloutcall`;
CREATE TABLE `generaloutcall` (
 `id` int(10) unsigned auto_increment,
 `extenumid` int(10) unsigned NOT NULL DEFAULT 0,
 `trunkfeaturesid` int(10) unsigned NOT NULL DEFAULT 0,
 `type` varchar(80) NOT NULL,
 `commented` tinyint(1) DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `generaloutcall__idx__trunkfeaturesid` ON `generaloutcall`(`trunkfeaturesid`);
CREATE INDEX `generaloutcall__idx__type` ON `generaloutcall`(`type`);
CREATE INDEX `generaloutcall__idx__commented` ON `generaloutcall`(`commented`);
CREATE UNIQUE INDEX `generaloutcall__uidx__extenumid` ON `generaloutcall`(`extenumid`);


DROP TABLE IF EXISTS `outcall`;
CREATE TABLE `outcall` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(128) NOT NULL DEFAULT '',
 `trunkfeaturesid` int(10) unsigned NOT NULL DEFAULT 0,
 `context` varchar(39) NOT NULL,
 `externprefix` varchar(20) NOT NULL DEFAULT '',
 `stripnum` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `mode` enum('wizard','extension') NOT NULL,
 `prefix` varchar(20) NOT NULL DEFAULT '',
 `numlen` char(2) NOT NULL,
 `extenumid` int(10) unsigned NOT NULL DEFAULT 0,
 `setcallerid` tinyint(1) NOT NULL DEFAULT 0,
 `callerid` varchar(80) NOT NULL DEFAULT '',
 `weight` tinyint(2) unsigned NOT NULL DEFAULT 1,
 `useenum` tinyint(1) NOT NULL DEFAULT 0,
 `hangupringtime` smallint(3) unsigned NOT NULL DEFAULT 0,
 `linked` tinyint(1) NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `outcall__idx__trunkfeaturesid` ON `outcall`(`trunkfeaturesid`);
CREATE INDEX `outcall__idx__context` ON `outcall`(`context`);
CREATE INDEX `outcall__idx__mode` ON `outcall`(`mode`);
CREATE INDEX `outcall__idx__numlen` ON `outcall`(`numlen`);
CREATE INDEX `outcall__idx__weight` ON `outcall`(`weight`);
CREATE INDEX `outcall__idx__linked` ON `outcall`(`linked`);
CREATE INDEX `outcall__idx__commented` ON `outcall`(`commented`);
CREATE UNIQUE INDEX `outcall__uidx__name` ON `outcall`(`name`);
CREATE UNIQUE INDEX `outcall__uidx__extenumid` ON `outcall`(`extenumid`);


DROP TABLE IF EXISTS `schedule`;
CREATE TABLE `schedule` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(128) NOT NULL DEFAULT '',
 `timebeg` varchar(5) NOT NULL DEFAULT '*',
 `timeend` varchar(5),
 `daynamebeg` varchar(3) NOT NULL DEFAULT '*',
 `daynameend` varchar(3),
 `daynumbeg` varchar(2) NOT NULL DEFAULT '*',
 `daynumend` varchar(2),
 `monthbeg` varchar(3) NOT NULL DEFAULT '*',
 `monthend` varchar(3),
 `typetrue` varchar(64) NOT NULL DEFAULT '',
 `typevaltrue` varchar(255) NOT NULL DEFAULT '',
 `typefalse` varchar(64) NOT NULL DEFAULT '',
 `typevalfalse` varchar(255) NOT NULL DEFAULT '',
 `publicholiday` tinyint(1) NOT NULL DEFAULT 0,
 `linked` tinyint(1) NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `schedule__idx__typetrue` ON `schedule`(`typetrue`);
CREATE INDEX `schedule__idx__typevaltrue` ON `schedule`(`typevaltrue`);
CREATE INDEX `schedule__idx__typefalse` ON `schedule`(`typefalse`);
CREATE INDEX `schedule__idx__typevalfalse` ON `schedule`(`typevalfalse`);
CREATE INDEX `schedule__idx__publicholiday` ON `schedule`(`publicholiday`);
CREATE INDEX `schedule__idx__linked` ON `schedule`(`linked`);
CREATE INDEX `schedule__idx__commented` ON `schedule`(`commented`);
CREATE UNIQUE INDEX `schedule__uidx__name` ON `schedule`(`name`);


DROP TABLE IF EXISTS `generalqueue`;
CREATE TABLE `generalqueue` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned DEFAULT 0,
 `var_metric` int(10) unsigned DEFAULT 0,
 `commented` tinyint(1) DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(128),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `generalqueue__idx__commented` ON `generalqueue`(`commented`);
CREATE INDEX `generalqueue__idx__filename` ON `generalqueue`(`filename`);
CREATE INDEX `generalqueue__idx__category` ON `generalqueue`(`category`);
CREATE INDEX `generalqueue__idx__var_name` ON `generalqueue`(`var_name`);

INSERT INTO `generalqueue` VALUES(1,0,0,0,'queues.conf','general','persistentmembers','yes');


DROP TABLE IF EXISTS `usersip`;
CREATE TABLE `usersip` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(40) NOT NULL,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `accountcode` varchar(20),
 `amaflags` varchar(13),
 `callgroup` varchar(180),
 `callerid` varchar(160),
 `canreinvite` char(3),
 `context` varchar(39),
 `defaultip` varchar(15),
 `dtmfmode` varchar(7),
 `fromuser` varchar(80),
 `fromdomain` varchar(80),
 `fullcontact` varchar(80),
 `host` varchar(31) NOT NULL,
 `insecure` varchar(11),
 `language` char(2),
 `mailbox` varchar(80),
 `md5secret` varchar(80),
 `nat` varchar(5) NOT NULL DEFAULT 'no',
 `deny` varchar(95),
 `permit` varchar(95),
 `mask` varchar(95),
 `pickupgroup` varchar(80),
 `port` smallint unsigned,
 `qualify` char(3),
 `restrictcid` char(1),
 `rtptimeout` char(3),
 `rtpholdtimeout` char(3),
 `secret` varchar(80),
 `type` varchar(6) NOT NULL DEFAULT 'friend',
 `username` varchar(80) NOT NULL,
 `disallow` varchar(100),
 `allow` varchar(100),
 `musiconhold` varchar(100),
 `regseconds` int(10) unsigned NOT NULL DEFAULT 0,
 `ipaddr` varchar(15) NOT NULL,
 `regexten` varchar(80) NOT NULL,
 `cancallforward` char(3),
 `setvar` varchar(100) NOT NULL,
 `call-limit` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `category` varchar(50) NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `usersip__idx__commented` ON `usersip`(`commented`);
CREATE INDEX `usersip__idx__category` ON `usersip`(`category`);
CREATE UNIQUE INDEX `usersip__uidx__name` ON `usersip`(`name`);

INSERT INTO `usersip` VALUES(1,'guest',0,'','documentation','','Guest','no','initconfig',NULL,'rfc2833',NULL,NULL,'','dynamic',NULL,NULL,'',NULL,'no',NULL,NULL,NULL,'',5060,'no',NULL,NULL,NULL,'guest','friend','guest',NULL,NULL,NULL,'','','',NULL,'',0,'user');
INSERT INTO `usersip` VALUES(2,'xivosb',0,'','documentation','','XivoSB','no','local-extensions',NULL,'rfc2833',NULL,NULL,'','dynamic',NULL,NULL,'',NULL,'no',NULL,NULL,NULL,'',5060,'no',NULL,NULL,NULL,'','friend','xivosb',NULL,NULL,NULL,'','','',NULL,'',0,'user');


DROP TABLE IF EXISTS `musiconhold`;
CREATE TABLE `musiconhold` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned DEFAULT 0,
 `var_metric` int(10) unsigned DEFAULT 0,
 `commented` tinyint(1) DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(128),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `musiconhold__idx__commented` ON `musiconhold`(`commented`);
CREATE UNIQUE INDEX `musiconhold__uidx__filename_category_var_name` ON `musiconhold`(`filename`(48),`category`,`var_name`);

INSERT INTO `musiconhold` VALUES(1,0,0,0,'musiconhold.conf','default','mode','custom');
INSERT INTO `musiconhold` VALUES(2,0,0,0,'musiconhold.conf','default','application','/usr/bin/madplay --mono -a -10 -R 8000 --output=raw:-');
INSERT INTO `musiconhold` VALUES(3,0,0,0,'musiconhold.conf','default','random','no');
INSERT INTO `musiconhold` VALUES(4,0,0,0,'musiconhold.conf','default','directory','/usr/share/asterisk/moh/default');


DROP TABLE IF EXISTS `agent`;
CREATE TABLE `agent` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned DEFAULT 0,
 `var_metric` int(10) unsigned DEFAULT 0,
 `commented` tinyint(1) DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(128),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `agent__idx__cat_metric` ON `agent`(`cat_metric`);
CREATE INDEX `agent__idx__var_metric` ON `agent`(`var_metric`);
CREATE INDEX `agent__idx__commented` ON `agent`(`commented`);
CREATE INDEX `agent__idx__filename` ON `agent`(`filename`);
CREATE INDEX `agent__idx__category` ON `agent`(`category`);
CREATE INDEX `agent__idx__var_name` ON `agent`(`var_name`);
CREATE INDEX `agent__idx__var_val` ON `agent`(`var_val`);

INSERT INTO `agent` VALUES(1,0,0,0,'agents.conf','general','persistentagents','yes');
INSERT INTO `agent` VALUES(2,1,1000000,0,'agents.conf','agents','group',1);


DROP TABLE IF EXISTS `agentfeatures`;
CREATE TABLE `agentfeatures` (
 `id` int(10) unsigned auto_increment,
 `agentid` int(10) unsigned NOT NULL,
 `numgroup` tinyint(2) unsigned NOT NULL,
 `firstname` varchar(128) NOT NULL DEFAULT '',
 `lastname` varchar(128) NOT NULL DEFAULT '',
 `number` varchar(40) NOT NULL,
 `passwd` varchar(128) NOT NULL,
 `commented` tinyint(1) DEFAULT 0,
 `silent` tinyint(1) DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `agentfeatures__idx__commented` ON `agentfeatures`(`commented`);
CREATE UNIQUE INDEX `agentfeatures__uidx__number` ON `agentfeatures`(`number`);
CREATE UNIQUE INDEX `agentfeatures__uidx__agentid` ON `agentfeatures`(`agentid`);


DROP TABLE IF EXISTS `agentgroup`;
CREATE TABLE `agentgroup` (
 `id` tinyint(2) unsigned auto_increment,
 `groupid` int(10) unsigned NOT NULL,
 `name` varchar(128) NOT NULL DEFAULT '',
 `groups` varchar(255) DEFAULT '',
 `commented` tinyint(1) DEFAULT 0,
 `deleted` tinyint(1) DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `agentgroup__idx__groupid` ON `agentgroup`(`groupid`);
CREATE INDEX `agentgroup__idx__name` ON `agentgroup`(`name`);
CREATE INDEX `agentgroup__idx__commented` ON `agentgroup`(`commented`);
CREATE INDEX `agentgroup__idx__deleted` ON `agentgroup`(`deleted`);

INSERT INTO `agentgroup` VALUES(1,2,'default','',0,0);


DROP TABLE IF EXISTS `rightcall`;
CREATE TABLE `rightcall` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(128) NOT NULL DEFAULT '',
 `passwd` varchar(40) NOT NULL DEFAULT '',
 `permit` tinyint(1) NOT NULL DEFAULT 0,
 `description` text DEFAULT '',
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `rightcall__idx__passwd` ON `rightcall`(`passwd`);
CREATE INDEX `rightcall__idx__permit` ON `rightcall`(`permit`);
CREATE INDEX `rightcall__idx__commented` ON `rightcall`(`commented`);
CREATE UNIQUE INDEX `rightcall__uidx__name` ON `rightcall`(`name`);


DROP TABLE IF EXISTS `rightcallexten`;
CREATE TABLE `rightcallexten` (
 `id` int(10) unsigned auto_increment,
 `rightcallid` int(10) unsigned NOT NULL DEFAULT 0,
 `exten` varchar(40) NOT NULL DEFAULT '',
 `extenhash` char(40) NOT NULL DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE UNIQUE INDEX `rightcallexten__uidx__rightcallid_extenhash` ON `rightcallexten`(`rightcallid`,`extenhash`);


DROP TABLE IF EXISTS `rightcallmember`;
CREATE TABLE `rightcallmember` (
 `id` int(10) unsigned auto_increment,
 `rightcallid` int(10) unsigned NOT NULL DEFAULT 0,
 `type` varchar(64) NOT NULL DEFAULT '',
 `typeval` varchar(128) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE UNIQUE INDEX `rightcallmember__uidx__rightcallid_type_typeval` ON `rightcallmember`(`rightcallid`,`type`,`typeval`);


DROP TABLE IF EXISTS `dialstatus`;
CREATE TABLE `dialstatus` (
 `id` int(10) unsigned auto_increment,
 `status` enum('noanswer','congestion','busy','chanunavail') NOT NULL,
 `category` enum('user','group') NOT NULL,
 `categoryval` varchar(128) NOT NULL DEFAULT '',
 `type` varchar(64) NOT NULL DEFAULT '',
 `typeval` varchar(255) NOT NULL DEFAULT '',
 `linked` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `dialstatus__idx__linked` ON `dialstatus`(`linked`);
CREATE INDEX `dialstatus__idx__type` ON `dialstatus`(`type`);
CREATE INDEX `dialstatus__idx__typeval` ON `dialstatus`(`typeval`);
CREATE UNIQUE INDEX `dialstatus__uidx__status_category_categoryval` ON `dialstatus`(`status`,`category`,`categoryval`);
