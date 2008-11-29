BEGIN TRANSACTION;

INSERT INTO tmp_phone (
	macaddr,
	vendor,
	model,
	proto,
	iduserfeatures,
	isinalan)
SELECT
	phone.macaddr,
	lower(phone.vendor),
	lower(phone.model),
	lower(phone.proto),
	0,
	ifnull(phone.isinalan,0)
FROM phone
WHERE ifnull(phone.iduserfeatures,0) = 0;

INSERT INTO tmp_phone (
	macaddr,
	vendor,
	model,
	proto,
	iduserfeatures,
	isinalan)
SELECT
	phone.macaddr,
	lower(phone.vendor),
	lower(phone.model),
	lower(phone.proto),
	tmp_userfeatures.id,
	ifnull(phone.isinalan,0)
FROM phone
INNER JOIN userfeatures
	ON userfeatures.id = phone.iduserfeatures
	AND lower(userfeatures.protocol) = lower(phone.proto)
INNER JOIN tmp_userfeatures
	ON lower(tmp_userfeatures.name) = lower(userfeatures.name)
	AND lower(tmp_userfeatures.protocol) = lower(userfeatures.protocol);

COMMIT;
