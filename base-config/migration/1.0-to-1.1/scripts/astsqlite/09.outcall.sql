
CREATE TABLE outcall_tmp AS SELECT * FROM outcall;

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
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX outcall__idx__exten ON outcall(exten);
CREATE INDEX outcall__idx__commented ON outcall(commented);
CREATE UNIQUE INDEX outcall__uidx__name ON outcall(name);
CREATE UNIQUE INDEX outcall__uidx__exten_context ON outcall(exten,context);

INSERT INTO outcall SELECT 
			id,
			name,
			exten,
			context,
			externprefix,
			stripnum,
			setcallerid,
			callerid,
			useenum,
			internal,
			preprocess_subroutine,
			hangupringtime,
			commented,
			''
		 FROM outcall_tmp;

DROP TABLE outcall_tmp;

