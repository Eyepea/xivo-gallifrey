INSERT INTO tmp_queuemember (
	queue_name,
	interface,
	"call-limit",
	paused,
	commented,
	usertype,
	userid,
	channel,
	category)
SELECT
	queuemember.queue_name,
	queuemember.interface,
	queuemember."call-limit",
	NULL,
	queuemember.commented,
	queuemember.usertype,
	queuemember.userid,
	queuemember.channel,
	queuemember.category
FROM queuemember;
