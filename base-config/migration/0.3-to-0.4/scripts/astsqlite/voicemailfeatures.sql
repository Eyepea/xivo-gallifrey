INSERT INTO tmp_voicemailfeatures (
	id,
	voicemailid,
	skipcheckpass)
SELECT
	NULL,
	uservoicemail.uniqueid,
	userfeatures.skipvoicemailpass
FROM
	userfeatures,
	uservoicemail,
	usersip
WHERE
	usersip.mailbox = uservoicemail.mailbox||'@'||uservoicemail.context
	AND userfeatures.protocol = 'sip'
	AND userfeatures.protocolid = usersip.id;

INSERT INTO tmp_voicemailfeatures (
	id,
	voicemailid,
	skipcheckpass)
SELECT
	NULL,
	uservoicemail.uniqueid,
	userfeatures.skipvoicemailpass
FROM
	userfeatures,
	uservoicemail,
	useriax
WHERE
	useriax.mailbox = uservoicemail.mailbox||'@'||uservoicemail.context
	AND userfeatures.protocol = 'iax'
	AND userfeatures.protocolid = useriax.id;
