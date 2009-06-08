BEGIN TRANSACTION;

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
 PRIMARY KEY(id)
);

CREATE INDEX outcall__idx__commented ON outcall(commented);
CREATE UNIQUE INDEX outcall__uidx__name ON outcall(name);
CREATE UNIQUE INDEX outcall__uidx__exten_context ON outcall(exten,context);


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
 PRIMARY KEY(queue_name,interface)
);

CREATE INDEX queuemember__idx__commented ON queuemember(commented);
CREATE INDEX queuemember__idx__usertype ON queuemember(usertype);
CREATE INDEX queuemember__idx__userid ON queuemember(userid);
CREATE INDEX queuemember__idx__channel ON queuemember(channel);
CREATE INDEX queuemember__idx__category ON queuemember(category);
CREATE UNIQUE INDEX queuemember__uidx__queue_name_channel_usertype_userid_category ON queuemember(queue_name,channel,usertype,userid,category);


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

COMMIT;
