CREATE TABLE qagent (
  agent_id int(6) NOT NULL auto_increment,
  agent varchar(40) NOT NULL default '',
  PRIMARY KEY  (agent_id)
) TYPE=MyISAM;

--
-- Table structure for table `qevent`
--

CREATE TABLE qevent (
  event_id int(2) NOT NULL default '0',
  event varchar(40) default NULL,
  PRIMARY KEY  (event_id)
) TYPE=MyISAM;

--
-- Dumping data for table `qevent`
--


INSERT INTO qevent VALUES (1,'ABANDON');
INSERT INTO qevent VALUES (2,'AGENTDUMP');
INSERT INTO qevent VALUES (3,'AGENTLOGIN');
INSERT INTO qevent VALUES (4,'AGENTCALLBACKLOGIN');
INSERT INTO qevent VALUES (5,'AGENTLOGOFF');
INSERT INTO qevent VALUES (6,'AGENTCALLBACKLOGOFF');
INSERT INTO qevent VALUES (7,'COMPLETEAGENT');
INSERT INTO qevent VALUES (8,'COMPLETECALLER');
INSERT INTO qevent VALUES (9,'CONFIGRELOAD');
INSERT INTO qevent VALUES (10,'CONNECT');
INSERT INTO qevent VALUES (11,'ENTERQUEUE');
INSERT INTO qevent VALUES (12,'EXITWITHKEY');
INSERT INTO qevent VALUES (13,'EXITWITHTIMEOUT');
INSERT INTO qevent VALUES (14,'QUEUESTART');
INSERT INTO qevent VALUES (15,'SYSCOMPAT');
INSERT INTO qevent VALUES (16,'TRANSFER');
INSERT INTO qevent VALUES (17,'PAUSE');
INSERT INTO qevent VALUES (18,'UNPAUSE');

--
-- Table structure for table `qname`
--

CREATE TABLE qname (
  qname_id int(6) NOT NULL auto_increment,
  queue varchar(40) NOT NULL default '',
  PRIMARY KEY  (qname_id)
) TYPE=MyISAM;

--
-- Table structure for table `queue_stats`
--

CREATE TABLE queue_stats (
  queue_stats_id int(12) NOT NULL auto_increment,
  uniqueid varchar(40) default NULL,
  datetime datetime NOT NULL default '0000-00-00 00:00:00',
  qname int(6) default NULL,
  qagent int(6) default NULL,
  qevent int(2) default NULL,
  info1 varchar(40) default NULL,
  info2 varchar(40) default NULL,
  info3 varchar(40) default NULL,
  PRIMARY KEY  (queue_stats_id),
  UNIQUE KEY unico (datetime,qname,qagent,qevent)
) TYPE=MyISAM;

