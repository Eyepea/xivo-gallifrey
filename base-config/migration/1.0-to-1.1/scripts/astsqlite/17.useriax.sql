
CREATE TABLE useriax_tmp AS SELECT * FROM useriax;

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
 requirecalltoken char(4) NOT NULL DEFAULT '',
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

INSERT INTO useriax SELECT
	 id,
	 name,
	 type,
	 username,
	 secret,
	 dbsecret,
	 context,
	 language,
	 accountcode,
 	amaflags,
	 mailbox,
	 callerid,
	 fullname,
	 cid_number,
	 trunk,
	 auth,
	 encryption,
	 maxauthreq,
	 inkeys,
	 outkey,
	 adsi,
	 transfer,
	 codecpriority,
	 jitterbuffer,
	 forcejitterbuffer,
	 sendani,
	 qualify,
	 qualifysmoothing,
	 qualifyfreqok,
	 qualifyfreqnotok,
	 timezone,
	 disallow,
	 allow,
	 mohinterpret,
	 mohsuggest,
	 deny,
	 permit,
	 defaultip,
	 sourceaddress,
	 setvar,
	 host,
	 port,
	 mask,
	 regexten,
	 peercontext,
	 ipaddr,
	 regseconds,
	 protocol,
	 category,
	 commented,
	 ''
 FROM useriax_tmp;
 
DROP TABLE useriax_tmp;


UPDATE OR IGNORE useriax SET
  var_val   = 'fr_FR'
WHERE
  var_name = 'language' AND
  var_val != 'en';

UPDATE OR IGNORE useriax SET
  var_val  = 'en_US'
WHERE
  var_name = 'language' AND
  var_val  = 'en';

