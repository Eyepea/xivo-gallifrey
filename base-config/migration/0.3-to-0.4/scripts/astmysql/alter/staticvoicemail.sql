RENAME TABLE `generalvoicemail` TO `staticvoicemail`;

DROP INDEX `generalvoicemail__idx__commented` ON `staticvoicemail`;
DROP INDEX `generalvoicemail__idx__filename` ON `staticvoicemail`;
DROP INDEX `generalvoicemail__idx__category` ON `staticvoicemail`;
DROP INDEX `generalvoicemail__idx__var_name` ON `staticvoicemail`;

CREATE INDEX `staticvoicemail__idx__commented` ON `staticvoicemail`(`commented`);
CREATE INDEX `staticvoicemail__idx__filename` ON `staticvoicemail`(`filename`);
CREATE INDEX `staticvoicemail__idx__category` ON `staticvoicemail`(`category`);
CREATE INDEX `staticvoicemail__idx__var_name` ON `staticvoicemail`(`var_name`);

UPDATE `staticvoicemail`
SET `var_val` = NULL, `commented` = 1
WHERE `filename` = 'voicemail.conf'
AND `category` = 'general'
AND `var_name` = 'cidinternalcontexts'
AND `var_val` = 'default';
