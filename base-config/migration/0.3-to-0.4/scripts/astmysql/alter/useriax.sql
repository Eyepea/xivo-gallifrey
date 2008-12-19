UPDATE `useriax` SET `ipaddr` = '' WHERE `ipaddr` IS NULL;
UPDATE `useriax` SET `trunk` = IF(`trunk` = 'yes',1,IFNULL(NULLIF(`trunk`,'no'),0));
UPDATE `useriax` SET `notransfer` = NULL;

ALTER TABLE `useriax` MODIFY COLUMN `username` varchar(80) AFTER `type`;
ALTER TABLE `useriax` MODIFY COLUMN `secret` varchar(80) NOT NULL DEFAULT '' AFTER `username`;
ALTER TABLE `useriax` MODIFY COLUMN `dbsecret` varchar(255) NOT NULL DEFAULT '' AFTER `secret`;
ALTER TABLE `useriax` MODIFY COLUMN `language` varchar(20) AFTER `context`;
ALTER TABLE `useriax` MODIFY COLUMN `accountcode` varchar(20) AFTER `language`;
ALTER TABLE `useriax` MODIFY COLUMN `amaflags` enum('default',
						    'omit',
						    'billing',
						    'documentation') DEFAULT 'default' AFTER `accountcode`;
ALTER TABLE `useriax` MODIFY COLUMN `callerid` varchar(160) AFTER `mailbox`;
ALTER TABLE `useriax` ADD COLUMN `fullname` varchar(80) AFTER `callerid`;
ALTER TABLE `useriax` ADD COLUMN `cid_number` varchar(80) AFTER `fullname`;
ALTER TABLE `useriax` MODIFY COLUMN `trunk` tinyint(1) NOT NULL DEFAULT 0 AFTER `cid_number`;
ALTER TABLE `useriax` MODIFY COLUMN `auth` enum('plaintext',
						'md5',
						'rsa',
						'plaintext,md5',
						'plaintext,rsa',
						'md5,rsa',
						'plaintext,md5,rsa') NOT NULL DEFAULT 'plaintext,md5' AFTER `trunk`;
ALTER TABLE `useriax` ADD COLUMN `encryption` enum('no','yes','aes128') AFTER `auth`;
ALTER TABLE `useriax` ADD COLUMN `maxauthreq` tinyint(2) unsigned AFTER `encryption`;
ALTER TABLE `useriax` MODIFY COLUMN `inkeys` varchar(80) AFTER `maxauthreq`;
ALTER TABLE `useriax` MODIFY COLUMN `outkey` varchar(80) AFTER `inkeys`;
ALTER TABLE `useriax` ADD COLUMN `adsi` tinyint(1) AFTER `outkey`;
ALTER TABLE `useriax` CHANGE COLUMN `notransfer` `transfer` enum('no','yes','mediaonly') AFTER `adsi`;
ALTER TABLE `useriax` ADD COLUMN `codecpriority` enum('disabled','host','caller','reqonly') AFTER `transfer`;
ALTER TABLE `useriax` ADD COLUMN `jitterbuffer` tinyint(1) AFTER `codecpriority`;
ALTER TABLE `useriax` ADD COLUMN `forcejitterbuffer` tinyint(1) AFTER `jitterbuffer`;
ALTER TABLE `useriax` ADD COLUMN `sendani` tinyint(1) NOT NULL DEFAULT 0 AFTER `forcejitterbuffer`;
ALTER TABLE `useriax` MODIFY COLUMN `qualify` varchar(4) NOT NULL DEFAULT 'no' AFTER `sendani`;
ALTER TABLE `useriax` ADD COLUMN `qualifysmoothing` tinyint(1) NOT NULL DEFAULT 0 AFTER `qualify`;
ALTER TABLE `useriax` ADD COLUMN `qualifyfreqok`int(10) unsigned NOT NULL DEFAULT 60000 AFTER `qualifysmoothing`;
ALTER TABLE `useriax` ADD COLUMN `qualifyfreqnotok` int(10) unsigned NOT NULL DEFAULT 10000 AFTER `qualifyfreqok`;
ALTER TABLE `useriax` ADD COLUMN `timezone` varchar(80) AFTER `qualifyfreqnotok`;
ALTER TABLE `useriax` ADD COLUMN `mohinterpret` varchar(80) AFTER `allow`;
ALTER TABLE `useriax` ADD COLUMN `mohsuggest` varchar(80) AFTER `mohinterpret`;
ALTER TABLE `useriax` MODIFY COLUMN `deny` varchar(31) AFTER `mohsuggest`;
ALTER TABLE `useriax` MODIFY COLUMN `permit` varchar(31) AFTER `deny`;
ALTER TABLE `useriax` MODIFY COLUMN `defaultip` varchar(255) AFTER `permit`;
ALTER TABLE `useriax` ADD COLUMN `sourceaddress` varchar(255) AFTER `defaultip`;
ALTER TABLE `useriax` ADD COLUMN `setvar` varchar(100) NOT NULL DEFAULT '' AFTER `sourceaddress`;
ALTER TABLE `useriax` MODIFY COLUMN `host` varchar(255) NOT NULL DEFAULT 'dynamic' AFTER `setvar`;
ALTER TABLE `useriax` ADD COLUMN `mask` varchar(15) AFTER `port`;
ALTER TABLE `useriax` ADD COLUMN `regexten` varchar(80) AFTER `mask`;
ALTER TABLE `useriax` ADD COLUMN `peercontext` varchar(80) AFTER `regexten`;
ALTER TABLE `useriax` MODIFY COLUMN `ipaddr` varchar(255) NOT NULL DEFAULT '' AFTER `peercontext`;
ALTER TABLE `useriax` MODIFY COLUMN `regseconds` int(10) unsigned NOT NULL DEFAULT 0 AFTER `ipaddr`;
ALTER TABLE `useriax` MODIFY COLUMN `commented` tinyint(1) NOT NULL DEFAULT 0 AFTER `category`;
ALTER TABLE `useriax` DROP COLUMN `md5secret`;
ALTER TABLE `useriax` DROP COLUMN `callgroup`;
ALTER TABLE `useriax` DROP COLUMN `call-limit`;

CREATE INDEX `useriax__idx__mailbox` ON `useriax`(`mailbox`);
CREATE INDEX `useriax__idx__name_host` ON `useriax`(`name`,`host`);
CREATE INDEX `useriax__idx__name_ipaddr_port` ON `useriax`(`name`,`ipaddr`,`port`);
CREATE INDEX `useriax__idx__ipaddr_port` ON `useriax`(`ipaddr`,`port`);
CREATE INDEX `useriax__idx__host_port` ON `useriax`(`host`,`port`);

UPDATE `useriax`
SET
	`type` = 'friend',
	`dbsecret` = '',
	`language` = NULL,
	`accountcode` = NULLIF(`accountcode`,''),
	`amaflags` = NULLIF(`amaflags`,''),
	`mailbox` = NULLIF(`mailbox`,''),
	`fullname` = NULL,
	`cid_number` = NULL,
	`trunk` = 0,
	`auth` = 'plaintext,md5',
	`encryption` = NULL,
	`maxauthreq` = NULL,
	`inkeys` = NULL,
	`outkey` = NULL,
	`adsi` = NULL,
	`transfer` = NULL,
	`codecpriority` = NULL,
	`jitterbuffer` = NULL,
	`forcejitterbuffer` = NULL,
	`sendani` = 0,
	`qualifysmoothing` = 0,
	`qualifyfreqok` = 60000,
	`qualifyfreqnotok` = 10000,
	`timezone` = NULL,
	`mohinterpret` = NULL,
	`mohsuggest` = NULL,
	`deny` = NULL,
	`permit` = NULL,
	`defaultip` = NULL,
	`sourceaddress` = NULL,
	`setvar` = '',
	`port` = NULL,
	`mask` = NULL,
	`regexten` = NULL,
	`peercontext` = NULL,
	`ipaddr` = IFNULL(`ipaddr`,''),
	`regseconds` = IFNULL(`regseconds`,0),
	`protocol` = 'iax',
	`category` = 'user'
WHERE category = 'user';

UPDATE `useriax`
SET
	`dbsecret` = '',
	`language` = NULL,
	`context` = NULLIF(`context`,''),
	`accountcode` = NULL,
	`amaflags` = NULL,
	`mailbox` = NULL,
	`fullname` = NULL,
	`cid_number` = NULL,
	`trunk` = 0,
	`auth` = 'plaintext,md5',
	`encryption` = NULL,
	`maxauthreq` = NULL,
	`inkeys` = NULL,
	`outkey` = NULL,
	`adsi` = NULL,
	`transfer` = NULL,
	`codecpriority` = NULL,
	`jitterbuffer` = NULL,
	`forcejitterbuffer` = NULL,
	`sendani` = 0,
	`qualifysmoothing` = 0,
	`qualifyfreqok` = 60000,
	`qualifyfreqnotok` = 10000,
	`timezone` = NULL,
	`mohinterpret` = NULL,
	`mohsuggest` = NULL,
	`deny` = NULL,
	`permit` = NULL,
	`defaultip` = NULL,
	`sourceaddress` = NULL,
	`setvar` = '',
	`mask` = NULL,
	`regexten` = NULL,
	`peercontext` = NULL,
	`ipaddr` = IFNULL(`ipaddr`,''),
	`regseconds` = IFNULL(`regseconds`,0),
	`protocol` = 'iax',
	`category` = 'trunk'
WHERE category = 'trunk';
