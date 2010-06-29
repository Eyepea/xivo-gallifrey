

ALTER TABLE `userfeatures` CHANGE COLUMN `callfilter` `incallfilter` tinyint(1) NOT NULL DEFAULT 0;
ALTER TABLE `userfeatures` ADD COLUMN `timezone` varchar(128) AFTER `internal`;

