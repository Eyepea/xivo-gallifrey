DROP TABLE IF EXISTS `context`;
CREATE TABLE `context` (
 `name` varchar(39) NOT NULL,
 `displayname` varchar(128) NOT NULL DEFAULT '',
 `entity` varchar(64),
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `context__idx__displayname` ON `context`(`displayname`);
CREATE INDEX `context__idx__entity` ON `context`(`entity`);
CREATE INDEX `context__idx__commented` ON `context`(`commented`);
