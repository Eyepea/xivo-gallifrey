
CREATE TABLE ldapfilter_old AS SELECT * FROM ldapfilter;

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

INSERT INTO ldapfilter SELECT * FROM ldapfilter_old;
DROP TABLE ldapfilter_old;
