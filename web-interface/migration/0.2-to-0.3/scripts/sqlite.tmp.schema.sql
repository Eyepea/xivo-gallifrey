BEGIN TRANSACTION;

DROP TABLE callfilter;
CREATE TABLE callfilter (
 id integer unsigned,
 name varchar(128) NOT NULL DEFAULT '',
 type varchar(14) NOT NULL DEFAULT 'bosssecretary',
 bosssecretary varchar(16),
 zone varchar(8) NOT NULL DEFAULT 'all',
 callerdisplay varchar(80) NOT NULL DEFAULT '',
 ringseconds tinyint unsigned NOT NULL DEFAULT 0,
 commented tinyint(1) NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX callfilter__idx__type ON callfilter(type);
CREATE INDEX callfilter__idx__bosssecretary ON callfilter(bosssecretary);
CREATE INDEX callfilter__idx__zone ON callfilter(zone);
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


DROP TABLE dialstatus;
CREATE TABLE dialstatus (
 id integer unsigned,
 status varchar(11) NOT NULL,
 category varchar(10) NOT NULL,
 categoryval varchar(128) NOT NULL DEFAULT '',
 type varchar(64) NOT NULL DEFAULT '',
 typeval varchar(255) NOT NULL DEFAULT '',
 applicationval varchar(80) NOT NULL DEFAULT '',
 linked tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE INDEX dialstatus__idx__type_typeval ON dialstatus(type,typeval);
CREATE INDEX dialstatus__idx__applicationval ON dialstatus(applicationval);
CREATE INDEX dialstatus__idx__linked ON dialstatus(linked);
CREATE UNIQUE INDEX dialstatus__uidx__status_category_categoryval ON dialstatus(status,category,categoryval);


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


DROP TABLE usercustom;
CREATE TABLE usercustom (
 id integer unsigned,
 name varchar(40),
 context varchar(39),
 interface varchar(128) NOT NULL,
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
CREATE UNIQUE INDEX usercustom__uidx__interface_category ON usercustom(interface,category);


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
 provisioningid mediumint unsigned,
 ringseconds tinyint unsigned NOT NULL DEFAULT 30,
 simultcalls tinyint unsigned NOT NULL DEFAULT 5,
 enableclient tinyint(1) NOT NULL DEFAULT 1,
 enablehint tinyint(1) NOT NULL DEFAULT 1,
 enablevoicemail tinyint(1) NOT NULL DEFAULT 0,
 skipvoicemailpass tinyint(1) NOT NULL DEFAULT 0,
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
 internal tinyint(1) NOT NULL DEFAULT 0,
 bsfilter varchar(9) NOT NULL DEFAULT 'no',
 commented tinyint(1) NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX userfeatures__idx__firstname ON userfeatures(firstname);
CREATE INDEX userfeatures__idx__lastname ON userfeatures(lastname);
CREATE INDEX userfeatures__idx__number ON userfeatures(number);
CREATE INDEX userfeatures__idx__context ON userfeatures(context);
CREATE INDEX userfeatures__idx__musiconhold ON userfeatures(musiconhold);
CREATE INDEX userfeatures__idx__provisioningid ON userfeatures(provisioningid);
CREATE UNIQUE INDEX userfeatures__uidx__protocol_name ON userfeatures(protocol,name);
CREATE UNIQUE INDEX userfeatures__uidx__protocol_protocolid ON userfeatures(protocol,protocolid);


DROP TABLE useriax;
CREATE TABLE useriax (
 id integer unsigned,
 name varchar(40) NOT NULL,
 commented tinyint(1) NOT NULL DEFAULT 0,
 username varchar(80) NOT NULL,
 type varchar(6) NOT NULL,
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
 callerid varchar(160),
 callgroup varchar(180),
 context varchar(39),
 defaultip varchar(15),
 host varchar(31) NOT NULL DEFAULT 'dynamic',
 language char(2),
 mailbox varchar(80),
 deny varchar(95),
 permit varchar(95),
 qualify char(3),
 disallow varchar(100),
 allow varchar(100),
 ipaddr varchar(15),
 port smallint unsigned,
 regseconds integer unsigned DEFAULT 0,
 'call-limit' tinyint unsigned NOT NULL DEFAULT 0,
 protocol char(3) NOT NULL DEFAULT 'iax',
 category varchar(5) NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX useriax__idx__commented ON useriax(commented);
CREATE INDEX useriax__idx__protocol ON useriax(protocol);
CREATE INDEX useriax__idx__category ON useriax(category);
CREATE UNIQUE INDEX useriax__uidx__name ON useriax(name);


DROP TABLE usersip;
CREATE TABLE usersip (
 id integer unsigned,
 name varchar(40) NOT NULL,
 commented tinyint(1) NOT NULL DEFAULT 0,
 accountcode varchar(20),
 amaflags varchar(13),
 callgroup varchar(180),
 callerid varchar(160),
 canreinvite char(3),
 context varchar(39),
 defaultip varchar(15),
 dtmfmode varchar(7),
 fromuser varchar(80),
 fromdomain varchar(80),
 fullcontact varchar(80),
 host varchar(31) NOT NULL,
 insecure varchar(11),
 language char(2),
 mailbox varchar(80),
 md5secret varchar(80),
 nat varchar(5) NOT NULL DEFAULT 'no',
 deny varchar(95),
 permit varchar(95),
 mask varchar(95),
 pickupgroup varchar(80),
 port smallint unsigned,
 qualify char(3),
 restrictcid char(1),
 rtptimeout char(3),
 rtpholdtimeout char(3),
 secret varchar(80),
 type varchar(6) NOT NULL,
 username varchar(80) NOT NULL,
 disallow varchar(100),
 allow varchar(100),
 musiconhold varchar(100),
 regseconds integer unsigned NOT NULL DEFAULT 0,
 ipaddr varchar(15),
 regexten varchar(80),
 cancallforward char(3),
 setvar varchar(100) NOT NULL,
 'call-limit' tinyint unsigned NOT NULL DEFAULT 0,
 protocol char(3) NOT NULL DEFAULT 'sip',
 category varchar(5) NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX usersip__idx__commented ON usersip(commented);
CREATE INDEX usersip__idx__protocol ON usersip(protocol);
CREATE INDEX usersip__idx__category ON usersip(category);
CREATE UNIQUE INDEX usersip__uidx__name ON usersip(name);


DROP TABLE uservoicemail;
CREATE TABLE uservoicemail (
 uniqueid integer unsigned,
 context varchar(39),
 mailbox varchar(40) NOT NULL DEFAULT '',
 password varchar(80) NOT NULL DEFAULT '',
 fullname varchar(80) NOT NULL DEFAULT '',
 email varchar(80) NOT NULL DEFAULT '',
 pager varchar(80) NOT NULL DEFAULT '',
 dialout varchar(39),
 callback varchar(39),
 exitcontext varchar(39),
 language varchar(20) NOT NULL DEFAULT '',
 tz varchar(80) NOT NULL DEFAULT '',
 attach tinyint(1) DEFAULT 1,
 saycid tinyint(1) DEFAULT 1,
 review tinyint(1) DEFAULT 0,
 operator tinyint(1) DEFAULT 0,
 envelope tinyint(1) DEFAULT 0,
 sayduration tinyint(1) DEFAULT 0,
 saydurationm tinyint unsigned DEFAULT 2,
 sendvoicemail tinyint(1) DEFAULT 0,
 deletevoicemail tinyint(1) NOT NULL DEFAULT 0,
 forcename tinyint(1) DEFAULT 0,
 forcegreetings tinyint(1) DEFAULT 0,
 hidefromdir varchar(3) NOT NULL DEFAULT 'no',
 maxmsg smallint unsigned DEFAULT 100,
 commented tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(uniqueid)
);

CREATE INDEX uservoicemail__idx__commented ON uservoicemail(commented);
CREATE INDEX uservoicemail__idx__context ON uservoicemail(context);
CREATE UNIQUE INDEX uservoicemail__uidx__mailbox_context ON uservoicemail(mailbox,context);

COMMIT;
