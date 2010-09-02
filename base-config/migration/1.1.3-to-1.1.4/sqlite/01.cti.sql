
UPDATE cticontexts SET directories = 'xivodir' WHERE
	id          = 3 AND
	directories = 'xivodir,internal';

UPDATE ctiprofiles SET maxgui = 5 WHERE id = 15;

UPDATE ctireversedirectories SET directories = '["xivodir"]' WHERE
	id          = 1 AND
	directories = '["xivodir","internal"]';

CREATE TABLE ctisheetactions_tmp AS SELECT * FROM ctisheetactions;

DROP TABLE ctisheetactions;
CREATE TABLE ctisheetactions (
 id integer unsigned,
 name varchar(50),
 description text NOT NULL,
 context varchar(50),
 whom varchar(50),
 extrarequests varchar(50),
 capaids text NOT NULL,
 sheet_info text,
 systray_info text,
 sheet_qtui text,
 action_info text,
 focus tinyint(1),
 deletable tinyint(1),
 PRIMARY KEY(id)
);

INSERT INTO ctisheetactions SELECT
 id,
 name,
 description,
 context,
 whom,
 '',
 capaids,
 sheet_info,
 systray_info,
 sheet_qtui,
 action_info,
 focus,
 deletable
FROM ctisheetactions_tmp;
DROP TABLE ctisheetactions_tmp;

