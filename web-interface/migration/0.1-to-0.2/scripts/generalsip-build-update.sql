SELECT
	'UPDATE tmp_generalsip SET var_val = '||quote(generalsip.var_val)||', commented = '||quote(generalsip.commented)||' 
	WHERE filename = '||quote(generalsip.filename)||' 
		AND category = '||quote(generalsip.category)||' 
		AND var_name = '||quote(generalsip.var_name)||';' 
FROM generalsip
WHERE generalsip.var_name != 'register'
	AND generalsip.var_name != 'context';
