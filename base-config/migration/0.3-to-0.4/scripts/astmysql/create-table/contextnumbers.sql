DROP TABLE IF EXISTS `contextnumbers`;
CREATE TABLE `contextnumbers` (
 `context` varchar(39) NOT NULL,
 `type` enum('user','group','queue','meetme','incall') NOT NULL,
 `numberbeg` varchar(16) NOT NULL DEFAULT '',
 `numberend` varchar(16) NOT NULL DEFAULT '',
 `didlength` tinyint(2) unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(`context`,`type`,`numberbeg`,`numberend`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `contextnumbers__idx__context_type` ON `contextnumbers`(`context`,`type`);
