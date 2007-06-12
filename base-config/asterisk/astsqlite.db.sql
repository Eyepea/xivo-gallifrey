BEGIN TRANSACTION;

DROP TABLE cdr;
CREATE TABLE cdr (
 id integer unsigned auto_increment,
 calldate char(19) DEFAULT '0000-00-00 00:00:00',
 clid varchar(80) NOT NULL DEFAULT '',
 src varchar(80) NOT NULL DEFAULT '',
 dst varchar(80) NOT NULL DEFAULT '',
 dcontext varchar(80) NOT NULL DEFAULT '',
 channel varchar(80) NOT NULL DEFAULT '',
 dstchannel varchar(80) NOT NULL DEFAULT '',
 lastapp varchar(80) NOT NULL DEFAULT '',
 lastdata varchar(80) NOT NULL DEFAULT '',
 answer char(19) DEFAULT '0000-00-00 00:00:00',
 end char(19) DEFAULT '0000-00-00 00:00:00',
 duration integer unsigned NOT NULL DEFAULT 0,
 billsec integer unsigned NOT NULL DEFAULT 0,
 disposition varchar(9) NOT NULL DEFAULT '',
 amaflags tinyint unsigned NOT NULL DEFAULT 0,
 accountcode varchar(20) NOT NULL DEFAULT '',
 uniqueid varchar(32) NOT NULL DEFAULT '',
 userfield varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY(id)
);

CREATE INDEX cdr__idx__calldate ON cdr(calldate);
CREATE INDEX cdr__idx__src ON cdr(src);
CREATE INDEX cdr__idx__dst ON cdr(dst);
CREATE INDEX cdr__idx__disposition ON cdr(disposition);


DROP TABLE didfeatures;
CREATE TABLE didfeatures (
 id integer unsigned,
 type varchar(50) NOT NULL,
 typeid integer unsigned NOT NULL,
 extenid integer unsigned NOT NULL,
 custom varchar(128) NOT NULL DEFAULT '',
 number varchar(80) NOT NULL,
 commented tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE INDEX didfeatures__idx__type_typeid ON didfeatures(type,typeid);
CREATE INDEX didfeatures__idx__commented ON didfeatures(commented);
CREATE UNIQUE INDEX didfeatures__uidx__extenid ON didfeatures(extenid);


DROP TABLE extenumbers;
CREATE TABLE extenumbers (
 id integer unsigned,
 number varchar(80) NOT NULL,
 context varchar(80) NOT NULL,
 PRIMARY KEY(id)
);

CREATE UNIQUE INDEX extenumbers__uidx__number_context ON extenumbers(number,context);


DROP TABLE groupfeatures;
CREATE TABLE groupfeatures (
 id tinyint unsigned,
 name varchar(255) NOT NULL,
 number varchar(80) DEFAULT '',
 context varchar(80),
 commented tinyint(1) DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE UNIQUE INDEX groupfeatures__uidx__name_commented ON groupfeatures(name,commented);


DROP TABLE meetme;
CREATE TABLE meetme (
 id integer unsigned,
 commented tinyint(1) DEFAULT 0,
 filename varchar(128) NOT NULL,
 category varchar(128) NOT NULL,
 var_name varchar(128) NOT NULL,
 var_val varchar(128),
 PRIMARY KEY(id)
);

CREATE INDEX meetme__idx__commented ON meetme(commented);
CREATE INDEX meetme__idx__filename ON meetme(filename);
CREATE INDEX meetme__idx__category ON meetme(category);
CREATE INDEX meetme__idx__var_name ON meetme(var_name);


DROP TABLE meetmefeatures;
CREATE TABLE meetmefeatures (
 id integer unsigned,
 name varchar(128) NOT NULL,
 number varchar(128) NOT NULL,
 meetmeid integer unsigned NOT NULL,
 mode varchar(6) NOT NULL DEFAULT 'all',
 musiconhold varchar(128) NOT NULL DEFAULT '',
 context varchar(80),
 exit tinyint(1) DEFAULT 0,
 quiet tinyint(1) DEFAULT 0,
 record tinyint(1) DEFAULT 0,
 video tinyint(1) DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE UNIQUE INDEX meetmefeatures__uidx__meetmeid ON meetmefeatures(meetmeid);
CREATE UNIQUE INDEX meetmefeatures__uidx__name ON meetmefeatures(name);
CREATE UNIQUE INDEX meetmefeatures__uidx__number ON meetmefeatures(number);


DROP TABLE phone;
CREATE TABLE phone (
 macaddr char(17) NOT NULL,
 vendor varchar(16) NOT NULL,
 model varchar(16) NOT NULL,
 proto varchar(50) NOT NULL,
 iduserfeatures integer unsigned,
 PRIMARY KEY(macaddr)
);

CREATE INDEX phone__idx__proto_iduserfeatures ON phone(proto,iduserfeatures);

DROP TABLE queue;
CREATE TABLE queue (
 name varchar(128) NOT NULL,
 musiconhold varchar(128),
 announce varchar(128),
 context varchar(80),
 timeout tinyint unsigned,
 'monitor-join' tinyint(1) DEFAULT 0,
 'monitor-format' varchar(128),
 'queue-youarenext' varchar(128),
 'queue-thereare' varchar(128),
 'queue-callswaiting' varchar(128),
 'queue-holdtime' varchar(128),
 'queue-minutes' varchar(128),
 'queue-seconds' varchar(128),
 'queue-lessthan' varchar(128),
 'queue-thankyou' varchar(128),
 'queue-reporthold' varchar(128),
 'periodic-announce' varchar(128),
 'announce-frequency' integer unsigned,
 'periodic-announce-frequency' integer unsigned,
 'announce-round-seconds' tinyint unsigned,
 'announce-holdtime' varchar(4),
 retry tinyint unsigned,
 wrapuptime tinyint unsigned,
 maxlen integer unsigned,
 servicelevel integer,
 strategy varchar(11),
 joinempty varchar(6),
 leavewhenempty varchar(6),
 eventmemberstatus tinyint(1) DEFAULT 0,
 eventwhencalled tinyint(1) DEFAULT 0,
 reportholdtime tinyint(1) DEFAULT 0,
 memberdelay integer unsigned,
 weight integer unsigned,
 timeoutrestart tinyint(1) DEFAULT 0,
 commented tinyint(1) DEFAULT 0,
 category char(5) NOT NULL,
 PRIMARY KEY(name)
);

CREATE INDEX queue__idx__commented ON queue(commented);
CREATE INDEX queue__idx__category ON queue(category);


DROP TABLE queuefeatures;
CREATE TABLE queuefeatures (
 id integer unsigned auto_increment,
 name varchar(255) NOT NULL,
 number varchar(80) DEFAULT '',
 context varchar(80),
 data_quality tinyint(1) DEFAULT 0,
 hitting_callee tinyint(1) DEFAULT 0,
 hitting_caller tinyint(1) DEFAULT 0,
 retries tinyint(1) DEFAULT 0,
 ring tinyint(1) DEFAULT 0,
 transfer_user tinyint(1) DEFAULT 0,
 transfer_call tinyint(1) DEFAULT 0,
 write_caller tinyint(1) DEFAULT 0,
 write_calling tinyint(1) DEFAULT 0,
 url varchar(255) DEFAULT '',
 announceoverride varchar(128) DEFAULT '',
 timeout tinyint unsigned,
 PRIMARY KEY(id)
);

CREATE UNIQUE INDEX queuefeatures__uidx__name ON queuefeatures(name);


DROP TABLE queuemember;
CREATE TABLE queuemember (
 queue_name varchar(128) NOT NULL,
 interface varchar(128) NOT NULL,
 penalty integer unsigned DEFAULT 0,
 'call-limit' integer unsigned DEFAULT 0,
 commented tinyint(1) DEFAULT 0,
 PRIMARY KEY(queue_name,interface)
);

CREATE INDEX queuemember__idx__commented ON queuemember(commented);


DROP TABLE trunkfeatures;
CREATE TABLE trunkfeatures (
 id integer unsigned,
 trunk varchar(50) NOT NULL,
 trunkid integer unsigned NOT NULL,
 registerid integer unsigned DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE INDEX trunkfeatures__idx__registerid ON trunkfeatures(registerid);
CREATE UNIQUE INDEX trunkfeatures__uidx__trunk_trunkid ON trunkfeatures(trunk,trunkid);


DROP TABLE usergroup;
CREATE TABLE usergroup (
 id integer unsigned,
 userid integer unsigned NOT NULL,
 groupid tinyint unsigned NOT NULL,
 PRIMARY KEY(id)
);

CREATE UNIQUE INDEX usergroup__uidx__userid_groupid ON usergroup(userid,groupid);


DROP TABLE useriax;
CREATE TABLE useriax (
 id integer unsigned,
 name varchar(80) NOT NULL,
 commented tinyint(1) NOT NULL DEFAULT 0,
 username varchar(80) NOT NULL,
 type varchar(6) NOT NULL DEFAULT 'friend',
 secret varchar(80),
 md5secret varchar(32),
 dbsecret varchar(100),
 notransfer char(3),
 trunk char(3),
 inkeys varchar(100),
 outkey varchar(100),
 auth varchar(100),
 accountcode varchar(100),
 amaflags varchar(13),
 callerid varchar(80),
 callgroup varchar(10),
 context varchar(80),
 defaultip varchar(15),
 host varchar(31) NOT NULL DEFAULT 'dynamic',
 language char(2),
 mailbox varchar(50),
 deny varchar(95),
 permit varchar(95),
 qualify char(3),
 disallow varchar(100),
 allow varchar(100),
 ipaddr varchar(15) NOT NULL,
 port smallint unsigned NOT NULL,
 regseconds integer unsigned DEFAULT 0,
 'call-limit' tinyint unsigned DEFAULT 0,
 category varchar(50) NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX useriax__idx__commented ON useriax(commented);
CREATE INDEX useriax__idx__category ON useriax(category);
CREATE UNIQUE INDEX useriax__uidx__name ON useriax(name);


DROP TABLE uservoicemail;
CREATE TABLE uservoicemail (
 id integer unsigned,
 customer_id varchar(11) NOT NULL DEFAULT '0',
 context varchar(80) NOT NULL DEFAULT '',
 mailbox varchar(11) NOT NULL DEFAULT '0',
 password varchar(5) NOT NULL DEFAULT '0',
 fullname varchar(150) NOT NULL DEFAULT '',
 email varchar(50) NOT NULL DEFAULT '',
 pager varchar(50) NOT NULL DEFAULT '',
 tz varchar(10) NOT NULL DEFAULT 'central',
 attach varchar(4) NOT NULL DEFAULT 'yes',
 saycid varchar(4) NOT NULL DEFAULT 'yes',
 dialout varchar(10) NOT NULL DEFAULT '',
 callback varchar(10) NOT NULL DEFAULT '',
 review varchar(4) NOT NULL DEFAULT 'no',
 operator varchar(4) NOT NULL DEFAULT 'no',
 envelope varchar(4) NOT NULL DEFAULT 'no',
 sayduration varchar(4) NOT NULL DEFAULT 'no',
 saydurationm tinyint unsigned NOT NULL DEFAULT 2,
 sendvoicemail varchar(4) NOT NULL DEFAULT 'no',
 'delete' varchar(4) NOT NULL DEFAULT 'no',
 nextaftercmd varchar(5) NOT NULL DEFAULT 'yes',
 forcename varchar(4) NOT NULL DEFAULT 'no',
 forcegreetings varchar(4) NOT NULL DEFAULT 'no',
 hidefromdir varchar(4) NOT NULL DEFAULT 'yes',
 commented tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE INDEX uservoicemail__idx__commented ON uservoicemail(commented);
CREATE INDEX uservoicemail__idx__context ON uservoicemail(context);
CREATE UNIQUE INDEX uservoicemail__uidx__mailbox_context ON uservoicemail(mailbox,context);


DROP TABLE userfeatures;
CREATE TABLE userfeatures (
 id integer unsigned,
 protocol varchar(50) NOT NULL,
 protocolid integer unsigned NOT NULL,
 firstname varchar(128) NOT NULL DEFAULT '',
 lastname varchar(128) NOT NULL DEFAULT '',
 name varchar(80) NOT NULL,
 number varchar(80) NOT NULL,
 context varchar(80),
 provisioningid mediumint unsigned NOT NULL,
 ringseconds tinyint unsigned DEFAULT 15,
 ringgroup tinyint(1) DEFAULT 0,
 simultcalls tinyint unsigned DEFAULT 1,
 popupwidget tinyint(1) DEFAULT 0,
 musiconhold varchar(128) NOT NULL DEFAULT '',
 comment text DEFAULT '',
 PRIMARY KEY(id)
);

CREATE INDEX userfeatures__idx__firstname ON userfeatures(firstname);
CREATE INDEX userfeatures__idx__lastname ON userfeatures(lastname);
CREATE INDEX userfeatures__idx__number ON userfeatures(number);
CREATE INDEX userfeatures__idx__context ON userfeatures(context);
CREATE INDEX userfeatures__idx__musiconhold ON userfeatures(musiconhold);
CREATE UNIQUE INDEX userfeatures__uidx__protocol_name ON userfeatures(protocol,name);
CREATE UNIQUE INDEX userfeatures__uidx__protocol_protocolid ON userfeatures(protocol,protocolid);
CREATE UNIQUE INDEX userfeatures__uidx__provisioningid ON userfeatures(provisioningid);

INSERT INTO userfeatures VALUES(1,'sip',1,'Guest','','guest','','initconfig',148378,30,0,5,0,'','');
INSERT INTO userfeatures VALUES(2,'sip',2,'XivoSB','','xivosb','','local-extensions',194867,30,0,5,0,'','');


DROP TABLE extensions;
CREATE TABLE extensions (
 id integer unsigned,
 commented tinyint(1) NOT NULL DEFAULT 0,
 context varchar(20) NOT NULL DEFAULT '',
 exten varchar(20) NOT NULL DEFAULT '',
 priority tinyint unsigned NOT NULL DEFAULT 0,
 app varchar(20) NOT NULL DEFAULT '',
 appdata varchar(128) NOT NULL DEFAULT '',
 name varchar(128) DEFAULT '',
 PRIMARY KEY(id)
);

CREATE INDEX extensions__idx__commented ON extensions(commented);
CREATE INDEX extensions__idx__context_exten_priority ON extensions(context,exten,priority);
CREATE INDEX extensions__idx__name ON extensions(name);

INSERT INTO extensions VALUES(1,0,'features','_*98',1,'VoiceMailMain','${CALLERID(num)}@${CONTEXT}|s','voicemsg');
INSERT INTO extensions VALUES(2,0,'features','_*98',2,'Hangup','','voicemsg');
INSERT INTO extensions VALUES(3,0,'features','_*20',1,'Macro','features|FWD/Unc/Status|||1','fwdundoall');
INSERT INTO extensions VALUES(4,0,'features','_*20',2,'Macro','features|FWD/RNA/Status|||1','fwdundoall');
INSERT INTO extensions VALUES(5,0,'features','_*20',3,'Macro','features|FWD/Busy/Status|all-forward-off|all-forward-off|1','fwdundoall');
INSERT INTO extensions VALUES(6,0,'features','_*21',1,'Macro','features|FWD/Unc/Status|forward-on|forward-off|1','fwdundounc');
INSERT INTO extensions VALUES(7,0,'features','_*22',1,'Macro','features|FWD/RNA/Status|forward-on|forward-off|1','fwdundorna');
INSERT INTO extensions VALUES(8,0,'features','_*23',1,'Macro','features|FWD/Busy/Status|forward-on|forward-off|1','fwdundobusy');
INSERT INTO extensions VALUES(9,0,'features','_*21.',1,'Answer','','fwdunc');
INSERT INTO extensions VALUES(10,0,'features','_*21.',2,'Wait',0.5,'fwdunc');
INSERT INTO extensions VALUES(11,0,'features','_*21.',3,'Set','USER=${CALLERID(num)}','fwdunc');
INSERT INTO extensions VALUES(12,0,'features','_*21.',4,'Set','DB(${CONTEXT}/users/${USER}/FWD/Unc/Number)=${EXTEN:3}','fwdunc');
INSERT INTO extensions VALUES(13,0,'features','_*21.',5,'Set','DB(${CONTEXT}/users/${USER}/FWD/Unc/Status)=1','fwdunc');
INSERT INTO extensions VALUES(14,0,'features','_*21.',6,'Playback','forward-on','fwdunc');
INSERT INTO extensions VALUES(15,0,'features','_*21.',7,'Hangup','','fwdunc');
INSERT INTO extensions VALUES(16,0,'features','_*22.',1,'Answer','','fwdrna');
INSERT INTO extensions VALUES(17,0,'features','_*22.',2,'Wait',0.5,'fwdrna');
INSERT INTO extensions VALUES(18,0,'features','_*22.',3,'Set','USER=${CALLERID(num)}','fwdrna');
INSERT INTO extensions VALUES(19,0,'features','_*22.',4,'Set','DB(${CONTEXT}/users/${USER}/FWD/RNA/Number)=${EXTEN:3}','fwdrna');
INSERT INTO extensions VALUES(20,0,'features','_*22.',5,'Set','DB(${CONTEXT}/users/${USER}/FWD/RNA/Status)=1','fwdrna');
INSERT INTO extensions VALUES(21,0,'features','_*22.',6,'Playback','forward-on','fwdrna');
INSERT INTO extensions VALUES(22,0,'features','_*22.',7,'Hangup','','fwdrna');
INSERT INTO extensions VALUES(23,0,'features','_*23.',1,'Answer','','fwdbusy');
INSERT INTO extensions VALUES(24,0,'features','_*23.',2,'Wait',0.5,'fwdbusy');
INSERT INTO extensions VALUES(25,0,'features','_*23.',3,'Set','USER=${CALLERID(num)}','fwdbusy');
INSERT INTO extensions VALUES(26,0,'features','_*23.',4,'Set','DB(${CONTEXT}/users/${USER}/FWD/Busy/Number)=${EXTEN:3}','fwdbusy');
INSERT INTO extensions VALUES(27,0,'features','_*23.',5,'Set','DB(${CONTEXT}/users/${USER}/FWD/Busy/Status)=1','fwdbusy');
INSERT INTO extensions VALUES(28,0,'features','_*23.',6,'Playback','forward-on','fwdbusy');
INSERT INTO extensions VALUES(29,0,'features','_*23.',7,'Hangup','','fwdbusy');
INSERT INTO extensions VALUES(30,0,'features','_*9',1,'Wait',1,'recsnd');
INSERT INTO extensions VALUES(31,0,'features','_*9',2,'Set','ROOT=/usr/share/asterisk/sounds/web-interface/recordings','recsnd');
INSERT INTO extensions VALUES(32,0,'features','_*9',3,'Set','FILE=${CALLERID(num)}-${EPOCH}','recsnd');
INSERT INTO extensions VALUES(33,0,'features','_*9',4,'Record','${ROOT}/${FILE}:wav','recsnd');
INSERT INTO extensions VALUES(34,0,'features','_*9',5,'Wait',1,'recsnd');
INSERT INTO extensions VALUES(35,0,'features','_*9',6,'Playback','${ROOT}/${FILE}','recsnd');
INSERT INTO extensions VALUES(36,0,'features','_*9',7,'Hangup','','recsnd');
INSERT INTO extensions VALUES(37,0,'features','_*10',1,'Answer','','phonestatus');
INSERT INTO extensions VALUES(38,0,'features','_*10',2,'Playback','status-phone','phonestatus');
INSERT INTO extensions VALUES(39,0,'features','_*10',3,'Playback','forward-inc','phonestatus');
INSERT INTO extensions VALUES(40,0,'features','_*10',4,'Macro','checkdbfeatures|FWD/Unc/Status|1|FWD/Unc/Number','phonestatus');
INSERT INTO extensions VALUES(41,0,'features','_*10',5,'Playback','forward-busy','phonestatus');
INSERT INTO extensions VALUES(42,0,'features','_*10',6,'Macro','checkdbfeatures|FWD/Busy/Status|1|FWD/Busy/Number','phonestatus');
INSERT INTO extensions VALUES(43,0,'features','_*10',7,'Playback','forward-rna','phonestatus');
INSERT INTO extensions VALUES(44,0,'features','_*10',8,'Macro','checkdbfeatures|FWD/RNA/Status|1|FWD/RNA/Number','phonestatus');
INSERT INTO extensions VALUES(45,0,'features','_*10',9,'Playback','vm-status','phonestatus');
INSERT INTO extensions VALUES(46,0,'features','_*10',10,'Macro','checkdbfeatures|VM','phonestatus');
INSERT INTO extensions VALUES(47,0,'features','_*10',11,'Playback','screening-status','phonestatus');
INSERT INTO extensions VALUES(48,0,'features','_*10',12,'Macro','checkdbfeatures|Screen','phonestatus');
INSERT INTO extensions VALUES(49,0,'features','_*10',13,'Playback','record-status','phonestatus');
INSERT INTO extensions VALUES(50,0,'features','_*10',14,'Macro','checkdbfeatures|Record','phonestatus');
INSERT INTO extensions VALUES(51,0,'features','_*10',15,'Playback','dnd-status','phonestatus');
INSERT INTO extensions VALUES(52,0,'features','_*10',16,'Macro','checkdbfeatures|DND','phonestatus');
INSERT INTO extensions VALUES(53,0,'features','_*10',17,'Playback','bye','phonestatus');
INSERT INTO extensions VALUES(54,0,'features','_*10',18,'Hangup','','phonestatus');
INSERT INTO extensions VALUES(55,0,'features','_*24',1,'Macro','features|VM|vm-on|vm-off','enablevm');
INSERT INTO extensions VALUES(56,0,'features','_*25',1,'Macro','features|DND|dnd-on|dnd-off','enablednd');
INSERT INTO extensions VALUES(57,0,'features','_*26',1,'Macro','features|Record|record-call-on|record-call-off','incallrec');
INSERT INTO extensions VALUES(58,0,'features','_*27',1,'Macro','features|Screen|screening-on|screening-off','incallfilter');
INSERT INTO extensions VALUES(59,0,'features','_*8.',1,'Pickup','${EXTEN:2}','pickup');


DROP TABLE generalsip;
CREATE TABLE generalsip (
 id integer unsigned,
 commented tinyint(1) DEFAULT 0,
 filename varchar(128) NOT NULL,
 category varchar(128) NOT NULL,
 var_name varchar(128) NOT NULL,
 var_val varchar(128),
 PRIMARY KEY(id)
);

CREATE INDEX generalsip__idx__commented ON generalsip(commented);
CREATE INDEX generalsip__idx__filename ON generalsip(filename);
CREATE INDEX generalsip__idx__category ON generalsip(category);
CREATE INDEX generalsip__idx__var_name ON generalsip(var_name);

INSERT INTO generalsip VALUES(1,0,'sip.conf','general','bindport',5060);
INSERT INTO generalsip VALUES(2,0,'sip.conf','general','bindaddr','0.0.0.0');
INSERT INTO generalsip VALUES(3,0,'sip.conf','general','srvlookup','no');
INSERT INTO generalsip VALUES(4,0,'sip.conf','general','language','fr');
INSERT INTO generalsip VALUES(5,0,'sip.conf','general','maxexpiry',3600);
INSERT INTO generalsip VALUES(6,0,'sip.conf','general','defaultexpiry',120);
INSERT INTO generalsip VALUES(7,0,'sip.conf','general','useragent','Xivo PBX');
INSERT INTO generalsip VALUES(8,0,'sip.conf','general','nat','yes');
INSERT INTO generalsip VALUES(9,0,'sip.conf','general','qualify','yes');
INSERT INTO generalsip VALUES(10,0,'sip.conf','general','rtcachefriends','yes');
INSERT INTO generalsip VALUES(11,0,'sip.conf','general','allowguest','yes');
INSERT INTO generalsip VALUES(12,0,'sip.conf','general','tos','lowdelay');
INSERT INTO generalsip VALUES(13,0,'sip.conf','general','relaxdtmf','yes');
INSERT INTO generalsip VALUES(14,0,'sip.conf','general','context','from-sip');
INSERT INTO generalsip VALUES(15,0,'sip.conf','general','checkmwi',10);
INSERT INTO generalsip VALUES(16,0,'sip.conf','general','vmexten','*98');
INSERT INTO generalsip VALUES(17,0,'sip.conf','general','videosupport','no');


DROP TABLE generaliax;
CREATE TABLE generaliax (
 id integer unsigned,
 commented tinyint(1) DEFAULT 0,
 filename varchar(128) NOT NULL,
 category varchar(128) NOT NULL,
 var_name varchar(128) NOT NULL,
 var_val varchar(128),
 PRIMARY KEY(id)
);

CREATE INDEX generaliax__idx__commented ON generaliax(commented);
CREATE INDEX generaliax__idx__filename ON generaliax(filename);
CREATE INDEX generaliax__idx__category ON generaliax(category);
CREATE INDEX generaliax__idx__var_name ON generaliax(var_name);

INSERT INTO generaliax VALUES(1,0,'iax.conf','general','bindport',4569);
INSERT INTO generaliax VALUES(2,0,'iax.conf','general','bindaddr','0.0.0.0');
INSERT INTO generaliax VALUES(3,0,'iax.conf','general','delayreject','no');
INSERT INTO generaliax VALUES(4,0,'iax.conf','general','language','fr');
INSERT INTO generaliax VALUES(5,0,'iax.conf','general','tos','lowdelay');
INSERT INTO generaliax VALUES(6,0,'iax.conf','general','qualify','yes');
INSERT INTO generaliax VALUES(7,0,'iax.conf','general','rtcachefriends','yes');
INSERT INTO generaliax VALUES(8,0,'iax.conf','general','jitterbuffer','no');


DROP TABLE generalvoicemail;
CREATE TABLE generalvoicemail (
 id integer unsigned,
 commented tinyint(1) DEFAULT 0,
 filename varchar(128) NOT NULL,
 category varchar(128) NOT NULL,
 var_name varchar(128) NOT NULL,
 var_val text,
 PRIMARY KEY(id)
);

CREATE INDEX generalvoicemail__idx__commented ON generalvoicemail(commented);
CREATE INDEX generalvoicemail__idx__filename ON generalvoicemail(filename);
CREATE INDEX generalvoicemail__idx__category ON generalvoicemail(category);
CREATE INDEX generalvoicemail__idx__var_name ON generalvoicemail(var_name);

INSERT INTO generalvoicemail VALUES(1,0,'voicemail.conf','general','maxmessage',180);
INSERT INTO generalvoicemail VALUES(2,0,'voicemail.conf','general','minmessage',5);
INSERT INTO generalvoicemail VALUES(3,0,'voicemail.conf','general','maxsilence',1);
INSERT INTO generalvoicemail VALUES(4,0,'voicemail.conf','general','review','yes');
INSERT INTO generalvoicemail VALUES(5,0,'voicemail.conf','general','serveremail','voicemail@xivo');
INSERT INTO generalvoicemail VALUES(6,0,'voicemail.conf','general','fromstring','XIVO PBX');
INSERT INTO generalvoicemail VALUES(7,0,'voicemail.conf','general','maxmsg',100);
INSERT INTO generalvoicemail VALUES(8,0,'voicemail.conf','general','emailsubject','Messagerie XIVO');
INSERT INTO generalvoicemail VALUES(9,0,'voicemail.conf','general','emailbody','Bonjour ${VM_NAME} !

Vous avez reçu un message d''une durée de ${VM_DUR} minute(s), il vous reste actuellement ${VM_MSGNUM} message(s) non lu(s) sur votre messagerie vocale : ${VM_MAILBOX}.
Le dernier a été envoyé par ${VM_CALLERID}, le ${VM_DATE}. Si vous le souhaitez vous pouvez l''écouter ou le consulter en tapant le *98 sur votre téléphone. Merci !

-- Messagerie XIVO --');
INSERT INTO generalvoicemail VALUES(10,0,'voicemail.conf','general','charset','UTF-8');
INSERT INTO generalvoicemail VALUES(11,0,'voicemail.conf','zonemessages','eu-fr','Europe/Paris|''vm-received'' q ''digits/at'' kM');
INSERT INTO generalvoicemail VALUES(12,0,'voicemail.conf','general','tz','eu-fr');
INSERT INTO generalvoicemail VALUES(13,0,'voicemail.conf','general','externpass','/usr/share/asterisk/bin/change-pass-vm');


DROP TABLE generalqueue;
CREATE TABLE generalqueue (
 id integer unsigned,
 commented tinyint(1) DEFAULT 0,
 filename varchar(128) NOT NULL,
 category varchar(128) NOT NULL,
 var_name varchar(128) NOT NULL,
 var_val varchar(128),
 PRIMARY KEY(id)
);

CREATE INDEX generalqueue__idx__commented ON generalqueue(commented);
CREATE INDEX generalqueue__idx__filename ON generalqueue(filename);
CREATE INDEX generalqueue__idx__category ON generalqueue(category);
CREATE INDEX generalqueue__idx__var_name ON generalqueue(var_name);

INSERT INTO generalqueue VALUES(1,0,'queues.conf','general','persistentmembers','yes');


DROP TABLE usersip;
CREATE TABLE usersip (
 id integer unsigned,
 name varchar(80) NOT NULL,
 commented tinyint(1) NOT NULL DEFAULT 0,
 accountcode varchar(20),
 amaflags varchar(13),
 callgroup varchar(10),
 callerid varchar(80),
 canreinvite char(3),
 context varchar(80),
 defaultip varchar(15),
 dtmfmode varchar(7),
 fromuser varchar(80),
 fromdomain varchar(80),
 fullcontact varchar(80),
 host varchar(31) NOT NULL,
 insecure varchar(11),
 language char(2),
 mailbox varchar(50),
 md5secret varchar(80),
 nat varchar(5) NOT NULL DEFAULT 'no',
 deny varchar(95),
 permit varchar(95),
 mask varchar(95),
 pickupgroup varchar(10),
 port varchar(5) NOT NULL,
 qualify char(3),
 restrictcid char(1),
 rtptimeout char(3),
 rtpholdtimeout char(3),
 secret varchar(80),
 type varchar(6) NOT NULL DEFAULT 'friend',
 username varchar(80) NOT NULL,
 disallow varchar(100),
 allow varchar(100),
 musiconhold varchar(100),
 regseconds integer unsigned NOT NULL DEFAULT 0,
 ipaddr varchar(15) NOT NULL,
 regexten varchar(80) NOT NULL,
 cancallforward char(3),
 setvar varchar(100) NOT NULL,
 'call-limit' tinyint unsigned DEFAULT 0,
 category varchar(50) NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX usersip__idx__commented ON usersip(commented);
CREATE INDEX usersip__idx__category ON usersip(category);
CREATE UNIQUE INDEX usersip__uidx__name ON usersip(name);

INSERT INTO usersip VALUES(1,'guest',0,'','documentation','','Guest','no','initconfig',NULL,'rfc2833',NULL,NULL,'','dynamic',NULL,NULL,'',NULL,'no',NULL,NULL,NULL,'',5060,'no',NULL,NULL,NULL,'guest','friend','guest',NULL,NULL,NULL,'','','',NULL,'',0,'user');
INSERT INTO usersip VALUES(2,'xivosb',0,'','documentation','','XivoSB','no','local-extensions',NULL,'rfc2833',NULL,NULL,'','dynamic',NULL,NULL,'',NULL,'no',NULL,NULL,NULL,'',5060,'no',NULL,NULL,NULL,'','friend','xivosb',NULL,NULL,NULL,'','','',NULL,'',0,'user');


DROP TABLE musiconhold;
CREATE TABLE musiconhold (
 id integer unsigned,
 commented tinyint(1) DEFAULT 0,
 filename varchar(128) NOT NULL,
 category varchar(128) NOT NULL,
 var_name varchar(128) NOT NULL,
 var_val varchar(128),
 PRIMARY KEY(id)
);

CREATE INDEX musiconhold__idx__commented ON musiconhold(commented);
CREATE UNIQUE INDEX musiconhold__uidx__filename_category_var_name ON musiconhold(filename,category,var_name);

INSERT INTO musiconhold VALUES(1,0,'musiconhold.conf','default','mode','custom');
INSERT INTO musiconhold VALUES(2,0,'musiconhold.conf','default','application','/usr/bin/madplay --mono -a -10 -R 8000 --output=raw:-');
INSERT INTO musiconhold VALUES(3,0,'musiconhold.conf','default','random','no');
INSERT INTO musiconhold VALUES(4,0,'musiconhold.conf','default','directory','/usr/share/asterisk/moh/default');

COMMIT;
