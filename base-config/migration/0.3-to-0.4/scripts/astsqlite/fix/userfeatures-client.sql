SELECT
	userfeatures.id,
	IFNULL(voicemail.email,IFNULL(NULLIF(userfeatures.number,''),userfeatures.name)),
	CASE WHEN
		LENGTH(IFNULL(voicemail.password,'')) < 4
	THEN
		'0000'
	ELSE
		voicemail.password
	END
FROM userfeatures
LEFT JOIN voicemail
ON userfeatures.voicemailid = voicemail.uniqueid
WHERE userfeatures.enableclient = 1
AND userfeatures.internal = 0;
