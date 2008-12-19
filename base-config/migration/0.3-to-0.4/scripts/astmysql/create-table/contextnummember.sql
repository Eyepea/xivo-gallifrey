DROP TABLE IF EXISTS `contextnummember`;
CREATE TABLE `contextnummember` (
 `context` varchar(39) NOT NULL,
 `type` enum('user','group','queue','meetme','incall') NOT NULL,
 `typeval` varchar(128) NOT NULL DEFAULT 0,
 `number` varchar(40) NOT NULL DEFAULT '',
 PRIMARY KEY(`context`,`type`,`typeval`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `contextnummember__idx__context` ON `contextnummember`(`context`);
CREATE INDEX `contextnummember__idx__context_type` ON `contextnummember`(`context`,`type`);
CREATE INDEX `contextnummember__idx__number` ON `contextnummember`(`number`);
