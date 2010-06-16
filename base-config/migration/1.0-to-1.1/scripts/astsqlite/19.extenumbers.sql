
-- INSERT
INSERT INTO extenumbers VALUES (NULL,'_*30.','7758898081b262cc0e42aed23cf601fba8969b08','','extenfeatures','agentstaticlogtoggle');
INSERT INTO extenumbers VALUES (NULL,'_*664.','9dfe780f1dc7fccbfc841b41a38933d4dab56369','','extenfeatures','callgroup');
INSERT INTO extenumbers VALUES (NULL,'_*667.','666f6f18439eb7f205b5932d7f9aef6d2e5ba9a3','','extenfeatures','callmeetme');
INSERT INTO extenumbers VALUES (NULL,'_*665.','7e2df45aedebded219eaa5fb84d6db7e8e24fc66','','extenfeatures','callqueue');
INSERT INTO extenumbers VALUES (NULL,'*26','f8aeb70618cc87f1143c7dff23cdc0d3d0a48a0c','','extenfeatures','callrecord');
INSERT INTO extenumbers VALUES (NULL,'_*666.','d7b68f456ddb50215670c5bfca921176a21c4270','','extenfeatures','calluser');
INSERT INTO extenumbers VALUES (NULL,'_*51.','fd3d50358d246ab2fbc32e14056e2f559d054792','','extenfeatures','groupaddmember');
INSERT INTO extenumbers VALUES (NULL,'_*52.','069a278d266d0cf2aa7abf42a732fc5ad109a3e6','','extenfeatures','groupremovemember');
INSERT INTO extenumbers VALUES (NULL,'_*50.','53f7e7fa7fbbabb1245ed8dedba78da442a8659f','','extenfeatures','grouptogglemember');
INSERT INTO extenumbers VALUES (NULL,'_*735.','32e9b3597f8b9cd2661f0c3d3025168baafca7e6','','extenfeatures','phoneprogfunckey');
INSERT INTO extenumbers VALUES (NULL,'_*56.','95d84232b10af6f6905dcd22f4261a4550461c7d','','extenfeatures','queueaddmember');
INSERT INTO extenumbers VALUES (NULL,'_*57.','3ad1e945e85735f6417e5a0aba7fde3bc9d2ffec','','extenfeatures','queueremovemember');
INSERT INTO extenumbers VALUES (NULL,'_*55.','f8085e23f56e5433006483dee5fe3db8c94a0a06','','extenfeatures','queuetogglemember');


-- DELETE
DELETE FROM extenumbers WHERE type = 'extenfeatures' AND exten = '*20';
DELETE FROM extenumbers WHERE type = 'extenfeatures' AND exten = '*21';
DELETE FROM extenumbers WHERE type = 'extenfeatures' AND exten = '*22';
DELETE FROM extenumbers WHERE type = 'extenfeatures' AND exten = '*23';

-- UPDATE
UPDATE extenumbers SET
		typeval = 'callrecord'
	WHERE
		type    = 'extenfeatures' AND
		exten   = '*26';

