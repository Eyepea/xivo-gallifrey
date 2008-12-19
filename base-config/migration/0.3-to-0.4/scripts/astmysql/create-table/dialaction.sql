DROP TABLE IF EXISTS `dialaction`;
CREATE TABLE `dialaction` (
 `event` enum('answer',
	      'noanswer',
	      'congestion',
	      'busy',
	      'chanunavail',
	      'inschedule',
	      'outschedule') NOT NULL,
 `category` enum('callfilter','group','incall','queue','schedule','user') NOT NULL,
 `categoryval` varchar(128) NOT NULL DEFAULT '',
 `action` enum('none',
 	       'endcall:busy',
 	       'endcall:congestion',
 	       'endcall:hangup',
 	       'user',
	       'group',
	       'queue',
	       'meetme',
	       'voicemail',
	       'schedule',
	       'voicemenu',
	       'extension',
	       'application:callbackdisa',
	       'application:disa',
	       'application:directory',
	       'application:faxtomail',
	       'application:voicemailmain',
	       'sound',
	       'custom') NOT NULL,
 `actionarg1` varchar(255) NOT NULL DEFAULT '',
 `actionarg2` varchar(255) NOT NULL DEFAULT '',
 `linked` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`event`,`category`,`categoryval`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `dialaction__idx__action_actionarg1` ON `dialaction`(`action`,`actionarg1`);
CREATE INDEX `dialaction__idx__actionarg2` ON `dialaction`(`actionarg2`);
CREATE INDEX `dialaction__idx__linked` ON `dialaction`(`linked`);
