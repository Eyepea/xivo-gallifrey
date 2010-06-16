

CREATE TABLE meetmefeatures_tmp AS SELECT * FROM meetmefeatures;

DROP TABLE meetmefeatures;
CREATE TABLE meetmefeatures (
 id integer unsigned,
 meetmeid integer unsigned NOT NULL,
 name varchar(80) NOT NULL,
 number varchar(40) NOT NULL,
 context varchar(39) NOT NULL,
 admin_typefrom varchar(9),
 admin_internalid integer unsigned,
 admin_externalid varchar(40),
 admin_identification varchar(11) NOT NULL,
 admin_mode varchar(6) NOT NULL,
 admin_announceusercount tinyint(1) NOT NULL DEFAULT 0,
 admin_announcejoinleave varchar(8) NOT NULL,
 admin_moderationmode tinyint(1) NOT NULL DEFAULT 0,
 admin_initiallymuted tinyint(1) NOT NULL DEFAULT 0,
 admin_musiconhold varchar(128),
 admin_poundexit tinyint(1) NOT NULL DEFAULT 0,
 admin_quiet tinyint(1) NOT NULL DEFAULT 0,
 admin_starmenu tinyint(1) NOT NULL DEFAULT 0,
 admin_closeconflastmarkedexit tinyint(1) NOT NULL DEFAULT 0,
 admin_enableexitcontext tinyint(1) NOT NULL DEFAULT 0,
 admin_exitcontext varchar(39),
 user_mode varchar(6) NOT NULL,
 user_announceusercount tinyint(1) NOT NULL DEFAULT 0,
 user_hiddencalls tinyint(1) NOT NULL DEFAULT 0,
 user_announcejoinleave varchar(8) NOT NULL,
 user_initiallymuted tinyint(1) NOT NULL DEFAULT 0,
 user_musiconhold varchar(128),
 user_poundexit tinyint(1) NOT NULL DEFAULT 0,
 user_quiet tinyint(1) NOT NULL DEFAULT 0,
 user_starmenu tinyint(1) NOT NULL DEFAULT 0,
 user_enableexitcontext tinyint(1) NOT NULL DEFAULT 0,
 user_exitcontext varchar(39),
 talkeroptimization tinyint(1) NOT NULL DEFAULT 0,
 record tinyint(1) NOT NULL DEFAULT 0,
 talkerdetection tinyint(1) NOT NULL DEFAULT 0,
 noplaymsgfirstenter tinyint(1) NOT NULL DEFAULT 0,
 durationm smallint unsigned,
 closeconfdurationexceeded tinyint(1) NOT NULL DEFAULT 0,
 nbuserstartdeductduration tinyint unsigned,
 timeannounceclose smallint unsigned,
 maxuser tinyint unsigned,
 startdate char(19),
 emailfrom varchar(255),
 emailfromname varchar(255),
 emailsubject varchar(255),
 emailbody text NOT NULL,
 preprocess_subroutine varchar(39),
 description text NOT NULL,
 PRIMARY KEY(id)
);

CREATE INDEX meetmefeatures__idx__number ON meetmefeatures(number);
CREATE INDEX meetmefeatures__idx__context ON meetmefeatures(context);
CREATE UNIQUE INDEX meetmefeatures__uidx__meetmeid ON meetmefeatures(meetmeid);
CREATE UNIQUE INDEX meetmefeatures__uidx__name ON meetmefeatures(name);

INSERT INTO meetmefeatures SELECT
	 id,
	 meetmeid,
	 name,
	 number,
	 context,
	 NULL,																								-- admin_typefrom
	 0,																										-- admin_internalid
	 '',																									-- admin_externalid
	 '',																									-- admin_identification
	 adminmode,																						-- admin_mode
	 announceusercount,																		-- admin_announceusercount
	 announcejoinleave,																		-- admin_announcejoinleave
	 0,																										-- admin_moderationmode
	 0,																										-- admin_initiallymuted
	 musiconhold,																					-- admin_musiconhold
	 poundexit,																						-- admin_poundexit
	 quiet,																								-- admin_quiet
	 starmenu,																						-- admin_starmenu
	 0,																										-- admin_closeconflastmarkedexit
	 enableexitcontext,																		-- admin_enableexitcontext
	 exitcontext,																					-- admin_exitcontext
	 
	 mode,																								-- user_mode
	 announceusercount,																		-- user_announceusercount
	 0,																										-- user_hiddencalls
	 announcejoinleave,																		-- user_announcejoinleave
	 0,																										-- user_initiallymuted
	 musiconhold,																					-- user_musiconhold
	 poundexit,																						-- user_poundexit
	 quiet,																								-- user_quiet
	 starmenu,																						-- user_starmenu
	 enableexitcontext,																		-- user_enableexitcontext
	 exitcontext,																					-- user_exitcontext
	 0,																										-- talkeroptimization
	 record,																							-- 
	 0,																										-- talkerdetection
	 0,																										-- noplaymsgfirstenter
	 0,																										-- durationm
	 0,																										-- closeconfdurationexceeded
	 0,																										-- nbuserstartdeductduration
	 0,																										-- timeannounceclose
	 0,																										-- maxuser
	 NULL,																								-- startdate
	 NULL,																								-- emailfrom
	 NULL,																								-- emailfromname
	 NULL,																								-- emailsubject
	 '',																									-- emailbody
	 preprocess_subroutine,
	 ''																									-- description

--	 alwayspromptpin,
 FROM meetmefeatures_tmp;
 
DROP TABLE meetmefeatures_tmp;


