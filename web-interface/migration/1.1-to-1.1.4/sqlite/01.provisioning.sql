
-- provisioning
CREATE TABLE provisioning (
 id integer unsigned,
 main_registrar   varchar(255) NOT NULL DEFAULT '',
 backup_registrar varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY(id)
);

INSERT INTO provisioning VALUES(1, '', '');

