
CREATE INDEX `outcall__idx__exten` ON `outcall`(`exten`);

ALTER TABLE `outcall` ADD COLUMN `description` text NOT NULL AFTER `commented`;

