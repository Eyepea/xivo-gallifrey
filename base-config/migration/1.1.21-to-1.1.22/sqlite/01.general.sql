
CREATE TABLE general_tmp AS SELECT * FROM general;

DROP TABLE general;
CREATE TABLE general
(
 id       integer unsigned,
 timezone varchar(128),
 externip varchar(32) DEFAULT NULL, 

 PRIMARY KEY(id)
);

INSERT INTO general SELECT
 id,
 timezone,
 NULL
FROM general_tmp;
DROP TABLE general_tmp;

