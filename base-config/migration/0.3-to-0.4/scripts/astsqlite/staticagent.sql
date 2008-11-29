UPDATE agent
SET var_name = 'endcall', var_val = 'yes'
WHERE filename = 'agents.conf'
AND category = 'agents'
AND var_name = 'createlink';

UPDATE agent
SET var_name = 'autologoffunavail', var_val = 'no'
WHERE filename = 'agents.conf'
AND category = 'agents'
AND var_name = 'savecallsin';

INSERT INTO agent (
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
	tmp_staticagent.cat_metric,
	tmp_staticagent.var_metric,
	tmp_staticagent.commented,
	tmp_staticagent.filename,
	tmp_staticagent.category,
	tmp_staticagent.var_name,
	tmp_staticagent.var_val
FROM tmp_staticagent
WHERE filename = 'agents.conf'
AND category = 'general'
AND var_name = 'multiplelogin';

DELETE FROM tmp_staticagent;

INSERT INTO tmp_staticagent (
	id,
	cat_metric,
	var_metric,
	commented,
	filename,
	category,
	var_name,
	var_val)
SELECT
	agent.id,
	agent.cat_metric,
	agent.var_metric,
	agent.commented,
	agent.filename,
	agent.category,
	agent.var_name,
	agent.var_val
FROM agent;
