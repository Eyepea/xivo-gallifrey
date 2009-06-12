ALTER TABLE `usersip` ADD COLUMN `lastms` varchar(15) NOT NULL DEFAULT '' AFTER `regserver`;

CREATE INDEX `usersip__idx__lastms` ON `usersip`(`lastms`);
