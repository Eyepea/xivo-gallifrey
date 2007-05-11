CREATE TABLE didfeatures (
 `id` int(11) unsigned auto_increment,
 `type` varchar(50) NOT NULL,
 `typeid` int(11) unsigned NOT NULL,
 `extenid` int(11) unsigned NOT NULL,
 `custom` varchar(128) NOT NULL default '',
 `number` varchar(80) NOT NULL,
 `commented` tinyint(1) NOT NULL default 0,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX didfeatures__idx__type_typeid ON didfeatures(type,typeid);
CREATE INDEX didfeatures__idx__commented ON didfeatures(commented);
CREATE UNIQUE INDEX didfeatures__uidx__extenid ON didfeatures(extenid);


CREATE TABLE extenumbers (
 `id` int(11) unsigned auto_increment,
 `number` varchar(80) NOT NULL,
 `context` varchar(80) NOT NULL,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE UNIQUE INDEX extenumbers__uidx__number_context ON extenumbers(number,context);


CREATE TABLE groupfeatures (
 `id` tinyint unsigned auto_increment,
 `name` varchar(255) NOT NULL,
 `number` varchar(80) default '',
 `context` varchar(80) NOT NULL default '',
 `commented` tinyint(1) default 0,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE UNIQUE INDEX groupfeatures__uidx__name_commented ON groupfeatures(name,commented);


CREATE TABLE meetme (
 `id` int(11) unsigned auto_increment,
 `cat_metric` int(11) unsigned default 0,
 `var_metric` int(11) unsigned default 0,
 `commented` tinyint(1) default 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(128) NOT NULL,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX meetme__idx__commented ON meetme(commented);
CREATE INDEX meetme__idx__filename ON meetme(filename);
CREATE INDEX meetme__idx__category ON meetme(category);
CREATE INDEX meetme__idx__var_name ON meetme(var_name);


CREATE TABLE meetmefeatures (
 `id` int(11) unsigned auto_increment,
 `name` varchar(128) NOT NULL,
 `number` varchar(128) NOT NULL,
 `meetmeid` int(11) unsigned NOT NULL,
 `mode` varchar(6) NOT NULL default 'all',
 `musiconhold` varchar(128) NOT NULL default '',
 `context` varchar(80) NOT NULL default '',
 `exit` tinyint(1) default 0,
 `quiet` tinyint(1) default 0,
 `record` tinyint(1) default 0,
 `video` tinyint(1) default 0,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE UNIQUE INDEX meetmefeatures__uidx__meetmeid ON meetmefeatures(meetmeid);
CREATE UNIQUE INDEX meetmefeatures__uidx__name ON meetmefeatures(name);
CREATE UNIQUE INDEX meetmefeatures__uidx__number ON meetmefeatures(number);


CREATE TABLE phone (
 `macaddr` char(17) NOT NULL,
 `vendor` varchar(16) NOT NULL,
 `model` varchar(16) NOT NULL,
 `proto` varchar(50) NOT NULL,
 `iduserfeatures` int(11) unsigned,
 PRIMARY KEY(macaddr)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX phone__idx__proto_iduserfeatures ON phone(proto,iduserfeatures);


CREATE TABLE queue (
 `name` varchar(128) NOT NULL,
 `musiconhold` varchar(128),
 `announce` varchar(128),
 `context` varchar(128),
 `timeout` tinyint unsigned,
 `monitor-join` tinyint(1) default 0,
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
 `announce-frequency` int(11),
 `periodic-announce-frequency` int(11),
 `announce-round-seconds` tinyint unsigned,
 `announce-holdtime` varchar(4),
 `retry` tinyint unsigned,
 `wrapuptime` tinyint unsigned,
 `maxlen` int(11) unsigned,
 `servicelevel` int(11),
 `strategy` varchar(11),
 `joinempty` varchar(6),
 `leavewhenempty` varchar(6),
 `eventmemberstatus` tinyint(1) default 0,
 `eventwhencalled` tinyint(1) default 0,
 `reportholdtime` tinyint(1) default 0,
 `memberdelay` int(11) unsigned,
 `weight` int(11) unsigned,
 `timeoutrestart` tinyint(1) default 0,
 `commented` tinyint(1) default 0,
 `category` enum('group','queue') NOT NULL,
 PRIMARY KEY(name)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX queue__idx__commented ON queue(commented);
CREATE INDEX queue__idx__category ON queue(category);


CREATE TABLE queuefeatures (
 `id` int(11) unsigned auto_increment,
 `name` varchar(255) NOT NULL,
 `number` varchar(80) default '',
 `context` varchar(80) NOT NULL default '',
 `data_quality` tinyint(1) default 0,
 `hitting_callee` tinyint(1) default 0,
 `hitting_caller` tinyint(1) default 0,
 `retries` tinyint(1) default 0,
 `ring` tinyint(1) default 0,
 `transfer_user` tinyint(1) default 0,
 `transfer_call` tinyint(1) default 0,
 `write_caller` tinyint(1) default 0,
 `write_calling` tinyint(1) default 0,
 `url` varchar(256) default '',
 `announceoverride` varchar(128) default '',
 `timeout` tinyint unsigned,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE UNIQUE INDEX queuefeatures__uidx__name ON queuefeatures(name);


CREATE TABLE queuemember (
 `queue_name` varchar(128) NOT NULL,
 `interface` varchar(128) NOT NULL,
 `penalty` int(11) unsigned default 0,
 `call-limit` int(11) unsigned default 0,
 `commented` tinyint(1) default 0,
 PRIMARY KEY(queue_name,interface)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX queuemember__idx__commented ON queuemember(commented);


CREATE TABLE trunkfeatures (
 `id` int(11) unsigned auto_increment,
 `trunk` varchar(50) NOT NULL,
 `trunkid` int(11) unsigned NOT NULL,
 `registerid` int(11) unsigned default 0,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX trunkfeatures__idx__registerid ON trunkfeatures(registerid);
CREATE UNIQUE INDEX trunkfeatures__uidx__trunk_trunkid ON trunkfeatures(trunk,trunkid);


CREATE TABLE usergroup (
 `id` int(11) unsigned auto_increment,
 `userid` int(11) unsigned NOT NULL,
 `groupid` tinyint unsigned NOT NULL,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE UNIQUE INDEX usergroup__uidx__userid_groupid ON usergroup(userid,groupid);


CREATE TABLE useriax (
 `id` int(11) unsigned auto_increment,
 `name` varchar(80) NOT NULL,
 `commented` tinyint(1) NOT NULL default 0,
 `username` varchar(80) NOT NULL,
 `type` varchar(6) NOT NULL default 'friend',
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
 `callerid` varchar(80),
 `callgroup` varchar(10),
 `context` varchar(80),
 `defaultip` varchar(15),
 `host` varchar(31) NOT NULL default 'dynamic',
 `language` char(2),
 `mailbox` varchar(50),
 `deny` varchar(95),
 `permit` varchar(95),
 `qualify` char(3),
 `disallow` varchar(100),
 `allow` varchar(100),
 `ipaddr` varchar(15) NOT NULL,
 `port` smallint unsigned NOT NULL,
 `regseconds` int(11) unsigned default 0,
 `call-limit` tinyint unsigned default 0,
 `category` varchar(50) NOT NULL,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX useriax__idx__commented ON useriax(commented);
CREATE INDEX useriax__idx__category ON useriax(category);
CREATE UNIQUE INDEX useriax__uidx__name ON useriax(name);


CREATE TABLE uservoicemail (
 `id` int(11) unsigned auto_increment,
 `customer_id` varchar(11) NOT NULL default '0',
 `context` varchar(50) NOT NULL default '',
 `mailbox` varchar(11) NOT NULL default '0',
 `password` varchar(5) NOT NULL default '0',
 `fullname` varchar(150) NOT NULL default '',
 `email` varchar(50) NOT NULL default '',
 `pager` varchar(50) NOT NULL default '',
 `tz` varchar(10) NOT NULL default 'central',
 `attach` varchar(4) NOT NULL default 'yes',
 `saycid` varchar(4) NOT NULL default 'yes',
 `dialout` varchar(10) NOT NULL default '',
 `callback` varchar(10) NOT NULL default '',
 `review` varchar(4) NOT NULL default 'no',
 `operator` varchar(4) NOT NULL default 'no',
 `envelope` varchar(4) NOT NULL default 'no',
 `sayduration` varchar(4) NOT NULL default 'no',
 `saydurationm` tinyint unsigned NOT NULL default 2,
 `sendvoicemail` varchar(4) NOT NULL default 'no',
 `delete` varchar(4) NOT NULL default 'no',
 `nextaftercmd` varchar(5) NOT NULL default 'yes',
 `forcename` varchar(4) NOT NULL default 'no',
 `forcegreetings` varchar(4) NOT NULL default 'no',
 `hidefromdir` varchar(4) NOT NULL default 'yes',
 `commented` tinyint(1) NOT NULL default 0,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX uservoicemail__idx__commented ON uservoicemail(commented);
CREATE INDEX uservoicemail__idx__context ON uservoicemail(context);
CREATE INDEX uservoicemail__idx__mailbox_context ON uservoicemail(mailbox,context);
CREATE UNIQUE INDEX uservoicemail__uidx__mailbox ON uservoicemail(mailbox);


CREATE TABLE userfeatures (
 `id` int(11) unsigned auto_increment,
 `protocol` varchar(50) NOT NULL,
 `protocolid` int(11) unsigned NOT NULL,
 `firstname` varchar(128) NOT NULL default '',
 `lastname` varchar(128) NOT NULL default '',
 `name` varchar(80) NOT NULL,
 `number` varchar(80) NOT NULL,
 `context` varchar(80) NOT NULL,
 `provisioningid` mediumint unsigned NOT NULL,
 `ringseconds` tinyint unsigned default 15,
 `ringgroup` tinyint(1) default 0,
 `simultcalls` tinyint unsigned default 1,
 `popupwidget` tinyint(1) default 0,
 `musiconhold` varchar(128) NOT NULL default '',
 `comment` text default '',
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX userfeatures__idx__firstname ON userfeatures(firstname);
CREATE INDEX userfeatures__idx__lastname ON userfeatures(lastname);
CREATE INDEX userfeatures__idx__number ON userfeatures(number);
CREATE INDEX userfeatures__idx__context ON userfeatures(context);
CREATE INDEX userfeatures__idx__musiconhold ON userfeatures(musiconhold);
CREATE UNIQUE INDEX userfeatures__uidx__protocol_name ON userfeatures(protocol,name);
CREATE UNIQUE INDEX userfeatures__uidx__protocol_protocolid ON userfeatures(protocol,protocolid);
CREATE UNIQUE INDEX userfeatures__uidx__provisioningid ON userfeatures(provisioningid);

INSERT INTO userfeatures VALUES(1,'sip',1,'Guest','','guest','','initconfig',116359,30,0,5,0,'','');


CREATE TABLE extensions (
 `id` int(11) unsigned auto_increment,
 `commented` tinyint(1) NOT NULL default 0,
 `context` varchar(20) NOT NULL default '',
 `exten` varchar(20) NOT NULL default '',
 `priority` tinyint unsigned NOT NULL default 0,
 `app` varchar(20) NOT NULL default '',
 `appdata` varchar(128) NOT NULL default '',
 `name` varchar(128) default '',
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX extensions__idx__commented ON extensions(commented);
CREATE INDEX extensions__idx__context_exten_priority ON extensions(context,exten,priority);
CREATE INDEX extensions__idx__name ON extensions(name);

INSERT INTO extensions VALUES(1,0,'features','_*98',1,'VoiceMailMain','${CALLERID(num)@default}','voicemsg');
INSERT INTO extensions VALUES(2,0,'features','_*98',2,'Hangup','','voicemsg');
INSERT INTO extensions VALUES(3,0,'features','_*20',1,'Noop','Not implemented undo all','fwdundoall');
INSERT INTO extensions VALUES(4,0,'features','_*21',1,'Macro','features|FWD/Unc/Status|forward-on|forward-off|1','fwdundounc');
INSERT INTO extensions VALUES(5,0,'features','_*22',1,'Macro','features|FWD/RNA/Status|forward-on|forward-off|1','fwdundorna');
INSERT INTO extensions VALUES(6,0,'features','_*23',1,'Macro','features|FWD/Busy/Status|forward-on|forward-off|1','fwdundobusy');
INSERT INTO extensions VALUES(7,0,'features','_*21.',1,'Answer','','fwdunc');
INSERT INTO extensions VALUES(8,0,'features','_*21.',2,'Wait',0.5,'fwdunc');
INSERT INTO extensions VALUES(9,0,'features','_*21.',3,'Set','USER=${CALLERID(num)}','fwdunc');
INSERT INTO extensions VALUES(10,0,'features','_*21.',4,'Set','DB(users/${USER}/FWD/Unc/Number)=${EXTEN:3}','fwdunc');
INSERT INTO extensions VALUES(11,0,'features','_*21.',5,'Set','DB(users/${USER}/FWD/Unc/Status)=1','fwdunc');
INSERT INTO extensions VALUES(12,0,'features','_*21.',6,'Playback','forward-on','fwdunc');
INSERT INTO extensions VALUES(13,0,'features','_*21.',7,'Hangup','','fwdunc');
INSERT INTO extensions VALUES(14,0,'features','_*22.',1,'Answer','','fwdrna');
INSERT INTO extensions VALUES(15,0,'features','_*22.',2,'Wait',0.5,'fwdrna');
INSERT INTO extensions VALUES(16,0,'features','_*22.',3,'Set','USER=${CALLERID(num)}','fwdrna');
INSERT INTO extensions VALUES(17,0,'features','_*22.',4,'Set','DB(users/${USER}/FWD/RNA/Number)=${EXTEN:3}','fwdrna');
INSERT INTO extensions VALUES(18,0,'features','_*22.',5,'Set','DB(users/${USER}/FWD/RNA/Status)=1','fwdrna');
INSERT INTO extensions VALUES(19,0,'features','_*22.',6,'Playback','forward-on','fwdrna');
INSERT INTO extensions VALUES(20,0,'features','_*22.',7,'Hangup','','fwdrna');
INSERT INTO extensions VALUES(21,0,'features','_*23.',1,'Answer','','fwdbusy');
INSERT INTO extensions VALUES(22,0,'features','_*23.',2,'Wait',0.5,'fwdbusy');
INSERT INTO extensions VALUES(23,0,'features','_*23.',3,'Set','USER=${CALLERID(num)}','fwdbusy');
INSERT INTO extensions VALUES(24,0,'features','_*23.',4,'Set','DB(users/${USER}/FWD/Busy/Number)=${EXTEN:3}','fwdbusy');
INSERT INTO extensions VALUES(25,0,'features','_*23.',5,'Set','DB(users/${USER}/FWD/Busy/Status)=1','fwdbusy');
INSERT INTO extensions VALUES(26,0,'features','_*23.',6,'Playback','forward-on','fwdbusy');
INSERT INTO extensions VALUES(27,0,'features','_*23.',7,'Hangup','','fwdbusy');
INSERT INTO extensions VALUES(28,0,'features','_*9',1,'Wait',1,'recsnd');
INSERT INTO extensions VALUES(29,0,'features','_*9',2,'Set','ROOT=/usr/share/asterisk/sounds/web-interface/recordings','recsnd');
INSERT INTO extensions VALUES(30,0,'features','_*9',3,'Set','FILE=${CALLERID(num)}-${EPOCH}','recsnd');
INSERT INTO extensions VALUES(31,0,'features','_*9',4,'Record','${ROOT}/${FILE}:wav','recsnd');
INSERT INTO extensions VALUES(32,0,'features','_*9',5,'Wait',1,'recsnd');
INSERT INTO extensions VALUES(33,0,'features','_*9',6,'Playback','${ROOT}/${FILE}','recsnd');
INSERT INTO extensions VALUES(34,0,'features','_*9',7,'Hangup','','recsnd');
INSERT INTO extensions VALUES(35,0,'features','_*24',1,'Macro','features|VM|vm-on|vm-off','enablevm');
INSERT INTO extensions VALUES(36,0,'features','_*25',1,'Macro','features|DND|dnd-on|dnd-off','enablednd');
INSERT INTO extensions VALUES(37,0,'features','_*26',1,'Macro','features|Record|to-record-call&on|to-record-call&off-duty','incallrec');
INSERT INTO extensions VALUES(38,0,'features','_*27',1,'Macro','features|Screen|screen-callee-options&on|screen-callee-options&off-duty','incallfilter');
INSERT INTO extensions VALUES(39,0,'features','_*8.',1,'Pickup','${EXTEN:2}','pickup');


CREATE TABLE generalsip (
 `id` int(11) unsigned auto_increment,
 `cat_metric` int(11) unsigned default 0,
 `var_metric` int(11) unsigned default 0,
 `commented` tinyint(1) default 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(128) NOT NULL,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX generalsip__idx__commented ON generalsip(commented);
CREATE INDEX generalsip__idx__filename ON generalsip(filename);
CREATE INDEX generalsip__idx__category ON generalsip(category);
CREATE INDEX generalsip__idx__var_name ON generalsip(var_name);

INSERT INTO generalsip VALUES(1,0,0,0,'sip.conf','general','bindport',5060);
INSERT INTO generalsip VALUES(2,0,0,0,'sip.conf','general','bindaddr','0.0.0.0');
INSERT INTO generalsip VALUES(3,0,0,0,'sip.conf','general','srvlookup','no');
INSERT INTO generalsip VALUES(4,0,0,0,'sip.conf','general','language','fr');
INSERT INTO generalsip VALUES(5,0,0,0,'sip.conf','general','maxexpiry',3600);
INSERT INTO generalsip VALUES(6,0,0,0,'sip.conf','general','defaultexpiry',120);
INSERT INTO generalsip VALUES(7,0,0,0,'sip.conf','general','useragent','Xivo PBX');
INSERT INTO generalsip VALUES(8,0,0,0,'sip.conf','general','nat','yes');
INSERT INTO generalsip VALUES(9,0,0,0,'sip.conf','general','qualify','yes');
INSERT INTO generalsip VALUES(10,0,0,0,'sip.conf','general','rtcachefriends','yes');
INSERT INTO generalsip VALUES(11,0,0,0,'sip.conf','general','allowguest','yes');
INSERT INTO generalsip VALUES(12,0,0,0,'sip.conf','general','tos','lowdelay');
INSERT INTO generalsip VALUES(13,0,0,0,'sip.conf','general','relaxdtmf','yes');
INSERT INTO generalsip VALUES(14,0,0,0,'sip.conf','general','context','from-sip');
INSERT INTO generalsip VALUES(15,0,0,0,'sip.conf','general','checkmwi',10);
INSERT INTO generalsip VALUES(16,0,0,0,'sip.conf','general','vmexten','*98');
INSERT INTO generalsip VALUES(17,0,0,0,'sip.conf','general','videosupport','no');


CREATE TABLE generaliax (
 `id` int(11) unsigned auto_increment,
 `cat_metric` int(11) unsigned default 0,
 `var_metric` int(11) unsigned default 0,
 `commented` tinyint(1) default 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(128) NOT NULL,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX generaliax__idx__commented ON generaliax(commented);
CREATE INDEX generaliax__idx__filename ON generaliax(filename);
CREATE INDEX generaliax__idx__category ON generaliax(category);
CREATE INDEX generaliax__idx__var_name ON generaliax(var_name);

INSERT INTO generaliax VALUES(1,0,0,0,'iax.conf','general','bindport',4569);
INSERT INTO generaliax VALUES(2,0,0,0,'iax.conf','general','bindaddr','0.0.0.0');
INSERT INTO generaliax VALUES(3,0,0,0,'iax.conf','general','delayreject','no');
INSERT INTO generaliax VALUES(4,0,0,0,'iax.conf','general','language','en');
INSERT INTO generaliax VALUES(5,0,0,0,'iax.conf','general','tos','lowdelay');
INSERT INTO generaliax VALUES(6,0,0,0,'iax.conf','general','qualify','yes');
INSERT INTO generaliax VALUES(7,0,0,0,'iax.conf','general','rtcachefriends','yes');
INSERT INTO generaliax VALUES(8,0,0,0,'iax.conf','general','jitterbuffer','no');


CREATE TABLE generalvoicemail (
 `id` int(11) unsigned auto_increment,
 `cat_metric` int(11) unsigned default 0,
 `var_metric` int(11) unsigned default 0,
 `commented` tinyint(1) default 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` text NOT NULL,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX generalvoicemail__idx__commented ON generalvoicemail(commented);
CREATE INDEX generalvoicemail__idx__filename ON generalvoicemail(filename);
CREATE INDEX generalvoicemail__idx__category ON generalvoicemail(category);
CREATE INDEX generalvoicemail__idx__var_name ON generalvoicemail(var_name);

INSERT INTO generalvoicemail VALUES(1,0,0,0,'voicemail.conf','general','maxmessage',180);
INSERT INTO generalvoicemail VALUES(2,0,0,0,'voicemail.conf','general','minmessage',5);
INSERT INTO generalvoicemail VALUES(3,0,0,0,'voicemail.conf','general','maxsilence',1);
INSERT INTO generalvoicemail VALUES(4,0,0,0,'voicemail.conf','general','review','yes');
INSERT INTO generalvoicemail VALUES(5,0,0,0,'voicemail.conf','general','serveremail','voicemail@xivo');
INSERT INTO generalvoicemail VALUES(6,0,0,0,'voicemail.conf','general','fromstring','XIVO PBX');
INSERT INTO generalvoicemail VALUES(7,0,0,0,'voicemail.conf','general','maxmsg',100);
INSERT INTO generalvoicemail VALUES(8,0,0,0,'voicemail.conf','general','emailsubject','Messagerie XIVO');
INSERT INTO generalvoicemail VALUES(9,0,0,0,'voicemail.conf','general','emailbody','Bonjour ${VM_NAME} !

Vous avez reçu un message d''une durée de ${VM_DUR} minutes, il vous reste actuellement ${VM_MSGNUM} message(s) non lu sur votre messagerie vocale : ${VM_MAILBOX}.
Le dernier a été envoyé par ${VM_CALLERID}, le ${VM_DATE}. Si vous le souhaitez vous pouvez l''écouter ou le consulter en tapant le *98 sur votre téléphone. Merci !

-- Messagerie XIVO --');
INSERT INTO generalvoicemail VALUES(10,0,0,0,'voicemail.conf','general','charset','UTF-8');
INSERT INTO generalvoicemail VALUES(11,0,0,0,'voicemail.conf','zonemessages','eu-fr','Europe/Paris|''vm-received'' q ''digits/at'' kM');
INSERT INTO generalvoicemail VALUES(12,0,0,0,'voicemail.conf','general','tz','eu-fr');


CREATE TABLE generalqueue (
 `id` int(11) unsigned auto_increment,
 `cat_metric` int(11) unsigned default 0,
 `var_metric` int(11) unsigned default 0,
 `commented` tinyint(1) default 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(128) NOT NULL,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX generalqueue__idx__commented ON generalqueue(commented);
CREATE INDEX generalqueue__idx__filename ON generalqueue(filename);
CREATE INDEX generalqueue__idx__category ON generalqueue(category);
CREATE INDEX generalqueue__idx__var_name ON generalqueue(var_name);

INSERT INTO generalqueue VALUES(1,0,0,0,'queues.conf','general','persistentmembers','yes');


CREATE TABLE usersip (
 `id` int(11) unsigned auto_increment,
 `name` varchar(80) NOT NULL,
 `commented` tinyint(1) NOT NULL default 0,
 `accountcode` varchar(20),
 `amaflags` varchar(13),
 `callgroup` varchar(10),
 `callerid` varchar(80),
 `canreinvite` char(3),
 `context` varchar(80),
 `defaultip` varchar(15),
 `dtmfmode` varchar(7),
 `fromuser` varchar(80),
 `fromdomain` varchar(80),
 `fullcontact` varchar(80),
 `host` varchar(31) NOT NULL,
 `insecure` varchar(11),
 `language` char(2),
 `mailbox` varchar(50),
 `md5secret` varchar(80),
 `nat` varchar(5) NOT NULL default 'no',
 `deny` varchar(95),
 `permit` varchar(95),
 `mask` varchar(95),
 `pickupgroup` varchar(10),
 `port` varchar(5) NOT NULL,
 `qualify` char(3),
 `restrictcid` char(1),
 `rtptimeout` char(3),
 `rtpholdtimeout` char(3),
 `secret` varchar(80),
 `type` varchar(6) NOT NULL default 'friend',
 `username` varchar(80) NOT NULL,
 `disallow` varchar(100),
 `allow` varchar(100),
 `musiconhold` varchar(100),
 `regseconds` int(11) unsigned NOT NULL default 0,
 `ipaddr` varchar(15) NOT NULL,
 `regexten` varchar(80) NOT NULL,
 `cancallforward` char(3),
 `setvar` varchar(100) NOT NULL,
 `call-limit` tinyint unsigned default 0,
 `category` varchar(50) NOT NULL,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX usersip__idx__commented ON usersip(commented);
CREATE INDEX usersip__idx__category ON usersip(category);
CREATE UNIQUE INDEX usersip__uidx__name ON usersip(name);

INSERT INTO usersip VALUES(1,'guest',0,'','documentation','','Guest','no','initconfig',NULL,'rfc2833',NULL,NULL,'','dynamic',NULL,NULL,'',NULL,'no',NULL,NULL,NULL,'',5060,'no',NULL,NULL,NULL,'guest','friend','guest',NULL,NULL,NULL,'','','',NULL,'',0,'user');


CREATE TABLE musiconhold (
 `id` int(11) unsigned auto_increment,
 `cat_metric` int(11) unsigned default 0,
 `var_metric` int(11) unsigned default 0,
 `commented` tinyint(1) default 0,
 `filename` varchar(128) NOT NULL,
 `category` varchar(128) NOT NULL,
 `var_name` varchar(128) NOT NULL,
 `var_val` varchar(128) NOT NULL,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX musiconhold__idx__commented ON musiconhold(commented);
CREATE UNIQUE INDEX musiconhold__uidx__filename_category_var_name ON musiconhold(filename(48),category,var_name);

INSERT INTO musiconhold VALUES(1,0,0,0,'musiconhold.conf','default','mode','custom');
INSERT INTO musiconhold VALUES(2,0,0,0,'musiconhold.conf','default','application','/usr/bin/madplay --mono -a -10 -R 8000 --output=raw:-');
INSERT INTO musiconhold VALUES(3,0,0,0,'musiconhold.conf','default','random','no');
INSERT INTO musiconhold VALUES(4,0,0,0,'musiconhold.conf','default','directory','/usr/share/asterisk/moh/default');
