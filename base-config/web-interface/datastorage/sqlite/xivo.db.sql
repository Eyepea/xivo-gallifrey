BEGIN TRANSACTION;

DROP TABLE i18ncache;
CREATE TABLE i18ncache (
 locale varchar(7) NOT NULL DEFAULT '',
 path varchar(255) NOT NULL DEFAULT '',
 language varchar(3) NOT NULL DEFAULT '',
 dcreate integer unsigned NOT NULL DEFAULT 0,
 dupdate integer unsigned NOT NULL DEFAULT 0,
 obj longblob NOT NULL,
 PRIMARY KEY(locale,path)
);

CREATE INDEX i18ncache__idx__language ON i18ncache(language);
CREATE INDEX i18ncache__idx__dupdate ON i18ncache(dupdate);


DROP TABLE session;
CREATE TABLE session (
 key varchar(32) NOT NULL DEFAULT '',
 start integer unsigned NOT NULL DEFAULT 0,
 expire integer unsigned NOT NULL DEFAULT 0,
 userid integer unsigned NOT NULL DEFAULT 0,
 data longblob NOT NULL,
 PRIMARY KEY(key)
);

CREATE INDEX session__idx__expire ON session(expire);
CREATE INDEX session__idx__userid ON session(userid);


DROP TABLE user;
CREATE TABLE user (
 id integer unsigned,
 login varchar(64) NOT NULL DEFAULT '',
 passwd varchar(64) NOT NULL DEFAULT '',
 meta varchar(5) NOT NULL DEFAULT 'user',
 valid tinyint(1) NOT NULL DEFAULT 1,
 time integer unsigned NOT NULL DEFAULT 0,
 dcreate integer unsigned NOT NULL DEFAULT 0,
 dupdate integer unsigned NOT NULL DEFAULT 0,
 obj longblob NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX user__idx__login ON user(login);
CREATE INDEX user__idx__passwd ON user(passwd);
CREATE INDEX user__idx__meta ON user(meta);
CREATE INDEX user__idx__valid ON user(valid);
CREATE INDEX user__idx__time ON user(time);
CREATE UNIQUE INDEX user__uidx__login_meta ON user(login,meta);

INSERT INTO user VALUES (1,'root','proformatique','root',1,0,strftime('%s',datetime('now','utc')),0,'');
INSERT INTO user VALUES (2,'admin','proformatique','admin',1,0,strftime('%s',datetime('now','utc')),0,'');

COMMIT;
