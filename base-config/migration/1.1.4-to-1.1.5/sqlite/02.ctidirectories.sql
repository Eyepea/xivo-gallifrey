
CREATE TABLE ctidirectoryfields (
	dir_id integer unsigned,
	fieldname varchar(255),
	value varchar(255),
	PRIMARY KEY(dir_id, fieldname)
);


INSERT INTO ctidirectoryfields SELECT id, 'phone', substr(field_phone, 3, length(field_phone)-4) FROM ctidirectories
	WHERE length(field_phone) > 0;
INSERT INTO ctidirectoryfields SELECT id, 'fullname', substr(field_fullname, 3, length(field_fullname)-4) FROM ctidirectories
	WHERE length(field_fullname) > 0;
INSERT INTO ctidirectoryfields SELECT id, 'company', substr(field_company, 3, length(field_company)-4) FROM ctidirectories
	WHERE length(field_company) > 0;
INSERT INTO ctidirectoryfields SELECT id, 'mail', substr(field_mail, 3, length(field_mail)-4) FROM ctidirectories
	WHERE length(field_mail) > 0;
INSERT INTO ctidirectoryfields SELECT id, 'firstname', substr(field_firstname, 3, length(field_firstname)-4) FROM ctidirectories
	WHERE length(field_firstname) > 0;
INSERT INTO ctidirectoryfields SELECT id, 'lastname', substr(field_lastname, 3, length(field_lastname)-4) FROM ctidirectories
	WHERE length(field_lastname) > 0;

CREATE TABLE ctidirectories_tmp AS SELECT * FROM ctidirectories;

DROP TABLE ctidirectories;
CREATE TABLE ctidirectories (
 id integer unsigned,
 name varchar(255),
 uri varchar(255),
 delimiter varchar(30),
 match_direct text NOT NULL,
 match_reverse text NOT NULL,
 display_reverse varchar(255),
 description varchar(255),
 deletable tinyint(1),
 PRIMARY KEY(id)
);

INSERT INTO ctidirectories SELECT
	id,
	name
	uri,
	delimiter
	match_direct,
	match_reverse,
	display_reverse,
	description,
	deletable
FROM ctidirectories_tmp;
DROP TABLE ctidirectories_tmp;
