UPDATE `usersip` SET `ipaddr` = '' WHERE `ipaddr` IS NULL;

ALTER TABLE `usersip` MODIFY COLUMN `type` enum('friend','peer','user') NOT NULL AFTER `name`;
ALTER TABLE `usersip` MODIFY COLUMN `username` varchar(80) AFTER `type`;
ALTER TABLE `usersip` MODIFY COLUMN `secret` varchar(80) NOT NULL DEFAULT '' AFTER `username`;
ALTER TABLE `usersip` MODIFY COLUMN `md5secret` varchar(32) NOT NULL DEFAULT '' AFTER `secret`;
ALTER TABLE `usersip` MODIFY COLUMN `language` varchar(20) AFTER `context`;
ALTER TABLE `usersip` MODIFY COLUMN `accountcode` varchar(20) AFTER `language`;
ALTER TABLE `usersip` MODIFY COLUMN `amaflags` enum('default',
						    'omit',
						    'billing',
						    'documentation') NOT NULL DEFAULT 'default' AFTER `accountcode`;
ALTER TABLE `usersip` ADD COLUMN `allowtransfer` tinyint(1) AFTER `amaflags`;
ALTER TABLE `usersip` MODIFY COLUMN `fromdomain` varchar(255) AFTER `fromuser`;
ALTER TABLE `usersip` ADD COLUMN `subscribemwi` tinyint(1) NOT NULL DEFAULT 1 AFTER `mailbox`;
ALTER TABLE `usersip` ADD COLUMN `buggymwi` tinyint(1) AFTER `subscribemwi`;
ALTER TABLE `usersip` MODIFY COLUMN `call-limit` tinyint(2) unsigned NOT NULL DEFAULT 0 AFTER `buggymwi`;
ALTER TABLE `usersip` MODIFY COLUMN `callerid` varchar(160) AFTER `call-limit`;
ALTER TABLE `usersip` ADD COLUMN `fullname` varchar(80) AFTER `callerid`;
ALTER TABLE `usersip` ADD COLUMN `cid_number` varchar(80) AFTER `fullname`;
ALTER TABLE `usersip` ADD COLUMN `maxcallbitrate` smallint(4) unsigned AFTER `cid_number`;
ALTER TABLE `usersip` MODIFY COLUMN `insecure` enum('port','invite','port,invite') AFTER `maxcallbitrate`;
ALTER TABLE `usersip` MODIFY COLUMN `nat` enum('no','yes','never','route') AFTER `insecure`;
ALTER TABLE `usersip` MODIFY COLUMN `canreinvite` enum('no','yes','update','nonat','update,nonat') AFTER `nat`;
ALTER TABLE `usersip` ADD COLUMN `promiscredir` tinyint(1) AFTER `canreinvite`;
ALTER TABLE `usersip` ADD COLUMN `usereqphone` tinyint(1) AFTER `promiscredir`;
ALTER TABLE `usersip` ADD COLUMN `videosupport` tinyint(1) AFTER `usereqphone`;
ALTER TABLE `usersip` ADD COLUMN `trustrpid` tinyint(1) AFTER `videosupport`;
ALTER TABLE `usersip` ADD COLUMN `sendrpid` tinyint(1) AFTER `trustrpid`;
ALTER TABLE `usersip` ADD COLUMN `allowsubscribe` tinyint(1) AFTER `sendrpid`;
ALTER TABLE `usersip` ADD COLUMN `allowoverlap` tinyint(1) AFTER `allowsubscribe`;
ALTER TABLE `usersip` MODIFY COLUMN `dtmfmode` enum('rfc2833','inband','info','auto') AFTER `allowoverlap`;
ALTER TABLE `usersip` ADD COLUMN `rfc2833compensate` tinyint(1) AFTER `dtmfmode`;
ALTER TABLE `usersip` MODIFY COLUMN `qualify` varchar(4) AFTER `rfc2833compensate`;
ALTER TABLE `usersip` ADD COLUMN `g726nonstandard` tinyint(1) AFTER `qualify`;
ALTER TABLE `usersip` ADD COLUMN `autoframing` tinyint(1) AFTER `allow`;
ALTER TABLE `usersip` CHANGE COLUMN `musiconhold` `mohinterpret` varchar(80) AFTER `autoframing`;
ALTER TABLE `usersip` ADD COLUMN `mohsuggest` varchar(80) AFTER `mohinterpret`;
ALTER TABLE `usersip` ADD COLUMN `useclientcode` tinyint(1) AFTER `mohsuggest`;
ALTER TABLE `usersip` ADD COLUMN `progressinband` enum('no','yes','never') AFTER `useclientcode`;
ALTER TABLE `usersip` ADD COLUMN `t38pt_udptl` tinyint(1) AFTER `progressinband`;
ALTER TABLE `usersip` ADD COLUMN `t38pt_rtp` tinyint(1) AFTER `t38pt_udptl`;
ALTER TABLE `usersip` ADD COLUMN `t38pt_tcp` tinyint(1) AFTER `t38pt_rtp`;
ALTER TABLE `usersip` ADD COLUMN `t38pt_usertpsource` tinyint(1) AFTER `t38pt_tcp`;
ALTER TABLE `usersip` MODIFY COLUMN `rtptimeout` tinyint unsigned AFTER `t38pt_usertpsource`;
ALTER TABLE `usersip` MODIFY COLUMN `rtpholdtimeout` tinyint unsigned AFTER `rtptimeout`;
ALTER TABLE `usersip` ADD COLUMN `rtpkeepalive` tinyint unsigned AFTER `rtpholdtimeout`;
ALTER TABLE `usersip` MODIFY COLUMN `deny` varchar(31) AFTER `rtpkeepalive`;
ALTER TABLE `usersip` MODIFY COLUMN `permit` varchar(31) AFTER `deny`;
ALTER TABLE `usersip` MODIFY COLUMN `defaultip` varchar(255) AFTER `permit`;
ALTER TABLE `usersip` MODIFY COLUMN `callgroup` varchar(180) AFTER `defaultip`;
ALTER TABLE `usersip` MODIFY COLUMN `pickupgroup` varchar(180) AFTER `callgroup`;
ALTER TABLE `usersip` MODIFY COLUMN `setvar` varchar(100) NOT NULL DEFAULT '' AFTER `pickupgroup`;
ALTER TABLE `usersip` MODIFY COLUMN `host` varchar(255) NOT NULL DEFAULT 'dynamic' AFTER `setvar`;
ALTER TABLE `usersip` MODIFY COLUMN `port` smallint unsigned AFTER `host`;
ALTER TABLE `usersip` ADD COLUMN `subscribecontext` varchar(80) AFTER `regexten`;
ALTER TABLE `usersip` MODIFY COLUMN `fullcontact` varchar(255) AFTER `subscribecontext`;
ALTER TABLE `usersip` ADD COLUMN `vmexten` varchar(40) AFTER `fullcontact`;
ALTER TABLE `usersip` ADD COLUMN `callingpres` tinyint(1) AFTER `vmexten`;
ALTER TABLE `usersip` MODIFY COLUMN `ipaddr` varchar(255) NOT NULL DEFAULT '' AFTER `callingpres`;
ALTER TABLE `usersip` MODIFY COLUMN `regseconds` int(10) unsigned NOT NULL DEFAULT 0 AFTER `ipaddr`;
ALTER TABLE `usersip` ADD COLUMN `regserver` varchar(20) AFTER `regseconds`;
ALTER TABLE `usersip` MODIFY COLUMN `commented` tinyint(1) NOT NULL DEFAULT 0 AFTER `category`;
ALTER TABLE `usersip` DROP COLUMN `restrictcid`;
ALTER TABLE `usersip` DROP COLUMN `cancallforward`;

CREATE INDEX `usersip__idx__mailbox` ON `usersip`(`mailbox`);
CREATE INDEX `usersip__idx__host_port` ON `usersip`(`host`,`port`);
CREATE INDEX `usersip__idx__ipaddr_port` ON `usersip`(`ipaddr`,`port`);

UPDATE `usersip`
SET
	`type` = 'friend',
	`md5secret` = '',
	`context` = IF(`context` = 'initconfig','xivo-initconfig',`context`),
	`language` = NULL,
	`accountcode` = NULLIF(`accountcode`,''),
	`amaflags` = IFNULL(NULLIF(`amaflags`,''),'default'),
	`allowtransfer` = NULL,
	`fromuser` = NULLIF(`fromuser`,''),
	`fromdomain` = NULLIF(`fromdomain`,''),
	`mailbox` = NULLIF(`mailbox`,''),
	`subscribemwi` = 1,
	`buggymwi` = 0,
	`username` = NULL,
	`cid_number` = NULL,
	`maxcallbitrate` = NULL,
	`insecure` = NULLIF(`insecure`,''),
	`nat` = NULLIF(`nat`,''),
	`canreinvite` = NULLIF(`canreinvite`,''),
	`promiscredir` = NULL,
	`usereqphone` = NULL,
	`videosupport` = NULL,
	`trustrpid` = NULL,
	`sendrpid` = NULL,
	`allowsubscribe` = NULL,
	`allowoverlap` = NULL,
	`dtmfmode` = NULLIF(`dtmfmode`,''),
	`rfc2833compensate` = NULL,
	`qualify` = NULLIF(`qualify`,''),
	`g726nonstandard` = NULL,
	`disallow` = NULLIF(`disallow`,''),
	`allow` = NULLIF(`allow`,''),
	`autoframing` = NULL,
	`mohinterpret` = NULL,
	`mohsuggest` = NULL,
	`useclientcode` = NULL,
	`progressinband` = NULL,
	`t38pt_udptl` = NULL,
	`t38pt_rtp` = NULL,
	`t38pt_tcp` = NULL,
	`t38pt_usertpsource` = NULL,
	`rtptimeout` = NULLIF(`rtptimeout`,''),
	`rtpholdtimeout` = NULLIF(`rtpholdtimeout`,''),
	`rtpkeepalive` = NULL,
	`deny` = NULLIF(`deny`,''),
	`permit` = NULLIF(`permit`,''),
	`defaultip` = NULLIF(`defaultip`,''),
	`callgroup` = NULLIF(`callgroup`,''),
	`pickupgroup` = NULLIF(`pickupgroup`,''),
	`setvar` = '',
	`host` = IFNULL(NULLIF(`host`,''),'dynamic'),
	`port` = NULLIF(`port`,''),
	`regexten` = NULLIF(`regexten`,''),
	`subscribecontext` = NULL,
	`fullcontact` = NULLIF(`fullcontact`,''),
	`vmexten` = NULL,
	`callingpres` = NULL,
	`ipaddr` = IFNULL(`ipaddr`,''),
	`regseconds` = IFNULL(`regseconds`,0),
	`regserver` = NULL,
	`protocol` = 'sip',
	`category` = 'user'
WHERE category = 'user';

UPDATE `usersip`
SET
	`md5secret` = '',
	`context` = IF(`context` = 'initconfig','xivo-initconfig',NULLIF(`context`,'')),
	`language` = NULL,
	`accountcode` = NULLIF(`accountcode`,''),
	`amaflags` = IFNULL(NULLIF(`amaflags`,''),'default'),
	`allowtransfer` = NULL,
	`fromuser` = NULLIF(`fromuser`,''),
	`fromdomain` = NULLIF(`fromdomain`,''),
	`mailbox` = NULL,
	`subscribemwi` = 0,
	`buggymwi` = NULL,
	`username` = NULL,
	`cid_number` = NULL,
	`maxcallbitrate` = NULL,
	`insecure` = NULLIF(`insecure`,''),
	`nat` = NULLIF(`nat`,''),
	`canreinvite` = NULLIF(`canreinvite`,''),
	`promiscredir` = NULL,
	`usereqphone` = NULL,
	`videosupport` = NULL,
	`trustrpid` = NULL,
	`sendrpid` = NULL,
	`allowsubscribe` = NULL,
	`allowoverlap` = NULL,
	`dtmfmode` = NULLIF(`dtmfmode`,''),
	`rfc2833compensate` = NULL,
	`qualify` = NULLIF(`qualify`,''),
	`g726nonstandard` = NULL,
	`disallow` = NULLIF(`disallow`,''),
	`allow` = NULLIF(`allow`,''),
	`autoframing` = NULL,
	`mohinterpret` = NULL,
	`mohsuggest` = NULL,
	`useclientcode` = NULL,
	`progressinband` = NULL,
	`t38pt_udptl` = NULL,
	`t38pt_rtp` = NULL,
	`t38pt_tcp` = NULL,
	`t38pt_usertpsource` = NULL,
	`rtptimeout` = NULLIF(`rtptimeout`,''),
	`rtpholdtimeout` = NULLIF(`rtpholdtimeout`,''),
	`rtpkeepalive` = NULL,
	`deny` = NULLIF(`deny`,''),
	`permit` = NULLIF(`permit`,''),
	`defaultip` = NULL,
	`callgroup` = NULL,
	`pickupgroup` = NULL,
	`setvar` = '',
	`host` = IFNULL(NULLIF(`host`,''),'dynamic'),
	`port` = NULLIF(`port`,''),
	`regexten` = NULLIF(`regexten`,''),
	`subscribecontext` = NULL,
	`fullcontact` = NULLIF(`fullcontact`,''),
	`vmexten` = NULL,
	`callingpres` = NULL,
	`ipaddr` = IFNULL(`ipaddr`,''),
	`regseconds` = IFNULL(`regseconds`,0),
	`regserver` = NULL,
	`protocol` = 'sip',
	`category` = 'trunk'
WHERE category = 'trunk';
