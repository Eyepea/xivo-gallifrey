
CREATE TABLE incall_tmp AS SELECT * FROM incall;

DROP TABLE incall;
CREATE TABLE incall (
 id integer unsigned,
 exten varchar(40) NOT NULL,
 context varchar(39) NOT NULL,
 preprocess_subroutine varchar(39),
 faxdetectenable tinyint(1) NOT NULL DEFAULT 0,
 faxdetecttimeout tinyint unsigned NOT NULL DEFAULT 4,
 faxdetectemail varchar(255) NOT NULL DEFAULT '',
 commented tinyint(1) NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX incall__idx__exten ON incall(exten);
CREATE INDEX incall__idx__context ON incall(context);
CREATE INDEX incall__idx__commented ON incall(commented);
CREATE UNIQUE INDEX incall__uidx__exten_context ON incall(exten,context);

INSERT INTO incall SELECT 
			id,
			exten,
			context,
			preprocess_subroutine,
			faxdetectenable,
			faxdetecttimeout,
			faxdetectemail,
			commented,
			''
		FROM incall_tmp;
DROP TABLE incall_tmp;

