SELECT 'UPDATE tmp_trunkfeatures SET registerid = 0, registercommented = 0
	WHERE registerid = '||quote(tmp_trunkfeatures.registerid)||'
		AND trunk = '||quote(tmp_trunkfeatures.trunk)||';
	
	DELETE FROM tmp_general'||tmp_trunkfeatures.trunk||' WHERE id = '||quote(tmp_trunkfeatures.registerid)||';'
FROM tmp_trunkfeatures
WHERE tmp_trunkfeatures.registerid != 0
GROUP BY tmp_trunkfeatures.trunk, tmp_trunkfeatures.registerid HAVING COUNT(*) > 1;
