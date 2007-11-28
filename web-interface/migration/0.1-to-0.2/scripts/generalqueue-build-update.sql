SELECT
	'UPDATE tmp_generalqueue SET var_val = '||quote(generalqueue.var_val)||', commented = '||quote(generalqueue.commented)||' 
	WHERE filename = '||quote(generalqueue.filename)||' 
		AND category = '||quote(generalqueue.category)||' 
		AND var_name = '||quote(generalqueue.var_name)||';' 
FROM generalqueue;
