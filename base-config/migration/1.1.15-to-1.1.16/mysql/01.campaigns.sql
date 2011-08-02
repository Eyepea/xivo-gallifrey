
CREATE TABLE `callcenter_campaigns_general` (
	`id`                        INT(10) unsigned auto_increment,
	`records_path`              VARCHAR(2038) DEFAULT NULL,
	`records_announce`          VARCHAR(255) DEFAULT NULL,

	`purge_syst_tagged_delay`   INTEGER DEFAULT 15552000, -- 6 months
	`purge_syst_tagged_at`      TIME DEFAULT '00:00',
	`purge_syst_untagged_delay` INTEGER DEFAULT 2592000,  -- 30 days
	`purge_syst_untagged_at`    TIME DEFAULT '00:00',
	`purge_punct_delay`         INTEGER DEFAULT 15552000, -- 6 months
	`purge_punct_at`            TIME DEFAULT '00:00',

	-- SVI closed choices (press #1, #2, ...) VARIABLES
	-- i.e: "lang=XIVO_LANG;os=XIVO_OS"
	`svichoices`                TEXT,
	-- SVI open entries
	-- i.e: "creditcard=XIVO_CREDITCARDNO;password=XIVO_CREDITCARDPWD"
	`svientries`                TEXT,
	-- SVI extra defined variables (retrieved from ERP, ...)
	-- i.e: "customer=CUSTOMERNO"
	`svivariables`              TEXT,
	PRIMARY KEY (`id`)
);

INSERT INTO `callcenter_campaigns_general` VALUES(1,NULL,NULL,15552000,'00:00',2592000,'00:00',15552000,'00:00',NULL,NULL,NULL);


CREATE TABLE `callcenter_campaigns_campaign` (
	`id`               INT(10) unsigned auto_increment,
	`name`             VARCHAR(255) UNIQUE NOT NULL,
	`start`            DATETIME NOT NULL,
	`end`              DATETIME DEFAULT NULL,
	`created_at`       DATETIME,
	PRIMARY KEY (`id`)
);

CREATE TABLE `callcenter_campaigns_campaign_filter` (
	`campaign_id`      INT(10) unsigned auto_increment,
	`type`             ENUM('agent', 'queue', 'skill', 'way') NOT NULL,
	`value`            VARCHAR(255) NOT NULL,
	PRIMARY KEY (`campaign_id`,`type`,`value`)
);

CREATE TABLE `callcenter_campaigns_tag` (
	`name`             VARCHAR(32),
	`label`            VARCHAR(255) NOT NULL,
	`action`           ENUM('removenow','keep','purge') NOT NULL,
	PRIMARY KEY (`name`)
);

INSERT INTO `callcenter_campaigns_tag` VALUES('notag', 'no tag', 'purge');

CREATE TABLE `callcenter_campaigns_records` (
	`id`                INTEGER NOT NULL auto_increment,
	`uniqueid`          VARCHAR(32) NOT NULL,
	`channel`           VARCHAR(32) NOT NULL,
	`filename`          VARCHAR(255) NOT NULL,
	`campaignkind`      VARCHAR(1) NOT NULL,

	`direction`         VARCHAR(1) NOT NULL,
	`calleridnum`       VARCHAR(32) NOT NULL,

	`callstart`         DOUBLE NOT NULL,
	`callstop`          DOUBLE NULL,
	`callduration`      INTEGER(11) NULL,
	`callstatus`        VARCHAR(16) NOT NULL,
	`recordstatus`      VARCHAR(16) NOT NULL,

	`skillrules`        VARCHAR(255) NOT NULL,
	`queuenames`        VARCHAR(255) NOT NULL,
	`agentnames`        VARCHAR(255) NOT NULL,
	`agentnumbers`      VARCHAR(255) NOT NULL,
	`agentrights`       VARCHAR(255) NOT NULL,

	`callrecordtag`     VARCHAR(16) NULL,
	`callrecordcomment` VARCHAR(255) NULL,

	`svientries`        VARCHAR(255) NULL,
	`svichoices`        VARCHAR(255) NULL,
	`svivariables`      VARCHAR(255) NULL,
	PRIMARY KEY (`id`)
);

