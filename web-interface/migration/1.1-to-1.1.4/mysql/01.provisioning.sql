
-- provisioning
CREATE TABLE `provisioning` (
 `id` tinyint(1) auto_increment,
 `registrar_main`   varchar(255) NOT NULL DEFAULT '',
 `registrar_backup` varchar(255) NOT NULL DEFAULT '',
 `proxy_main`       varchar(255) NOT NULL DEFAULT '',
 `proxy_backup`     varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM;

INSERT INTO `provisioning` VALUES(1, '', '', '', '');

