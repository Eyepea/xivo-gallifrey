ALTER TABLE `rightcall` ADD COLUMN `context` varchar(39) NOT NULL AFTER `name`;

CREATE INDEX `rightcall__idx__context` ON `rightcall`(`context`);

UPDATE `rightcall` SET `context` = 'default';
