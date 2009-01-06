RENAME TABLE `generalsip` TO `staticsip`;

DROP INDEX `generalsip__idx__commented` ON `staticsip`;
DROP INDEX `generalsip__idx__filename` ON `staticsip`;
DROP INDEX `generalsip__idx__category` ON `staticsip`;
DROP INDEX `generalsip__idx__var_name` ON `staticsip`;

CREATE INDEX `staticsip__idx__commented` ON `staticsip`(`commented`);
CREATE INDEX `staticsip__idx__filename` ON `staticsip`(`filename`);
CREATE INDEX `staticsip__idx__category` ON `staticsip`(`category`);
CREATE INDEX `staticsip__idx__var_name` ON `staticsip`(`var_name`);

UPDATE `staticsip`
SET `var_name` = 'mohinterpret'
WHERE `filename` = 'sip.conf'
AND `category` = 'general'
AND `var_name` = 'musiconhold';

UPDATE `staticsip`
SET `var_val` = NULL, `commented` = 1
WHERE `filename` = 'sip.conf'
AND `category` = 'general'
AND `var_name` = 'context'
AND `var_val` = 'default';

UPDATE `staticsip`
SET `var_val` = 'no', `commented` = 0
WHERE `filename` = 'sip.conf'
AND `category` = 'general'
AND `var_name` = 'insecure'
AND `var_val` IS NULL;

UPDATE `staticsip`
SET `var_val` = 'yes', `commented` = 0
WHERE `filename` = 'sip.conf'
AND `category` = 'general'
AND `var_name` = 'callevents';

UPDATE `staticsip`
SET `var_val` = 'yes', `commented` = 0
WHERE `filename` = 'sip.conf'
AND `category` = 'general'
AND `var_name` = 'ignoreregexpire';

DELETE FROM `staticsip`
WHERE `filename` = 'sip.conf'
AND `category` = 'general'
AND `var_name` IN('tos','ospauth');
