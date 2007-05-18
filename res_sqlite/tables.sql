/*
 * Tables for res_sqlite.so.
 */

/*
 * RealTime static table.
 */
CREATE TABLE ast_config (
 id integer unsigned,
 commented tinyint(1) DEFAULT 0,
 filename varchar(128) NOT NULL,
 category varchar(128) NOT NULL,
 var_name varchar(128) NOT NULL,
 var_val varchar(128),
 PRIMARY KEY(id)
);

CREATE INDEX ast_config__idx__filename_commented ON ast_config(filename, commented);
CREATE INDEX ast_config__idx__category ON ast_config(category);
CREATE INDEX ast_config__idx__var_name ON ast_config(var_name);

/*
 * CDR table (this table is automatically created if non existent).
 * 
 * CREATE TABLE cdr (
 *  id integer unsigned auto_increment,
 *  calldate char(19) DEFAULT '0000-00-00 00:00:00',
 *  clid varchar(80) NOT NULL DEFAULT '',
 *  src varchar(80) NOT NULL DEFAULT '',
 *  dst varchar(80) NOT NULL DEFAULT '',
 *  dcontext varchar(80) NOT NULL DEFAULT '',
 *  channel varchar(80) NOT NULL DEFAULT '',
 *  dstchannel varchar(80) NOT NULL DEFAULT '',
 *  lastapp varchar(80) NOT NULL DEFAULT '',
 *  lastdata varchar(80) NOT NULL DEFAULT '',
 *  answer char(19) DEFAULT '0000-00-00 00:00:00',
 *  end char(19) DEFAULT '0000-00-00 00:00:00',
 *  duration integer unsigned NOT NULL DEFAULT 0,
 *  billsec integer unsigned NOT NULL DEFAULT 0,
 *  disposition varchar(9) NOT NULL DEFAULT '',
 *  amaflags tinyint unsigned NOT NULL DEFAULT 0,
 *  accountcode varchar(20) NOT NULL DEFAULT '',
 *  uniqueid varchar(32) NOT NULL DEFAULT '',
 *  userfield varchar(255) NOT NULL DEFAULT '',
 *  PRIMARY KEY(id)
 * );
 *
 * CREATE INDEX cdr__idx__disposition ON cdr(dispotion);
 * CREATE INDEX cdr__idx__src ON cdr(src);
 * CREATE INDEX cdr__idx__dst ON cdr(dst);
 * CREATE INDEX cdr__idx__calldate ON cdr(calldate);
 */

/*
 * SIP RealTime table.
 */
CREATE TABLE ast_sip (
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
 PRIMARY KEY(id)
);

CREATE INDEX ast_sip__idx__commented ON ast_sip(commented);
CREATE UNIQUE INDEX ast_sip__uidx__name ON ast_sip(name);

/*
 * Dialplan RealTime table.
 */
CREATE TABLE ast_exten (
 id integer unsigned,
 commented tinyint(1) NOT NULL DEFAULT 0,
 context varchar(20) NOT NULL DEFAULT '',
 exten varchar(20) NOT NULL DEFAULT '',
 priority tinyint unsigned NOT NULL DEFAULT 0,
 app varchar(20) NOT NULL DEFAULT '',
 appdata varchar(128) NOT NULL DEFAULT '',
 PRIMARY KEY(id)
);

CREATE INDEX ast_exten__idx__commented ON ast_exten(commented);
CREATE INDEX ast_exten__idx__context_exten_priority ON ast_exten(context, exten, priority);
