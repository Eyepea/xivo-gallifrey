BEGIN TRANSACTION;

INSERT INTO tmp_cdr (
	calldate,
	clid,
	src,
	dst,
	dcontext,
	channel,
	dstchannel,
	lastapp,
	lastdata,
	answer,
	end,
	duration,
	billsec,
	disposition,
	amaflags,
	accountcode,
	uniqueid,
	userfield)
SELECT
	calldate,
	clid,
	src,
	dst,
	dcontext,
	channel,
	dstchannel,
	lastapp,
	lastdata,
	answer,
	end,
	duration,
	billsec,
	disposition,
	amaflags,
	accountcode,
	uniqueid,
	userfield
FROM cdr ORDER BY calldate ASC;

COMMIT;
