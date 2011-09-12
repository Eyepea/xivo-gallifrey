
CREATE TABLE userfeatures_tmp AS SELECT * FROM userfeatures;

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
 userfield varchar(128) NOT NULL DEFAULT '',
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

INSERT INTO userfeatures SELECT
 id,
 protocol,
 protocolid,
 firstname,
 lastname,
 name,
 number,
 context,
 voicemailid,
 agentid,
 provisioningid,
 ringseconds,
 simultcalls,
 enableclient,
 loginclient,
 passwdclient,
 profileclient,
 enablehint,
 enablevoicemail,
 enablexfer,
 enableautomon,
 callrecord,
 incallfilter,
 enablednd,
 enableunc,
 destunc,
 enablerna,
 destrna,
 enablebusy,
 destbusy,
 musiconhold,
 outcallerid,
 mobilephonenumber,
 '',
 bsfilter,
 preprocess_subroutine,
 internal,
 timezone,
 commented,
 description
FROM userfeatures_tmp;
DROP TABLE userfeatures_tmp;

