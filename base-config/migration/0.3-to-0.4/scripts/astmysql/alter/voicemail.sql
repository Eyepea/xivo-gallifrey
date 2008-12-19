RENAME TABLE `uservoicemail` TO `voicemail`;

DROP INDEX `uservoicemail__idx__commented` ON `voicemail`;
DROP INDEX `uservoicemail__idx__context` ON `voicemail`;
DROP INDEX `uservoicemail__uidx__mailbox_context` ON `voicemail`;

ALTER TABLE `voicemail` MODIFY COLUMN `context` varchar(39) NOT NULL;
ALTER TABLE `voicemail` MODIFY COLUMN `mailbox` varchar(40) NOT NULL;
ALTER TABLE `voicemail` MODIFY COLUMN `email` varchar(80);
ALTER TABLE `voicemail` MODIFY COLUMN `pager` varchar(80);
ALTER TABLE `voicemail` MODIFY COLUMN `language` varchar(20);
ALTER TABLE `voicemail` MODIFY COLUMN `tz` varchar(80);
ALTER TABLE `voicemail` MODIFY COLUMN `attach` tinyint(1);
ALTER TABLE `voicemail` MODIFY COLUMN `saycid` tinyint(1);
ALTER TABLE `voicemail` MODIFY COLUMN `review` tinyint(1);
ALTER TABLE `voicemail` MODIFY COLUMN `operator` tinyint(1);
ALTER TABLE `voicemail` MODIFY COLUMN `envelope` tinyint(1);
ALTER TABLE `voicemail` MODIFY COLUMN `sayduration` tinyint(1);
ALTER TABLE `voicemail` MODIFY COLUMN `saydurationm` tinyint(2) unsigned;
ALTER TABLE `voicemail` MODIFY COLUMN `sendvoicemail` tinyint(1);
ALTER TABLE `voicemail` MODIFY COLUMN `forcename` tinyint(1);
ALTER TABLE `voicemail` MODIFY COLUMN `forcegreetings` tinyint(1);
ALTER TABLE `voicemail` MODIFY COLUMN `maxmsg` smallint(4) unsigned;

CREATE INDEX `voicemail__idx__context` ON `voicemail`(`context`);
CREATE INDEX `voicemail__idx__commented` ON `voicemail`(`commented`);
CREATE UNIQUE INDEX `voicemail__uidx__mailbox_context` ON `voicemail`(`mailbox`,`context`);

UPDATE `voicemail`
SET
	`email` = NULLIF(`email`,''),
	`pager` = NULLIF(`pager`,''),
	`dialout` = NULL,
	`callback` = NULL,
	`exitcontext` = NULL,
	`language` = NULLIF(`language`,''),
	`tz` = NULLIF(`tz`,'');
