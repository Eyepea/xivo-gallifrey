GRANT ALL PRIVILEGES ON `asterisk`.* TO `asterisk`@`localhost` IDENTIFIED BY PASSWORD '*DBA86DFECE903EB25FE460A66BDCDA790A1CA4A4';
CREATE DATABASE IF NOT EXISTS `asterisk` DEFAULT CHARACTER SET utf8;
USE `asterisk`;


DROP TABLE IF EXISTS `accessfeatures`;
CREATE TABLE `accessfeatures` (
 `id` int(10) unsigned auto_increment,
 `host` varchar(255) NOT NULL DEFAULT '',
 `type` enum('phonebook') NOT NULL,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `accessfeatures__idx__host` ON `accessfeatures`(`host`);
CREATE INDEX `accessfeatures__idx__type` ON `accessfeatures`(`type`);
CREATE INDEX `accessfeatures__idx__commented` ON `accessfeatures`(`commented`);
CREATE UNIQUE INDEX `accessfeatures__uidx__host_type` ON `accessfeatures`(`host`,`type`);


DROP TABLE IF EXISTS `agent`;
CREATE TABLE `agent` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(128),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `agent__idx__cat_metric` ON `agent`(`cat_metric`);
CREATE INDEX `agent__idx__var_metric` ON `agent`(`var_metric`);
CREATE INDEX `agent__idx__commented` ON `agent`(`commented`);
CREATE INDEX `agent__idx__filename` ON `agent`(`filename`);
CREATE INDEX `agent__idx__category` ON `agent`(`category`);
CREATE INDEX `agent__idx__var_name` ON `agent`(`var_name`);
CREATE INDEX `agent__idx__var_val` ON `agent`(`var_val`);

INSERT INTO `agent` VALUES (1,0,0,0,'agents.conf','general','persistentagents','yes');
INSERT INTO `agent` VALUES (2,1,1000000,0,'agents.conf','agents','group',1);


DROP TABLE IF EXISTS `agentfeatures`;
CREATE TABLE `agentfeatures` (
 `id` int(10) unsigned auto_increment,
 `agentid` int(10) unsigned NOT NULL,
 `numgroup` tinyint(2) unsigned NOT NULL,
 `firstname` varchar(128) NOT NULL DEFAULT '',
 `lastname` varchar(128) NOT NULL DEFAULT '',
 `number` varchar(40) NOT NULL,
 `passwd` varchar(128) NOT NULL,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `silent` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `agentfeatures__idx__commented` ON `agentfeatures`(`commented`);
CREATE UNIQUE INDEX `agentfeatures__uidx__number` ON `agentfeatures`(`number`);
CREATE UNIQUE INDEX `agentfeatures__uidx__agentid` ON `agentfeatures`(`agentid`);


DROP TABLE IF EXISTS `agentgroup`;
CREATE TABLE `agentgroup` (
 `id` tinyint(2) unsigned auto_increment,
 `groupid` int(10) unsigned NOT NULL,
 `name` varchar(128) NOT NULL DEFAULT '',
 `groups` varchar(255) NOT NULL DEFAULT '',
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `deleted` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `agentgroup__idx__groupid` ON `agentgroup`(`groupid`);
CREATE INDEX `agentgroup__idx__name` ON `agentgroup`(`name`);
CREATE INDEX `agentgroup__idx__commented` ON `agentgroup`(`commented`);
CREATE INDEX `agentgroup__idx__deleted` ON `agentgroup`(`deleted`);

INSERT INTO `agentgroup` VALUES (1,2,'default','',0,0);


DROP TABLE IF EXISTS `callfilter`;
CREATE TABLE `callfilter` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(128) NOT NULL DEFAULT '',
 `type` enum('bosssecretary') NOT NULL,
 `bosssecretary` enum('bossfirst-serial','bossfirst-simult','secretary-serial','secretary-simult','all'),
 `zone` enum('internal','external','all') NOT NULL,
 `callerdisplay` varchar(80) NOT NULL DEFAULT '',
 `ringseconds` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `callfilter__idx__type` ON `callfilter`(`type`);
CREATE INDEX `callfilter__idx__bosssecretary` ON `callfilter`(`bosssecretary`);
CREATE INDEX `callfilter__idx__zone` ON `callfilter`(`zone`);
CREATE INDEX `callfilter__idx__commented` ON `callfilter`(`commented`);
CREATE UNIQUE INDEX `callfilter__uidx__name` ON `callfilter`(`name`);


DROP TABLE IF EXISTS `callfiltermember`;
CREATE TABLE `callfiltermember` (
 `id` int(10) unsigned auto_increment,
 `callfilterid` int(10) unsigned NOT NULL DEFAULT 0,
 `type` enum('user') NOT NULL,
 `typeval` varchar(128) NOT NULL DEFAULT 0,
 `ringseconds` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `priority` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `bstype` enum('boss','secretary'),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `callfiltermember__idx__priority` ON `callfiltermember`(`priority`);
CREATE INDEX `callfiltermember__idx__bstype` ON `callfiltermember`(`bstype`);
CREATE UNIQUE INDEX `callfiltermember__uidx__callfilterid_type_typeval` ON `callfiltermember`(`callfilterid`,`type`,`typeval`);


DROP TABLE IF EXISTS `cdr`;
CREATE TABLE `cdr` (
 `id` int(10) unsigned auto_increment,
 `calldate` timestamp DEFAULT 0,
 `clid` varchar(80) NOT NULL DEFAULT '',
 `src` varchar(80) NOT NULL DEFAULT '',
 `dst` varchar(80) NOT NULL DEFAULT '',
 `dcontext` varchar(39) NOT NULL DEFAULT '',
 `channel` varchar(80) NOT NULL DEFAULT '',
 `dstchannel` varchar(80) NOT NULL DEFAULT '',
 `lastapp` varchar(80) NOT NULL DEFAULT '',
 `lastdata` varchar(80) NOT NULL DEFAULT '',
 `answer` timestamp DEFAULT 0,
 `end` timestamp DEFAULT 0,
 `duration` int(10) unsigned NOT NULL DEFAULT 0,
 `billsec` int(10) unsigned NOT NULL DEFAULT 0,
 `disposition` varchar(9) NOT NULL DEFAULT '',
 `amaflags` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `accountcode` varchar(20) NOT NULL DEFAULT '',
 `uniqueid` varchar(32) NOT NULL DEFAULT '',
 `userfield` varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `cdr__idx__calldate` ON `cdr`(`calldate`);
CREATE INDEX `cdr__idx__clid` ON `cdr`(`clid`);
CREATE INDEX `cdr__idx__src` ON `cdr`(`src`);
CREATE INDEX `cdr__idx__dst` ON `cdr`(`dst`);
CREATE INDEX `cdr__idx__channel` ON `cdr`(`channel`);
CREATE INDEX `cdr__idx__duration` ON `cdr`(`duration`);
CREATE INDEX `cdr__idx__disposition` ON `cdr`(`disposition`);
CREATE INDEX `cdr__idx__amaflags` ON `cdr`(`amaflags`);
CREATE INDEX `cdr__idx__accountcode` ON `cdr`(`accountcode`);
CREATE INDEX `cdr__idx__userfield` ON `cdr`(`userfield`);


DROP TABLE IF EXISTS `dialstatus`;
CREATE TABLE `dialstatus` (
 `id` int(10) unsigned auto_increment,
 `status` enum('noanswer','congestion','busy','chanunavail') NOT NULL,
 `category` enum('user','group','callfilter') NOT NULL,
 `categoryval` varchar(128) NOT NULL DEFAULT '',
 `type` enum('endcall','user','group','queue','meetme','schedule','application','sound','custom') NOT NULL,
 `typeval` varchar(255) NOT NULL DEFAULT '',
 `applicationval` varchar(80) NOT NULL DEFAULT '',
 `linked` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `dialstatus__idx__type_typeval` ON `dialstatus`(`type`,`typeval`);
CREATE INDEX `dialstatus__idx__applicationval` ON `dialstatus`(`applicationval`);
CREATE INDEX `dialstatus__idx__linked` ON `dialstatus`(`linked`);
CREATE UNIQUE INDEX `dialstatus__uidx__status_category_categoryval` ON `dialstatus`(`status`,`category`,`categoryval`);


DROP TABLE IF EXISTS `extensions`;
CREATE TABLE `extensions` (
 `id` int(10) unsigned auto_increment,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `context` varchar(39) NOT NULL DEFAULT '',
 `exten` varchar(40) NOT NULL DEFAULT '',
 `priority` tinyint(3) unsigned NOT NULL DEFAULT 0,
 `app` varchar(128) NOT NULL DEFAULT '',
 `appdata` varchar(128) NOT NULL DEFAULT '',
 `name` varchar(128) NOT NULL DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `extensions__idx__commented` ON `extensions`(`commented`);
CREATE INDEX `extensions__idx__context_exten_priority` ON `extensions`(`context`,`exten`,`priority`);
CREATE INDEX `extensions__idx__name` ON `extensions`(`name`);

INSERT INTO `extensions` VALUES (NULL,1,'features','*33',1,'AgentLogin','','agentdynamiclogin');
INSERT INTO `extensions` VALUES (NULL,1,'features','_*31.',1,'Macro','agentstaticlogin|${EXTEN:3}','agentstaticlogin');
INSERT INTO `extensions` VALUES (NULL,1,'features','_*32.',1,'Macro','agentstaticlogoff|${EXTEN:3}|fr','agentstaticlogoff');
INSERT INTO `extensions` VALUES (NULL,0,'features','_*37.',1,'Macro','bsfilter|${EXTEN:3}','bsfilter');
INSERT INTO `extensions` VALUES (NULL,1,'features','*34',1,'Macro','calllistening','calllistening');
INSERT INTO `extensions` VALUES (NULL,0,'features','*36',1,'Directory','${CONTEXT}','directoryaccess');
INSERT INTO `extensions` VALUES (NULL,0,'features','*25',1,'Macro','enablednd','enablednd');
INSERT INTO `extensions` VALUES (NULL,0,'features','*24',1,'Macro','enablevm','enablevm');
INSERT INTO `extensions` VALUES (NULL,0,'features','_*23.',1,'Macro','fwdbusy|${EXTEN:3}','fwdbusy');
INSERT INTO `extensions` VALUES (NULL,0,'features','_*22.',1,'Macro','fwdrna|${EXTEN:3}','fwdrna');
INSERT INTO `extensions` VALUES (NULL,0,'features','_*21.',1,'Macro','fwdunc|${EXTEN:3}','fwdunc');
INSERT INTO `extensions` VALUES (NULL,0,'features','*20',1,'Macro','fwdundoall','fwdundoall');
INSERT INTO `extensions` VALUES (NULL,0,'features','*23',1,'Macro','fwdundobusy','fwdundobusy');
INSERT INTO `extensions` VALUES (NULL,0,'features','*22',1,'Macro','fwdundorna','fwdundorna');
INSERT INTO `extensions` VALUES (NULL,0,'features','*21',1,'Macro','fwdundounc','fwdundounc');
INSERT INTO `extensions` VALUES (NULL,0,'features','*27',1,'Macro','incallfilter','incallfilter');
INSERT INTO `extensions` VALUES (NULL,1,'features','*26',1,'Macro','incallrec','incallrec');
INSERT INTO `extensions` VALUES (NULL,0,'features','*10',1,'Macro','phonestatus','phonestatus');
INSERT INTO `extensions` VALUES (NULL,0,'features','_*8.',1,'Pickup','${EXTEN:2}','pickup');
INSERT INTO `extensions` VALUES (NULL,0,'features','*9',1,'Macro','recsnd|/usr/share/asterisk/sounds/web-interface/recordings|wav','recsnd');
INSERT INTO `extensions` VALUES (NULL,1,'features','*35',1,'Macro','vmdelete|/var/spool/asterisk/voicemail','vmdelete');
INSERT INTO `extensions` VALUES (NULL,0,'features','*98',1,'Macro','voicemsg','voicemsg');


DROP TABLE IF EXISTS `extenumbers`;
CREATE TABLE `extenumbers` (
 `id` int(10) unsigned auto_increment,
 `exten` varchar(40) NOT NULL DEFAULT '',
 `extenhash` char(40) NOT NULL DEFAULT '',
 `context` varchar(39) NOT NULL,
 `type` enum('extenfeatures','featuremap','generalfeatures','group','handynumbers','incall','meetme','outcall','queue','user') NOT NULL,
 `typeval` varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `extenumbers__idx__exten` ON `extenumbers`(`exten`);
CREATE INDEX `extenumbers__idx__extenhash` ON `extenumbers`(`extenhash`);
CREATE INDEX `extenumbers__idx__context` ON `extenumbers`(`context`);
CREATE INDEX `extenumbers__idx__type` ON `extenumbers`(`type`);
CREATE INDEX `extenumbers__idx__typeval` ON `extenumbers`(`typeval`);

INSERT INTO `extenumbers` VALUES (NULL,'*8','aa5820277564fac26df0e3dc72f796407597721d','','generalfeatures','pickupexten');
INSERT INTO `extenumbers` VALUES (NULL,'700','d8e4bbea3af2e4861ad5a445aaec573e02f9aca2','','generalfeatures','parkext');
INSERT INTO `extenumbers` VALUES (NULL,'*0','e914c907ff6d7a8ffefae72fe47363726b39d112','','featuremap','disconnect');
INSERT INTO `extenumbers` VALUES (NULL,'*1','8e04e82b8798d3979eded4ca2afdda3bebcb963d','','featuremap','blindxfer');
INSERT INTO `extenumbers` VALUES (NULL,'*2','6951109c8ca021277336cc2c8f6ac7f47d3b30e9','','featuremap','atxfer');
INSERT INTO `extenumbers` VALUES (NULL,'*3','68631b4b53ba2a27a969ca63bdcdc00805c2c258','','featuremap','automon');
INSERT INTO `extenumbers` VALUES (NULL,'*33','197040c848ebe83ae2e7f0b26a8667291d6b2746','','extenfeatures','agentdynamiclogin');
INSERT INTO `extenumbers` VALUES (NULL,'_*31.','678fe23ee0d6aa64460584bebbed210e270d662f','','extenfeatures','agentstaticlogin');
INSERT INTO `extenumbers` VALUES (NULL,'_*32.','3ae0f1ff0ef4907faa2dad5da7bb891c9dbf45ad','','extenfeatures','agentstaticlogoff');
INSERT INTO `extenumbers` VALUES (NULL,'_*37.','249b00b17a5983bbb2af8ed0af2ab1a74abab342','','extenfeatures','bsfilter');
INSERT INTO `extenumbers` VALUES (NULL,'*34','668a8d2d8fe980b663e2cdcecb977860e1b272f3','','extenfeatures','calllistening');
INSERT INTO `extenumbers` VALUES (NULL,'*36','f9b69fe3c361ddfc2ae49e048460ea197ea850c8','','extenfeatures','directoryaccess');
INSERT INTO `extenumbers` VALUES (NULL,'*25','c0d236c38bf8d5d84a2e154203cd2a18b86c6b2a','','extenfeatures','enablednd');
INSERT INTO `extenumbers` VALUES (NULL,'*24','0af70ed679c6db8c1128f2eb1e05d95a7a2fe4b6','','extenfeatures','enablevm');
INSERT INTO `extenumbers` VALUES (NULL,'_*23.','a1968a70f1d265b8aa263e73c79259961c4f7bbb','','extenfeatures','fwdbusy');
INSERT INTO `extenumbers` VALUES (NULL,'_*22.','00638af9e028d4cd454c00f43caf5626baa7d84c','','extenfeatures','fwdrna');
INSERT INTO `extenumbers` VALUES (NULL,'_*21.','52c97d56ebcca524ccf882590e94c52f6db24649','','extenfeatures','fwdunc');
INSERT INTO `extenumbers` VALUES (NULL,'*20','934aca632679075488681be0e9904cf9102f8766','','extenfeatures','fwdundoall');
INSERT INTO `extenumbers` VALUES (NULL,'*23','572a822ffb7c680bd0f92cfde0b64530bd362696','','extenfeatures','fwdundobusy');
INSERT INTO `extenumbers` VALUES (NULL,'*22','724827dabea7a207bfef4d948984a2e4da9de3ab','','extenfeatures','fwdundorna');
INSERT INTO `extenumbers` VALUES (NULL,'*21','8fa35a886d3149c94d23ba4e69c041c1fe6468b9','','extenfeatures','fwdundounc');
INSERT INTO `extenumbers` VALUES (NULL,'*27','663b9615ba92c21f80acac52d60b28a8d1fb1c58','','extenfeatures','incallfilter');
INSERT INTO `extenumbers` VALUES (NULL,'*26','f8aeb70618cc87f1143c7dff23cdc0d3d0a48a0c','','extenfeatures','incallrec');
INSERT INTO `extenumbers` VALUES (NULL,'*10','eecefbd85899915e6fc2ff5a8ea44c2c83597cd6','','extenfeatures','phonestatus');
INSERT INTO `extenumbers` VALUES (NULL,'_*8.','b349d094036a97a7e0631ba60de759a9597c1c3a','','extenfeatures','pickup');
INSERT INTO `extenumbers` VALUES (NULL,'*9','e28d0f359da60dcf86340435478b19388b1b1d05','','extenfeatures','recsnd');
INSERT INTO `extenumbers` VALUES (NULL,'*35','c4230e424c7189becec7ee35e9509829c5aea039','','extenfeatures','vmdelete');
INSERT INTO `extenumbers` VALUES (NULL,'*98','6fb653e9eaf6f4d9c8d2cb48d1a6e3f4d4085710','','extenfeatures','voicemsg');


DROP TABLE IF EXISTS `features`;
CREATE TABLE `features` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(255),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `features__idx__commented` ON `features`(`commented`);
CREATE INDEX `features__idx__filename` ON `features`(`filename`);
CREATE INDEX `features__idx__category` ON `features`(`category`);
CREATE INDEX `features__idx__var_name` ON `features`(`var_name`);

INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','parkext','700');
INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','context','parkedcalls');
INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','parkingtime','45');
INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','parkpos','701-750');
INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','parkfindnext','no');
INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','adsipark','no');
INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','transferdigittimeout','3');
INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','featuredigittimeout','500');
INSERT INTO `features` VALUES (NULL,0,0,1,'features.conf','general','courtesytone',NULL);
INSERT INTO `features` VALUES (NULL,0,0,1,'features.conf','general','xfersound',NULL);
INSERT INTO `features` VALUES (NULL,0,0,1,'features.conf','general','xferfailsound',NULL);
INSERT INTO `features` VALUES (NULL,0,0,0,'features.conf','general','pickupexten','*8');
INSERT INTO `features` VALUES (NULL,1,0,0,'features.conf','featuremap','blindxfer','*1');
INSERT INTO `features` VALUES (NULL,1,0,0,'features.conf','featuremap','atxfer','*2');
INSERT INTO `features` VALUES (NULL,1,0,0,'features.conf','featuremap','automon','*3');
INSERT INTO `features` VALUES (NULL,1,0,0,'features.conf','featuremap','disconnect','*0');


DROP TABLE IF EXISTS `generaliax`;
CREATE TABLE `generaliax` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(255),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `generaliax__idx__commented` ON `generaliax`(`commented`);
CREATE INDEX `generaliax__idx__filename` ON `generaliax`(`filename`);
CREATE INDEX `generaliax__idx__category` ON `generaliax`(`category`);
CREATE INDEX `generaliax__idx__var_name` ON `generaliax`(`var_name`);

INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','bindport','4569');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','bindaddr','0.0.0.0');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','iaxcompat','no');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','authdebug','yes');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','delayreject','no');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','trunkfreq','20');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','trunktimestamps','yes');
INSERT INTO `generaliax` VALUES (NULL,0,0,1,'iax.conf','general','regcontext',NULL);
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','minregexpire','60');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','maxregexpire','60');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','bandwidth','high');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','tos','lowdelay');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','jitterbuffer','no');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','forcejitterbuffer','no');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','dropcount','3');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','maxjitterbuffer','1000');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','maxjitterinterps','10');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','resyncthreshold','1000');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','minexcessbuffer','10');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','maxexcessbuffer','50');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','jittershrinkrate','2');
INSERT INTO `generaliax` VALUES (NULL,0,0,1,'iax.conf','general','accountcode',NULL);
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','amaflags','default');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','mailboxdetail','yes');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','notransfer','no');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','language','fr');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','encryption','no');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','maxauthreq','0');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','codecpriority','host');
INSERT INTO `generaliax` VALUES (NULL,0,0,1,'iax.conf','general','disallow',NULL);
INSERT INTO `generaliax` VALUES (NULL,0,0,1,'iax.conf','general','allow',NULL);
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','rtcachefriends','yes');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','rtupdate','yes');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','rtignoreregexpire','no');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','rtautoclear','no');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','pingtime','20');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','lagrqtime','10');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','nochecksums','no');
INSERT INTO `generaliax` VALUES (NULL,0,0,0,'iax.conf','general','autokill','yes');


DROP TABLE IF EXISTS `generalqueue`;
CREATE TABLE `generalqueue` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(128),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `generalqueue__idx__commented` ON `generalqueue`(`commented`);
CREATE INDEX `generalqueue__idx__filename` ON `generalqueue`(`filename`);
CREATE INDEX `generalqueue__idx__category` ON `generalqueue`(`category`);
CREATE INDEX `generalqueue__idx__var_name` ON `generalqueue`(`var_name`);

INSERT INTO `generalqueue` VALUES (1,0,0,0,'queues.conf','general','persistentmembers','yes');


DROP TABLE IF EXISTS `generalsip`;
CREATE TABLE `generalsip` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(255),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `generalsip__idx__commented` ON `generalsip`(`commented`);
CREATE INDEX `generalsip__idx__filename` ON `generalsip`(`filename`);
CREATE INDEX `generalsip__idx__category` ON `generalsip`(`category`);
CREATE INDEX `generalsip__idx__var_name` ON `generalsip`(`var_name`);

INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','bindport',5060);
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','bindaddr','0.0.0.0');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','videosupport','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','autocreatepeer','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','allowguest','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','promiscredir','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','autodomain','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,1,'sip.conf','general','domain',NULL);
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','allowexternaldomains','yes');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','usereqphone','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','realm','xivo');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','alwaysauthreject','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','useragent','XIVO PBX');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','checkmwi',10);
INSERT INTO `generalsip` VALUES (NULL,0,0,1,'sip.conf','general','regcontext',NULL);
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','callerid','xivo');
INSERT INTO `generalsip` VALUES (NULL,0,0,1,'sip.conf','general','fromdomain',NULL);
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','sipdebug','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','dumphistory','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','recordhistory','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','callevents','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','tos','lowdelay');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','ospauth','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,1,'sip.conf','general','localnet',NULL);
INSERT INTO `generalsip` VALUES (NULL,0,0,1,'sip.conf','general','externip',NULL);
INSERT INTO `generalsip` VALUES (NULL,0,0,1,'sip.conf','general','externhost',NULL);
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','externrefresh',10);
INSERT INTO `generalsip` VALUES (NULL,0,0,1,'sip.conf','general','outboundproxy',NULL);
INSERT INTO `generalsip` VALUES (NULL,0,0,1,'sip.conf','general','outboundproxyport',NULL);
INSERT INTO `generalsip` VALUES (NULL,0,0,1,'sip.conf','general','disallow',NULL);
INSERT INTO `generalsip` VALUES (NULL,0,0,1,'sip.conf','general','allow',NULL);
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','relaxdtmf','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','compactheaders','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','rtptimeout',0);
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','rtpholdtimeout',0);
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','rtpkeepalive',0);
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','notifymimetype','application/simple-message-summary');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','srvlookup','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','pedantic','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','maxexpiry',3600);
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','defaultexpiry',120);
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','registertimeout',20);
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','registerattempts',0);
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','notifyringing','yes');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','context','default');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','nat','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','dtmfmode','info');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','qualify','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','useclientcode','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','progressinband','never');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','language','fr');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','musiconhold','default');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','vmexten','*98');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','trustrpid','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','sendrpid','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','canreinvite','yes');
INSERT INTO `generalsip` VALUES (NULL,0,0,1,'sip.conf','general','insecure',NULL);
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','rtcachefriends','yes');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','rtupdate','yes');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','ignoreregexpire','no');
INSERT INTO `generalsip` VALUES (NULL,0,0,0,'sip.conf','general','rtautoclear','no');


DROP TABLE IF EXISTS `generalvoicemail`;
CREATE TABLE `generalvoicemail` (
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

CREATE INDEX `generalvoicemail__idx__commented` ON `generalvoicemail`(`commented`);
CREATE INDEX `generalvoicemail__idx__filename` ON `generalvoicemail`(`filename`);
CREATE INDEX `generalvoicemail__idx__category` ON `generalvoicemail`(`category`);
CREATE INDEX `generalvoicemail__idx__var_name` ON `generalvoicemail`(`var_name`);

INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','maxmsg','100');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','silencethreshold','256');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','minmessage','0');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','maxmessage','0');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','maxsilence','15');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','review','yes');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','operator','yes');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','format','wav');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','maxlogins','3');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','envelope','yes');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','saycid','no');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','cidinternalcontexts','default');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','sayduration','yes');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','saydurationm','2');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','forcename','no');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','forcegreetings','no');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','maxgreet','0');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','skipms','3000');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','sendvoicemail','no');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','usedirectory','yes');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','nextaftercmd','yes');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','dialout',NULL);
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','callback',NULL);
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','exitcontext',NULL);
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','attach','yes');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','mailcmd','/usr/sbin/sendmail -t');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','serveremail','xivo');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','charset','UTF-8');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','fromstring','XIVO PBX');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','emaildateformat','%A, %B %d, %Y à %r');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','pbxskip','no');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','emailsubject','Messagerie XIVO');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','emailbody','Bonjour ${VM_NAME} !

Vous avez reçu un message d\'une durée de ${VM_DUR} minute(s), il vous reste actuellement ${VM_MSGNUM} message(s) non lu(s) sur votre messagerie vocale : ${VM_MAILBOX}.

Le dernier a été envoyé par ${VM_CALLERID}, le ${VM_DATE}. Si vous le souhaitez vous pouvez l\'écouter ou le consulter en tapant le *98 sur votre téléphone.

Merci.

-- Messagerie XIVO --');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','pagerfromstring','XIVO PBX');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','pagersubject',NULL);
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','pagerbody',NULL);
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','adsifdn','0000000F');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','adsisec','9BDBF7AC');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','adsiver','1');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','searchcontexts','no');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,0,'voicemail.conf','general','externpass','/usr/share/asterisk/bin/change-pass-vm');
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','externnotify',NULL);
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','odbcstorage',NULL);
INSERT INTO `generalvoicemail` VALUES (NULL,0,0,1,'voicemail.conf','general','odbctable',NULL);
INSERT INTO `generalvoicemail` VALUES (NULL,1,0,0,'voicemail.conf','zonemessages','eu-fr','Europe/Paris|\'vm-received\' q \'digits/at\' kM');


DROP TABLE IF EXISTS `groupfeatures`;
CREATE TABLE `groupfeatures` (
 `id` tinyint(2) unsigned auto_increment,
 `name` varchar(128) NOT NULL,
 `number` varchar(40) NOT NULL DEFAULT '',
 `context` varchar(39) NOT NULL,
 `timeout` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `deleted` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `groupfeatures__idx__name` ON `groupfeatures`(`name`);
CREATE INDEX `groupfeatures__idx__deleted` ON `groupfeatures`(`deleted`);


DROP TABLE IF EXISTS `handynumbers`;
CREATE TABLE `handynumbers` (
 `id` int(10) unsigned auto_increment,
 `exten` varchar(40) NOT NULL DEFAULT '',
 `trunkfeaturesid` int(10) unsigned NOT NULL DEFAULT 0,
 `type` enum('emergency','special') NOT NULL,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `handynumbers__idx__trunkfeaturesid` ON `handynumbers`(`trunkfeaturesid`);
CREATE INDEX `handynumbers__idx__type` ON `handynumbers`(`type`);
CREATE INDEX `handynumbers__idx__commented` ON `handynumbers`(`commented`);
CREATE UNIQUE INDEX `handynumbers__uidx__exten` ON `handynumbers`(`exten`);


DROP TABLE IF EXISTS `incall`;
CREATE TABLE `incall` (
 `id` int(10) unsigned auto_increment,
 `exten` varchar(40) NOT NULL,
 `context` varchar(39) NOT NULL,
 `type` enum('endcall','user','group','queue','meetme','schedule','application','sound','custom') NOT NULL,
 `typeval` varchar(255) NOT NULL DEFAULT '',
 `applicationval` varchar(80) NOT NULL DEFAULT '',
 `linked` tinyint(1) NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `incall__idx__type_typeval` ON `incall`(`type`,`typeval`);
CREATE INDEX `incall__idx__applicationval` ON `incall`(`applicationval`);
CREATE INDEX `incall__idx__linked` ON `incall`(`linked`);
CREATE INDEX `incall__idx__commented` ON `incall`(`commented`);
CREATE UNIQUE INDEX `incall__uidx__exten_context` ON `incall`(`exten`,`context`);


DROP TABLE IF EXISTS `meetme`;
CREATE TABLE `meetme` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(128),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `meetme__idx__commented` ON `meetme`(`commented`);
CREATE INDEX `meetme__idx__filename` ON `meetme`(`filename`);
CREATE INDEX `meetme__idx__category` ON `meetme`(`category`);
CREATE INDEX `meetme__idx__var_name` ON `meetme`(`var_name`);

INSERT INTO `meetme` VALUES (1,0,0,0,'meetme.conf','general','audiobuffer','32');


DROP TABLE IF EXISTS `meetmefeatures`;
CREATE TABLE `meetmefeatures` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(128) NOT NULL,
 `number` varchar(40) NOT NULL,
 `meetmeid` int(10) unsigned NOT NULL,
 `mode` enum('talk','listen','all') NOT NULL DEFAULT 'all',
 `musiconhold` varchar(128) NOT NULL DEFAULT '',
 `context` varchar(39) NOT NULL,
 `poundexit` tinyint(1) NOT NULL DEFAULT 0,
 `quiet` tinyint(1) NOT NULL DEFAULT 0,
 `record` tinyint(1) NOT NULL DEFAULT 0,
 `adminmode` tinyint(1) NOT NULL DEFAULT 0,
 `announceusercount` tinyint(1) NOT NULL DEFAULT 0,
 `announcejoinleave` tinyint(1) NOT NULL DEFAULT 0,
 `alwayspromptpin` tinyint(1) NOT NULL DEFAULT 0,
 `starmenu` tinyint(1) NOT NULL DEFAULT 0,
 `enableexitcontext` tinyint(1) NOT NULL DEFAULT 0,
 `exitcontext` varchar(39) NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE UNIQUE INDEX `meetmefeatures__uidx__meetmeid` ON `meetmefeatures`(`meetmeid`);
CREATE UNIQUE INDEX `meetmefeatures__uidx__name` ON `meetmefeatures`(`name`);
CREATE UNIQUE INDEX `meetmefeatures__uidx__number` ON `meetmefeatures`(`number`);


DROP TABLE IF EXISTS `musiconhold`;
CREATE TABLE `musiconhold` (
 `id` int(10) unsigned auto_increment,
 `cat_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `var_metric` int(10) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(128),
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `musiconhold__idx__commented` ON `musiconhold`(`commented`);
CREATE UNIQUE INDEX `musiconhold__uidx__filename_category_var_name` ON `musiconhold`(`filename`(64),`category`,`var_name`);

INSERT INTO `musiconhold` VALUES (1,0,0,0,'musiconhold.conf','default','mode','custom');
INSERT INTO `musiconhold` VALUES (2,0,0,0,'musiconhold.conf','default','application','/usr/bin/madplay --mono -a -10 -R 8000 --output=raw:-');
INSERT INTO `musiconhold` VALUES (3,0,0,0,'musiconhold.conf','default','random','no');
INSERT INTO `musiconhold` VALUES (4,0,0,0,'musiconhold.conf','default','directory','/usr/share/asterisk/moh/default');


DROP TABLE IF EXISTS `outcall`;
CREATE TABLE `outcall` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(128) NOT NULL,
 `exten` varchar(40) NOT NULL,
 `context` varchar(39) NOT NULL,
 `externprefix` varchar(20) NOT NULL DEFAULT '',
 `stripnum` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `setcallerid` tinyint(1) NOT NULL DEFAULT 0,
 `callerid` varchar(80) NOT NULL DEFAULT '',
 `useenum` tinyint(1) NOT NULL DEFAULT 0,
 `internal` tinyint(1) NOT NULL DEFAULT 0,
 `hangupringtime` smallint(3) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `outcall__idx__commented` ON `outcall`(`commented`);
CREATE UNIQUE INDEX `outcall__uidx__name` ON `outcall`(`name`);
CREATE UNIQUE INDEX `outcall__uidx__exten_context` ON `outcall`(`exten`,`context`);


DROP TABLE IF EXISTS `outcalltrunk`;
CREATE TABLE `outcalltrunk` (
 `outcallid` int(10) unsigned NOT NULL DEFAULT 0,
 `trunkfeaturesid` int(10) unsigned NOT NULL DEFAULT 0,
 `priority` tinyint(2) unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(`outcallid`,`trunkfeaturesid`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `outcalltrunk__idx__priority` ON `outcalltrunk`(`priority`);


DROP TABLE IF EXISTS `phone`;
CREATE TABLE `phone` (
 `macaddr` char(17) NOT NULL,
 `vendor` varchar(16) NOT NULL,
 `model` varchar(16) NOT NULL,
 `proto` varchar(50) NOT NULL,
 `iduserfeatures` int(10) unsigned NOT NULL,
 `isinalan` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`macaddr`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `phone__idx__proto_iduserfeatures` ON `phone`(`proto`,`iduserfeatures`);


DROP TABLE IF EXISTS `phonebook`;
CREATE TABLE `phonebook` (
 `id` int(10) unsigned auto_increment,
 `title` enum('mr','mrs','ms') NOT NULL,
 `firstname` varchar(128) NOT NULL DEFAULT '',
 `lastname` varchar(128) NOT NULL DEFAULT '',
 `displayname` varchar(64) NOT NULL DEFAULT '',
 `society` varchar(128) NOT NULL DEFAULT '',
 `email` varchar(255) NOT NULL DEFAULT '',
 `url` varchar(255) NOT NULL DEFAULT '',
 `image` blob,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `phonebook__idx__title` ON `phonebook`(`title`);
CREATE INDEX `phonebook__idx__firstname` ON `phonebook`(`firstname`);
CREATE INDEX `phonebook__idx__lastname` ON `phonebook`(`lastname`);
CREATE INDEX `phonebook__idx__displayname` ON `phonebook`(`displayname`);
CREATE INDEX `phonebook__idx__society` ON `phonebook`(`society`);
CREATE INDEX `phonebook__idx__email` ON `phonebook`(`email`);


DROP TABLE IF EXISTS `phonebookaddress`;
CREATE TABLE `phonebookaddress` (
 `id` int(10) unsigned auto_increment,
 `phonebookid` int(10) unsigned NOT NULL,
 `address1` varchar(30) NOT NULL DEFAULT '',
 `address2` varchar(30) NOT NULL DEFAULT '',
 `city` varchar(128) NOT NULL DEFAULT '',
 `state` varchar(128) NOT NULL DEFAULT '',
 `zipcode` varchar(16) NOT NULL DEFAULT '',
 `country` varchar(3) NOT NULL DEFAULT '',
 `type` enum('home','office','other') NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `phonebookaddress__idx__address1` ON `phonebookaddress`(`address1`);
CREATE INDEX `phonebookaddress__idx__address2` ON `phonebookaddress`(`address2`);
CREATE INDEX `phonebookaddress__idx__city` ON `phonebookaddress`(`city`);
CREATE INDEX `phonebookaddress__idx__state` ON `phonebookaddress`(`state`);
CREATE INDEX `phonebookaddress__idx__zipcode` ON `phonebookaddress`(`zipcode`);
CREATE INDEX `phonebookaddress__idx__country` ON `phonebookaddress`(`country`);
CREATE INDEX `phonebookaddress__idx__type` ON `phonebookaddress`(`type`);
CREATE UNIQUE INDEX `phonebookaddress__uidx__phonebookid_type` ON `phonebookaddress`(`phonebookid`,`type`);


DROP TABLE IF EXISTS `phonebooknumber`;
CREATE TABLE `phonebooknumber` (
 `id` int(10) unsigned auto_increment,
 `phonebookid` int(10) unsigned NOT NULL,
 `number` varchar(40) NOT NULL DEFAULT '',
 `type` enum('home','office','mobile','fax','other') NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `phonebooknumber__idx__number` ON `phonebooknumber`(`number`);
CREATE INDEX `phonebooknumber__idx__type` ON `phonebooknumber`(`type`);
CREATE UNIQUE INDEX `phonebooknumber__uidx__phonebookid_type` ON `phonebooknumber`(`phonebookid`,`type`);


DROP TABLE IF EXISTS `phonefunckey`;
CREATE TABLE `phonefunckey` (
 `iduserfeatures` int(10) unsigned NOT NULL,
 `fknum` int(10) unsigned NOT NULL,
 `exten` varchar(40) NOT NULL DEFAULT '',
 `typeextenumbers` enum('user','group','queue','meetme','extenfeatures','featuremap','generalfeatures'),
 `typevalextenumbers` varchar(255) NOT NULL DEFAULT '',
 `supervision` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`iduserfeatures`,`fknum`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `phonefunckey__idx__exten` ON `phonefunckey`(`exten`);
CREATE INDEX `phonefunckey__idx__typeextenumbers_typevalextenumbers` ON `phonefunckey`(`typeextenumbers`,`typevalextenumbers`);


DROP TABLE IF EXISTS `queue`;
CREATE TABLE `queue` (
 `name` varchar(128) NOT NULL,
 `musiconhold` varchar(128),
 `announce` varchar(128),
 `context` varchar(39),
 `timeout` tinyint(2) unsigned DEFAULT 0,
 `monitor-join` tinyint(1) NOT NULL DEFAULT 0,
 `monitor-format` varchar(128),
 `queue-youarenext` varchar(128),
 `queue-thereare` varchar(128),
 `queue-callswaiting` varchar(128),
 `queue-holdtime` varchar(128),
 `queue-minutes` varchar(128),
 `queue-seconds` varchar(128),
 `queue-lessthan` varchar(128),
 `queue-thankyou` varchar(128),
 `queue-reporthold` varchar(128),
 `periodic-announce` varchar(128),
 `announce-frequency` int(10) unsigned,
 `periodic-announce-frequency` int(10) unsigned,
 `announce-round-seconds` tinyint(2) unsigned,
 `announce-holdtime` varchar(4),
 `retry` tinyint(2) unsigned,
 `wrapuptime` int(10) unsigned,
 `maxlen` int(10) unsigned,
 `servicelevel` int(11),
 `strategy` varchar(11),
 `joinempty` varchar(6),
 `leavewhenempty` varchar(6),
 `eventmemberstatus` tinyint(1) NOT NULL DEFAULT 0,
 `eventwhencalled` tinyint(1) NOT NULL DEFAULT 0,
 `reportholdtime` tinyint(1) NOT NULL DEFAULT 0,
 `memberdelay` int(10) unsigned,
 `weight` int(10) unsigned,
 `timeoutrestart` tinyint(1) NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `category` enum('group','queue') NOT NULL,
 PRIMARY KEY(`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `queue__idx__commented` ON `queue`(`commented`);
CREATE INDEX `queue__idx__category` ON `queue`(`category`);


DROP TABLE IF EXISTS `queuefeatures`;
CREATE TABLE `queuefeatures` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(128) NOT NULL,
 `number` varchar(40) NOT NULL DEFAULT '',
 `context` varchar(39),
 `data_quality` tinyint(1) NOT NULL DEFAULT 0,
 `hitting_callee` tinyint(1) NOT NULL DEFAULT 0,
 `hitting_caller` tinyint(1) NOT NULL DEFAULT 0,
 `retries` tinyint(1) NOT NULL DEFAULT 0,
 `ring` tinyint(1) NOT NULL DEFAULT 0,
 `transfer_user` tinyint(1) NOT NULL DEFAULT 0,
 `transfer_call` tinyint(1) NOT NULL DEFAULT 0,
 `write_caller` tinyint(1) NOT NULL DEFAULT 0,
 `write_calling` tinyint(1) NOT NULL DEFAULT 0,
 `url` varchar(255) NOT NULL DEFAULT '',
 `announceoverride` varchar(128) NOT NULL DEFAULT '',
 `timeout` smallint(4) unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE UNIQUE INDEX `queuefeatures__uidx__name` ON `queuefeatures`(`name`);


DROP TABLE IF EXISTS `queuemember`;
CREATE TABLE `queuemember` (
 `queue_name` varchar(128) NOT NULL,
 `interface` varchar(128) NOT NULL,
 `penalty` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `call-limit` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `usertype` enum('agent','user') NOT NULL,
 `userid` int(10) unsigned NOT NULL,
 `channel` varchar(25) NOT NULL,
 `category` enum('group','queue') NOT NULL,
 PRIMARY KEY(`queue_name`,`interface`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `queuemember__idx__commented` ON `queuemember`(`commented`);
CREATE INDEX `queuemember__idx__usertype` ON `queuemember`(`usertype`);
CREATE INDEX `queuemember__idx__userid` ON `queuemember`(`userid`);
CREATE INDEX `queuemember__idx__channel` ON `queuemember`(`channel`);
CREATE INDEX `queuemember__idx__category` ON `queuemember`(`category`);
CREATE UNIQUE INDEX `queuemember__uidx__queue_name_channel_usertype_userid_category` ON `queuemember`(`queue_name`,`channel`,`usertype`,`userid`,`category`);


DROP TABLE IF EXISTS `rightcall`;
CREATE TABLE `rightcall` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(128) NOT NULL DEFAULT '',
 `passwd` varchar(40) NOT NULL DEFAULT '',
 `authorization` tinyint(1) NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `rightcall__idx__passwd` ON `rightcall`(`passwd`);
CREATE INDEX `rightcall__idx__authorization` ON `rightcall`(`authorization`);
CREATE INDEX `rightcall__idx__commented` ON `rightcall`(`commented`);
CREATE UNIQUE INDEX `rightcall__uidx__name` ON `rightcall`(`name`);


DROP TABLE IF EXISTS `rightcallexten`;
CREATE TABLE `rightcallexten` (
 `id` int(10) unsigned auto_increment,
 `rightcallid` int(10) unsigned NOT NULL DEFAULT 0,
 `exten` varchar(40) NOT NULL DEFAULT '',
 `extenhash` char(40) NOT NULL DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE UNIQUE INDEX `rightcallexten__uidx__rightcallid_extenhash` ON `rightcallexten`(`rightcallid`,`extenhash`);


DROP TABLE IF EXISTS `rightcallmember`;
CREATE TABLE `rightcallmember` (
 `id` int(10) unsigned auto_increment,
 `rightcallid` int(10) unsigned NOT NULL DEFAULT 0,
 `type` enum('user','group','incall','outcall') NOT NULL,
 `typeval` varchar(128) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE UNIQUE INDEX `rightcallmember__uidx__rightcallid_type_typeval` ON `rightcallmember`(`rightcallid`,`type`,`typeval`);


DROP TABLE IF EXISTS `schedule`;
CREATE TABLE `schedule` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(128) NOT NULL DEFAULT '',
 `timebeg` varchar(5) NOT NULL DEFAULT '*',
 `timeend` varchar(5),
 `daynamebeg` varchar(3) NOT NULL DEFAULT '*',
 `daynameend` varchar(3),
 `daynumbeg` varchar(2) NOT NULL DEFAULT '*',
 `daynumend` varchar(2),
 `monthbeg` varchar(3) NOT NULL DEFAULT '*',
 `monthend` varchar(3),
 `typetrue` enum('endcall','user','group','queue','meetme','schedule','application','sound','custom') NOT NULL,
 `typevaltrue` varchar(255) NOT NULL DEFAULT '',
 `applicationvaltrue` varchar(80) NOT NULL DEFAULT '',
 `typefalse` enum('endcall','user','group','queue','meetme','schedule','application','sound','custom') NOT NULL,
 `applicationvalfalse` varchar(80) NOT NULL DEFAULT '',
 `typevalfalse` varchar(255) NOT NULL DEFAULT '',
 `publicholiday` tinyint(1) NOT NULL DEFAULT 0,
 `linked` tinyint(1) NOT NULL DEFAULT 0,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `schedule__idx__typetrue_typevaltrue` ON `schedule`(`typetrue`,`typevaltrue`);
CREATE INDEX `schedule__idx__applicationvaltrue` ON `schedule`(`applicationvaltrue`);
CREATE INDEX `schedule__idx__typefalse_typevalfalse` ON `schedule`(`typefalse`,`typevalfalse`);
CREATE INDEX `schedule__idx__applicationvalfalse` ON `schedule`(`applicationvalfalse`);
CREATE INDEX `schedule__idx__publicholiday` ON `schedule`(`publicholiday`);
CREATE INDEX `schedule__idx__linked` ON `schedule`(`linked`);
CREATE INDEX `schedule__idx__commented` ON `schedule`(`commented`);
CREATE UNIQUE INDEX `schedule__uidx__name` ON `schedule`(`name`);


DROP TABLE IF EXISTS `serverfeatures`;
CREATE TABLE `serverfeatures` (
 `id` int(10) unsigned auto_increment,
 `serverid` int(10) unsigned NOT NULL,
 `type` enum('phonebook') NOT NULL,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `serverfeatures__idx__serverid` ON `serverfeatures`(`serverid`);
CREATE INDEX `serverfeatures__idx__type` ON `serverfeatures`(`type`);
CREATE INDEX `serverfeatures__idx__commented` ON `serverfeatures`(`commented`);
CREATE UNIQUE INDEX `serverfeatures__uidx__serverid_type` ON `serverfeatures`(`serverid`,`type`);


DROP TABLE IF EXISTS `trunkfeatures`;
CREATE TABLE `trunkfeatures` (
 `id` int(10) unsigned auto_increment,
 `trunk` varchar(50) NOT NULL,
 `trunkid` int(10) unsigned NOT NULL,
 `registerid` int(10) unsigned NOT NULL DEFAULT 0,
 `registercommented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `trunkfeatures__idx__registerid` ON `trunkfeatures`(`registerid`);
CREATE INDEX `trunkfeatures__idx__registercommented` ON `trunkfeatures`(`registercommented`);
CREATE UNIQUE INDEX `trunkfeatures__uidx__trunk_trunkid` ON `trunkfeatures`(`trunk`,`trunkid`);


DROP TABLE IF EXISTS `usercustom`;
CREATE TABLE `usercustom` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(40),
 `context` varchar(39),
 `interface` varchar(128) NOT NULL,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `protocol` enum('custom') NOT NULL DEFAULT 'custom',
 `category` enum('user','trunk') NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `usercustom__idx__name` ON `usercustom`(`name`);
CREATE INDEX `usercustom__idx__context` ON `usercustom`(`context`);
CREATE INDEX `usercustom__idx__commented` ON `usercustom`(`commented`);
CREATE INDEX `usercustom__idx__protocol` ON `usercustom`(`protocol`);
CREATE INDEX `usercustom__idx__category` ON `usercustom`(`category`);
CREATE UNIQUE INDEX `usercustom__uidx__interface_category` ON `usercustom`(`interface`,`category`);


DROP TABLE IF EXISTS `userfeatures`;
CREATE TABLE `userfeatures` (
 `id` int(10) unsigned auto_increment,
 `protocol` varchar(50) NOT NULL,
 `protocolid` int(10) unsigned NOT NULL,
 `firstname` varchar(128) NOT NULL DEFAULT '',
 `lastname` varchar(128) NOT NULL DEFAULT '',
 `name` varchar(128) NOT NULL,
 `number` varchar(40) NOT NULL,
 `context` varchar(39),
 `provisioningid` mediumint(8) unsigned,
 `ringseconds` tinyint(2) unsigned NOT NULL DEFAULT 30,
 `simultcalls` tinyint(2) unsigned NOT NULL DEFAULT 5,
 `enableclient` tinyint(1) NOT NULL DEFAULT 0,
 `enablehint` tinyint(1) NOT NULL DEFAULT 0,
 `enablevoicemail` tinyint(1) NOT NULL DEFAULT 0,
 `skipvoicemailpass` tinyint(1) NOT NULL DEFAULT 0,
 `enablexfer` tinyint(1) NOT NULL DEFAULT 0,
 `enableautomon` tinyint(1) NOT NULL DEFAULT 0,
 `callrecord` tinyint(1) NOT NULL DEFAULT 0,
 `callfilter` tinyint(1) NOT NULL DEFAULT 0,
 `enablednd` tinyint(1) NOT NULL DEFAULT 0,
 `enableunc` tinyint(1) NOT NULL DEFAULT 0,
 `destunc` varchar(128) NOT NULL DEFAULT '',
 `enablerna` tinyint(1) NOT NULL DEFAULT 0,
 `destrna` varchar(128) NOT NULL DEFAULT '',
 `enablebusy` tinyint(1) NOT NULL DEFAULT 0,
 `destbusy` varchar(128) NOT NULL DEFAULT '',
 `musiconhold` varchar(128) NOT NULL DEFAULT '',
 `outcallerid` varchar(80) NOT NULL DEFAULT '',
 `internal` tinyint(1) NOT NULL DEFAULT 0,
 `bsfilter` enum('no','boss','secretary') NOT NULL DEFAULT 'no',
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `description` text NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `userfeatures__idx__firstname` ON `userfeatures`(`firstname`);
CREATE INDEX `userfeatures__idx__lastname` ON `userfeatures`(`lastname`);
CREATE INDEX `userfeatures__idx__number` ON `userfeatures`(`number`);
CREATE INDEX `userfeatures__idx__context` ON `userfeatures`(`context`);
CREATE INDEX `userfeatures__idx__musiconhold` ON `userfeatures`(`musiconhold`);
CREATE INDEX `userfeatures__idx__provisioningid` ON `userfeatures`(`provisioningid`);
CREATE INDEX `userfeatures__idx__internal` ON `userfeatures`(`internal`);
CREATE INDEX `userfeatures__idx__commented` ON `userfeatures`(`commented`);
CREATE UNIQUE INDEX `userfeatures__uidx__protocol_name` ON `userfeatures`(`protocol`,`name`);
CREATE UNIQUE INDEX `userfeatures__uidx__protocol_protocolid` ON `userfeatures`(`protocol`,`protocolid`);

INSERT INTO `userfeatures` VALUES (1,'sip',1,'Guest','','guest','','initconfig',148378,30,5,0,0,0,0,0,0,0,0,0,0,'',0,'',0,'','','',1,'no',0,'');


DROP TABLE IF EXISTS `useriax`;
CREATE TABLE `useriax` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(40) NOT NULL,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `username` varchar(80) NOT NULL,
 `type` enum('friend','peer','user') NOT NULL,
 `secret` varchar(80),
 `md5secret` varchar(32),
 `dbsecret` varchar(100),
 `notransfer` char(3),
 `trunk` char(3),
 `inkeys` varchar(100),
 `outkey` varchar(100),
 `auth` varchar(100),
 `accountcode` varchar(100),
 `amaflags` varchar(13),
 `callerid` varchar(160),
 `callgroup` varchar(180),
 `context` varchar(39),
 `defaultip` varchar(15),
 `host` varchar(31) NOT NULL DEFAULT 'dynamic',
 `language` char(2),
 `mailbox` varchar(80),
 `deny` varchar(95),
 `permit` varchar(95),
 `qualify` char(3),
 `disallow` varchar(100),
 `allow` varchar(100),
 `ipaddr` varchar(15),
 `port` smallint unsigned,
 `regseconds` int(10) unsigned DEFAULT 0,
 `call-limit` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `protocol` enum('iax') NOT NULL DEFAULT 'iax',
 `category` enum('user','trunk') NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `useriax__idx__commented` ON `useriax`(`commented`);
CREATE INDEX `useriax__idx__protocol` ON `useriax`(`protocol`);
CREATE INDEX `useriax__idx__category` ON `useriax`(`category`);
CREATE UNIQUE INDEX `useriax__uidx__name` ON `useriax`(`name`);


DROP TABLE IF EXISTS `usersip`;
CREATE TABLE `usersip` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(40) NOT NULL,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 `accountcode` varchar(20),
 `amaflags` varchar(13),
 `callgroup` varchar(180),
 `callerid` varchar(160),
 `canreinvite` char(3),
 `context` varchar(39),
 `defaultip` varchar(15),
 `dtmfmode` varchar(7),
 `fromuser` varchar(80),
 `fromdomain` varchar(80),
 `fullcontact` varchar(80),
 `host` varchar(31) NOT NULL,
 `insecure` varchar(11),
 `language` char(2),
 `mailbox` varchar(80),
 `md5secret` varchar(80),
 `nat` varchar(5) NOT NULL DEFAULT 'no',
 `deny` varchar(95),
 `permit` varchar(95),
 `mask` varchar(95),
 `pickupgroup` varchar(80),
 `port` smallint unsigned,
 `qualify` char(3),
 `restrictcid` char(1),
 `rtptimeout` char(3),
 `rtpholdtimeout` char(3),
 `secret` varchar(80),
 `type` enum('friend','peer','user') NOT NULL,
 `username` varchar(80) NOT NULL,
 `disallow` varchar(100),
 `allow` varchar(100),
 `musiconhold` varchar(100),
 `regseconds` int(10) unsigned NOT NULL DEFAULT 0,
 `ipaddr` varchar(15),
 `regexten` varchar(80),
 `cancallforward` char(3),
 `setvar` varchar(100) NOT NULL,
 `call-limit` tinyint(2) unsigned NOT NULL DEFAULT 0,
 `protocol` enum('sip') NOT NULL DEFAULT 'sip',
 `category` enum('user','trunk') NOT NULL,
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `usersip__idx__commented` ON `usersip`(`commented`);
CREATE INDEX `usersip__idx__protocol` ON `usersip`(`protocol`);
CREATE INDEX `usersip__idx__category` ON `usersip`(`category`);
CREATE UNIQUE INDEX `usersip__uidx__name` ON `usersip`(`name`);

INSERT INTO `usersip` VALUES (1,'guest',0,'','documentation','','Guest','no','initconfig',NULL,'rfc2833',NULL,NULL,'','dynamic',NULL,NULL,'',NULL,'no',NULL,NULL,NULL,'',NULL,'no',NULL,NULL,NULL,'guest','friend','guest',NULL,NULL,NULL,0,NULL,NULL,NULL,'',0,'sip','user');


DROP TABLE IF EXISTS `uservoicemail`;
CREATE TABLE `uservoicemail` (
 `uniqueid` int(10) unsigned auto_increment,
 `context` varchar(39),
 `mailbox` varchar(40) NOT NULL DEFAULT '',
 `password` varchar(80) NOT NULL DEFAULT '',
 `fullname` varchar(80) NOT NULL DEFAULT '',
 `email` varchar(80) NOT NULL DEFAULT '',
 `pager` varchar(80) NOT NULL DEFAULT '',
 `dialout` varchar(39),
 `callback` varchar(39),
 `exitcontext` varchar(39),
 `language` varchar(20) NOT NULL DEFAULT '',
 `tz` varchar(80) NOT NULL DEFAULT '',
 `attach` tinyint(1) DEFAULT 1,
 `saycid` tinyint(1) DEFAULT 1,
 `review` tinyint(1) DEFAULT 0,
 `operator` tinyint(1) DEFAULT 0,
 `envelope` tinyint(1) DEFAULT 0,
 `sayduration` tinyint(1) DEFAULT 0,
 `saydurationm` tinyint(2) unsigned DEFAULT 2,
 `sendvoicemail` tinyint(1) DEFAULT 0,
 `deletevoicemail` tinyint(1) NOT NULL DEFAULT 0,
 `forcename` tinyint(1) DEFAULT 0,
 `forcegreetings` tinyint(1) DEFAULT 0,
 `hidefromdir` enum('yes','no') NOT NULL DEFAULT 'no',
 `maxmsg` smallint(4) unsigned DEFAULT 100,
 `commented` tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(`uniqueid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE INDEX `uservoicemail__idx__commented` ON `uservoicemail`(`commented`);
CREATE INDEX `uservoicemail__idx__context` ON `uservoicemail`(`context`);
CREATE UNIQUE INDEX `uservoicemail__uidx__mailbox_context` ON `uservoicemail`(`mailbox`,`context`);
