DROP INDEX `usercustom__uidx__interface_category` ON `usercustom`;

ALTER TABLE `usercustom` ADD COLUMN `intfsuffix` varchar(32) NOT NULL DEFAULT '' AFTER `interface`;

CREATE UNIQUE INDEX `usercustom__uidx__interface_intfsuffix_category` ON `usercustom`(`interface`,`intfsuffix`,`category`);
