SELECT
	'UPDATE tmp_usersip
	SET setvar = '||QUOTE('XIVO_USERID='||userfeatures.id)||', "call-limit" = '||(userfeatures.simultcalls * 2)||'
	WHERE id = '||QUOTE(usersip.id)||';'
FROM
	userfeatures,
	usersip
WHERE
	usersip.id = userfeatures.protocolid
	AND userfeatures.protocol = 'sip';
