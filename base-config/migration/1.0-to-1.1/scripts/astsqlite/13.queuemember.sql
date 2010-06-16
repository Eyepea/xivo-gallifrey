
CREATE TABLE queuemember_tmp AS SELECT * FROM queuemember;

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

INSERT INTO queuemember SELECT
	 queue_name,
	 interface,
	 penalty,
	 "call-limit",
	 paused,
	 commented,
	 usertype,
	 userid,
	 channel,
	 category,
	 '',
	 ''
 FROM queuemember_tmp;

DROP TABLE queuemember_tmp;
