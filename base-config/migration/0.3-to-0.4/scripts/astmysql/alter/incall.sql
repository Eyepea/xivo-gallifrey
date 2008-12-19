DROP INDEX `incall__idx__type_typeval` ON `incall`;
DROP INDEX `incall__idx__applicationval` ON `incall`;
DROP INDEX `incall__idx__linked` ON `incall`;

ALTER TABLE `incall` DROP COLUMN `type`;
ALTER TABLE `incall` DROP COLUMN `typeval`;
ALTER TABLE `incall` DROP COLUMN `applicationval`;
ALTER TABLE `incall` DROP COLUMN `linked`;

ALTER TABLE `incall` DEFAULT CHARACTER SET `ascii`;

ALTER TABLE `incall` ADD COLUMN `preprocess_subroutine` varchar(39) AFTER `context`;
ALTER TABLE `incall` ADD COLUMN `faxdetectenable` tinyint(1) NOT NULL DEFAULT 0 AFTER `preprocess_subroutine`;
ALTER TABLE `incall` ADD COLUMN `faxdetecttimeout` tinyint(2) unsigned NOT NULL DEFAULT 4 AFTER `faxdetectenable`;
ALTER TABLE `incall` ADD COLUMN `faxdetectemail` varchar(255) NOT NULL DEFAULT '' AFTER `faxdetecttimeout`;

UPDATE `incall` SET `context` = 'from-extern';
