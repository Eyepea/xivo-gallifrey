
CREATE TABLE `staticsccp` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` text,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `staticsccp__idx__commented` ON `staticvoicemail`(`commented`);
CREATE INDEX `staticsccp__idx__filename` ON `staticvoicemail`(`filename`);
CREATE INDEX `staticsccp__idx__category` ON `staticvoicemail`(`category`);
CREATE INDEX `staticsccp__idx__var_name` ON `staticvoicemail`(`var_name`);

INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','servername','Asterisk');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','keepalive',60);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','debug','');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','dateFormat','D.M.Y');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','port',2000);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','firstdigittimeout',16);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','digittimeout',8);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','autoanswer_ring_time',0);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','autoanswer_tone','0x32');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','remotehangup_tone','0x32');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','transfer_tone',0);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','callwaiting_tone','0x2d');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','musicclass','default');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','dnd','on');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','sccp_tos','0x68');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','sccp_cos',4);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','audio_tos','0xB8');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','audio_cos',6);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','video_tos','0x88');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','video_cos',5);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','echocancel','on');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','silencesuppression','off');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','trustphoneip','no');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','private','on');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','protocolversion',11);
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','disallow','all');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','language','fr_FR');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','hotline_enabled','yes');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','hotline_context','xivo-initconfig');
INSERT INTO `staticsccp` VALUES(NULL,0,0,0,'sccp.conf','general','hotline_extension','sccp');


CREATE TABLE `usersccp`
(
 `id` int(10) unsigned auto_increment,
 `name` varchar(128),
 `devicetype` varchar(64),            -- phone model, ie 7960
 `keepalive` int(3) unsigned,        -- i.e 60
 `tzoffset` varchar(3),               -- ie: +1 == Europe/Paris
 `dtmfmode` varchar(16),               -- outofband, inband
 `transfer` varchar(3),                -- on, off, NULL
 `park` varchar(3),                    -- on, off, NULL
 `cfwdall` varchar(3),                 -- on, off, NULL
 `cfwdbusy` varchar(3),                -- on, off, NULL
 `cfwdnoanswer` varchar(3),            -- on, off, NULL
 `mwilamp` varchar(5),                 -- on, off, wink, flash, blink, NULL
 `mwioncall` varchar(3),               -- on, off, NULL
 `dnd` varchar(6),                     -- on, off, NULL
 `pickupexten` varchar(3),             -- on, off, NULL
 `pickupcontext` varchar(64),          -- pickup context name
 `pickupmodeanswer` varchar(3),        -- on, off, NULL
 `permit` varchar(31),                 -- 192.168.0.0/255.255.255.0
 `deny` varchar(31),                   -- 0.0.0.0/0.0.0.0
 `addons` varchar(24),                 -- comma separated addons list. i.e 7914,7914
 `imageversion` varchar(64),           -- i.e P00405000700
 `trustphoneip` varchar(3),            -- yes, no, NULL
 `nat` varchar(3),                     -- on, off, NULL
 `directrtp` varchar(3),               -- on, off, NULL
 `earlyrtp` varchar(7),                -- none, offhook, dial, ringout, NULL
 `private` varchar(3),                 -- on, off, NULL
 `privacy` varchar(4),                 -- on, off, full, NULL
 `protocol` varchar(4) NOT NULL DEFAULT 'sccp', -- required for join with userfeatures

 -- softkeys
 `softkey_onhook`      varchar(1024),
 `softkey_connected`   varchar(1024),
 `softkey_onhold`      varchar(1024),
 `softkey_ringin`      varchar(1024),
 `softkey_offhook`     varchar(1024),
 `softkey_conntrans`   varchar(1024),
 `softkey_digitsfoll`  varchar(1024),
 `softkey_connconf`    varchar(1024),
 `softkey_ringout`     varchar(1024),
 `softkey_offhookfeat` varchar(1024),
 `softkey_onhint`      varchar(1024),
           
 `defaultline` int(10) unsigned,
 `commented` int(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
);

CREATE UNIQUE INDEX `usersccp__uidx__name` ON `usersccp`(`name`);


DROP TABLE IF EXISTS `sccpline`;
CREATE TABLE `sccpline`
(
 `id` int(10) unsigned auto_increment,
 `name` varchar(80) NOT NULL,
 `pin` varchar(8) NOT NULL DEFAULT '',
 `label` varchar(128) NOT NULL DEFAULT '',
 `description` text,
 `context` varchar(64),
 `incominglimit` int(3) unsigned,
 `transfer` varchar(3) DEFAULT 'on',                    -- on, off, NULL
 `mailbox` varchar(64) DEFAULT NULL,
 `vmnum` varchar(64) DEFAULT NULL,
 `meetmenum` varchar(64) DEFAULT NULL,
 `cid_name` varchar(64) NOT NULL DEFAULT '',
 `cid_num` varchar(64) NOT NULL DEFAULT '',
 `trnsfvm` varchar(64),
 `secondary_dialtone_digits` varchar(10),
 `secondary_dialtone_tone` int(5) unsigned,
 `musicclass` varchar(32),
 `language` varchar(32),                                 -- en, fr, ...
 `accountcode` varchar(32),
 `audio_tos` varchar(8),
 `audio_cos` int(3) unsigned,
 `video_tos` varchar(8),
 `video_cos` int(3) unsigned,
 `echocancel` varchar(3) DEFAULT 'on',                   -- on, off, NULL
 `silencesuppression` varchar(3) DEFAULT 'on',           -- on, off, NULL
 `callgroup` varchar(64) DEFAULT '',                     -- i.e: 1,4-9
 `pickupgroup` varchar(64) DEFAULT '',                   -- i.e: 1,3-9
 `amaflags` varchar(16) DEFAULT '',                      -- default, omit, billing, documentation
 `adhocnumber` varchar(64),
 `setvar` varchar(512) DEFAULT '',
 `commented` int(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
);

CREATE UNIQUE INDEX `sccpline__uidx__name` ON `sccpline`(`name`);


