BEGIN TRANSACTION;

INSERT INTO tmp_usersip (
	name,
	commented,
	accountcode,
	amaflags,
	callgroup,
	callerid,
	canreinvite,
	context,
	defaultip,
	dtmfmode,
	fromuser,
	fromdomain,
	fullcontact,
	host,
	insecure,
	language,
	mailbox,
	md5secret,
	nat,
	deny,
	permit,
	mask,
	pickupgroup,
	port,
	qualify,
	restrictcid,
	rtptimeout,
	rtpholdtimeout,
	secret,
	type,
	username,
	disallow,
	allow,
	musiconhold,
	regseconds,
	ipaddr,
	regexten,
	cancallforward,
	setvar,
	'call-limit',
	category)
SELECT
	lower(usersip.name),
	usersip.commented,
	usersip.accountcode,
	usersip.amaflags,
	usersip.callgroup,
	usersip.callerid,
	usersip.canreinvite,
	CASE WHEN usersip.context = 'local-extensions' THEN 'default' ELSE usersip.context END,
	usersip.defaultip,
	usersip.dtmfmode,
	usersip.fromuser,
	usersip.fromdomain,
	usersip.fullcontact,
	usersip.host,
	usersip.insecure,
	usersip.language,
	usersip.mailbox,
	usersip.md5secret,
	usersip.nat,
	usersip.deny,
	usersip.permit,
	usersip.mask,
	usersip.pickupgroup,
	nullif(usersip.port,''),
	usersip.qualify,
	usersip.restrictcid,
	usersip.rtptimeout,
	usersip.rtpholdtimeout,
	usersip.secret,
	usersip.type,
	usersip.username,
	usersip.disallow,
	usersip.allow,
	lower(usersip.musiconhold),
	usersip.regseconds,
	usersip.ipaddr,
	NULL,
	usersip.cancallforward,
	usersip.setvar,
	usersip.'call-limit',
	usersip.category
FROM usersip
INNER JOIN trunkfeatures
ON trunkfeatures.trunk = 'sip'
	AND trunkfeatures.trunkid = usersip.id
WHERE usersip.category = 'trunk';

INSERT INTO tmp_trunkfeatures (
	trunk,
	trunkid,
	registerid,
	registercommented)
SELECT
	trunkfeatures.trunk,
	tmp_usersip.id,
	ifnull(tmp_generalsip.id,0),
	ifnull(tmp_generalsip.commented,0)
FROM trunkfeatures
INNER JOIN usersip
INNER JOIN tmp_usersip
LEFT JOIN generalsip
LEFT JOIN tmp_generalsip
ON trunkfeatures.trunk = 'sip'
	AND trunkfeatures.trunkid = usersip.id
	AND lower(usersip.name) = lower(tmp_usersip.name)
	AND tmp_usersip.category = 'trunk'
	AND trunkfeatures.registerid = generalsip.id
	AND generalsip.filename = 'sip.conf'
	AND generalsip.category = 'general'
	AND generalsip.var_name = 'register'
	AND tmp_generalsip.filename = 'sip.conf'
	AND tmp_generalsip.category = 'general'
	AND tmp_generalsip.var_name = 'register'
	AND generalsip.var_val = tmp_generalsip.var_val;

COMMIT;
