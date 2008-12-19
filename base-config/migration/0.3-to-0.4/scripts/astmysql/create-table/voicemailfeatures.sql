DROP TABLE IF EXISTS `voicemailfeatures`;
CREATE TABLE `voicemailfeatures` (
 `id` int(10) unsigned auto_increment,
 `voicemailid` int(10) unsigned,
 `skipcheckpass` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE UNIQUE INDEX `voicemailfeatures__uidx__voicemailid` ON `voicemailfeatures`(`voicemailid`);
