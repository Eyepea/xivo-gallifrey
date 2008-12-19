DROP INDEX `accessfeatures__idx__type` ON `accessfeatures`;
DROP INDEX `accessfeatures__uidx__host_type` ON `accessfeatures`;

ALTER TABLE `accessfeatures` CHANGE COLUMN `type` `feature` enum('phonebook') NOT NULL;

CREATE INDEX `accessfeatures__idx__feature` ON `accessfeatures`(`feature`);
CREATE UNIQUE INDEX `accessfeatures__uidx__host_feature` ON `accessfeatures`(`host`,`feature`);
