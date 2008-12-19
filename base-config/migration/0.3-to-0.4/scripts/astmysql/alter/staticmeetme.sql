RENAME TABLE `meetme` TO `staticmeetme`;

DROP INDEX `meetme__idx__commented` ON `staticmeetme`;
DROP INDEX `meetme__idx__filename` ON `staticmeetme`;
DROP INDEX `meetme__idx__category` ON `staticmeetme`;
DROP INDEX `meetme__idx__var_name` ON `staticmeetme`;

CREATE INDEX `staticmeetme__idx__commented` ON `staticmeetme`(`commented`);
CREATE INDEX `staticmeetme__idx__filename` ON `staticmeetme`(`filename`);
CREATE INDEX `staticmeetme__idx__category` ON `staticmeetme`(`category`);
CREATE INDEX `staticmeetme__idx__var_name` ON `staticmeetme`(`var_name`);

UPDATE `staticmeetme`
SET `var_name` = 'audiobuffers'
WHERE `filename` = 'meetme.conf'
AND `category` = 'general'
AND `var_name` = 'audiobuffer';
