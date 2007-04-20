BEGIN TRANSACTION;

CREATE TABLE i18n_cache (
  dcreate integer unsigned NOT NULL default 0,
  dupdate integer unsigned NOT NULL default 0,
  locale varchar(7) NOT NULL default '',
  language varchar(3) NOT NULL default '',
  path varchar(255) NOT NULL default '',
  obj longblob NOT NULL
);

CREATE INDEX i18n_cache__idx__dupdate ON i18n_cache(dupdate);
CREATE INDEX i18n_cache__idx__language ON i18n_cache(language);
CREATE INDEX i18n_cache__idx__locale ON i18n_cache(locale);


CREATE TABLE session (
  sesskey varchar(32) NOT NULL default '',
  expire integer unsigned NOT NULL default 0,
  data longblob NOT NULL,
  user_id integer unsigned default 0,
  start integer unsigned default 0,
  PRIMARY KEY(sesskey)
);

CREATE INDEX session__idx__expire ON session(expire);
CREATE INDEX session__idx__user_id ON session(user_id);


CREATE TABLE user (
  id integer unsigned NOT NULL,
  login varchar(255) NOT NULL default '',
  passwd varchar(255) NOT NULL default '',
  meta varchar(5) default 'user',
  valid varchar(5) default 'true',
  time integer unsigned NOT NULL default 0,
  obj longblob NOT NULL,
  dcreate integer unsigned NOT NULL default 0,
  dupdate integer unsigned NOT NULL default 0,
  PRIMARY KEY(id)
);

CREATE INDEX user__idx__login ON user(login);
CREATE INDEX user__idx__meta ON user(meta);
CREATE INDEX user__idx__passwd ON user(passwd);
CREATE INDEX user__idx__time ON user(time);
CREATE INDEX user__idx__valid ON user(valid);
CREATE UNIQUE INDEX user__uidx__login_meta ON user(login,meta);

INSERT INTO user VALUES(1,'root','proformatique','root','true',0,'',strftime('%s',datetime('now','utc')),0);
INSERT INTO user VALUES(2,'admin','proformatique','admin','true',0,'',strftime('%s',datetime('now','utc')),0);

COMMIT;
