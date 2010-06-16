
CREATE TABLE meetmeguest (
 id integer unsigned,
 meetmefeaturesid integer unsigned NOT NULL,
 fullname varchar(255) NOT NULL,
 telephonenumber varchar(40),
 email varchar(320),
 PRIMARY KEY(id)
);

CREATE INDEX meetmeguest__idx__meetmefeaturesid ON meetmeguest(meetmefeaturesid);
CREATE INDEX meetmeguest__idx__fullname ON meetmeguest(fullname);
CREATE INDEX meetmeguest__idx__email ON meetmeguest(email);

