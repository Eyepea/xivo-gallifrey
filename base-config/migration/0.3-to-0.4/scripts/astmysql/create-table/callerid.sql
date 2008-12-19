DROP TABLE IF EXISTS `callerid`;
CREATE TABLE `callerid` (
 `mode` enum('prepend','overwrite','append'),
 `callerdisplay` varchar(80) NOT NULL DEFAULT '',
 `type` enum('callfilter','incall','group','queue') NOT NULL,
 `typeval` int(10) unsigned NOT NULL,
 PRIMARY KEY(`type`,`typeval`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
