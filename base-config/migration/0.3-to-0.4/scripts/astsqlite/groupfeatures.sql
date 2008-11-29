INSERT INTO tmp_groupfeatures (
	id,
	name,
	number,
	context,
	transfer_user,
	transfer_call,
	write_caller,
	write_calling,
	timeout,
	preprocess_subroutine,
	deleted)
SELECT
	groupfeatures.id,
	groupfeatures.name,
	groupfeatures.number,
	groupfeatures.context,
	0,
	0,
	0,
	0,
	groupfeatures.timeout,
	NULL,
	groupfeatures.deleted
FROM groupfeatures;

INSERT INTO tmp_contextnummember (
	context,
	type,
	typeval,
	number)
SELECT
	tmp_groupfeatures.context,
	'group',
	tmp_groupfeatures.id,
	IFNULL(tmp_groupfeatures.number,'')
FROM tmp_groupfeatures
WHERE deleted = 0;
