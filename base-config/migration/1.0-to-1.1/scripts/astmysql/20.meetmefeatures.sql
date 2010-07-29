

ALTER TABLE `meetmefeatures` RENAME TO `meetmefeatures_tmp`;

CREATE TABLE `meetmefeatures` (
 `id` int(10) unsigned auto_increment,
 `meetmeid` int(10) unsigned NOT NULL,
 `name` varchar(80) NOT NULL,
 `number` varchar(40) NOT NULL,
 `context` varchar(39) NOT NULL,
 `admin_typefrom` enum('none','internal','external','undefined'),
 `admin_internalid` int(10) unsigned,
 `admin_externalid` varchar(40),
 `admin_identification` enum('calleridnum','pin','all') NOT NULL,
 `admin_mode` enum('listen','talk','all') NOT NULL,
 `admin_announceusercount` tinyint(1) NOT NULL DEFAULT 0,
 `admin_announcejoinleave` enum('no','yes','noreview') NOT NULL,
 `admin_moderationmode` tinyint(1) NOT NULL DEFAULT 0,
 `admin_initiallymuted` tinyint(1) NOT NULL DEFAULT 0,
 `admin_musiconhold` varchar(128),
 `admin_poundexit` tinyint(1) NOT NULL DEFAULT 0,
 `admin_quiet` tinyint(1) NOT NULL DEFAULT 0,
 `admin_starmenu` tinyint(1) NOT NULL DEFAULT 0,
 `admin_closeconflastmarkedexit` tinyint(1) NOT NULL DEFAULT 0,
 `admin_enableexitcontext` tinyint(1) NOT NULL DEFAULT 0,
 `admin_exitcontext` varchar(39),
 `user_mode` enum('listen','talk','all') NOT NULL,
 `user_announceusercount` tinyint(1) NOT NULL DEFAULT 0,
 `user_hiddencalls` tinyint(1) NOT NULL DEFAULT 0,
 `user_announcejoinleave` enum('no','yes','noreview') NOT NULL,
 `user_initiallymuted` tinyint(1) NOT NULL DEFAULT 0,
 `user_musiconhold` varchar(128),
 `user_poundexit` tinyint(1) NOT NULL DEFAULT 0,
 `user_quiet` tinyint(1) NOT NULL DEFAULT 0,
 `user_starmenu` tinyint(1) NOT NULL DEFAULT 0,
 `user_enableexitcontext` tinyint(1) NOT NULL DEFAULT 0,
 `user_exitcontext` varchar(39),
 `talkeroptimization` tinyint(1) NOT NULL DEFAULT 0,
 `record` tinyint(1) NOT NULL DEFAULT 0,
 `talkerdetection` tinyint(1) NOT NULL DEFAULT 0,
 `noplaymsgfirstenter` tinyint(1) NOT NULL DEFAULT 0,
 `durationm` smallint(3) unsigned,
 `closeconfdurationexceeded` tinyint(1) NOT NULL DEFAULT 0,
 `nbuserstartdeductduration` tinyint(2) unsigned,
 `timeannounceclose` smallint(3) unsigned,
 `maxuser` tinyint(2) unsigned,
 `startdate` datetime,
 `emailfrom` varchar(255),
 `emailfromname` varchar(255),
 `emailsubject` varchar(255),
 `emailbody` text NOT NULL,
 `preprocess_subroutine` varchar(39),
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `meetmefeatures__idx__number` ON `meetmefeatures`(`number`);
CREATE INDEX `meetmefeatures__idx__context` ON `meetmefeatures`(`context`);
CREATE UNIQUE INDEX `meetmefeatures__uidx__meetmeid` ON `meetmefeatures`(`meetmeid`);
CREATE UNIQUE INDEX `meetmefeatures__uidx__name` ON `meetmefeatures`(`name`);

INSERT INTO `meetmefeatures` SELECT
	 `id`,
	 `meetmeid`,
	 `name`,
	 `number`,
	 `context`,
	 NULL,																								-- admin_typefrom
	 0,																										-- admin_internalid
	 '',																									-- admin_externalid
	 '',																									-- admin_identification
	 `adminmode`,																						-- admin_mode
	 `announceusercount`,																		-- admin_announceusercount
	 `announcejoinleave`,																		-- admin_announcejoinleave
	 0,																										-- admin_moderationmode
	 0,																										-- admin_initiallymuted
	 `musiconhold`,																					-- admin_musiconhold
	 `poundexit`,																						-- admin_poundexit
	 `quiet`,																								-- admin_quiet
	 `starmenu`,																						-- admin_starmenu
	 0,																										-- admin_closeconflastmarkedexit
	 `enableexitcontext`,																		-- admin_enableexitcontext
	 `exitcontext`,																					-- admin_exitcontext
	 
	 `mode`,																								-- user_mode
	 `announceusercount`,																		-- user_announceusercount
	 0,																										-- user_hiddencalls
	 `announcejoinleave`,																		-- user_announcejoinleave
	 0,																										-- user_initiallymuted
	 `musiconhold`,																					-- user_musiconhold
	 `poundexit`,																						-- user_poundexit
	 `quiet`,																								-- user_quiet
	 `starmenu`,																						-- user_starmenu
	 `enableexitcontext`,																		-- user_enableexitcontext
	 `exitcontext`,																					-- user_exitcontext
	 0,																										-- talkeroptimization
	 `record`,																							-- 
	 0,																										-- talkerdetection
	 0,																										-- noplaymsgfirstenter
	 0,																										-- durationm
	 0,																										-- closeconfdurationexceeded
	 0,																										-- nbuserstartdeductduration
	 0,																										-- timeannounceclose
	 NULL,																								-- maxuser
	 NULL,																								-- startdate
	 NULL,																								-- emailfrom
	 NULL,																								-- emailfromname
	 NULL,																								-- emailsubject
	 '',																									-- emailbody
	 `preprocess_subroutine`,
	 ''																									-- description
 FROM `meetmefeatures_tmp`;
 
DROP TABLE `meetmefeatures_tmp`;


