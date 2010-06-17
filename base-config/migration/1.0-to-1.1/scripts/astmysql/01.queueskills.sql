
-- new tables creation
CREATE TABLE `agentqueueskill` (
 `agentid` int(10) unsigned,
 `skillid` int(10) unsigned,
 `weight` int(3) unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(`agentid`, `skillid`)
);

CREATE INDEX `agentqueueskill__idx__agentid` ON `agentqueueskill`(`agentid`);

CREATE TABLE `queueskillcat` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(64) NOT NULL DEFAULT '',
 PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE UNIQUE INDEX `queueskillcat__uidx__name` ON `queueskillcat`(`name`);

-- queueskill values
CREATE TABLE `queueskill` (
 `id` int(10) unsigned auto_increment,
 `catid` int(10) unsigned NOT NULL DEFAULT 1,
 `name` varchar(64) NOT NULL DEFAULT '',
 `description` text,
 `printscreen` varchar(5),
 PRIMARY KEY(`id`)
);

CREATE INDEX `queueskill__idx__catid` ON `queueskill`(`catid`);
CREATE UNIQUE INDEX `queueskill__uidx__name` ON `queueskill`(`name`);

-- queueskill rules;
CREATE TABLE `queueskillrule` (
 `id` int(10) unsigned auto_increment,
 `name` varchar(64) NOT NULL DEFAULT '',
 `rule` text,
 PRIMARY KEY(`id`)
);

-- user queueskills
CREATE TABLE `userqueueskill` (
 `userid` int(10) unsigned,
 `skillid` int(10) unsigned,
 `weight` int(3) unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(`userid`, `skillid`)
);

CREATE INDEX `userqueueskill__idx__userid` ON `userqueueskill`(`userid`);

