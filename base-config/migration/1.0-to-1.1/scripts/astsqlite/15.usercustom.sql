
CREATE TABLE usercustom_tmp AS SELECT * FROM usercustom;

DROP TABLE usercustom;
CREATE TABLE usercustom (
 id integer unsigned,
 name varchar(40),
 context varchar(39),
 interface varchar(128) NOT NULL,
 intfsuffix varchar(32) NOT NULL DEFAULT '',
 commented tinyint(1) NOT NULL DEFAULT 0,
 protocol char(6) NOT NULL DEFAULT 'custom',
 category varchar(5) NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX usercustom__idx__name ON usercustom(name);
CREATE INDEX usercustom__idx__context ON usercustom(context);
CREATE INDEX usercustom__idx__commented ON usercustom(commented);
CREATE INDEX usercustom__idx__protocol ON usercustom(protocol);
CREATE INDEX usercustom__idx__category ON usercustom(category);
CREATE UNIQUE INDEX usercustom__uidx__interface_intfsuffix_category ON usercustom(interface,intfsuffix,category);

INSERT INTO usercustom SELECT * FROM usercustom_tmp;

DROP TABLE usercustom_tmp;
