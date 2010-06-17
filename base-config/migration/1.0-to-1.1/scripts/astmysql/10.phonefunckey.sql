

ALTER TABLE `phonefunckey` CHANGE `typeextenumbersright` `typeextenumbersright` enum('agent', 'group', 'meetme', 'queue', 'user');
ALTER TABLE `phonefunckey` ADD COLUMN `label` varchar(32) AFTER `typevalextenumbersright`;
ALTER TABLE `phonefunckey` ADD COLUMN `progfunckey` tinyint(1) NOT NULL DEFAULT 0 AFTER `supervision`;

CREATE INDEX `phonefunckey__idx__progfunckey` ON `phonefunckey`(`progfunckey`);

