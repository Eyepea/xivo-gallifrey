
-- provisioning
CREATE TABLE provisioning (
 id integer unsigned,
 registrar_main   varchar(255) NOT NULL DEFAULT '',
 registrar_backup varchar(255) NOT NULL DEFAULT '',
 proxy_main       varchar(255) NOT NULL DEFAULT '',
 proxy_backup     varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY(id)
);

INSERT INTO provisioning VALUES(1, '', '', '', '');

