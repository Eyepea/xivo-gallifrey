
-- provisioning
CREATE TABLE provisioning (
 id integer unsigned,
 registrar_main   varchar(255) NOT NULL DEFAULT '',
 registrar_backup varchar(255) NOT NULL DEFAULT '',
 proxy_main       varchar(255) NOT NULL DEFAULT '',
 proxy_backup     varchar(255) NOT NULL DEFAULT '',
 PRIMARY KEY(id)
);

-- we set default registrar_main and proxy_main value as VOIP interface address
-- if such interface does not exist, we set empty values
INSERT OR IGNORE INTO provisioning SELECT 1, address, '', address, '' FROM netiface WHERE networktype = 'voip' LIMIT 1;
INSERT OR IGNORE INTO provisioning VALUES(1, '', '', '', '');

