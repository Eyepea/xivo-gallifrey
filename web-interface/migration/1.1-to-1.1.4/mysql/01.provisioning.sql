
-- provisioning
CREATE TABLE `provisioning` (
 `id` tinyint(1) auto_increment,
 `main_registrar`   varchar(255) NOT NULL DEFAULT '',
 `backup_registrar` varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM;

INSERT INTO `provisioning` VALUES(1, '', '');

