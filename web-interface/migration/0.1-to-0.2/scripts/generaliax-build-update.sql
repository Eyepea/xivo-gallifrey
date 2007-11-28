SELECT
	'UPDATE tmp_generaliax SET var_val = '||quote(generaliax.var_val)||', commented = '||quote(generaliax.commented)||' 
	WHERE filename = '||quote(generaliax.filename)||' 
		AND category = '||quote(generaliax.category)||' 
		AND var_name = '||quote(generaliax.var_name)||';' 
FROM generaliax
WHERE generaliax.var_name != 'register'
	AND generaliax.var_name != 'context';
