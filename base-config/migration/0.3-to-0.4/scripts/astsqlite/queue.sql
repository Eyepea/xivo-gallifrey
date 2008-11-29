INSERT INTO tmp_queue (
	name,
	musiconhold,
	announce,
	context,
	timeout,
	"monitor-type",
	"monitor-format",
	"queue-youarenext",
	"queue-thereare",
	"queue-callswaiting",
	"queue-holdtime",
	"queue-minutes",
	"queue-seconds",
	"queue-lessthan",
	"queue-thankyou",
	"queue-reporthold",
	"periodic-announce",
	"announce-frequency",
	"periodic-announce-frequency",
	"announce-round-seconds",
	"announce-holdtime",
	retry,
	wrapuptime,
	maxlen,
	servicelevel,
	strategy,
	joinempty,
	leavewhenempty,
	eventmemberstatus,
	eventwhencalled,
	reportholdtime,
	memberdelay,
	weight,
	timeoutrestart,
	commented,
	category)
SELECT
	queue.name,
	queue.musiconhold,
	queue.announce,
	NULLIF(queue.context,''),
	queue.timeout,
	CASE WHEN queue."monitor-join" = 'no' THEN NULL ELSE 'mixmonitor' END,
	queue."monitor-format",
	CASE WHEN queue."queue-youarenext" IS NULL
	THEN
		'queue-youarenext'
	ELSE
		'/var/lib/pf-xivo/sounds/acd/'||SUBSTR(queue."queue-youarenext",46,LENGTH(queue."queue-youarenext"))
	END,
	CASE WHEN queue."queue-thereare" IS NULL
	THEN
		'queue-thereare'
	ELSE
		'/var/lib/pf-xivo/sounds/acd/'||SUBSTR(queue."queue-thereare",46,LENGTH(queue."queue-thereare"))
	END,
	CASE WHEN queue."queue-callswaiting" IS NULL
	THEN
		'queue-callswaiting'
	ELSE
		'/var/lib/pf-xivo/sounds/acd/'||SUBSTR(queue."queue-callswaiting",46,LENGTH(queue."queue-callswaiting"))
	END,
	CASE WHEN queue."queue-holdtime" IS NULL
	THEN
		'queue-holdtime'
	ELSE
		'/var/lib/pf-xivo/sounds/acd/'||SUBSTR(queue."queue-holdtime",46,LENGTH(queue."queue-holdtime"))
	END,
	CASE WHEN queue."queue-minutes" IS NULL
	THEN
		'queue-minutes'
	ELSE
		'/var/lib/pf-xivo/sounds/acd/'||SUBSTR(queue."queue-minutes",46,LENGTH(queue."queue-minutes"))
	END,
	CASE WHEN queue."queue-seconds" IS NULL
	THEN
		'queue-seconds'
	ELSE
		'/var/lib/pf-xivo/sounds/acd/'||SUBSTR(queue."queue-seconds",46,LENGTH(queue."queue-seconds"))
	END,
	CASE WHEN queue."queue-lessthan" IS NULL
	THEN
		'queue-lessthan'
	ELSE
		'/var/lib/pf-xivo/sounds/acd/'||SUBSTR(queue."queue-lessthan",46,LENGTH(queue."queue-lessthan"))
	END,
	CASE WHEN queue."queue-thankyou" IS NULL
	THEN
		'queue-thankyou'
	ELSE
		'/var/lib/pf-xivo/sounds/acd/'||SUBSTR(queue."queue-thankyou",46,LENGTH(queue."queue-thankyou"))
	END,
	CASE WHEN queue."queue-reporthold" IS NULL
	THEN
		'queue-reporthold'
	ELSE
		'/var/lib/pf-xivo/sounds/acd/'||SUBSTR(queue."queue-reporthold",46,LENGTH(queue."queue-reporthold"))
	END,
	CASE WHEN queue."periodic-announce" IS NULL
	THEN
		'queue-periodic-announce'
	ELSE
		'/var/lib/pf-xivo/sounds/acd/'||SUBSTR(queue."periodic-announce",46,LENGTH(queue."periodic-announce"))
	END,
	queue."announce-frequency",
	queue."periodic-announce-frequency",
	queue."announce-round-seconds",
	queue."announce-holdtime",
	queue.retry,
	queue.wrapuptime,
	queue.maxlen,
	queue.servicelevel,
	queue.strategy,
	queue.joinempty,
	queue.leavewhenempty,
	1,
	1,
	queue.reportholdtime,
	queue.memberdelay,
	queue.weight,
	queue.timeoutrestart,
	queue.commented,
	queue.category
FROM queue;

INSERT INTO tmp_contextmember (
	context,
	type,
	typeval,
	varname)
SELECT
	tmp_queue.context,
	'queue',
	tmp_queue.name,
	'context'
FROM tmp_queue
WHERE context IS NOT NULL;
