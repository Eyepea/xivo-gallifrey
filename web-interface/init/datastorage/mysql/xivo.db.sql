GRANT ALL PRIVILEGES ON xivo.* TO xivo@localhost IDENTIFIED BY PASSWORD '7560ba9c16089426';
CREATE DATABASE IF NOT EXISTS `xivo` DEFAULT CHARACTER SET utf8;
USE `xivo`;

DROP TABLE IF EXISTS `i18n_cache`;
CREATE TABLE `i18n_cache` (
 `dcreate` int(10) unsigned NOT NULL DEFAULT 0,
 `dupdate` int(10) unsigned NOT NULL DEFAULT 0,
 `locale` varchar(7) NOT NULL DEFAULT '',
 `language` varchar(3) NOT NULL DEFAULT '',
 `path` varchar(255) NOT NULL DEFAULT '',
 `obj` longblob NOT NULL,
 PRIMARY KEY(`locale`,`path`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `i18n_cache__idx__dupdate` ON `i18n_cache`(`dupdate`);
CREATE INDEX `i18n_cache__idx__locale` ON `i18n_cache`(`locale`);
CREATE INDEX `i18n_cache__idx__language` ON `i18n_cache`(`language`);
CREATE INDEX `i18n_cache__idx__path` ON `i18n_cache`(`path`);


DROP TABLE IF EXISTS `session`;
CREATE TABLE `session` (
 `sesskey` varchar(32) NOT NULL DEFAULT '',
 `expire` int(10) unsigned NOT NULL DEFAULT 0,
 `data` longblob NOT NULL,
 `user_id` int(11) unsigned DEFAULT 0,
 `start` int(10) unsigned DEFAULT 0,
 PRIMARY KEY(`sesskey`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `session__idx__expire` ON `session`(`expire`);
CREATE INDEX `session__idx__user_id` ON `session`(`user_id`);


DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
 `id` int(11) unsigned auto_increment,
 `login` varchar(255) NOT NULL DEFAULT '',
 `passwd` varchar(255) NOT NULL DEFAULT '',
 `meta` enum('user','admin','root') DEFAULT 'user',
 `valid` tinyint(1) unsigned DEFAULT 1,
 `time` int(11) unsigned NOT NULL DEFAULT 0,
 `obj` longblob NOT NULL,
 `dcreate` int(11) unsigned NOT NULL DEFAULT 0,
 `dupdate` int(11) unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `user__idx__login` ON `user`(`login`);
CREATE INDEX `user__idx__meta` ON `user`(`meta`);
CREATE INDEX `user__idx__passwd` ON `user`(`passwd`);
CREATE INDEX `user__idx__time` ON `user`(`time`);
CREATE INDEX `user__idx__valid` ON `user`(`valid`);
CREATE UNIQUE INDEX `user__uidx__login_meta` ON `user`(`login`,`meta`);

INSERT INTO `user` VALUES(1,'root','proformatique','root',1,0,'',UNIX_TIMESTAMP(UTC_TIMESTAMP()),0);
INSERT INTO `user` VALUES(2,'admin','proformatique','admin',1,0,'',UNIX_TIMESTAMP(UTC_TIMESTAMP()),0);
