/*
 * XiVO Base-Config
 * Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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


-- agent queueskills
DROP TABLE agentqueueskill;
CREATE TABLE agentqueueskill
(
 agentid integer unsigned,
 skillid integer unsigned,
 weight integer unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(agentid, skillid)
);

CREATE INDEX agentqueueskill__idx__agentid ON agentqueueskill(agentid);


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
CREATE INDEX cdr__idx__dstchannel ON cdr(dstchannel);
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


DROP TABLE cticontexts;
CREATE TABLE cticontexts (
 id integer unsigned,
 name varchar(50),
 directories text NOT NULL,
 display text NOT NULL,
 description text NOT NULL,
 deletable tinyint(1),
 PRIMARY KEY(id)
);

INSERT INTO cticontexts VALUES(3,'default','xivodir,internal','Display','Contexte par défaut',1);


DROP TABLE ctidirectories;
CREATE TABLE ctidirectories (
 id integer unsigned,
 name varchar(255),
 uri varchar(255),
 delimiter varchar(20),
 match_direct text NOT NULL,
 match_reverse text NOT NULL,
 field_phone text NOT NULL,
 field_fullname varchar(255),
 field_company varchar(255),
 field_mail varchar(255),
 field_firstname varchar(255),
 field_lastname varchar(255),
 display_reverse varchar(255),
 description varchar(255),
 deletable tinyint(1),
 PRIMARY KEY(id)
);

INSERT INTO ctidirectories VALUES(4,'xivodir','phonebook','','["phonebook.firstname","phonebook.lastname","phonebook.displayname","phonebook.society","phonebooknumber.office.number"]','["phonebooknumber.office.number","phonebooknumber.mobile.number"]','["phonebooknumber.office.number"]','["phonebook.fullname"]','["phonebook.society"]','["phonebook.email"]','["phonebook.firstname"]','["phonebook.lastname"]','["{db-fullname}"]','Répertoire XiVO Externe',1);
INSERT INTO ctidirectories VALUES(5,'internal','internal','','','','','["{internal-fullname}"]','','','','','','Répertoire XiVO Interne',1);


DROP TABLE ctidisplays;
CREATE TABLE ctidisplays (
 id integer unsigned,
 name varchar(50),
 data text NOT NULL,
 deletable tinyint(1),
 description text NOT NULL,
 PRIMARY KEY(id)
);

INSERT INTO ctidisplays VALUES(4,'Display','{"10": [ "Numéro","phone","","{db-phone}" ],"20": [ "Nom","","","{db-fullname}" ],"30": [ "Entreprise","","Inconnue","{db-company}" ],"40": [ "E-mail","","","{db-mail}({xivo-directory})" ]}',1,'Affichage par défaut');


DROP TABLE ctimain;
CREATE TABLE ctimain (
 id integer unsigned,
 commandset varchar(20),
 fagi_ip varchar(255),
 fagi_port integer unsigned,
 cti_ip varchar(255),
 cti_port integer unsigned,
 webi_ip varchar(255),
 webi_port integer unsigned,
 info_ip varchar(255),
 info_port integer unsigned,
 announce_ip varchar(255),
 announce_port integer unsigned,
 asterisklist varchar(255),
 updates_period integer unsigned,
 socket_timeout integer unsigned,
 login_timeout integer unsigned,
 parting_astid_context varchar(255),
 PRIMARY KEY(id)
);

INSERT INTO ctimain VALUES(1, 'xivocti', '0.0.0.0', 5002, '0.0.0.0', 5003, '127.0.0.1', 5004, '127.0.0.1', 5005, '127.0.0.1', 5006, 1, 60, 10, 5, 'context');


DROP TABLE ctiphonehints;
CREATE TABLE ctiphonehints (
 id integer unsigned,
 number integer,
 name varchar(255),
 color varchar(128),
 PRIMARY KEY(id)
);

INSERT INTO ctiphonehints VALUES(1,-2,'Inexistant','#030303');
INSERT INTO ctiphonehints VALUES(2,-1,'Désactivé','#000000');
INSERT INTO ctiphonehints VALUES(3,0,'Disponible','#0DFF25');
INSERT INTO ctiphonehints VALUES(4,1,'En ligne OU appelle','#FF032D');
INSERT INTO ctiphonehints VALUES(5,2,'Occupé','#FF0008');
INSERT INTO ctiphonehints VALUES(6,4,'Indisponible','#FFFFFF');
INSERT INTO ctiphonehints VALUES(7,8,'Sonne','#1B0AFF');
INSERT INTO ctiphonehints VALUES(8,9,'(En Ligne OU Appelle) ET Sonne','#FF0526');
INSERT INTO ctiphonehints VALUES(9,16,'En Attente','#F7FF05');


DROP TABLE ctipresences;
CREATE TABLE ctipresences (
 id integer unsigned,
 name varchar(255),
 description varchar(255),
 deletable tinyint(1),
 PRIMARY KEY(id)
);

INSERT INTO ctipresences VALUES(1,'xivo','De base non supprimable',0);


DROP TABLE ctiprofiles;
CREATE TABLE ctiprofiles (
 id integer unsigned,
 xlets text,
 funcs varchar(255),
 maxgui integer,
 appliname varchar(255),
 name varchar(40) unique,
 presence varchar(255),
 services varchar(255),
 preferences varchar(2048),
 deletable tinyint(1),
 PRIMARY KEY(id)
);

INSERT INTO ctiprofiles VALUES(9,'[[ "queues", "dock", "fms", "N/A" ],[ "queuedetails", "dock", "fms", "N/A" ],[ "queueentrydetails", "dock", "fcms", "N/A" ],[ "agents", "dock", "fcms", "N/A" ],[ "agentdetails", "dock", "fcms", "N/A" ],[ "identity", "grid", "fcms", "0" ],[ "conference", "dock", "fcm", "N/A" ]]','agents,presence,switchboard',-1,'Superviseur','agentsup','xivo','','',1);
INSERT INTO ctiprofiles VALUES(10,'[[ "queues", "dock", "ms", "N/A" ],[ "identity", "grid", "fcms", "0" ],[ "customerinfo", "dock", "cms", "N/A" ],[ "agentdetails", "dock", "cms", "N/A" ]]','presence',-1,'Agent','agent','xivo','','',1);
INSERT INTO ctiprofiles VALUES(11,'[[ "tabber", "grid", "fcms", "N/A" ],[ "dial", "grid", "fcms", "2" ],[ "search", "tab", "fcms", "0" ],[ "customerinfo", "tab", "fcms", "4" ],[ "identity", "grid", "fcms", "0" ],[ "fax", "tab", "fcms", "N/A" ],[ "history", "tab", "fcms", "N/A" ],[ "directory", "tab", "fcms", "N/A" ],[ "features", "tab", "fcms", "N/A" ],[ "mylocaldir", "tab", "fcms", "N/A" ],[ "conference", "tab", "fcms", "N/A" ]]','presence,customerinfo',-1,'Client','client','xivo','','',1);
INSERT INTO ctiprofiles VALUES(12,'[[ "tabber", "grid", "fcms", "N/A" ],[ "dial", "grid", "fcms", "2" ],[ "search", "tab", "fcms", "0" ],[ "customerinfo", "tab", "fcms", "4" ],[ "identity", "grid", "fcms", "0" ],[ "fax", "tab", "fcms", "N/A" ],[ "history", "tab", "fcms", "N/A" ],[ "directory", "tab", "fcms", "N/A" ],[ "features", "tab", "fcms", "N/A" ],[ "mylocaldir", "tab", "fcms", "N/A" ],[ "conference", "tab", "fcms", "N/A" ],[ "outlook", "tab", "fcms", "N/A" ]]','presence,customerinfo',-1,'Client+Outlook','clientoutlook','xivo','','',1);
INSERT INTO ctiprofiles VALUES(13,'[[ "datetime", "dock", "fm", "N/A" ]]','',-1,'Horloge','clock','xivo','','',1);
INSERT INTO ctiprofiles VALUES(14,'[[ "dial", "dock", "fm", "N/A" ],[ "operator", "dock", "fcm", "N/A" ],[ "datetime", "dock", "fcm", "N/A" ],[ "identity", "grid", "fcms", "0" ],[ "calls", "dock", "fcm", "N/A" ],[ "parking", "dock", "fcm", "N/A" ],[ "calls", "dock", "fcm", "N/A" ]]','presence,switchboard,search,dial',-1,'Opérateur','oper','xivo','','',1);
INSERT INTO ctiprofiles VALUES(15,'[[ "parking", "dock", "fcms", "N/A" ],[ "search", "dock", "fcms", "N/A" ],[ "calls", "dock", "fcms", "N/A" ],[ "switchboard", "dock", "fcms", "N/A" ],[ "customerinfo", "dock", "fcms", "N/A" ],[ "datetime", "dock", "fcms", "N/A" ],[ "dial", "dock", "fcms", "N/A" ],[ "identity", "grid", "fcms", "0" ],[ "messages", "dock", "fcms", "N/A" ],[ "operator", "dock", "fcms", "N/A" ]]','switchboard,dial,presence,customerinfo,search,agents,conference,directory,features,history,fax,chitchat,database','','Switchboard','switchboard','xivo','','',1);


DROP TABLE ctireversedirectories;
CREATE TABLE ctireversedirectories (
 id integer unsigned,
 number varchar(50),
 directories text NOT NULL,
 description text NOT NULL,
 deletable tinyint(1),
 PRIMARY KEY(id)
);

INSERT INTO ctireversedirectories VALUES(1,'default','["xivodir","internal"]','Répertoires XiVO',1);


DROP TABLE ctisheetactions;
CREATE TABLE ctisheetactions (
 id integer unsigned,
 name varchar(50),
 description text NOT NULL,
 context varchar(50),
 whom varchar(50),
 capaids text NOT NULL,
 sheet_info varchar(50),
 systray_info varchar(50),
 sheet_qtui varchar(50),
 action_info varchar(50),
 focus tinyint(1),
 deletable tinyint(1),
 PRIMARY KEY(id)
);

INSERT INTO ctisheetactions VALUES(6,'dial','','["default"]','dest','["agentsup","agent","client"]','{"10": [ "","text","Inconnu","Appel {xivo-direction} de {xivo-calleridnum}" ],"20": [ "Numéro entrant","phone","Inconnu","{xivo-calleridnum}" ],"30": [ "Nom","text","Inconnu","{db-fullname}" ],"40": [ "Numéro appelé","phone","Inconnu","{xivo-calledidnum}" ]}','{"10": [ "","title","","Appel {xivo-direction}" ],"20": [ "","body","Inconnu","appel de {xivo-calleridnum} pour {xivo-calledidnum}" ],"30": [ "","body","Inconnu","{db-fullname} (selon {xivo-directory})" ],"40": [ "","body","","le {xivo-date}, il est {xivo-time}" ]}','','{"10": [ "","urlauto","","http://www.google.fr/search?q={xivo-calleridnum}" ]}','',1);
INSERT INTO ctisheetactions VALUES(7,'queue','','["default"]','dest','["agentsup","agent","client"]','{"10": [ "","text","Inconnu","Appel {xivo-direction} de la File {xivo-queuename}" ],"20": [ "Numéro entrant","phone","Inconnu","{xivo-calleridnum}" ],"30": [ "Nom","text","Inconnu","{db-fullname}" ]}','{"10": [ "","title","","Appel {xivo-direction} de la File {xivo-queuename}" ],"20": [ "","body","Inconnu","appel de {xivo-calleridnum} pour {xivo-calledidnum}" ],"30": [ "","body","Inconnu","{db-fullname} (selon {xivo-directory})" ],"40": [ "","body","","le {xivo-date}, il est {xivo-time}" ]}','file:///etc/pf-xivo/ctiservers/form.ui','{}','',1);
INSERT INTO ctisheetactions VALUES(8,'custom1','','["default"]','all','["agentsup","agent","client"]','{"10": [ "","text","Inconnu","Appel {xivo-direction} (Custom)" ],"20": [ "Numéro entrant","phone","Inconnu","{xivo-calleridnum}" ],"30": [ "Nom","text","Inconnu","{db-fullname}" ]}','{"10": [ "","title","","Appel {xivo-direction} (Custom)" ],"20": [ "","body","Inconnu","appel de {xivo-calleridnum} pour {xivo-calledidnum}" ],"30": [ "","body","Inconnu","{db-fullname} (selon {xivo-directory})" ],"40": [ "","body","","le {xivo-date}, il est {xivo-time}" ]}','','{}','',1);


DROP TABLE ctisheetevents;
CREATE TABLE ctisheetevents (
 id integer unsigned,
 agentlinked varchar(50),
 agentunlinked varchar(50),
 faxreceived varchar(50),
 incomingqueue varchar(50),
 incominggroup varchar(50),
 incomingdid varchar(50),
 dial varchar(50),
 link varchar(50),
 unlink varchar(50),
 custom text NOT NULL,
 PRIMARY KEY(id)
);

INSERT INTO ctisheetevents VALUES(1,'','','','','','','dial','','','{"custom-example1": "custom1"}');


DROP TABLE ctistatus;
CREATE TABLE ctistatus (
 id integer unsigned,
 presence_id integer unsigned,
 name varchar(255),
 display_name varchar(255),
 actions varchar(255),
 color varchar(20),
 access_status varchar(255),
 deletable tinyint(1),
 PRIMARY KEY(id)
);

INSERT INTO ctistatus VALUES(1,1,'available','Disponible','enablednd(false)','#08FD20','1,2,6',1);
INSERT INTO ctistatus VALUES(2,1,'away','Parti loin','enablednd(true)','#FDE50A','1,6',1);
INSERT INTO ctistatus VALUES(6,1,'outtolunch','Parti Manger','enablednd(true)','#001AFF','1,2,6',1);
INSERT INTO ctistatus VALUES(7,2,'sdsds','dsd','','#FFFFFF','',1);
INSERT INTO ctistatus VALUES(8,2,'outtolunch','sdf','','#FFFFFF','',1);
INSERT INTO ctistatus VALUES(9,3,'outtolunch','Miam','','#FFB163','',1);
INSERT INTO ctistatus VALUES(10,1,'donotdisturb','Ne pas déranger','enablednd(true)','#FF032D','1,2,6',1);
INSERT INTO ctistatus VALUES(11,1,'berightback','Bientôt de retour','enablednd(true)','#FFB545','1,2,6,10',1);


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

INSERT INTO extensions VALUES (NULL,1,'xivo-features','_*33.',1,'Macro','agentdynamiclogin|${EXTEN:3}','agentdynamiclogin');
INSERT INTO extensions VALUES (NULL,1,'xivo-features','_*31.',1,'Macro','agentstaticlogin|${EXTEN:3}','agentstaticlogin');
INSERT INTO extensions VALUES (NULL,1,'xivo-features','_*32.',1,'Macro','agentstaticlogoff|${EXTEN:3}','agentstaticlogoff');
INSERT INTO extensions VALUES (NULL,1,'xivo-features','_*30.',1,'Macro','agentstaticlogtoggle|${EXTEN:3}','agentstaticlogtoggle');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*37.',1,'Macro','bsfilter|${EXTEN:3}','bsfilter');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*664.',1,'Macro','group|${EXTEN:4}|','callgroup');
INSERT INTO extensions VALUES (NULL,1,'xivo-features','*34',1,'Macro','calllistening','calllistening');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*667.',1,'Macro','meetme|${EXTEN:4}|','callmeetme');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*665.',1,'Macro','queue|${EXTEN:4}|','callqueue');
INSERT INTO extensions VALUES (NULL,1,'xivo-features','*26',1,'Macro','callrecord','callrecord');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*666.',1,'Macro','user|${EXTEN:4}|','calluser');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*36',1,'Directory','${CONTEXT}','directoryaccess');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*25',1,'Macro','enablednd','enablednd');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*90',1,'Macro','enablevm','enablevm');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*91',1,'Macro','enablevmbox','enablevmbox');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*91.',1,'Macro','enablevmbox|${EXTEN:3}','enablevmboxslt');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*90.',1,'Macro','enablevm|${EXTEN:3}','enablevmslt');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*23.',1,'Macro','feature_forward|busy|${EXTEN:3}','fwdbusy');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*22.',1,'Macro','feature_forward|rna|${EXTEN:3}','fwdrna');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*21.',1,'Macro','feature_forward|unc|${EXTEN:3}','fwdunc');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*20',1,'Macro','fwdundoall','fwdundoall');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*51.',1,'Macro','groupmember|group|add|${EXTEN:3}','groupaddmember');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*52.',1,'Macro','groupmember|group|remove|${EXTEN:3}','groupremovemember');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*50.',1,'Macro','groupmember|group|toggle|${EXTEN:3}','grouptogglemember');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*48378',1,'Macro','guestprov','guestprov');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*27',1,'Macro','incallfilter','incallfilter');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*735.',1,'Macro','phoneprogfunckey|${EXTEN:0:4}|${EXTEN:4}','phoneprogfunckey');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*10',1,'Macro','phonestatus','phonestatus');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*8.',1,'Pickup','${EXTEN:2}%${CONTEXT}@PICKUPMARK','pickup');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*56.',1,'Macro','groupmember|queue|add|${EXTEN:3}','queueaddmember');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*57.',1,'Macro','groupmember|queue|remove|${EXTEN:3}','queueremovemember');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*55.',1,'Macro','groupmember|queue|toggle|${EXTEN:3}','queuetogglemember');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*9',1,'Macro','recsnd|wav','recsnd');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*99.',1,'Macro','vmboxmsg|${EXTEN:3}','vmboxmsgslt');
INSERT INTO extensions VALUES (NULL,1,'xivo-features','_*93.',1,'Macro','vmboxpurge|${EXTEN:3}','vmboxpurgeslt');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*97.',1,'Macro','vmbox|${EXTEN:3}','vmboxslt');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','*98',1,'Macro','vmusermsg','vmusermsg');
INSERT INTO extensions VALUES (NULL,1,'xivo-features','*92',1,'Macro','vmuserpurge','vmuserpurge');
INSERT INTO extensions VALUES (NULL,1,'xivo-features','_*92.',1,'Macro','vmuserpurge|${EXTEN:3}','vmuserpurgeslt');
INSERT INTO extensions VALUES (NULL,0,'xivo-features','_*96.',1,'Macro','vmuser|${EXTEN:3}','vmuserslt');


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
INSERT INTO extenumbers VALUES (NULL,'_*33.','269371911e5bac9176919fa42e66814882c496e1','','extenfeatures','agentdynamiclogin');
INSERT INTO extenumbers VALUES (NULL,'_*31.','678fe23ee0d6aa64460584bebbed210e270d662f','','extenfeatures','agentstaticlogin');
INSERT INTO extenumbers VALUES (NULL,'_*32.','3ae0f1ff0ef4907faa2dad5da7bb891c9dbf45ad','','extenfeatures','agentstaticlogoff');
INSERT INTO extenumbers VALUES (NULL,'_*30.','7758898081b262cc0e42aed23cf601fba8969b08','','extenfeatures','agentstaticlogtoggle');
INSERT INTO extenumbers VALUES (NULL,'_*37.','249b00b17a5983bbb2af8ed0af2ab1a74abab342','','extenfeatures','bsfilter');
INSERT INTO extenumbers VALUES (NULL,'_*664.','9dfe780f1dc7fccbfc841b41a38933d4dab56369','','extenfeatures','callgroup');
INSERT INTO extenumbers VALUES (NULL,'*34','668a8d2d8fe980b663e2cdcecb977860e1b272f3','','extenfeatures','calllistening');
INSERT INTO extenumbers VALUES (NULL,'_*667.','666f6f18439eb7f205b5932d7f9aef6d2e5ba9a3','','extenfeatures','callmeetme');
INSERT INTO extenumbers VALUES (NULL,'_*665.','7e2df45aedebded219eaa5fb84d6db7e8e24fc66','','extenfeatures','callqueue');
INSERT INTO extenumbers VALUES (NULL,'*26','f8aeb70618cc87f1143c7dff23cdc0d3d0a48a0c','','extenfeatures','callrecord');
INSERT INTO extenumbers VALUES (NULL,'_*666.','d7b68f456ddb50215670c5bfca921176a21c4270','','extenfeatures','calluser');
INSERT INTO extenumbers VALUES (NULL,'*36','f9b69fe3c361ddfc2ae49e048460ea197ea850c8','','extenfeatures','directoryaccess');
INSERT INTO extenumbers VALUES (NULL,'*25','c0d236c38bf8d5d84a2e154203cd2a18b86c6b2a','','extenfeatures','enablednd');
INSERT INTO extenumbers VALUES (NULL,'*90','2fc9fcda52bd8293da1bfa68cbdb8974fafd409e','','extenfeatures','enablevm');
INSERT INTO extenumbers VALUES (NULL,'*91','880d3330b465056ede825e1fbc8ceb50fd816e1d','','extenfeatures','enablevmbox');
INSERT INTO extenumbers VALUES (NULL,'_*91.','936ec7abe6019d9d47d8be047ef6fc0ebc334c00','','extenfeatures','enablevmboxslt');
INSERT INTO extenumbers VALUES (NULL,'_*90.','9fdaa61ea338dcccf1450949cbf6f7f99f1ccc54','','extenfeatures','enablevmslt');
INSERT INTO extenumbers VALUES (NULL,'_*23.','a1968a70f1d265b8aa263e73c79259961c4f7bbb','','extenfeatures','fwdbusy');
INSERT INTO extenumbers VALUES (NULL,'_*22.','00638af9e028d4cd454c00f43caf5626baa7d84c','','extenfeatures','fwdrna');
INSERT INTO extenumbers VALUES (NULL,'_*21.','52c97d56ebcca524ccf882590e94c52f6db24649','','extenfeatures','fwdunc');
INSERT INTO extenumbers VALUES (NULL,'*20','934aca632679075488681be0e9904cf9102f8766','','extenfeatures','fwdundoall');
INSERT INTO extenumbers VALUES (NULL,'_*51.','fd3d50358d246ab2fbc32e14056e2f559d054792','','extenfeatures','groupaddmember');
INSERT INTO extenumbers VALUES (NULL,'_*52.','069a278d266d0cf2aa7abf42a732fc5ad109a3e6','','extenfeatures','groupremovemember');
INSERT INTO extenumbers VALUES (NULL,'_*50.','53f7e7fa7fbbabb1245ed8dedba78da442a8659f','','extenfeatures','grouptogglemember');
INSERT INTO extenumbers VALUES (NULL,'*48378','e27276ceefcc71a5d2def28c9b59a6410959eb43','','extenfeatures','guestprov');
INSERT INTO extenumbers VALUES (NULL,'*27','663b9615ba92c21f80acac52d60b28a8d1fb1c58','','extenfeatures','incallfilter');
INSERT INTO extenumbers VALUES (NULL,'_*735.','32e9b3597f8b9cd2661f0c3d3025168baafca7e6','','extenfeatures','phoneprogfunckey');
INSERT INTO extenumbers VALUES (NULL,'*10','eecefbd85899915e6fc2ff5a8ea44c2c83597cd6','','extenfeatures','phonestatus');
INSERT INTO extenumbers VALUES (NULL,'_*8.','b349d094036a97a7e0631ba60de759a9597c1c3a','','extenfeatures','pickup');
INSERT INTO extenumbers VALUES (NULL,'_*56.','95d84232b10af6f6905dcd22f4261a4550461c7d','','extenfeatures','queueaddmember');
INSERT INTO extenumbers VALUES (NULL,'_*57.','3ad1e945e85735f6417e5a0aba7fde3bc9d2ffec','','extenfeatures','queueremovemember');
INSERT INTO extenumbers VALUES (NULL,'_*55.','f8085e23f56e5433006483dee5fe3db8c94a0a06','','extenfeatures','queuetogglemember');
INSERT INTO extenumbers VALUES (NULL,'*9','e28d0f359da60dcf86340435478b19388b1b1d05','','extenfeatures','recsnd');
INSERT INTO extenumbers VALUES (NULL,'_*99.','6c92223f2ea0cfd9fad3db2f288ebdc9c64dc8f5','','extenfeatures','vmboxmsgslt');
INSERT INTO extenumbers VALUES (NULL,'_*93.','7d891f90799fd6cb5bc85c4bd227a3357096be8f','','extenfeatures','vmboxpurgeslt');
INSERT INTO extenumbers VALUES (NULL,'_*97.','8bdbf6703cf5225aad457422afdda738b9bd628c','','extenfeatures','vmboxslt');
INSERT INTO extenumbers VALUES (NULL,'*98','6fb653e9eaf6f4d9c8d2cb48d1a6e3f4d4085710','','extenfeatures','vmusermsg');
INSERT INTO extenumbers VALUES (NULL,'*92','97f991a4ffd7fa843bc0ca3bdc730851382c5cdf','','extenfeatures','vmuserpurge');
INSERT INTO extenumbers VALUES (NULL,'_*92.','36711086667cbfc27488236e0e0fdd2d7f896f6b','','extenfeatures','vmuserpurgeslt');
INSERT INTO extenumbers VALUES (NULL,'_*96.','ac6c7ac899867fe0120fe20120fae163012615f2','','extenfeatures','vmuserslt');


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
CREATE INDEX groupfeatures__idx__number ON groupfeatures(number);
CREATE INDEX groupfeatures__idx__context ON groupfeatures(context);
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
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX incall__idx__exten ON incall(exten);
CREATE INDEX incall__idx__context ON incall(context);
CREATE INDEX incall__idx__commented ON incall(commented);
CREATE UNIQUE INDEX incall__uidx__exten_context ON incall(exten,context);


DROP TABLE ldapfilter;
CREATE TABLE ldapfilter (
 id integer unsigned,
 ldapserverid integer unsigned NOT NULL,
 name varchar(128) NOT NULL DEFAULT '',
 user varchar(255),
 passwd varchar(255),
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
 meetmeid integer unsigned NOT NULL,
 name varchar(80) NOT NULL,
 number varchar(40) NOT NULL,
 context varchar(39) NOT NULL,
 admin_typefrom varchar(9),
 admin_internalid integer unsigned,
 admin_externalid varchar(40),
 admin_identification varchar(11) NOT NULL,
 admin_mode varchar(6) NOT NULL,
 admin_announceusercount tinyint(1) NOT NULL DEFAULT 0,
 admin_announcejoinleave varchar(8) NOT NULL,
 admin_moderationmode tinyint(1) NOT NULL DEFAULT 0,
 admin_initiallymuted tinyint(1) NOT NULL DEFAULT 0,
 admin_musiconhold varchar(128),
 admin_poundexit tinyint(1) NOT NULL DEFAULT 0,
 admin_quiet tinyint(1) NOT NULL DEFAULT 0,
 admin_starmenu tinyint(1) NOT NULL DEFAULT 0,
 admin_closeconflastmarkedexit tinyint(1) NOT NULL DEFAULT 0,
 admin_enableexitcontext tinyint(1) NOT NULL DEFAULT 0,
 admin_exitcontext varchar(39),
 user_mode varchar(6) NOT NULL,
 user_announceusercount tinyint(1) NOT NULL DEFAULT 0,
 user_hiddencalls tinyint(1) NOT NULL DEFAULT 0,
 user_announcejoinleave varchar(8) NOT NULL,
 user_initiallymuted tinyint(1) NOT NULL DEFAULT 0,
 user_musiconhold varchar(128),
 user_poundexit tinyint(1) NOT NULL DEFAULT 0,
 user_quiet tinyint(1) NOT NULL DEFAULT 0,
 user_starmenu tinyint(1) NOT NULL DEFAULT 0,
 user_enableexitcontext tinyint(1) NOT NULL DEFAULT 0,
 user_exitcontext varchar(39),
 talkeroptimization tinyint(1) NOT NULL DEFAULT 0,
 record tinyint(1) NOT NULL DEFAULT 0,
 talkerdetection tinyint(1) NOT NULL DEFAULT 0,
 noplaymsgfirstenter tinyint(1) NOT NULL DEFAULT 0,
 durationm smallint unsigned,
 closeconfdurationexceeded tinyint(1) NOT NULL DEFAULT 0,
 nbuserstartdeductduration tinyint unsigned,
 timeannounceclose smallint unsigned,
 maxuser tinyint unsigned,
 startdate char(19),
 emailfrom varchar(255),
 emailfromname varchar(255),
 emailsubject varchar(255),
 emailbody text NOT NULL,
 preprocess_subroutine varchar(39),
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX meetmefeatures__idx__number ON meetmefeatures(number);
CREATE INDEX meetmefeatures__idx__context ON meetmefeatures(context);
CREATE UNIQUE INDEX meetmefeatures__uidx__meetmeid ON meetmefeatures(meetmeid);
CREATE UNIQUE INDEX meetmefeatures__uidx__name ON meetmefeatures(name);


DROP TABLE meetmeguest;
CREATE TABLE meetmeguest (
 id integer unsigned,
 meetmefeaturesid integer unsigned NOT NULL,
 fullname varchar(255) NOT NULL,
 telephonenumber varchar(40),
 email varchar(320),
 PRIMARY KEY(id)
);

CREATE INDEX meetmeguest__idx__meetmefeaturesid ON meetmeguest(meetmefeaturesid);
CREATE INDEX meetmeguest__idx__fullname ON meetmeguest(fullname);
CREATE INDEX meetmeguest__idx__email ON meetmeguest(email);


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
 preprocess_subroutine varchar(39),
 hangupringtime smallint unsigned NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX outcall__idx__exten ON outcall(exten);
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
 label varchar(32),
 supervision tinyint(1) NOT NULL DEFAULT 0,
 progfunckey tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(iduserfeatures,fknum)
);

CREATE INDEX phonefunckey__idx__exten ON phonefunckey(exten);
CREATE INDEX phonefunckey__idx__progfunckey ON phonefunckey(progfunckey);
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
 ringinuse tinyint(1) NOT NULL DEFAULT 0,
 reportholdtime tinyint(1) NOT NULL DEFAULT 0,
 memberdelay integer unsigned,
 weight integer unsigned,
 timeoutrestart tinyint(1) NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 category char(5) NOT NULL,
 autopause tinyint(1) NOT NULL DEFAULT 0,
 setinterfacevar tinyint(1) NOT NULL DEFAULT 0,
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

CREATE INDEX queuefeatures__idx__number ON queuefeatures(number);
CREATE INDEX queuefeatures__idx__context ON queuefeatures(context);
CREATE UNIQUE INDEX queuefeatures__uidx__name ON queuefeatures(name);


DROP TABLE queuemember;
CREATE TABLE queuemember (
 queue_name varchar(128) NOT NULL,
 interface varchar(128) NOT NULL,
 penalty tinyint unsigned NOT NULL DEFAULT 0,
 "call-limit" tinyint unsigned NOT NULL DEFAULT 0,
 paused tinyint unsigned,
 commented tinyint(1) NOT NULL DEFAULT 0,
 usertype varchar(5) NOT NULL,
 userid integer unsigned NOT NULL,
 channel varchar(25) NOT NULL,
 category char(5) NOT NULL,
 skills char(64) NOT NULL DEFAULT '',
 state_interface varchar(128) NOT NULL DEFAULT '',
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
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','language','fr_FR');
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
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','requirecalltoken','no');
-- warning: asterisk crash if set to NULL value
INSERT INTO staticiax VALUES (NULL,0,0,0,'iax.conf','general','calltokenoptional','0.0.0.0');


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
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','useragent','XiVO PBX');
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
INSERT INTO staticsip VALUES (NULL,0,0,0,'sip.conf','general','language','fr_FR');
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
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','fromstring','XiVO PBX');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','emaildateformat','%A %d %B %Y à %H:%M:%S %Z');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','emaildatelocale','fr_FR.UTF-8');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','pbxskip','no');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','emailsubject','Messagerie XiVO');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','emailbody','Bonjour ${VM_NAME} !

Vous avez reçu un message d''une durée de ${VM_DUR} minute(s), il vous reste actuellement ${VM_MSGNUM} message(s) non lu(s) sur votre messagerie vocale : ${VM_MAILBOX}.

Le dernier a été envoyé par ${VM_CALLERID}, le ${VM_DATE}. Si vous le souhaitez vous pouvez l''écouter ou le consulter en tapant le *98 sur votre téléphone.

Merci.

-- Messagerie XiVO --');
INSERT INTO staticvoicemail VALUES (NULL,0,0,0,'voicemail.conf','general','pagerfromstring','XiVO PBX');
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

DROP TABLE staticsccp;
CREATE TABLE staticsccp (
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
CREATE INDEX staticsccp__idx__category ON staticsccp(category);
CREATE INDEX staticsccp__idx__commented ON staticsccp(commented);
CREATE INDEX staticsccp__idx__filename ON staticsccp(filename);
CREATE INDEX staticsccp__idx__var_name ON staticsccp(var_name);

INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','servername','Asterisk');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','keepalive',60);
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','debug','');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','dateFormat','D.M.Y');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','port',2000);
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','firstdigittimeout',16);
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','digittimeout',8);
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','autoanswer_ring_time',0);
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','autoanswer_tone','0x32');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','remotehangup_tone','0x32');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','transfer_tone',0);
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','callwaiting_tone','0x2d');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','musicclass','default');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','dnd','on');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','sccp_tos','0x68');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','sccp_cos',4);
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','audio_tos','0xB8');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','audio_cos',6);
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','video_tos','0x88');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','video_cos',5);
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','echocancel','on');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','silencesuppression','off');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','trustphoneip','no');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','private','on');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','protocolversion',11);
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','disallow','all');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','language','fr_FR');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','hotline_enabled','yes');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','hotline_context','xivo-initconfig');
INSERT INTO staticsccp VALUES(NULL,0,0,0,'sccp.conf','general','hotline_extension','sccp');


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
 protocol char(6) NOT NULL DEFAULT 'custom',
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
 incallfilter tinyint(1) NOT NULL DEFAULT 0,
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
 timezone varchar(128),
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
 requirecalltoken char(4) NOT NULL DEFAULT 'no',
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

-- queueskill categories
DROP TABLE queueskillcat;
CREATE TABLE queueskillcat (
 id integer unsigned,
 name varchar(64) NOT NULL DEFAULT '',
 PRIMARY KEY(id)
);

CREATE UNIQUE INDEX queueskillcat__uidx__name ON queueskillcat(name);

-- queueskill values
DROP TABLE queueskill;
CREATE TABLE queueskill (
 id integer unsigned,
 catid integer unsigned NOT NULL DEFAULT 1,
 name varchar(64) NOT NULL DEFAULT '',
 description text,
 printscreen varchar(5),
 PRIMARY KEY(id)
);

CREATE INDEX queueskill__idx__catid ON queueskill(catid);
CREATE UNIQUE INDEX queueskill__uidx__name ON queueskill(name);

-- queueskill rules;
DROP TABLE queueskillrule;
CREATE TABLE queueskillrule
(
 id integer unsigned,
 name varchar(64) NOT NULL DEFAULT '',
 rule text,
 PRIMARY KEY(id)
);

-- user queueskills
DROP TABLE userqueueskill;
CREATE TABLE userqueueskill
(
 userid integer unsigned,
 skillid integer unsigned,
 weight integer unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(userid, skillid)
);

CREATE INDEX userqueueskill__idx__userid ON userqueueskill(userid);


-- http://chan-sccp-b.sourceforge.net/doc/structsccp__device.html
DROP TABLE usersccp;
CREATE TABLE usersccp
(
 id integer unsigned,
 name varchar(128),
 devicetype varchar(64),            -- phone model, ie 7960
 keepalive tinyint unsigned,        -- i.e 60
 tzoffset varchar(3),               -- ie: +1 == Europe/Paris
 dtmfmode varchar(16),               -- outofband, inband
 transfer varchar(3),                -- on, off, NULL
 park varchar(3),                    -- on, off, NULL
 cfwdall varchar(3),                 -- on, off, NULL
 cfwdbusy varchar(3),                -- on, off, NULL
 cfwdnoanswer varchar(3),            -- on, off, NULL
 mwilamp varchar(3),                 -- on, off, NULL
 mwioncall varchar(3),               -- on, off, NULL
 dnd varchar(6),                     -- on, off, NULL
 pickupexten varchar(3),             -- on, off, NULL
 pickupcontext varchar(64),          -- pickup context name
 pickupmodeanswer varchar(3),        -- on, off, NULL
 permit varchar(31),                 -- 192.168.0.0/255.255.255.0
 deny varchar(31),                   -- 0.0.0.0/0.0.0.0
 addons varchar(24),                 -- comma separated addons list. i.e 7914,7914
 imageversion varchar(64),           -- i.e P00405000700
 trustphoneip varchar(3),            -- yes, no, NULL
 nat varchar(3),                     -- on, off, NULL
 directrtp varchar(3),               -- on, off, NULL
 earlyrtp varchar(3),                -- on, off, NULL
 private varchar(3),                 -- on, off, NULL
 privacy varchar(4),                 -- on, off, full, NULL
 protocol varchar(4) NOT NULL DEFAULT 'sccp', -- required for join with userfeatures

 -- softkeys
 softkey_onhook      varchar(1024),
 softkey_connected   varchar(1024),
 softkey_onhold      varchar(1024),
 softkey_ringin      varchar(1024),
 softkey_offhook     varchar(1024),
 softkey_conntrans   varchar(1024),
 softkey_digitsfoll  varchar(1024),
 softkey_connconf    varchar(1024),
 softkey_ringout     varchar(1024),
 softkey_offhookfeat varchar(1024),
 softkey_onhint      varchar(1024),
           
 defaultline integer unsigned,
 commented tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE UNIQUE INDEX usersccp__uidx__name ON usersccp(name);
--INSERT INTO usersccp VALUES (NULL, 'SEP00164766A428', '7960', '2', 'inband', 'on', 'on', 'on', 'on', '', 0)

-- http://chan-sccp-b.sourceforge.net/doc/structsccp__line.html
DROP TABLE sccpline;
CREATE TABLE sccpline
(
 id integer unsigned,
 name varchar(80) NOT NULL,
 pin varchar(8) NOT NULL DEFAULT '',
 label varchar(128) NOT NULL DEFAULT '',
 description text,
 context varchar(64),
 incominglimit tinyint unsigned,
 transfer varchar(3) DEFAULT 'on',                    -- on, off, NULL
 mailbox varchar(64) DEFAULT NULL,
 vmnum varchar(64) DEFAULT NULL,
 meetmenum varchar(64) DEFAULT NULL,
 cid_name varchar(64) NOT NULL DEFAULT '',
 cid_num varchar(64) NOT NULL DEFAULT '',
 trnsfvm varchar(64),
 secondary_dialtone_digits varchar(10),
 secondary_dialtone_tone integer unsigned,
 musicclass varchar(32),
 language varchar(32),                                 -- en, fr, ...
 accountcode varchar(32),
 audio_tos varchar(8),
 audio_cos integer unsigned,
 video_tos varchar(8),
 video_cos integer unsigned,
 echocancel varchar(3) DEFAULT 'on',                   -- on, off, NULL
 silencesuppression varchar(3) DEFAULT 'on',           -- on, off, NULL
 callgroup varchar(64) DEFAULT '',                     -- i.e: 1,4-9
 pickupgroup varchar(64) DEFAULT '',                   -- i.e: 1,3-9
 amaflags varchar(16) DEFAULT '',                      -- default, omit, billing, documentation
 adhocnumber varchar(64),
 setvar varchar(512) DEFAULT '',
 commented tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE UNIQUE INDEX sccpline__uidx__name ON sccpline(name);

-- INSERT INTO sccpline (id, name, pin, label) VALUES (NULL, '160', '1234', 'ligne 160');


DROP TABLE IF EXISTS general;
CREATE TABLE general
(
 id       integer unsigned,
 timezone varchar(128),
 PRIMARY KEY(id)
);

INSERT INTO general VALUES (1, 'Europe/Paris');


COMMIT;
