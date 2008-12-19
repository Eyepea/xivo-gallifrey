DROP TABLE IF EXISTS `contextmember`;
CREATE TABLE `contextmember` (
 `context` varchar(39) NOT NULL,
 `type` varchar(32) NOT NULL,
 `typeval` varchar(128) NOT NULL DEFAULT '',
 `varname` varchar(128) NOT NULL DEFAULT '',
 PRIMARY KEY(`context`,`type`,`typeval`,`varname`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `contextmember__idx__context` ON `contextmember`(`context`);
CREATE INDEX `contextmember__idx__context_type` ON `contextmember`(`context`,`type`);
