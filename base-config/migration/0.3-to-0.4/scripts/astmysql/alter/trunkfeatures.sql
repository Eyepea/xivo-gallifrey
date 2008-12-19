DROP INDEX `trunkfeatures__uidx__trunk_trunkid` ON `trunkfeatures`;

ALTER TABLE `trunkfeatures` ADD COLUMN `description` text NOT NULL AFTER `registercommented`;

ALTER TABLE `trunkfeatures` CHANGE COLUMN `trunk` `protocol` varchar(50) NOT NULL;
ALTER TABLE `trunkfeatures` CHANGE COLUMN `trunkid` `protocolid` int(10) unsigned NOT NULL;

CREATE UNIQUE INDEX `trunkfeatures__uidx__protocol_protocolid` ON `trunkfeatures`(`protocol`,`protocolid`);
