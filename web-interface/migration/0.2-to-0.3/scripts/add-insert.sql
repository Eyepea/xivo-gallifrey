BEGIN TRANSACTION;

INSERT INTO extensions VALUES (NULL,0,'features','_*37.',1,'Macro','bsfilter|${EXTEN:3}','bsfilter');

INSERT INTO extenumbers VALUES (NULL,'_*37.','249b00b17a5983bbb2af8ed0af2ab1a74abab342','','extenfeatures','bsfilter');

COMMIT;
