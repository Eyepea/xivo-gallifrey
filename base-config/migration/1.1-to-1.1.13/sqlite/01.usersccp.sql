
CREATE TABLE usersccp_tmp AS SELECT * FROM usersccp;

DROP TABLE usersccp;
CREATE TABLE usersccp
(
 id integer unsigned,
 name varchar(128),
 devicetype varchar(64),            -- phone model, ie 7960
 keepalive tinyint unsigned,        -- i.e 60
 tzoffset varchar(3),               -- ie: +1 == Europe/Paris
 dtmfmode varchar(16),               -- outofband, inband
 transfer varchar(3),                -- on, off, NULL
 park varchar(3),                    -- on, off, NULL
 cfwdall varchar(3),                 -- on, off, NULL
 cfwdbusy varchar(3),                -- on, off, NULL
 cfwdnoanswer varchar(3),            -- on, off, NULL
 mwilamp varchar(5),                 -- on, off, wink, flash, blink, NULL
 mwioncall varchar(3),               -- on, off, NULL
 dnd varchar(6),                     -- on, off, NULL
 pickupexten varchar(3),             -- on, off, NULL
 pickupcontext varchar(64),          -- pickup context name
 pickupmodeanswer varchar(3),        -- on, off, NULL
 permit varchar(31),                 -- 192.168.0.0/255.255.255.0
 deny varchar(31),                   -- 0.0.0.0/0.0.0.0
 addons varchar(24),                 -- comma separated addons list. i.e 7914,7914
 imageversion varchar(64),           -- i.e P00405000700
 trustphoneip varchar(3),            -- yes, no, NULL
 nat varchar(3),                     -- on, off, NULL
 directrtp varchar(3),               -- on, off, NULL
 earlyrtp varchar(7),                -- none, offhook, dial, ringout, NULLon, off, NULL
 private varchar(3),                 -- on, off, NULL
 privacy varchar(4),                 -- on, off, full, NULL
 protocol varchar(4) NOT NULL DEFAULT 'sccp', -- required for join with userfeatures

 -- softkeys
 softkey_onhook      varchar(1024),
 softkey_connected   varchar(1024),
 softkey_onhold      varchar(1024),
 softkey_ringin      varchar(1024),
 softkey_offhook     varchar(1024),
 softkey_conntrans   varchar(1024),
 softkey_digitsfoll  varchar(1024),
 softkey_connconf    varchar(1024),
 softkey_ringout     varchar(1024),
 softkey_offhookfeat varchar(1024),
 softkey_onhint      varchar(1024),
           
 defaultline integer unsigned,
 commented tinyint(1) NOT NULL DEFAULT 0,
 PRIMARY KEY(id)
);

CREATE UNIQUE INDEX usersccp__uidx__name ON usersccp(name);

INSERT INTO usersccp  SELECT * FROM usersccp_tmp;

