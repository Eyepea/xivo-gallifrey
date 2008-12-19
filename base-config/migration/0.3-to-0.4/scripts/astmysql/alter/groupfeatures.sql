ALTER TABLE `groupfeatures` ADD COLUMN `transfer_user` tinyint(1) NOT NULL DEFAULT 0 AFTER `context`;
ALTER TABLE `groupfeatures` ADD COLUMN `transfer_call` tinyint(1) NOT NULL DEFAULT 0 AFTER `transfer_user`;
ALTER TABLE `groupfeatures` ADD COLUMN `write_caller` tinyint(1) NOT NULL DEFAULT 0 AFTER `transfer_call`;
ALTER TABLE `groupfeatures` ADD COLUMN `write_calling` tinyint(1) NOT NULL DEFAULT 0 AFTER `write_caller`;
ALTER TABLE `groupfeatures` ADD COLUMN `preprocess_subroutine` varchar(39) AFTER `timeout`; 
