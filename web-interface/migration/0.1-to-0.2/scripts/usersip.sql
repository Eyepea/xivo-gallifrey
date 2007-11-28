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
	CASE WHEN length(ifnull(userfeatures.number,'')) > 0
	THEN userfeatures.number||'@'||
		CASE WHEN usersip.context = 'local-extensions'
		THEN 'default'
		ELSE usersip.context END
	ELSE '' END,
	usersip.md5secret,
	usersip.nat,
	usersip.deny,
	usersip.permit,
	usersip.mask,
	usersip.pickupgroup,
	NULL,
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
INNER JOIN userfeatures
ON userfeatures.protocol = 'sip'
	AND userfeatures.protocolid = usersip.id
WHERE usersip.category = 'user'
	AND usersip.name NOT IN('guest','xivosb');

INSERT INTO tmp_userfeatures (
	protocol,
	protocolid,
	firstname,
	lastname,
	name,
	number,
	context,
	provisioningid,
	ringseconds,
	simultcalls,
	enableclient,
	enablehint,
	enablevoicemail,
	skipvoicemailpass,
	enablexfer,
	enableautomon,
	callrecord,
	callfilter,
	enablednd,
	enableunc,
	destunc,
	enablerna,
	destrna,
	enablebusy,
	destbusy,
	musiconhold,
	outcallerid,
	internal,
	commented,
	description)
SELECT
	userfeatures.protocol,
	tmp_usersip.id,
	userfeatures.firstname,
	userfeatures.lastname,
	lower(tmp_usersip.name),
	userfeatures.number,
	tmp_usersip.context,
	userfeatures.provisioningid,
	userfeatures.ringseconds,
	userfeatures.simultcalls,
	userfeatures.popupwidget,
	1,
	1,
	1,
	0,
	0,
	0,
	0,
	0,
	0,
	'',
	0,
	'',
	0,
	'',
	lower(userfeatures.musiconhold),
	CASE WHEN length(userfeatures.outnumber) > 0 THEN userfeatures.outnumber ELSE 'default' END,
	0,
	tmp_usersip.commented,
	userfeatures.comment
FROM userfeatures
INNER JOIN tmp_usersip
ON userfeatures.protocol = 'sip'
	AND lower(userfeatures.name) = lower(tmp_usersip.name)
	AND tmp_usersip.category = 'user' AND tmp_usersip.name != 'guest';

INSERT INTO tmp_extensions (
	commented,
	context,
	exten,
	priority,
	app,
	appdata,
	name)
SELECT
	0,
	tmp_userfeatures.context,
	tmp_userfeatures.number,
	1,
	'Macro',
	'incoming_user',
	''
FROM tmp_userfeatures
WHERE length(tmp_userfeatures.number) > 0
        AND tmp_userfeatures.number IS NOT NULL;

INSERT INTO tmp_extensions (
	commented,
	context,
	exten,
	priority,
	app,
	appdata,
	name)
SELECT
	0,
	'hints',
	tmp_userfeatures.number,
	1,
	'Macro',
	'SIP/'||tmp_userfeatures.name,
	''
FROM tmp_userfeatures
WHERE length(tmp_userfeatures.number) > 0
        AND tmp_userfeatures.number IS NOT NULL;

INSERT INTO tmp_extenumbers (
	exten,
	extenhash,
	context,
	type,
	typeval)
SELECT
	tmp_userfeatures.number,
	'tohash;'||tmp_userfeatures.number,
	tmp_userfeatures.context,
	'user',
	tmp_userfeatures.id
FROM tmp_userfeatures
WHERE length(tmp_userfeatures.number) > 0
        AND tmp_userfeatures.number IS NOT NULL;

COMMIT;
