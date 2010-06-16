
CREATE TABLE phonefunckey_old AS SELECT * FROM phonefunckey;

DROP TABLE phonefunckey;
CREATE TABLE phonefunckey (
 iduserfeatures integer unsigned NOT NULL,
 fknum smallint unsigned NOT NULL,
 exten varchar(40),
 typeextenumbers varchar(64),
 typevalextenumbers varchar(255),
 typeextenumbersright varchar(64),
 typevalextenumbersright varchar(255),
 label varchar(32),
 supervision tinyint(1) NOT NULL DEFAULT 0,
 progfunckey tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(iduserfeatures,fknum)
);

CREATE INDEX phonefunckey__idx__exten ON phonefunckey(exten);
CREATE INDEX phonefunckey__idx__progfunckey ON phonefunckey(progfunckey);
CREATE INDEX phonefunckey__idx__typeextenumbers_typevalextenumbers ON phonefunckey(typeextenumbers,typevalextenumbers);
CREATE INDEX phonefunckey__idx__typeextenumbersright_typevalextenumbersright ON phonefunckey(typeextenumbersright,typevalextenumbersright);

INSERT INTO phonefunckey SELECT 
	 iduserfeatures,
	 fknum,
	 exten,
	 typeextenumbers,
	 typevalextenumbers,
	 typeextenumbersright,
	 typevalextenumbersright,
	 '',
	 supervision,
	 0
 FROM phonefunckey_old;
 
DROP TABLE phonefunckey_old;
