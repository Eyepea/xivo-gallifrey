RENAME TABLE `generaliax` TO `staticiax`;

DROP INDEX `generaliax__idx__commented` ON `staticiax`;
DROP INDEX `generaliax__idx__filename` ON `staticiax`;
DROP INDEX `generaliax__idx__category` ON `staticiax`;
DROP INDEX `generaliax__idx__var_name` ON `staticiax`;

CREATE INDEX `staticiax__idx__commented` ON `staticiax`(`commented`);
CREATE INDEX `staticiax__idx__filename` ON `staticiax`(`filename`);
CREATE INDEX `staticiax__idx__category` ON `staticiax`(`category`);
CREATE INDEX `staticiax__idx__var_name` ON `staticiax`(`var_name`);

UPDATE `staticiax`
SET `var_val` = NULL, `commented` = 1
WHERE `filename` = 'iax.conf'
AND `category` = 'general'
AND `var_name` = 'tos';

UPDATE `staticiax`
SET `var_name` = 'transfer', `var_val` = 'yes'
WHERE `filename` = 'iax.conf'
AND `category` = 'general'
AND `var_name` = 'notransfer'
AND `var_val` != 'yes';

UPDATE `staticiax`
SET `var_name` = 'transfer', var_val = 'no'
WHERE `filename` = 'iax.conf'
AND `category` = 'general'
AND `var_name` = 'notransfer'
AND var_val = 'yes';

UPDATE `staticiax`
SET var_val = 0
WHERE `filename` = 'iax.conf'
AND `category` = 'general'
AND `var_name` = 'maxauthreq'
AND var_val = 3;

UPDATE `staticiax`
SET `var_val` = 'yes', `commented` = 0
WHERE `filename` = 'iax.conf'
AND `category` = 'general'
AND `var_name` = 'rtignoreregexpire';

DELETE FROM `staticiax`
WHERE `filename` = 'iax.conf'
AND `category` = 'general'
AND `var_name` IN('dropcount',
		  'minexcessbuffer',
		  'maxexcessbuffer',
		  'jittershrinkrate',
		  'mailboxdetail');
