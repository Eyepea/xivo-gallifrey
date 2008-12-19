DROP TABLE IF EXISTS `voicemenu`;
CREATE TABLE `voicemenu` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(29) NOT NULL DEFAULT '',
 `number` varchar(40) NOT NULL,
 `context` varchar(39) NOT NULL,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `voicemenu__idx__number` ON `voicemenu`(`number`);
CREATE INDEX `voicemenu__idx__context` ON `voicemenu`(`context`);
CREATE INDEX `voicemenu__idx__commented` ON `voicemenu`(`commented`);
CREATE UNIQUE INDEX `voicemenu__uidx__name` ON `voicemenu`(`name`);
