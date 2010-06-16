
-- new tables creation
CREATE TABLE agentqueueskill
(
 agentid integer unsigned,
 skillid integer unsigned,
 weight integer unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(agentid, skillid)
);

CREATE INDEX agentqueueskill__idx__agentid ON agentqueueskill(agentid);


CREATE TABLE queueskillcat (
 id integer unsigned,
 name varchar(64) NOT NULL DEFAULT '',
 PRIMARY KEY(id)
);

CREATE UNIQUE INDEX queueskillcat__uidx__name ON queueskillcat(name);


CREATE TABLE queueskill (
 id integer unsigned,
 catid integer unsigned NOT NULL DEFAULT 1,
 name varchar(64) NOT NULL DEFAULT '',
 description text,
 printscreen varchar(5),
 PRIMARY KEY(id)
);

CREATE INDEX queueskill__idx__catid ON queueskill(catid);
CREATE UNIQUE INDEX queueskill__uidx__name ON queueskill(name);


CREATE TABLE queueskillrule
(
 id integer unsigned,
 name varchar(64) NOT NULL DEFAULT '',
 rule text,
 PRIMARY KEY(id)
);


CREATE TABLE userqueueskill
(
 userid integer unsigned,
 skillid integer unsigned,
 weight integer unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(userid, skillid)
);

CREATE INDEX userqueueskill__idx__userid ON userqueueskill(userid);

