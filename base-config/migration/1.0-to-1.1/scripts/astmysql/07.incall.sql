
CREATE INDEX `incall__idx__exten` ON `incall`(`exten`);
CREATE INDEX `incall__idx__context` ON `incall`(`context`);

ALTER TABLE `incall` ADD COLUMN `description` text NOT NULL AFTER `commented`;

