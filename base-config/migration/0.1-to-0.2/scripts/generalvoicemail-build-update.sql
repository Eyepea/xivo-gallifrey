SELECT
	'UPDATE tmp_generalvoicemail SET var_val = '||quote(generalvoicemail.var_val)||', commented = '||quote(generalvoicemail.commented)||'  
	WHERE filename = '||quote(generalvoicemail.filename)||' 
		AND category = '||quote(generalvoicemail.category)||' 
		AND var_name = '||quote(generalvoicemail.var_name)||';' 
FROM generalvoicemail
WHERE generalvoicemail.category != 'zonemessages';
