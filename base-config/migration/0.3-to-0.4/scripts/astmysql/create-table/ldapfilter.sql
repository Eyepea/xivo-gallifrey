DROP TABLE IF EXISTS `ldapfilter`;
CREATE TABLE `ldapfilter` (
 `id` int(10) unsigned auto_increment,
 `ldapserverid` int(10) unsigned NOT NULL,
 `name` varchar(128) NOT NULL DEFAULT '',
 `user` varchar(255) NOT NULL DEFAULT '',
 `passwd` varchar(255) NOT NULL DEFAULT '',
 `basedn` varchar(255) NOT NULL DEFAULT '',
 `filter` varchar(255) NOT NULL DEFAULT '',
 `attrdisplayname` varchar(255) NOT NULL DEFAULT '',
 `attrphonenumber` varchar(255) NOT NULL DEFAULT '',
 `additionaltype` enum('office','home','mobile','fax','other','custom') NOT NULL,
 `additionaltext` varchar(16) NOT NULL DEFAULT '',
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `ldapfilter__idx__ldapserverid` ON `ldapfilter`(`ldapserverid`);
CREATE INDEX `ldapfilter__idx__commented` ON `ldapfilter`(`commented`);
CREATE UNIQUE INDEX `ldapfilter__uidx__name` ON `ldapfilter`(`name`);
