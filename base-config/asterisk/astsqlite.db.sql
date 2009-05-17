/*
 * XiVO Base-Config
 * Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

BEGIN TRANSACTION;

DROP TABLE accessfeatures;
CREATE TABLE accessfeatures (
 id integer unsigned,
 host varchar(255) NOT NULL DEFAULT '',
 feature varchar(9) NOT NULL,
 commented tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE INDEX accessfeatures__idx__host ON accessfeatures(host);
CREATE INDEX accessfeatures__idx__feature ON accessfeatures(feature);
CREATE INDEX accessfeatures__idx__commented ON accessfeatures(commented);
CREATE UNIQUE INDEX accessfeatures__uidx__host_type ON accessfeatures(host,feature);


DROP TABLE agentfeatures;
CREATE TABLE agentfeatures (
 id integer unsigned,
 agentid integer unsigned NOT NULL,
 numgroup tinyint unsigned NOT NULL,
 firstname varchar(128) NOT NULL DEFAULT '',
 lastname varchar(128) NOT NULL DEFAULT '',
 number varchar(40) NOT NULL,
 passwd varchar(128) NOT NULL,
 context varchar(39) NOT NULL,
 language varchar(20) NOT NULL,
 silent tinyint(1) NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX agentfeatures__idx__commented ON agentfeatures(commented);
CREATE UNIQUE INDEX agentfeatures__uidx__number ON agentfeatures(number);
CREATE UNIQUE INDEX agentfeatures__uidx__agentid ON agentfeatures(agentid);


DROP TABLE agentgroup;
CREATE TABLE agentgroup (
 id tinyint unsigned,
 groupid integer unsigned NOT NULL,
 name varchar(128) NOT NULL DEFAULT '',
 groups varchar(255) NOT NULL DEFAULT '',
 commented tinyint(1) NOT NULL DEFAULT 0,
 deleted tinyint(1) NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX agentgroup__idx__groupid ON agentgroup(groupid);
CREATE INDEX agentgroup__idx__name ON agentgroup(name);
CREATE INDEX agentgroup__idx__commented ON agentgroup(commented);
CREATE INDEX agentgroup__idx__deleted ON agentgroup(deleted);

INSERT INTO agentgroup VALUES (1,3,'default','',0,0,'');


DROP TABLE callerid;
CREATE TABLE callerid (
 mode varchar(9),
 callerdisplay varchar(80) NOT NULL DEFAULT '',
 type varchar(32) NOT NULL,
 typeval integer unsigned NOT NULL,
 PRIMARY KEY(type,typeval)
);


DROP TABLE callfilter;
CREATE TABLE callfilter (
 id integer unsigned,
 name varchar(128) NOT NULL DEFAULT '',
 context varchar(39) NOT NULL,
 type varchar(14) NOT NULL DEFAULT 'bosssecretary',
 bosssecretary varchar(16),
 callfrom varchar(8) NOT NULL DEFAULT 'all',
 ringseconds tinyint unsigned NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX callfilter__idx__context ON callfilter(context);
CREATE INDEX callfilter__idx__type ON callfilter(type);
CREATE INDEX callfilter__idx__bosssecretary ON callfilter(bosssecretary);
CREATE INDEX callfilter__idx__callfrom ON callfilter(callfrom);
CREATE INDEX callfilter__idx__commented ON callfilter(commented);
CREATE UNIQUE INDEX callfilter__uidx__name ON callfilter(name);


DROP TABLE callfiltermember;
CREATE TABLE callfiltermember (
 id integer unsigned,
 callfilterid integer unsigned NOT NULL DEFAULT 0,
 type char(4) NOT NULL DEFAULT 'user',
 typeval varchar(128) NOT NULL DEFAULT 0,
 ringseconds tinyint unsigned NOT NULL DEFAULT 0,
 priority tinyint unsigned NOT NULL DEFAULT 0,
 bstype varchar(9),
 active tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE INDEX callfiltermember__idx__priority ON callfiltermember(priority);
CREATE INDEX callfiltermember__idx__bstype ON callfiltermember(bstype);
CREATE INDEX callfiltermember__idx__active ON callfiltermember(active);
CREATE UNIQUE INDEX callfiltermember__uidx__callfilterid_type_typeval ON callfiltermember(callfilterid,type,typeval);


DROP TABLE cdr;
CREATE TABLE cdr (
 id integer unsigned,
 calldate char(19) DEFAULT '0000-00-00 00:00:00',
 clid varchar(80) NOT NULL DEFAULT '',
 src varchar(80) NOT NULL DEFAULT '',
 dst varchar(80) NOT NULL DEFAULT '',
 dcontext varchar(39) NOT NULL DEFAULT '',
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
CREATE INDEX cdr__idx__clid ON cdr(clid);
CREATE INDEX cdr__idx__src ON cdr(src);
CREATE INDEX cdr__idx__dst ON cdr(dst);
CREATE INDEX cdr__idx__channel ON cdr(channel);
CREATE INDEX cdr__idx__duration ON cdr(duration);
CREATE INDEX cdr__idx__disposition ON cdr(disposition);
CREATE INDEX cdr__idx__amaflags ON cdr(amaflags);
CREATE INDEX cdr__idx__accountcode ON cdr(accountcode);
CREATE INDEX cdr__idx__userfield ON cdr(userfield);


DROP TABLE context;
CREATE TABLE context (
 name varchar(39) NOT NULL,
 displayname varchar(128) NOT NULL DEFAULT '',
 entity varchar(64),
 commented tinyint(1) NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(name)
);

CREATE INDEX context__idx__displayname ON context(displayname);
CREATE INDEX context__idx__entity ON context(entity);
CREATE INDEX context__idx__commented ON context(commented);


DROP TABLE contextinclude;
CREATE TABLE contextinclude (
 context varchar(39) NOT NULL,
 include varchar(39) NOT NULL,
 priority tinyint unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(context,include)
);

CREATE INDEX contextinclude__idx__context ON contextinclude(context);
CREATE INDEX contextinclude__idx__include ON contextinclude(include);
CREATE INDEX contextinclude__idx__priority ON contextinclude(priority);


DROP TABLE contextnummember;
CREATE TABLE contextnummember (
 context varchar(39) NOT NULL,
 type varchar(6) NOT NULL,
 typeval varchar(128) NOT NULL DEFAULT 0,
 number varchar(40) NOT NULL DEFAULT '',
 PRIMARY KEY(context,type,typeval)
);

CREATE INDEX contextnummember__idx__context ON contextnummember(context);
CREATE INDEX contextnummember__idx__context_type ON contextnummember(context,type);
CREATE INDEX contextnummember__idx__number ON contextnummember(number);


DROP TABLE contextmember;
CREATE TABLE contextmember (
 context varchar(39) NOT NULL,
 type varchar(32) NOT NULL,
 typeval varchar(128) NOT NULL DEFAULT '',
 varname varchar(128) NOT NULL DEFAULT '',
 PRIMARY KEY(context,type,typeval,varname)
);

CREATE INDEX contextmember__idx__context ON contextmember(context);
CREATE INDEX contextmember__idx__context_type ON contextmember(context,type);


DROP TABLE contextnumbers;
CREATE TABLE contextnumbers (
 context varchar(39) NOT NULL,
 type varchar(6) NOT NULL,
 numberbeg varchar(16) NOT NULL DEFAULT '',
 numberend varchar(16) NOT NULL DEFAULT '',
 didlength tinyint unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(context,type,numberbeg,numberend)
);

CREATE INDEX contextnumbers__idx__context_type ON contextnumbers(context,type);


DROP TABLE dialaction;
CREATE TABLE dialaction (
 event varchar(11) NOT NULL,
 category varchar(10) NOT NULL,
 categoryval varchar(128) NOT NULL DEFAULT '',
 action varchar(64) NOT NULL DEFAULT 'none',
 actionarg1 varchar(255) NOT NULL DEFAULT '',
 actionarg2 varchar(255) NOT NULL DEFAULT '',
 linked tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(event,category,categoryval)
);

CREATE INDEX dialaction__idx__action_actionarg1 ON dialaction(action,actionarg1);
CREATE INDEX dialaction__idx__actionarg2 ON dialaction(actionarg2);
CREATE INDEX dialaction__idx__linked ON dialaction(linked);


DROP TABLE extensions;
CREATE TABLE extensions (
 id integer unsigned,
 commented tinyint(1) NOT NULL DEFAULT 0,
 context varchar(39) NOT NULL DEFAULT '',
 exten varchar(40) NOT NULL DEFAULT '',
 priority tinyint unsigned NOT NULL DEFAULT 0,
 app varchar(128) NOT NULL DEFAULT '',
 appdata varchar(128) NOT NULL DEFAULT '',
 name varchar(128) NOT NULL DEFAULT '',
 PRIMARY KEY(id)
);

CREATE INDEX extensions__idx__commented ON extensions(commented);
CREATE INDEX extensions__idx__context_exten_priority ON extensions(context,exten,priority);
CREATE INDEX extensions__idx__name ON extensions(name);

INSERT INTO extensions VALUES (NULL,1,'xivo-features','*33',1,'AgentLogin','','agentdynamiclogin');
INSERT INTO extensions VALUES (NULL,1,'xivo-features','_*31.',1,'Macro','agentstaticlogin|${EXTEN:3}','agentstaticlogin');
INSERT INTO extensions VALUES (NULL,1,'xivo-features','_*32.',1,'Macro','agentstaticlogoff|${EXTEN:3}','agentstaticlogoff');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*37.',1,'Macro','bsfilter|${EXTEN:3}','bsfilter');
INSERT INTO extensions VALUES (NULL,1,'xivo-features','*34',1,'Macro','calllistening','calllistening');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*36',1,'Directory','${CONTEXT}','directoryaccess');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*25',1,'Macro','enablednd','enablednd');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*24',1,'Macro','enablevm','enablevm');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*23.',1,'Macro','fwdbusy|${EXTEN:3}','fwdbusy');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*22.',1,'Macro','fwdrna|${EXTEN:3}','fwdrna');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*21.',1,'Macro','fwdunc|${EXTEN:3}','fwdunc');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*20',1,'Macro','fwdundoall','fwdundoall');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*23',1,'Macro','fwdundobusy','fwdundobusy');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*22',1,'Macro','fwdundorna','fwdundorna');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*21',1,'Macro','fwdundounc','fwdundounc');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*48378',1,'Macro','guestprov','guestprov');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*27',1,'Macro','incallfilter','incallfilter');
INSERT INTO extensions VALUES (NULL,1,'xivo-features','*26',1,'Macro','incallrec','incallrec');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*10',1,'Macro','phonestatus','phonestatus');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*8.',1,'Pickup','${EXTEN:2}%${CONTEXT}@PICKUPMARK','pickup');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*9',1,'Macro','recsnd|wav','recsnd');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*6.',1,'Macro','vmbox|${EXTEN:2}','vmbox');
INSERT INTO extensions VALUES (NULL,1,'xivo-features','*35',1,'Macro','vmdelete','vmdelete');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*7.',1,'Macro','vmuser|${EXTEN:2}','vmuser');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*98',1,'Macro','voicemsg','voicemsg');


DROP TABLE extenumbers;
CREATE TABLE extenumbers (
 id integer unsigned,
 exten varchar(40) NOT NULL DEFAULT '',
 extenhash char(40) NOT NULL DEFAULT '',
 context varchar(39) NOT NULL,
 type varchar(64) NOT NULL DEFAULT '',
 typeval varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY(id)
);

CREATE INDEX extenumbers__idx__exten ON extenumbers(exten);
CREATE INDEX extenumbers__idx__extenhash ON extenumbers(extenhash);
CREATE INDEX extenumbers__idx__context ON extenumbers(context);
CREATE INDEX extenumbers__idx__type ON extenumbers(type);
CREATE INDEX extenumbers__idx__typeval ON extenumbers(typeval);

INSERT INTO extenumbers VALUES (NULL,'*8','aa5820277564fac26df0e3dc72f796407597721d','','generalfeatures','pickupexten');
INSERT INTO extenumbers VALUES (NULL,'700','d8e4bbea3af2e4861ad5a445aaec573e02f9aca2','','generalfeatures','parkext');
INSERT INTO extenumbers VALUES (NULL,'*0','e914c907ff6d7a8ffefae72fe47363726b39d112','','featuremap','disconnect');
INSERT INTO extenumbers VALUES (NULL,'*1','8e04e82b8798d3979eded4ca2afdda3bebcb963d','','featuremap','blindxfer');
INSERT INTO extenumbers VALUES (NULL,'*2','6951109c8ca021277336cc2c8f6ac7f47d3b30e9','','featuremap','atxfer');
INSERT INTO extenumbers VALUES (NULL,'*3','68631b4b53ba2a27a969ca63bdcdc00805c2c258','','featuremap','automon');
INSERT INTO extenumbers VALUES (NULL,'*33','197040c848ebe83ae2e7f0b26a8667291d6b2746','','extenfeatures','agentdynamiclogin');
INSERT INTO extenumbers VALUES (NULL,'_*31.','678fe23ee0d6aa64460584bebbed210e270d662f','','extenfeatures','agentstaticlogin');
INSERT INTO extenumbers VALUES (NULL,'_*32.','3ae0f1ff0ef4907faa2dad5da7bb891c9dbf45ad','','extenfeatures','agentstaticlogoff');
INSERT INTO extenumbers VALUES (NULL,'_*37.','249b00b17a5983bbb2af8ed0af2ab1a74abab342','','extenfeatures','bsfilter');
INSERT INTO extenumbers VALUES (NULL,'*34','668a8d2d8fe980b663e2cdcecb977860e1b272f3','','extenfeatures','calllistening');
INSERT INTO extenumbers VALUES (NULL,'*36','f9b69fe3c361ddfc2ae49e048460ea197ea850c8','','extenfeatures','directoryaccess');
INSERT INTO extenumbers VALUES (NULL,'*25','c0d236c38bf8d5d84a2e154203cd2a18b86c6b2a','','extenfeatures','enablednd');
INSERT INTO extenumbers VALUES (NULL,'*24','0af70ed679c6db8c1128f2eb1e05d95a7a2fe4b6','','extenfeatures','enablevm');
INSERT INTO extenumbers VALUES (NULL,'_*23.','a1968a70f1d265b8aa263e73c79259961c4f7bbb','','extenfeatures','fwdbusy');
INSERT INTO extenumbers VALUES (NULL,'_*22.','00638af9e028d4cd454c00f43caf5626baa7d84c','','extenfeatures','fwdrna');
INSERT INTO extenumbers VALUES (NULL,'_*21.','52c97d56ebcca524ccf882590e94c52f6db24649','','extenfeatures','fwdunc');
INSERT INTO extenumbers VALUES (NULL,'*20','934aca632679075488681be0e9904cf9102f8766','','extenfeatures','fwdundoall');
INSERT INTO extenumbers VALUES (NULL,'*23','572a822ffb7c680bd0f92cfde0b64530bd362696','','extenfeatures','fwdundobusy');
INSERT INTO extenumbers VALUES (NULL,'*22','724827dabea7a207bfef4d948984a2e4da9de3ab','','extenfeatures','fwdundorna');
INSERT INTO extenumbers VALUES (NULL,'*21','8fa35a886d3149c94d23ba4e69c041c1fe6468b9','','extenfeatures','fwdundounc');
INSERT INTO extenumbers VALUES (NULL,'*48378','e27276ceefcc71a5d2def28c9b59a6410959eb43','','extenfeatures','guestprov');
INSERT INTO extenumbers VALUES (NULL,'*27','663b9615ba92c21f80acac52d60b28a8d1fb1c58','','extenfeatures','incallfilter');
INSERT INTO extenumbers VALUES (NULL,'*26','f8aeb70618cc87f1143c7dff23cdc0d3d0a48a0c','','extenfeatures','incallrec');
INSERT INTO extenumbers VALUES (NULL,'*10','eecefbd85899915e6fc2ff5a8ea44c2c83597cd6','','extenfeatures','phonestatus');
INSERT INTO extenumbers VALUES (NULL,'_*8.','b349d094036a97a7e0631ba60de759a9597c1c3a','','extenfeatures','pickup');
INSERT INTO extenumbers VALUES (NULL,'*9','e28d0f359da60dcf86340435478b19388b1b1d05','','extenfeatures','recsnd');
INSERT INTO extenumbers VALUES (NULL,'_*6.','34ff9e7d19f841a4b47bd6300898c593ebd9368d','','extenfeatures','vmbox');
INSERT INTO extenumbers VALUES (NULL,'*35','c4230e424c7189becec7ee35e9509829c5aea039','','extenfeatures','vmdelete');
INSERT INTO extenumbers VALUES (NULL,'_*7.','2e62c97c5e934f40309436048edebcbff2533f16','','extenfeatures','vmuser');
INSERT INTO extenumbers VALUES (NULL,'*98','6fb653e9eaf6f4d9c8d2cb48d1a6e3f4d4085710','','extenfeatures','voicemsg');


DROP TABLE features;
CREATE TABLE features (
 id integer unsigned,
 cat_metric integer unsigned NOT NULL DEFAULT 0,
 var_metric integer unsigned NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 filename varchar(128) NOT NULL,
 category varchar(128) NOT NULL,
 var_name varchar(128) NOT NULL,
 var_val varchar(255),
 PRIMARY KEY(id)
);

CREATE INDEX features__idx__commented ON features(commented);
CREATE INDEX features__idx__filename ON features(filename);
CREATE INDEX features__idx__category ON features(category);
CREATE INDEX features__idx__var_name ON features(var_name);

INSERT INTO features VALUES (NULL,0,0,0,'features.conf','general','parkext','700');
INSERT INTO features VALUES (NULL,0,0,0,'features.conf','general','context','parkedcalls');
INSERT INTO features VALUES (NULL,0,0,0,'features.conf','general','parkingtime','45');
INSERT INTO features VALUES (NULL,0,0,0,'features.conf','general','parkpos','701-750');
INSERT INTO features VALUES (NULL,0,0,0,'features.conf','general','parkfindnext','no');
INSERT INTO features VALUES (NULL,0,0,0,'features.conf','general','adsipark','no');
INSERT INTO features VALUES (NULL,0,0,0,'features.conf','general','transferdigittimeout','3');
INSERT INTO features VALUES (NULL,0,0,0,'features.conf','general','featuredigittimeout','500');
INSERT INTO features VALUES (NULL,0,0,1,'features.conf','general','courtesytone',NULL);
INSERT INTO features VALUES (NULL,0,0,1,'features.conf','general','xfersound',NULL);
INSERT INTO features VALUES (NULL,0,0,1,'features.conf','general','xferfailsound',NULL);
INSERT INTO features VALUES (NULL,0,0,0,'features.conf','general','pickupexten','*8');
INSERT INTO features VALUES (NULL,1,0,0,'features.conf','featuremap','blindxfer','*1');
INSERT INTO features VALUES (NULL,1,0,0,'features.conf','featuremap','atxfer','*2');
INSERT INTO features VALUES (NULL,1,0,0,'features.conf','featuremap','automon','*3');
INSERT INTO features VALUES (NULL,1,0,0,'features.conf','featuremap','disconnect','*0');


DROP TABLE groupfeatures;
CREATE TABLE groupfeatures (
 id tinyint unsigned NOT NULL,
 name varchar(128) NOT NULL,
 number varchar(40) NOT NULL DEFAULT '',
 context varchar(39) NOT NULL,
 transfer_user tinyint(1) NOT NULL DEFAULT 0,
 transfer_call tinyint(1) NOT NULL DEFAULT 0,
 write_caller tinyint(1) NOT NULL DEFAULT 0,
 write_calling tinyint(1) NOT NULL DEFAULT 0,
 timeout tinyint unsigned NOT NULL DEFAULT 0,
 preprocess_subroutine varchar(39),
 deleted tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE INDEX groupfeatures__idx__name ON groupfeatures(name);
CREATE INDEX groupfeatures__idx__deleted ON groupfeatures(deleted);


DROP TABLE handynumbers;
CREATE TABLE handynumbers (
 id integer unsigned,
 exten varchar(40) NOT NULL DEFAULT '',
 trunkfeaturesid integer unsigned NOT NULL DEFAULT 0,
 type varchar(9) NOT NULL,
 commented tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE INDEX handynumbers__idx__trunkfeaturesid ON handynumbers(trunkfeaturesid);
CREATE INDEX handynumbers__idx__type ON handynumbers(type);
CREATE INDEX handynumbers__idx__commented ON handynumbers(commented);
CREATE UNIQUE INDEX handynumbers__uidx__exten ON handynumbers(exten);


DROP TABLE incall;
CREATE TABLE incall (
 id integer unsigned,
 exten varchar(40) NOT NULL,
 context varchar(39) NOT NULL,
 preprocess_subroutine varchar(39),
 faxdetectenable tinyint(1) NOT NULL DEFAULT 0,
 faxdetecttimeout tinyint unsigned NOT NULL DEFAULT 4,
 faxdetectemail varchar(255) NOT NULL DEFAULT '',
 commented tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE INDEX incall__idx__commented ON incall(commented);
CREATE UNIQUE INDEX incall__uidx__exten_context ON incall(exten,context);


DROP TABLE ldapfilter;
CREATE TABLE ldapfilter (
 id integer unsigned,
 ldapserverid integer unsigned NOT NULL,
 name varchar(128) NOT NULL DEFAULT '',
 user varchar(255) NOT NULL DEFAULT '',
 passwd varchar(255) NOT NULL DEFAULT '',
 basedn varchar(255) NOT NULL DEFAULT '',
 filter varchar(255) NOT NULL DEFAULT '',
 attrdisplayname varchar(255) NOT NULL DEFAULT '',
 attrphonenumber varchar(255) NOT NULL DEFAULT '',
 additionaltype varchar(6) NOT NULL,
 additionaltext varchar(16) NOT NULL DEFAULT '',
 commented tinyint(1) NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX ldapfilter__idx__ldapserverid ON ldapfilter(ldapserverid);
CREATE INDEX ldapfilter__idx__commented ON ldapfilter(commented);
CREATE UNIQUE INDEX ldapfilter__uidx__name ON ldapfilter(name);


DROP TABLE meetmefeatures;
CREATE TABLE meetmefeatures (
 id integer unsigned,
 name varchar(128) NOT NULL,
 number varchar(40) NOT NULL,
 meetmeid integer unsigned NOT NULL,
 mode varchar(6) NOT NULL DEFAULT 'all',
 musiconhold varchar(128) NOT NULL DEFAULT '',
 context varchar(39) NOT NULL,
 poundexit tinyint(1) NOT NULL DEFAULT 0,
 quiet tinyint(1) NOT NULL DEFAULT 0,
 record tinyint(1) NOT NULL DEFAULT 0,
 adminmode tinyint(1) NOT NULL DEFAULT 0,
 announceusercount tinyint(1) NOT NULL DEFAULT 0,
 announcejoinleave tinyint(1) NOT NULL DEFAULT 0,
 alwayspromptpin tinyint(1) NOT NULL DEFAULT 0,
 starmenu tinyint(1) NOT NULL DEFAULT 0,
 enableexitcontext tinyint(1) NOT NULL DEFAULT 0,
 exitcontext varchar(39) NOT NULL,
 preprocess_subroutine varchar(39),
 PRIMARY KEY(id)
);

CREATE UNIQUE INDEX meetmefeatures__uidx__meetmeid ON meetmefeatures(meetmeid);
CREATE UNIQUE INDEX meetmefeatures__uidx__name ON meetmefeatures(name);
CREATE UNIQUE INDEX meetmefeatures__uidx__number ON meetmefeatures(number);


DROP TABLE musiconhold;
CREATE TABLE musiconhold (
 id integer unsigned,
 cat_metric integer unsigned NOT NULL DEFAULT 0,
 var_metric integer unsigned NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 filename varchar(128) NOT NULL,
 category varchar(128) NOT NULL,
 var_name varchar(128) NOT NULL,
 var_val varchar(128),
 PRIMARY KEY(id)
);

CREATE INDEX musiconhold__idx__commented ON musiconhold(commented);
CREATE UNIQUE INDEX musiconhold__uidx__filename_category_var_name ON musiconhold(filename,category,var_name);

INSERT INTO musiconhold VALUES (1,0,0,0,'musiconhold.conf','default','mode','custom');
INSERT INTO musiconhold VALUES (2,0,0,0,'musiconhold.conf','default','application','/usr/bin/madplay --mono -a -10 -R 8000 --output=raw:-');
INSERT INTO musiconhold VALUES (3,0,0,0,'musiconhold.conf','default','random','no');
INSERT INTO musiconhold VALUES (4,0,0,0,'musiconhold.conf','default','directory','/var/lib/pf-xivo/moh/default');


DROP TABLE outcall;
CREATE TABLE outcall (
 id integer unsigned,
 name varchar(128) NOT NULL,
 exten varchar(40) NOT NULL,
 context varchar(39) NOT NULL,
 externprefix varchar(20) NOT NULL DEFAULT '',
 stripnum tinyint unsigned NOT NULL DEFAULT 0,
 setcallerid tinyint(1) NOT NULL DEFAULT 0,
 callerid varchar(80) NOT NULL DEFAULT '',
 useenum tinyint(1) NOT NULL DEFAULT 0,
 internal tinyint(1) NOT NULL DEFAULT 0,
 hangupringtime smallint unsigned NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE INDEX outcall__idx__commented ON outcall(commented);
CREATE UNIQUE INDEX outcall__uidx__name ON outcall(name);
CREATE UNIQUE INDEX outcall__uidx__exten_context ON outcall(exten,context);


DROP TABLE outcalltrunk;
CREATE TABLE outcalltrunk (
 outcallid integer unsigned NOT NULL DEFAULT 0,
 trunkfeaturesid integer unsigned NOT NULL DEFAULT 0,
 priority tinyint unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(outcallid,trunkfeaturesid)
);

CREATE INDEX outcalltrunk__idx__priority ON outcalltrunk(priority);


DROP TABLE phone;
CREATE TABLE phone (
 macaddr char(17) NOT NULL,
 vendor varchar(16) NOT NULL,
 model varchar(16) NOT NULL,
 proto varchar(50) NOT NULL,
 iduserfeatures integer unsigned NOT NULL,
 isinalan tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(macaddr)
);

CREATE INDEX phone__idx__proto_iduserfeatures ON phone(proto,iduserfeatures);


DROP TABLE phonebook;
CREATE TABLE phonebook (
 id integer unsigned,
 title varchar(3) NOT NULL,
 firstname varchar(128) NOT NULL DEFAULT '',
 lastname varchar(128) NOT NULL DEFAULT '',
 displayname varchar(64) NOT NULL DEFAULT '',
 society varchar(128) NOT NULL DEFAULT '',
 email varchar(255) NOT NULL DEFAULT '',
 url varchar(255) NOT NULL DEFAULT '',
 image blob,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX phonebook__idx__title ON phonebook(title);
CREATE INDEX phonebook__idx__firstname ON phonebook(firstname);
CREATE INDEX phonebook__idx__lastname ON phonebook(lastname);
CREATE INDEX phonebook__idx__displayname ON phonebook(displayname);
CREATE INDEX phonebook__idx__society ON phonebook(society);
CREATE INDEX phonebook__idx__email ON phonebook(email);


DROP TABLE phonebookaddress;
CREATE TABLE phonebookaddress (
 id integer unsigned,
 phonebookid integer unsigned NOT NULL,
 address1 varchar(30) NOT NULL DEFAULT '',
 address2 varchar(30) NOT NULL DEFAULT '',
 city varchar(128) NOT NULL DEFAULT '',
 state varchar(128) NOT NULL DEFAULT '',
 zipcode varchar(16) NOT NULL DEFAULT '',
 country varchar(3) NOT NULL DEFAULT '',
 type varchar(6) NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX phonebookaddress__idx__address1 ON phonebookaddress(address1);
CREATE INDEX phonebookaddress__idx__address2 ON phonebookaddress(address2);
CREATE INDEX phonebookaddress__idx__city ON phonebookaddress(city);
CREATE INDEX phonebookaddress__idx__state ON phonebookaddress(state);
CREATE INDEX phonebookaddress__idx__zipcode ON phonebookaddress(zipcode);
CREATE INDEX phonebookaddress__idx__country ON phonebookaddress(country);
CREATE INDEX phonebookaddress__idx__type ON phonebookaddress(type);
CREATE UNIQUE INDEX phonebookaddress__uidx__phonebookid_type ON phonebookaddress(phonebookid,type);


DROP TABLE phonebooknumber;
CREATE TABLE phonebooknumber (
 id integer unsigned,
 phonebookid integer unsigned NOT NULL,
 number varchar(40) NOT NULL DEFAULT '',
 type varchar(6) NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX phonebooknumber__idx__number ON phonebooknumber(number);
CREATE INDEX phonebooknumber__idx__type ON phonebooknumber(type);
CREATE UNIQUE INDEX phonebooknumber__uidx__phonebookid_type ON phonebooknumber(phonebookid,type);


DROP TABLE phonefunckey;
CREATE TABLE phonefunckey (
 iduserfeatures integer unsigned NOT NULL,
 fknum smallint unsigned NOT NULL,
 exten varchar(40),
 typeextenumbers varchar(64),
 typevalextenumbers varchar(255),
 typeextenumbersright varchar(64),
 typevalextenumbersright varchar(255),
 supervision tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(iduserfeatures,fknum)
);

CREATE INDEX phonefunckey__idx__exten ON phonefunckey(exten);
CREATE INDEX phonefunckey__idx__typeextenumbers_typevalextenumbers ON phonefunckey(typeextenumbers,typevalextenumbers);
CREATE INDEX phonefunckey__idx__typeextenumbersright_typevalextenumbersright ON phonefunckey(typeextenumbersright,typevalextenumbersright);


DROP TABLE queue;
CREATE TABLE queue (
 name varchar(128) NOT NULL,
 musiconhold varchar(128),
 announce varchar(128),
 context varchar(39),
 timeout tinyint unsigned DEFAULT 0,
 "monitor-type" varchar(10),
 "monitor-format" varchar(128),
 "queue-youarenext" varchar(128),
 "queue-thereare" varchar(128),
 "queue-callswaiting" varchar(128),
 "queue-holdtime" varchar(128),
 "queue-minutes" varchar(128),
 "queue-seconds" varchar(128),
 "queue-lessthan" varchar(128),
 "queue-thankyou" varchar(128),
 "queue-reporthold" varchar(128),
 "periodic-announce" text,
 "announce-frequency" integer unsigned,
 "periodic-announce-frequency" integer unsigned,
 "announce-round-seconds" tinyint unsigned,
 "announce-holdtime" varchar(4),
 retry tinyint unsigned,
 wrapuptime tinyint unsigned,
 maxlen integer unsigned,
 servicelevel int(11),
 strategy varchar(11),
 joinempty varchar(6),
 leavewhenempty varchar(6),
 eventmemberstatus tinyint(1) NOT NULL DEFAULT 0,
 eventwhencalled tinyint(1) NOT NULL DEFAULT 0,
 reportholdtime tinyint(1) NOT NULL DEFAULT 0,
 memberdelay integer unsigned,
 weight integer unsigned,
 timeoutrestart tinyint(1) NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 category char(5) NOT NULL,
 PRIMARY KEY(name)
);

CREATE INDEX queue__idx__commented ON queue(commented);
CREATE INDEX queue__idx__category ON queue(category);


DROP TABLE queuefeatures;
CREATE TABLE queuefeatures (
 id integer unsigned,
 name varchar(128) NOT NULL,
 number varchar(40) DEFAULT '',
 context varchar(39),
 data_quality tinyint(1) NOT NULL DEFAULT 0,
 hitting_callee tinyint(1) NOT NULL DEFAULT 0,
 hitting_caller tinyint(1) NOT NULL DEFAULT 0,
 retries tinyint(1) NOT NULL DEFAULT 0,
 ring tinyint(1) NOT NULL DEFAULT 0,
 transfer_user tinyint(1) NOT NULL DEFAULT 0,
 transfer_call tinyint(1) NOT NULL DEFAULT 0,
 write_caller tinyint(1) NOT NULL DEFAULT 0,
 write_calling tinyint(1) NOT NULL DEFAULT 0,
 url varchar(255) NOT NULL DEFAULT '',
 announceoverride varchar(128) NOT NULL DEFAULT '',
 timeout smallint unsigned NOT NULL DEFAULT 0,
 preprocess_subroutine varchar(39),
 PRIMARY KEY(id)
);

CREATE UNIQUE INDEX queuefeatures__uidx__name ON queuefeatures(name);


DROP TABLE queuemember;
CREATE TABLE queuemember (
 queue_name varchar(128) NOT NULL,
 interface varchar(128) NOT NULL,
 penalty tinyint unsigned NOT NULL DEFAULT 0,
 "call-limit" tinyint unsigned NOT NULL DEFAULT 0,
 paused tinyint unsigned NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 usertype varchar(5) NOT NULL,
 userid integer unsigned NOT NULL,
 channel varchar(25) NOT NULL,
 category char(5) NOT NULL,
 PRIMARY KEY(queue_name,interface)
);

CREATE INDEX queuemember__idx__commented ON queuemember(commented);
CREATE INDEX queuemember__idx__usertype ON queuemember(usertype);
CREATE INDEX queuemember__idx__userid ON queuemember(userid);
CREATE INDEX queuemember__idx__channel ON queuemember(channel);
CREATE INDEX queuemember__idx__category ON queuemember(category);
CREATE UNIQUE INDEX queuemember__uidx__queue_name_channel_usertype_userid_category ON queuemember(queue_name,channel,usertype,userid,category);


DROP TABLE rightcall;
CREATE TABLE rightcall (
 id integer unsigned,
 name varchar(128) NOT NULL DEFAULT '',
 context varchar(39) NOT NULL,
 passwd varchar(40) NOT NULL DEFAULT '',
 authorization tinyint(1) NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX rightcall__idx__context ON rightcall(context);
CREATE INDEX rightcall__idx__passwd ON rightcall(passwd);
CREATE INDEX rightcall__idx__authorization ON rightcall(authorization);
CREATE INDEX rightcall__idx__commented ON rightcall(commented);
CREATE UNIQUE INDEX rightcall__uidx__name ON rightcall(name);


DROP TABLE rightcallexten;
CREATE TABLE rightcallexten (
 id integer unsigned,
 rightcallid integer unsigned NOT NULL DEFAULT 0,
 exten varchar(40) NOT NULL DEFAULT '',
 extenhash char(40) NOT NULL DEFAULT '',
 PRIMARY KEY(id)
);

CREATE UNIQUE INDEX rightcallexten__uidx__rightcallid_extenhash ON rightcallexten(rightcallid,extenhash);


DROP TABLE rightcallmember;
CREATE TABLE rightcallmember (
 id integer unsigned,
 rightcallid integer unsigned NOT NULL DEFAULT 0,
 type varchar(64) NOT NULL DEFAULT '',
 typeval varchar(128) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE UNIQUE INDEX rightcallmember__uidx__rightcallid_type_typeval ON rightcallmember(rightcallid,type,typeval);


DROP TABLE schedule;
CREATE TABLE schedule (
 id integer unsigned,
 name varchar(128) NOT NULL DEFAULT '',
 context varchar(39) NOT NULL,
 timebeg varchar(5) NOT NULL DEFAULT '*',
 timeend varchar(5),
 daynamebeg varchar(3) NOT NULL DEFAULT '*',
 daynameend varchar(3),
 daynumbeg varchar(2) NOT NULL DEFAULT '*',
 daynumend varchar(2),
 monthbeg varchar(3) NOT NULL DEFAULT '*',
 monthend varchar(3),
 publicholiday tinyint(1) NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE INDEX schedule__idx__context ON schedule(context);
CREATE INDEX schedule__idx__publicholiday ON schedule(publicholiday);
CREATE INDEX schedule__idx__commented ON schedule(commented);
CREATE UNIQUE INDEX schedule__uidx__name ON schedule(name);


DROP TABLE serverfeatures;
CREATE TABLE serverfeatures (
 id integer unsigned,
 serverid integer unsigned NOT NULL,
 feature varchar(9) NOT NULL,
 type char(4) NOT NULL,
 commented tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE INDEX serverfeatures__idx__serverid ON serverfeatures(serverid);
CREATE INDEX serverfeatures__idx__feature ON serverfeatures(feature);
CREATE INDEX serverfeatures__idx__type ON serverfeatures(type);
CREATE INDEX serverfeatures__idx__commented ON serverfeatures(commented);
CREATE UNIQUE INDEX serverfeatures__uidx__serverid_feature_type ON serverfeatures(serverid,feature,type);


DROP TABLE staticagent;
CREATE TABLE staticagent (
 id integer unsigned,
 cat_metric integer unsigned NOT NULL DEFAULT 0,
 var_metric integer unsigned NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 filename varchar(128) NOT NULL,
 category varchar(128) NOT NULL,
 var_name varchar(128) NOT NULL,
 var_val varchar(255),
 PRIMARY KEY(id)
);

CREATE INDEX staticagent__idx__cat_metric ON staticagent(cat_metric);
CREATE INDEX staticagent__idx__var_metric ON staticagent(var_metric);
CREATE INDEX staticagent__idx__commented ON staticagent(commented);
CREATE INDEX staticagent__idx__filename ON staticagent(filename);
CREATE INDEX staticagent__idx__category ON staticagent(category);
CREATE INDEX staticagent__idx__var_name ON staticagent(var_name);
CREATE INDEX staticagent__idx__var_val ON staticagent(var_val);

INSERT INTO staticagent VALUES (1,0,0,0,'agents.conf','general','persistentagents','yes');
INSERT INTO staticagent VALUES (2,0,0,0,'agents.conf','general','multiplelogin','yes');
INSERT INTO staticagent VALUES (3,1,1000000,0,'agents.conf','agents','group',1);


DROP TABLE staticiax;
CREATE TABLE staticiax (
 id integer unsigned,
 cat_metric integer unsigned NOT NULL DEFAULT 0,
 var_metric integer unsigned NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 filename varchar(128) NOT NULL,
 category varchar(128) NOT NULL,
 var_name varchar(128) NOT NULL,
 var_val varchar(255),
 PRIMARY KEY(id)
);

CREATE INDEX staticiax__idx__commented ON staticiax(commented);
CREATE INDEX staticiax__idx__filename ON staticiax(filename);
CREATE INDEX staticiax__idx__category ON staticiax(category);
CREATE INDEX staticiax__idx__var_name ON staticiax(var_name);

INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','bindport',4569);
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','bindaddr','0.0.0.0');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','iaxthreadcount',10);
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','iaxmaxthreadcount',100);
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','iaxcompat','no');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','authdebug','yes');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','delayreject','no');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','trunkfreq',20);
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','trunktimestamps','yes');
INSERT INTO staticiax VALUES (NULL,0,0,1,'iax.conf','general','regcontext',NULL);
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','minregexpire',60);
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','maxregexpire',60);
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','bandwidth','high');
INSERT INTO staticiax VALUES (NULL,0,0,1,'iax.conf','general','tos',NULL);
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','jitterbuffer','no');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','forcejitterbuffer','no');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','maxjitterbuffer',1000);
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','maxjitterinterps',10);
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','resyncthreshold',1000);
INSERT INTO staticiax VALUES (NULL,0,0,1,'iax.conf','general','accountcode',NULL);
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','amaflags','default');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','adsi','no');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','transfer','yes');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','language','fr');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','mohinterpret','default');
INSERT INTO staticiax VALUES (NULL,0,0,1,'iax.conf','general','mohsuggest',NULL);
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','encryption','no');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','maxauthreq',3);
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','codecpriority','host');
INSERT INTO staticiax VALUES (NULL,0,0,1,'iax.conf','general','disallow',NULL);
INSERT INTO staticiax VALUES (NULL,0,0,1,'iax.conf','general','allow',NULL);
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','rtcachefriends','yes');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','rtupdate','yes');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','rtignoreregexpire','yes');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','rtautoclear','no');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','pingtime',20);
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','lagrqtime',10);
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','nochecksums','no');
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','autokill','yes');


DROP TABLE staticmeetme;
CREATE TABLE staticmeetme (
 id integer unsigned,
 cat_metric integer unsigned NOT NULL DEFAULT 0,
 var_metric integer unsigned NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 filename varchar(128) NOT NULL,
 category varchar(128) NOT NULL,
 var_name varchar(128) NOT NULL,
 var_val varchar(128),
 PRIMARY KEY(id)
);

CREATE INDEX staticmeetme__idx__commented ON staticmeetme(commented);
CREATE INDEX staticmeetme__idx__filename ON staticmeetme(filename);
CREATE INDEX staticmeetme__idx__category ON staticmeetme(category);
CREATE INDEX staticmeetme__idx__var_name ON staticmeetme(var_name);

INSERT INTO staticmeetme VALUES (NULL,0,0,0,'meetme.conf','general','audiobuffers',32);


DROP TABLE staticqueue;
CREATE TABLE staticqueue (
 id integer unsigned,
 cat_metric integer unsigned NOT NULL DEFAULT 0,
 var_metric integer unsigned NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 filename varchar(128) NOT NULL,
 category varchar(128) NOT NULL,
 var_name varchar(128) NOT NULL,
 var_val varchar(128),
 PRIMARY KEY(id)
);

CREATE INDEX staticqueue__idx__commented ON staticqueue(commented);
CREATE INDEX staticqueue__idx__filename ON staticqueue(filename);
CREATE INDEX staticqueue__idx__category ON staticqueue(category);
CREATE INDEX staticqueue__idx__var_name ON staticqueue(var_name);

INSERT INTO staticqueue VALUES (NULL,0,0,0,'queues.conf','general','persistentmembers','yes');
INSERT INTO staticqueue VALUES (NULL,0,0,0,'queues.conf','general','autofill','no');
INSERT INTO staticqueue VALUES (NULL,0,0,0,'queues.conf','general','monitor-type','no');


DROP TABLE staticsip;
CREATE TABLE staticsip (
 id integer unsigned,
 cat_metric integer unsigned NOT NULL DEFAULT 0,
 var_metric integer unsigned NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 filename varchar(128) NOT NULL,
 category varchar(128) NOT NULL,
 var_name varchar(128) NOT NULL,
 var_val varchar(255),
 PRIMARY KEY(id)
);

CREATE INDEX staticsip__idx__commented ON staticsip(commented);
CREATE INDEX staticsip__idx__filename ON staticsip(filename);
CREATE INDEX staticsip__idx__category ON staticsip(category);
CREATE INDEX staticsip__idx__var_name ON staticsip(var_name);

INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','bindport',5060);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','bindaddr','0.0.0.0');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','videosupport','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','autocreatepeer','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','allowguest','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','allowsubscribe','yes');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','allowoverlap','yes');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','promiscredir','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','autodomain','no');
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','domain',NULL);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','allowexternaldomains','yes');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','usereqphone','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','realm','xivo');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','alwaysauthreject','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','limitonpeer','yes');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','useragent','XIVO PBX');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','checkmwi',10);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','buggymwi','no');
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','regcontext',NULL);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','callerid','xivo');
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','fromdomain',NULL);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','sipdebug','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','dumphistory','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','recordhistory','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','callevents','yes');
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','tos_sip',NULL);
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','tos_audio',NULL);
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','tos_video',NULL);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','t38pt_udptl','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','t38pt_rtp','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','t38pt_tcp','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','t38pt_usertpsource','no');
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','localnet',NULL);
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','externip',NULL);
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','externhost',NULL);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','externrefresh',10);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','matchexterniplocally','no');
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','outboundproxy',NULL);
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','outboundproxyport',NULL);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','g726nonstandard','no');
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','disallow',NULL);
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','allow',NULL);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','t1min',100);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','relaxdtmf','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','rfc2833compensate','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','compactheaders','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','rtptimeout',0);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','rtpholdtimeout',0);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','rtpkeepalive',0);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','directrtpsetup','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','notifymimetype','application/simple-message-summary');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','srvlookup','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','pedantic','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','minexpiry',60);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','maxexpiry',3600);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','defaultexpiry',120);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','registertimeout',20);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','registerattempts',0);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','notifyringing','yes');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','notifyhold','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','allowtransfer','yes');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','maxcallbitrate',384);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','autoframing','yes');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','jbenable','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','jbforce','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','jbmaxsize',200);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','jbresyncthreshold',1000);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','jbimpl','fixed');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','jblog','no');
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','context',NULL);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','nat','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','dtmfmode','info');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','qualify','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','useclientcode','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','progressinband','never');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','language','fr');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','mohinterpret','default');
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','mohsuggest',NULL);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','vmexten','*98');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','trustrpid','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','sendrpid','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','canreinvite','no');
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','insecure','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','rtcachefriends','yes');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','rtupdate','yes');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','ignoreregexpire','yes');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','rtsavesysname','no');
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','rtautoclear','no');
INSERT INTO staticsip VALUES (NULL,0,0,1,'sip.conf','general','subscribecontext',NULL);
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','assertedidentity','no');


DROP TABLE staticvoicemail;
CREATE TABLE staticvoicemail (
 id integer unsigned,
 cat_metric integer unsigned NOT NULL DEFAULT 0,
 var_metric integer unsigned NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 filename varchar(128) NOT NULL,
 category varchar(128) NOT NULL,
 var_name varchar(128) NOT NULL,
 var_val text,
 PRIMARY KEY(id)
);

CREATE INDEX staticvoicemail__idx__commented ON staticvoicemail(commented);
CREATE INDEX staticvoicemail__idx__filename ON staticvoicemail(filename);
CREATE INDEX staticvoicemail__idx__category ON staticvoicemail(category);
CREATE INDEX staticvoicemail__idx__var_name ON staticvoicemail(var_name);

INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','maxmsg',100);
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','silencethreshold',256);
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','minmessage',0);
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','maxmessage',0);
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','maxsilence',15);
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','review','yes');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','operator','yes');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','format','wav');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','maxlogins',3);
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','envelope','yes');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','saycid','no');
INSERT INTO staticvoicemail VALUES (NULL,0,0,1,'voicemail.conf','general','cidinternalcontexts',NULL);
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','sayduration','yes');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','saydurationm',2);
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','forcename','no');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','forcegreetings','no');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','tempgreetwarn','yes');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','maxgreet',0);
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','skipms',3000);
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','sendvoicemail','no');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','usedirectory','yes');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','nextaftercmd','yes');
INSERT INTO staticvoicemail VALUES (NULL,0,0,1,'voicemail.conf','general','dialout',NULL);
INSERT INTO staticvoicemail VALUES (NULL,0,0,1,'voicemail.conf','general','callback',NULL);
INSERT INTO staticvoicemail VALUES (NULL,0,0,1,'voicemail.conf','general','exitcontext',NULL);
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','attach','yes');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','volgain',0);
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','mailcmd','/usr/sbin/sendmail -t');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','serveremail','xivo');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','charset','UTF-8');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','fromstring','XIVO PBX');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','emaildateformat','%A, %B %d, %Y à %r');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','pbxskip','no');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','emailsubject','Messagerie XIVO');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','emailbody','Bonjour ${VM_NAME} !

Vous avez reçu un message d''une durée de ${VM_DUR} minute(s), il vous reste actuellement ${VM_MSGNUM} message(s) non lu(s) sur votre messagerie vocale : ${VM_MAILBOX}.

Le dernier a été envoyé par ${VM_CALLERID}, le ${VM_DATE}. Si vous le souhaitez vous pouvez l''écouter ou le consulter en tapant le *98 sur votre téléphone.

Merci.

-- Messagerie XIVO --');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','pagerfromstring','XIVO PBX');
INSERT INTO staticvoicemail VALUES (NULL,0,0,1,'voicemail.conf','general','pagersubject',NULL);
INSERT INTO staticvoicemail VALUES (NULL,0,0,1,'voicemail.conf','general','pagerbody',NULL);
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','adsifdn','0000000F');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','adsisec','9BDBF7AC');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','adsiver',1);
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','searchcontexts','no');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','externpass','/usr/share/asterisk/bin/change-pass-vm');
INSERT INTO staticvoicemail VALUES (NULL,0,0,1,'voicemail.conf','general','externnotify',NULL);
INSERT INTO staticvoicemail VALUES (NULL,0,0,1,'voicemail.conf','general','smdiport',NULL);
INSERT INTO staticvoicemail VALUES (NULL,0,0,1,'voicemail.conf','general','odbcstorage',NULL);
INSERT INTO staticvoicemail VALUES (NULL,0,0,1,'voicemail.conf','general','odbctable',NULL);
INSERT INTO staticvoicemail VALUES (NULL,1,0,0,'voicemail.conf','zonemessages','eu-fr','Europe/Paris|''vm-received'' q ''digits/at'' kM');


DROP TABLE trunkfeatures;
CREATE TABLE trunkfeatures (
 id integer unsigned,
 protocol varchar(50) NOT NULL,
 protocolid integer unsigned NOT NULL,
 registerid integer unsigned NOT NULL DEFAULT 0,
 registercommented tinyint(1) NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX trunkfeatures__idx__registerid ON trunkfeatures(registerid);
CREATE INDEX trunkfeatures__idx__registercommented ON trunkfeatures(registercommented);
CREATE UNIQUE INDEX trunkfeatures__uidx__protocol_protocolid ON trunkfeatures(protocol,protocolid);


DROP TABLE usercustom;
CREATE TABLE usercustom (
 id integer unsigned,
 name varchar(40),
 context varchar(39),
 interface varchar(128) NOT NULL,
 intfsuffix varchar(32) NOT NULL DEFAULT '',
 commented tinyint(1) NOT NULL DEFAULT 0,
 protocol char(5) NOT NULL DEFAULT 'custom',
 category varchar(5) NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX usercustom__idx__name ON usercustom(name);
CREATE INDEX usercustom__idx__context ON usercustom(context);
CREATE INDEX usercustom__idx__commented ON usercustom(commented);
CREATE INDEX usercustom__idx__protocol ON usercustom(protocol);
CREATE INDEX usercustom__idx__category ON usercustom(category);
CREATE UNIQUE INDEX usercustom__uidx__interface_intfsuffix_category ON usercustom(interface,intfsuffix,category);


DROP TABLE userfeatures;
CREATE TABLE userfeatures (
 id integer unsigned,
 protocol varchar(50) NOT NULL,
 protocolid integer unsigned NOT NULL,
 firstname varchar(128) NOT NULL DEFAULT '',
 lastname varchar(128) NOT NULL DEFAULT '',
 name varchar(128) NOT NULL,
 number varchar(40) NOT NULL,
 context varchar(39),
 voicemailid integer unsigned,
 agentid integer unsigned,
 provisioningid mediumint unsigned,
 ringseconds tinyint unsigned NOT NULL DEFAULT 30,
 simultcalls tinyint unsigned NOT NULL DEFAULT 5,
 enableclient tinyint(1) NOT NULL DEFAULT 1,
 loginclient varchar(64) NOT NULL DEFAULT '',
 passwdclient varchar(64) NOT NULL DEFAULT '',
 profileclient varchar(64) NOT NULL DEFAULT '',
 enablehint tinyint(1) NOT NULL DEFAULT 1,
 enablevoicemail tinyint(1) NOT NULL DEFAULT 0,
 enablexfer tinyint(1) NOT NULL DEFAULT 0,
 enableautomon tinyint(1) NOT NULL DEFAULT 0,
 callrecord tinyint(1) NOT NULL DEFAULT 0,
 callfilter tinyint(1) NOT NULL DEFAULT 0,
 enablednd tinyint(1) NOT NULL DEFAULT 0,
 enableunc tinyint(1) NOT NULL DEFAULT 0,
 destunc varchar(128) NOT NULL DEFAULT '',
 enablerna tinyint(1) NOT NULL DEFAULT 0,
 destrna varchar(128) NOT NULL DEFAULT '',
 enablebusy tinyint(1) NOT NULL DEFAULT 0,
 destbusy varchar(128) NOT NULL DEFAULT '',
 musiconhold varchar(128) NOT NULL DEFAULT '',
 outcallerid varchar(80) NOT NULL DEFAULT '',
 mobilephonenumber varchar(128) NOT NULL DEFAULT '',
 bsfilter varchar(9) NOT NULL DEFAULT 'no',
 preprocess_subroutine varchar(39),
 internal tinyint(1) NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX userfeatures__idx__firstname ON userfeatures(firstname);
CREATE INDEX userfeatures__idx__lastname ON userfeatures(lastname);
CREATE INDEX userfeatures__idx__number ON userfeatures(number);
CREATE INDEX userfeatures__idx__context ON userfeatures(context);
CREATE INDEX userfeatures__idx__voicemailid ON userfeatures(voicemailid);
CREATE INDEX userfeatures__idx__agentid ON userfeatures(agentid);
CREATE INDEX userfeatures__idx__provisioningid ON userfeatures(provisioningid);
CREATE INDEX userfeatures__idx__loginclient ON userfeatures(loginclient);
CREATE INDEX userfeatures__idx__musiconhold ON userfeatures(musiconhold);
CREATE INDEX userfeatures__idx__internal ON userfeatures(internal);
CREATE INDEX userfeatures__idx__commented ON userfeatures(commented);
CREATE UNIQUE INDEX userfeatures__uidx__protocol_name ON userfeatures(protocol,name);
CREATE UNIQUE INDEX userfeatures__uidx__protocol_protocolid ON userfeatures(protocol,protocolid);

INSERT INTO userfeatures VALUES (1,'sip',1,'Guest','','guest','','xivo-initconfig',NULL,NULL,148378,
				 30,5,0,'','','',0,0,0,0,0,0,0,0,'',0,'',0,'','','','','no',NULL,1,0,'');


DROP TABLE useriax;
CREATE TABLE useriax (
 id integer unsigned,
 name varchar(40) NOT NULL,
 type varchar(6) NOT NULL,
 username varchar(80),
 secret varchar(80) NOT NULL DEFAULT '',
 dbsecret varchar(255) NOT NULL DEFAULT '',
 context varchar(39),
 language varchar(20),
 accountcode varchar(20),
 amaflags varchar(13) DEFAULT 'default',
 mailbox varchar(80),
 callerid varchar(160),
 fullname varchar(80),
 cid_number varchar(80),
 trunk tinyint(1) NOT NULL DEFAULT 0,
 auth varchar(17) NOT NULL DEFAULT 'plaintext,md5',
 encryption varchar(6),
 maxauthreq tinyint unsigned,
 inkeys varchar(80),
 outkey varchar(80),
 adsi tinyint(1),
 transfer varchar(9),
 codecpriority varchar(8),
 jitterbuffer tinyint(1),
 forcejitterbuffer tinyint(1),
 sendani tinyint(1) NOT NULL DEFAULT 0,
 qualify varchar(4) NOT NULL DEFAULT 'no',
 qualifysmoothing tinyint(1) NOT NULL DEFAULT 0,
 qualifyfreqok integer unsigned NOT NULL DEFAULT 60000,
 qualifyfreqnotok integer unsigned NOT NULL DEFAULT 10000,
 timezone varchar(80),
 disallow varchar(100),
 allow varchar(100),
 mohinterpret varchar(80),
 mohsuggest varchar(80),
 deny varchar(31),
 permit varchar(31),
 defaultip varchar(255),
 sourceaddress varchar(255),
 setvar varchar(100) NOT NULL DEFAULT '',
 host varchar(255) NOT NULL DEFAULT 'dynamic',
 port smallint unsigned,
 mask varchar(15),
 regexten varchar(80),
 peercontext varchar(80),
 ipaddr varchar(255) NOT NULL DEFAULT '',
 regseconds integer unsigned NOT NULL DEFAULT 0,
 protocol char(3) NOT NULL DEFAULT 'iax',
 category varchar(5) NOT NULL,
 commented tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE INDEX useriax__idx__mailbox ON useriax(mailbox);
CREATE INDEX useriax__idx__protocol ON useriax(protocol);
CREATE INDEX useriax__idx__category ON useriax(category);
CREATE INDEX useriax__idx__commented ON useriax(commented);
CREATE INDEX useriax__idx__name_host ON useriax(name,host);
CREATE INDEX useriax__idx__name_ipaddr_port ON useriax(name,ipaddr,port);
CREATE INDEX useriax__idx__ipaddr_port ON useriax(ipaddr,port);
CREATE INDEX useriax__idx__host_port ON useriax(host,port);
CREATE UNIQUE INDEX useriax__uidx__name ON useriax(name);


DROP TABLE usersip;
CREATE TABLE usersip (
 id integer unsigned,
 name varchar(40) NOT NULL,
 type varchar(6) NOT NULL,
 username varchar(80),
 secret varchar(80) NOT NULL DEFAULT '',
 md5secret varchar(32) NOT NULL DEFAULT '',
 context varchar(39),
 language varchar(20),
 accountcode varchar(20),
 amaflags varchar(13) NOT NULL DEFAULT 'default',
 allowtransfer tinyint(1),
 fromuser varchar(80),
 fromdomain varchar(255),
 mailbox varchar(80),
 subscribemwi tinyint(1) NOT NULL DEFAULT 1,
 buggymwi tinyint(1),
 "call-limit" tinyint unsigned NOT NULL DEFAULT 0,
 callerid varchar(160),
 fullname varchar(80),
 cid_number varchar(80),
 maxcallbitrate smallint unsigned,
 insecure varchar(11),
 nat varchar(5),
 canreinvite varchar(12),
 promiscredir tinyint(1),
 usereqphone tinyint(1),
 videosupport tinyint(1),
 trustrpid tinyint(1),
 sendrpid tinyint(1),
 allowsubscribe tinyint(1),
 allowoverlap tinyint(1),
 dtmfmode varchar(7),
 rfc2833compensate tinyint(1),
 qualify varchar(4),
 g726nonstandard tinyint(1),
 disallow varchar(100),
 allow varchar(100),
 autoframing tinyint(1),
 mohinterpret varchar(80),
 mohsuggest varchar(80),
 useclientcode tinyint(1),
 progressinband varchar(5),
 t38pt_udptl tinyint(1),
 t38pt_rtp tinyint(1),
 t38pt_tcp tinyint(1),
 t38pt_usertpsource tinyint(1),
 rtptimeout tinyint unsigned,
 rtpholdtimeout tinyint unsigned,
 rtpkeepalive tinyint unsigned,
 deny varchar(31),
 permit varchar(31),
 defaultip varchar(255),
 callgroup varchar(180),
 pickupgroup varchar(180),
 setvar varchar(100) NOT NULL DEFAULT '',
 host varchar(255) NOT NULL DEFAULT 'dynamic',
 port smallint unsigned,
 regexten varchar(80),
 subscribecontext varchar(80),
 fullcontact varchar(255),
 vmexten varchar(40),
 callingpres tinyint(1),
 ipaddr varchar(255) NOT NULL DEFAULT '',
 regseconds integer unsigned NOT NULL DEFAULT 0,
 regserver varchar(20),
 lastms varchar(15) NOT NULL DEFAULT '',
 protocol char(3) NOT NULL DEFAULT 'sip',
 category varchar(5) NOT NULL,
 commented tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE INDEX usersip__idx__mailbox ON usersip(mailbox);
CREATE INDEX usersip__idx__protocol ON usersip(protocol);
CREATE INDEX usersip__idx__category ON usersip(category);
CREATE INDEX usersip__idx__commented ON usersip(commented);
CREATE INDEX usersip__idx__host_port ON usersip(host,port);
CREATE INDEX usersip__idx__ipaddr_port ON usersip(ipaddr,port);
CREATE INDEX usersip__idx__lastms ON usersip(lastms);
CREATE UNIQUE INDEX usersip__uidx__name ON usersip(name);

INSERT INTO usersip VALUES (1,'guest','friend','guest','guest','','xivo-initconfig',NULL,
			    NULL,'default',NULL,NULL,NULL,NULL,0,NULL,0,'Guest',
			    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
			    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
			    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'XIVO_USERID=1',
			    'dynamic',NULL,NULL,NULL,NULL,NULL,NULL,'',0,NULL,'','sip','user',0);


DROP TABLE voicemail;
CREATE TABLE voicemail (
 uniqueid integer unsigned,
 context varchar(39) NOT NULL,
 mailbox varchar(40) NOT NULL,
 password varchar(80) NOT NULL DEFAULT '',
 fullname varchar(80) NOT NULL DEFAULT '',
 email varchar(80),
 pager varchar(80),
 dialout varchar(39),
 callback varchar(39),
 exitcontext varchar(39),
 language varchar(20),
 tz varchar(80),
 attach tinyint(1),
 saycid tinyint(1),
 review tinyint(1),
 operator tinyint(1),
 envelope tinyint(1),
 sayduration tinyint(1),
 saydurationm tinyint unsigned,
 sendvoicemail tinyint(1),
 deletevoicemail tinyint(1) NOT NULL DEFAULT 0,
 forcename tinyint(1),
 forcegreetings tinyint(1),
 hidefromdir varchar(3) NOT NULL DEFAULT 'no',
 maxmsg smallint unsigned,
 commented tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(uniqueid)
);

CREATE INDEX voicemail__idx__context ON voicemail(context);
CREATE INDEX voicemail__idx__commented ON voicemail(commented);
CREATE UNIQUE INDEX voicemail__uidx__mailbox_context ON voicemail(mailbox,context);


DROP TABLE voicemailfeatures;
CREATE TABLE voicemailfeatures (
 id integer unsigned,
 voicemailid integer unsigned,
 skipcheckpass tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE UNIQUE INDEX voicemailfeatures__uidx__voicemailid ON voicemailfeatures(voicemailid);


DROP TABLE voicemenu;
CREATE TABLE voicemenu (
 id integer unsigned,
 name varchar(29) NOT NULL DEFAULT '',
 number varchar(40) NOT NULL,
 context varchar(39) NOT NULL,
 commented tinyint(1) NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX voicemenu__idx__number ON voicemenu(number);
CREATE INDEX voicemenu__idx__context ON voicemenu(context);
CREATE INDEX voicemenu__idx__commented ON voicemenu(commented);
CREATE UNIQUE INDEX voicemenu__uidx__name ON voicemenu(name);

COMMIT;
