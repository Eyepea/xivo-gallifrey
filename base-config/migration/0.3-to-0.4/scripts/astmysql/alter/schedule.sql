DROP INDEX `schedule__idx__typetrue_typevaltrue` ON `schedule`;
DROP INDEX `schedule__idx__applicationvaltrue` ON `schedule`;
DROP INDEX `schedule__idx__typefalse_typevalfalse` ON `schedule`;
DROP INDEX `schedule__idx__applicationvalfalse` ON `schedule`;
DROP INDEX `schedule__idx__linked` ON `schedule`;

ALTER TABLE `schedule` ADD COLUMN `context` varchar(39) NOT NULL AFTER `name`;

ALTER TABLE `schedule` MODIFY COLUMN `daynamebeg` enum('*','sun','mon','tue','wed','thu','fri','sat') NOT NULL DEFAULT '*';
ALTER TABLE `schedule` MODIFY COLUMN `daynameend` enum('sun','mon','tue','wed','thu','fri','sat');

ALTER TABLE `schedule` MODIFY COLUMN `monthbeg` enum('*',
						     'jan',
						     'feb',
						     'mar',
						     'apr',
						     'may',
						     'jun',
						     'jul',
						     'aug',
						     'sep',
						     'oct',
						     'nov',
						     'dec') NOT NULL DEFAULT '*';

ALTER TABLE `schedule` MODIFY COLUMN `monthend` enum('jan',
						     'feb',
						     'mar',
						     'apr',
						     'may',
						     'jun',
						     'jul',
						     'aug',
						     'sep',
						     'oct',
						     'nov',
						     'dec');

ALTER TABLE `schedule` DROP COLUMN `typetrue`;
ALTER TABLE `schedule` DROP COLUMN `typevaltrue`;
ALTER TABLE `schedule` DROP COLUMN `applicationvaltrue`;
ALTER TABLE `schedule` DROP COLUMN `typefalse`;
ALTER TABLE `schedule` DROP COLUMN `typevalfalse`;
ALTER TABLE `schedule` DROP COLUMN `applicationvalfalse`;
ALTER TABLE `schedule` DROP COLUMN `linked`;

CREATE INDEX `schedule__idx__context` ON `schedule`(`context`);

UPDATE `schedule` SET `context` = 'default';
