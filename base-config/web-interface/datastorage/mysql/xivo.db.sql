CREATE TABLE i18n_cache (
 `dcreate` int(10) unsigned NOT NULL default 0,
 `dupdate` int(10) unsigned NOT NULL default 0,
 `locale` varchar(7) NOT NULL default '',
 `language` varchar(3) NOT NULL default '',
 `path` varchar(255) NOT NULL default '',
 `obj` longblob NOT NULL,
 PRIMARY KEY(locale,path)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX i18n_cache__idx__dupdate ON i18n_cache(dupdate);
CREATE INDEX i18n_cache__idx__locale ON i18n_cache(locale);
CREATE INDEX i18n_cache__idx__language ON i18n_cache(language);
CREATE INDEX i18n_cache__idx__path ON i18n_cache(path);


CREATE TABLE session (
 `sesskey` varchar(32) NOT NULL default '',
 `expire` int(10) unsigned NOT NULL default 0,
 `data` longblob NOT NULL,
 `user_id` int(11) unsigned default 0,
 `start` int(10) unsigned default 0,
 PRIMARY KEY(sesskey)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX session__idx__expire ON session(expire);
CREATE INDEX session__idx__user_id ON session(user_id);


CREATE TABLE user (
 `id` int(11) unsigned auto_increment,
 `login` varchar(255) NOT NULL default '',
 `passwd` varchar(255) NOT NULL default '',
 `meta` enum('user','admin','root') default 'user',
 `valid` tinyint(1) unsigned default 1,
 `time` int(11) unsigned NOT NULL default 0,
 `obj` longblob NOT NULL,
 `dcreate` int(11) unsigned NOT NULL default 0,
 `dupdate` int(11) unsigned NOT NULL default 0,
 PRIMARY KEY(id)
) TYPE=MyISAM DEFAULT CHARACTER SET utf8;

CREATE INDEX user__idx__login ON user(login);
CREATE INDEX user__idx__meta ON user(meta);
CREATE INDEX user__idx__passwd ON user(passwd);
CREATE INDEX user__idx__time ON user(time);
CREATE INDEX user__idx__valid ON user(valid);
CREATE UNIQUE INDEX user__uidx__login_meta ON user(login,meta);

INSERT INTO user VALUES(1,'root','proformatique','root',1,0,'',UNIX_TIMESTAMP(UTC_TIMESTAMP()),0);
INSERT INTO user VALUES(2,'admin','proformatique','admin',1,0,'',UNIX_TIMESTAMP(UTC_TIMESTAMP()),0);
