
BEGIN TRANSACTION;

-- migrate server table
CREATE TABLE netiface_old AS SELECT * FROM netiface;


DROP TABLE netiface;
CREATE TABLE netiface (
 name varchar(64) NOT NULL DEFAULT '',
 ifname varchar(64) NOT NULL DEFAULT '',
 hwtypeid smallint unsigned NOT NULL DEFAULT 65534,
 networktype char(4) NOT NULL,
 type char(5) NOT NULL,
 family varchar(5) NOT NULL,
 method varchar(6) NOT NULL,
 address varchar(39),
 netmask varchar(39),
 broadcast varchar(39),
 gateway varchar(39),
 mtu smallint unsigned,
 vlanrawdevice varchar(64),
 vlanid smallint unsigned,
 options text NOT NULL,
 disable tinyint(1) NOT NULL DEFAULT 0,
 dcreate integer unsigned NOT NULL DEFAULT 0,
 description text NOT NULL,
 PRIMARY KEY(name)
);

CREATE INDEX netiface__idx__hwtypeid ON netiface(hwtypeid);
CREATE INDEX netiface__idx__networktype ON netiface(networktype);
CREATE INDEX netiface__idx__type ON netiface(type);
CREATE INDEX netiface__idx__family ON netiface(family);
CREATE INDEX netiface__idx__method ON netiface(method);
CREATE INDEX netiface__idx__address ON netiface(address);
CREATE INDEX netiface__idx__netmask ON netiface(netmask);
CREATE INDEX netiface__idx__broadcast ON netiface(broadcast);
CREATE INDEX netiface__idx__gateway ON netiface(gateway);
CREATE INDEX netiface__idx__mtu ON netiface(mtu);
CREATE INDEX netiface__idx__vlanrawdevice ON netiface(vlanrawdevice);
CREATE INDEX netiface__idx__vlanid ON netiface(vlanid);
CREATE INDEX netiface__idx__disable ON netiface(disable);
CREATE UNIQUE INDEX netiface__uidx__ifname ON netiface(ifname);


INSERT INTO netiface (
 name,
 ifname,
 hwtypeid,
 networktype,
 type,
 family,
 method,
 address,
 netmask,
 broadcast,
 gateway,
 mtu,
 vlanrawdevice,
 vlanid,
 options,
 disable,
 dcreate,
 description)
SELECT
 name,
 ifname,
 hwtypeid,
 networktype,
 type,
 family,
 method,
 address,
 netmask,
 broadcast,
 gateway,
 mtu,
 vlanrawdevice,
 vlanid,
 options,
 disable,
 dcreate,
 description
FROM netiface_old;


DROP TABLE netiface_old;
COMMIT;
