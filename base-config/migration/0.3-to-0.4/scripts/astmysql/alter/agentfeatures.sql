ALTER TABLE `agentfeatures` MODIFY COLUMN `commented` tinyint(1) NOT NULL DEFAULT 0 AFTER `silent`;
ALTER TABLE `agentfeatures` ADD COLUMN `context` varchar(39) NOT NULL AFTER `passwd`;
ALTER TABLE `agentfeatures` ADD COLUMN `language` varchar(20) NOT NULL AFTER `context`;
ALTER TABLE `agentfeatures` ADD COLUMN `description` text NOT NULL AFTER `commented`;

UPDATE `agentfeatures` SET `context` = 'default', `language` = 'fr';
