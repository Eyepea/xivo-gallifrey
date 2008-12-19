DROP INDEX `callfilter__idx__zone` ON `callfilter`;

ALTER TABLE `callfilter` ADD COLUMN `context` varchar(39) NOT NULL AFTER `name`;
ALTER TABLE `callfilter` CHANGE COLUMN `zone` `callfrom` enum('internal','external','all') NOT NULL;
ALTER TABLE `callfilter` DROP COLUMN `callerdisplay`;

CREATE INDEX `callfilter__idx__context` ON `callfilter`(`context`);
CREATE INDEX `callfilter__idx__callfrom` ON `callfilter`(`callfrom`);

UPDATE `callfilter` SET `context` = 'default';
