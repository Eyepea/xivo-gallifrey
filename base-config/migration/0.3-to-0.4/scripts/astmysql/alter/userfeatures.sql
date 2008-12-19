ALTER TABLE `userfeatures` ADD COLUMN `voicemailid` int(10) unsigned AFTER `context`;
ALTER TABLE `userfeatures` ADD COLUMN `agentid` int(10) unsigned AFTER `voicemailid`;
ALTER TABLE `userfeatures` ADD COLUMN `loginclient` varchar(64) NOT NULL DEFAULT '' AFTER `enableclient`;
ALTER TABLE `userfeatures` ADD COLUMN `passwdclient` varchar(64) NOT NULL DEFAULT '' AFTER `loginclient`;
ALTER TABLE `userfeatures` ADD COLUMN `profileclient` varchar(64) NOT NULL DEFAULT '' AFTER `passwdclient`;
ALTER TABLE `userfeatures` ADD COLUMN `mobilephonenumber` varchar(128) NOT NULL DEFAULT '' AFTER `outcallerid`;
ALTER TABLE `userfeatures` ADD COLUMN `preprocess_subroutine` varchar(39) AFTER `bsfilter`;

ALTER TABLE `userfeatures` MODIFY COLUMN `internal` tinyint(1) NOT NULL DEFAULT 0 AFTER `preprocess_subroutine`;

ALTER TABLE `userfeatures` DROP COLUMN `skipvoicemailpass`;

CREATE INDEX `userfeatures__idx__voicemailid` ON `userfeatures`(`voicemailid`);
CREATE INDEX `userfeatures__idx__agentid` ON `userfeatures`(`agentid`);
CREATE INDEX `userfeatures__idx__loginclient` ON `userfeatures`(`loginclient`);

UPDATE `userfeatures` SET `context` = 'xivo-initconfig' WHERE `context` = 'initconfig';
