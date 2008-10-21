BEGIN TRANSACTION;

DROP TABLE ctilog;
CREATE TABLE ctilog (
 eventdate char(19) DEFAULT '0000-00-00 00:00:00',
 loginclient varchar(64),
 company varchar(64),
 status varchar(32),
 action varchar(32),
 arguments varchar(255) NOT NULL
);

CREATE INDEX ctilog__idx__eventdate ON ctilog(eventdate);
CREATE INDEX ctilog__idx__loginclient ON ctilog(loginclient);
CREATE INDEX ctilog__idx__company ON ctilog(company);
CREATE INDEX ctilog__idx__status ON ctilog(status);
CREATE INDEX ctilog__idx__action ON ctilog(action);

COMMIT;
