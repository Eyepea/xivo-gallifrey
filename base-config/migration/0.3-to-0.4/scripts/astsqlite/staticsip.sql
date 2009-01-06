UPDATE generalsip
SET var_name = 'mohinterpret'
WHERE filename = 'sip.conf'
AND category = 'general'
AND var_name = 'musiconhold';

UPDATE generalsip
SET var_val = NULL, commented = 1
WHERE filename = 'sip.conf'
AND category = 'general'
AND var_name = 'context'
AND var_val = 'default';

UPDATE generalsip
SET var_val = 'no', commented = 0
WHERE filename = 'sip.conf'
AND category = 'general'
AND var_name = 'insecure'
AND var_val IS NULL;

UPDATE generalsip
SET var_val = 'yes', commented = 0
WHERE filename = 'sip.conf'
AND category = 'general'
AND var_name = 'callevents';

UPDATE generalsip
SET var_val = 'yes', commented = 0
WHERE filename = 'sip.conf'
AND category = 'general'
AND var_name = 'ignoreregexpire';

DELETE FROM generalsip
WHERE filename = 'sip.conf'
AND category = 'general'
AND var_name IN('tos','ospauth');

INSERT INTO generalsip (
	id,
	cat_metric,
	var_metric,
	commented,
	filename,
	category,
	var_name,
	var_val)
SELECT
	NULL,
	tmp_staticsip.cat_metric,
	tmp_staticsip.var_metric,
	tmp_staticsip.commented,
	tmp_staticsip.filename,
	tmp_staticsip.category,
	tmp_staticsip.var_name,
	tmp_staticsip.var_val
FROM tmp_staticsip
WHERE filename = 'sip.conf'
AND category = 'general'
AND var_name IN('allowsubscribe',
		'allowoverlap',
		'limitonpeer',
		'buggymwi',
		'tos_sip',
		'tos_audio',
		'tos_video',
		't38pt_udptl',
		't38pt_rtp',
		't38pt_tcp',
		't38pt_usertpsource',
		'matchexterniplocally',
		'g726nonstandard',
		't1min',
		'rfc2833compensate',
		'directrtpsetup',
		'minexpiry',
		'notifyhold',
		'allowtransfer',
		'maxcallbitrate',
		'autoframing',
		'jbenable',
		'jbforce',
		'jbmaxsize',
		'jbresyncthreshold',
		'jbimpl',
		'jblog',
		'mohsuggest',
		'rtsavesysname',
		'subscribecontext');

DELETE FROM tmp_staticsip;

INSERT INTO tmp_staticsip (
	id,
	cat_metric,
	var_metric,
	commented,
	filename,
	category,
	var_name,
	var_val)
SELECT
	generalsip.id,
	generalsip.cat_metric,
	generalsip.var_metric,
	generalsip.commented,
	generalsip.filename,
	generalsip.category,
	generalsip.var_name,
	generalsip.var_val
FROM generalsip;

INSERT INTO tmp_contextmember (
	context,
	type,
	typeval,
	varname)
SELECT
	tmp_staticsip.var_val,
	'generalsip',
	'',
	tmp_staticsip.var_name
FROM tmp_staticsip
WHERE filename = 'sip.conf'
AND category = 'general'
AND var_name IN('context','regcontext')
AND NULLIF(var_val,'') IS NOT NULL
AND commented = 0;
