ALTER IGNORE TABLE `serverfeatures` DROP INDEX `serverfeatures__uidx__serverid_type`;

ALTER TABLE `serverfeatures` CHANGE COLUMN `type` `feature` enum('phonebook') NOT NULL;
ALTER TABLE `serverfeatures` ADD COLUMN `type` enum('xivo','ldap') NOT NULL AFTER `feature`;

UPDATE `serverfeatures` SET `type` = 'xivo';

CREATE INDEX `serverfeatures__idx__feature` ON `serverfeatures`(`feature`);
CREATE UNIQUE INDEX `serverfeatures__uidx__serverid_feature_type` ON `serverfeatures`(`serverid`,`feature`,`type`);
