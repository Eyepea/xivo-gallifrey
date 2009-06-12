INSERT INTO `extenumbers` VALUES (NULL,'*91','880d3330b465056ede825e1fbc8ceb50fd816e1d','','extenfeatures','enablevmbox');
INSERT INTO `extenumbers` VALUES (NULL,'_*91.','936ec7abe6019d9d47d8be047ef6fc0ebc334c00','','extenfeatures','enablevmboxslt');
INSERT INTO `extenumbers` VALUES (NULL,'_*90.','9fdaa61ea338dcccf1450949cbf6f7f99f1ccc54','','extenfeatures','enablevmslt');
INSERT INTO `extenumbers` VALUES (NULL,'*48378','e27276ceefcc71a5d2def28c9b59a6410959eb43','','extenfeatures','guestprov');
INSERT INTO `extenumbers` VALUES (NULL,'_*99.','6c92223f2ea0cfd9fad3db2f288ebdc9c64dc8f5','','extenfeatures','vmboxmsgslt');
INSERT INTO `extenumbers` VALUES (NULL,'_*93.','7d891f90799fd6cb5bc85c4bd227a3357096be8f','','extenfeatures','vmboxpurgeslt');
INSERT INTO `extenumbers` VALUES (NULL,'_*97.','8bdbf6703cf5225aad457422afdda738b9bd628c','','extenfeatures','vmboxslt');
INSERT INTO `extenumbers` VALUES (NULL,'_*92.','36711086667cbfc27488236e0e0fdd2d7f896f6b','','extenfeatures','vmuserpurgeslt');
INSERT INTO `extenumbers` VALUES (NULL,'_*96.','ac6c7ac899867fe0120fe20120fae163012615f2','','extenfeatures','vmuserslt');

UPDATE `extenumbers`
SET `exten` = CONCAT('_',`exten`,'.'),
    `extenhash` = SHA1(CONCAT(`exten`,'.'))
WHERE `type` = 'extenfeatures'
AND `typeval` = 'agentdynamiclogin';

UPDATE `extenumbers`
SET `exten` = CONCAT('_',`exten`,'.'),
    `extenhash` = SHA1(CONCAT(`exten`,'.'))
WHERE `type` = 'extenfeatures'
AND `typeval` = 'agentstaticlogoff'
AND SUBSTR(`exten`,1,1) != '_';

UPDATE `extenumbers`
SET `exten` = '*90',
    `extenhash` = '2fc9fcda52bd8293da1bfa68cbdb8974fafd409e'
WHERE `type` = 'extenfeatures'
AND `typeval` = 'enablevm'
AND `exten` = '*24';

UPDATE `extenumbers`
SET `typeval` = 'vmuserpurge'
WHERE `type` = 'extenfeatures'
AND `typeval` = 'vmdelete';

UPDATE `extenumbers`
SET `exten` = '*92',
    `extenhash` = '97f991a4ffd7fa843bc0ca3bdc730851382c5cdf'
WHERE `type` = 'extenfeatures'
AND `typeval` = 'vmuserpurge'
AND `exten` = '*35';

UPDATE `extenumbers`
SET `typeval` = 'vmusermsg'
WHERE `type` = 'extenfeatures'
AND `typeval` = 'voicemsg';
