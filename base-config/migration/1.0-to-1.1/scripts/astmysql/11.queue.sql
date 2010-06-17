
ALTER TABLE `queue` ADD COLUMN `ringinuse` tinyint(1) NOT NULL DEFAULT 0                AFTER `eventwhencalled`;
ALTER TABLE `queue` ADD COLUMN `autopause` tinyint(1) unsigned NOT NULL DEFAULT 0       AFTER `category`;
ALTER TABLE `queue` ADD COLUMN `setinterfacevar` tinyint(1) unsigned NOT NULL DEFAULT 0 AFTER `autopause`;

