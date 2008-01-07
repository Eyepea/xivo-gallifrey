BEGIN TRANSACTION;

INSERT INTO tmp_outcalltrunk (
	outcallid,
	trunkfeaturesid,
	priority)
SELECT
	outcall.id,
	outcall.trunkfeaturesid,
	0
FROM outcall;

INSERT INTO tmp_outcall (
	id,
	name,
	exten,
	context,
	externprefix,
	stripnum,
	setcallerid,
	callerid,
	useenum,
	internal,
	hangupringtime,
	commented)
SELECT
	outcall.id,
	outcall.name,
	outcall.exten,
	outcall.context,
	outcall.externprefix,
	outcall.stripnum,
	outcall.setcallerid,
	outcall.callerid,
	outcall.useenum,
	outcall.internal,
	outcall.hangupringtime,
	outcall.commented
FROM outcall;

COMMIT;
