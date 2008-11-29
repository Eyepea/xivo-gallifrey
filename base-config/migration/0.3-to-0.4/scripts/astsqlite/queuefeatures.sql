INSERT INTO tmp_queuefeatures (
	id,
	name,
	number,
	context,
	data_quality,
	hitting_callee,
	hitting_caller,
	retries,
	ring,
	transfer_user,
	transfer_call,
	write_caller,
	write_calling,
	url,
	announceoverride,
	timeout,
	preprocess_subroutine)
SELECT
	queuefeatures.id,
	queuefeatures.name,
	queuefeatures.number,
	queuefeatures.context,
	queuefeatures.data_quality,
	queuefeatures.hitting_callee,
	queuefeatures.hitting_caller,
	queuefeatures.retries,
	queuefeatures.ring,
	queuefeatures.transfer_user,
	queuefeatures.transfer_call,
	queuefeatures.write_caller,
	queuefeatures.write_calling,
	queuefeatures.url,
	queuefeatures.announceoverride,
	queuefeatures.timeout,
	NULL
FROM queuefeatures;

INSERT INTO tmp_contextnummember (
	context,
	type,
	typeval,
	number)
SELECT
	tmp_queuefeatures.context,
	'queue',
	tmp_queuefeatures.id,
	IFNULL(tmp_queuefeatures.number,'')
FROM tmp_queuefeatures;
