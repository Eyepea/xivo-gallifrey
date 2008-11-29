BEGIN TRANSACTION;

INSERT INTO tmp_groupfeatures (
	id,
	name,
	number,
	context,
	timeout,
	deleted)
SELECT
	groupfeatures.id,
	lower(groupfeatures.name),
	groupfeatures.number,
	CASE WHEN groupfeatures.context = 'local-extensions' THEN 'default' ELSE groupfeatures.context END,
	0,
	groupfeatures.commented
FROM groupfeatures;

INSERT INTO tmp_extensions (
	commented,
	context,
	exten,
	priority,
	app,
	appdata,
	name)
SELECT
	0,
	tmp_groupfeatures.context,
	tmp_groupfeatures.number,
	1,
	'Macro',
	'incoming_group',
	''
FROM tmp_groupfeatures
WHERE length(tmp_groupfeatures.number) > 0
        AND tmp_groupfeatures.number IS NOT NULL;

INSERT INTO tmp_extenumbers (
	exten,
	extenhash,
	context,
	type,
	typeval)
SELECT
	tmp_groupfeatures.number,
	'tohash;'||tmp_groupfeatures.number,
	tmp_groupfeatures.context,
	'group',
	tmp_groupfeatures.id
FROM tmp_groupfeatures
WHERE length(tmp_groupfeatures.number) > 0
        AND tmp_groupfeatures.number IS NOT NULL;

COMMIT;
