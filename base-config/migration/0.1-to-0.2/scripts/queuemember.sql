BEGIN TRANSACTION;

INSERT INTO tmp_queuemember (
	queue_name,
	interface,
	penalty,
	'call-limit',
	commented,
	usertype,
	userid,
	channel,
	category)
SELECT
	lower(queue_name),
	CASE WHEN tmp_userfeatures.protocol = 'sip' THEN 'SIP' ELSE 'IAX2' END||'/'||tmp_userfeatures.name,
	0,
	0,
	queuemember.commented,
	'user',
	tmp_userfeatures.id,
	CASE WHEN tmp_userfeatures.protocol = 'sip' THEN 'SIP' ELSE 'IAX2' END,
	'group'
FROM queuemember
INNER JOIN tmp_userfeatures
ON queuemember.interface LIKE 'local/'||tmp_userfeatures.number||'@%';

COMMIT;
