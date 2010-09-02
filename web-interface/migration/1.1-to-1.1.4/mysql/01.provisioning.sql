
-- provisioning
CREATE TABLE `provisioning` (
 `id` tinyint(1) auto_increment,
 `registrar_main`   varchar(255) NOT NULL DEFAULT '',
 `registrar_backup` varchar(255) NOT NULL DEFAULT '',
 `proxy_main`       varchar(255) NOT NULL DEFAULT '',
 `proxy_backup`     varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM;

-- we set default registrar_main and proxy_main value as VOIP interface address
-- if such interface does not exist, we set empty values
INSERT IGNORE INTO `provisioning` SELECT 1, `address`, '', `address`, '' FROM `netiface` WHERE `networktype` = 'voip' LIMIT 1;
INSERT IGNORE INTO `provisioning` VALUES(1, '', '', '', '');

