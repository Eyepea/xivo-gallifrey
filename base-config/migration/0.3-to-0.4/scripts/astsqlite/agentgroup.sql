DELETE FROM tmp_agentgroup;

INSERT INTO tmp_agentgroup (
	id,
	groupid,
	name,
	groups,
	commented,
	deleted,
	description)
SELECT
	agentgroup.id,
	agentgroup.groupid,
	agentgroup.name,
	agentgroup.groups,
	agentgroup.commented,
	agentgroup.deleted,
	''
FROM agentgroup;
