

ALTER TABLE `queuemember` ADD COLUMN `skills` varchar(64) NOT NULL DEFAULT ''           AFTER `category`;
ALTER TABLE `queuemember` ADD COLUMN `state_interface` varchar(128) NOT NULL DEFAULT '' AFTER `skills`;

