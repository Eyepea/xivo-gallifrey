RENAME TABLE `agent` TO `staticagent`;

DROP INDEX `agent__idx__commented` ON `staticagent`;
DROP INDEX `agent__idx__filename` ON `staticagent`;
DROP INDEX `agent__idx__category` ON `staticagent`;
DROP INDEX `agent__idx__var_name` ON `staticagent`;

CREATE INDEX `staticagent__idx__commented` ON `staticagent`(`commented`);
CREATE INDEX `staticagent__idx__filename` ON `staticagent`(`filename`);
CREATE INDEX `staticagent__idx__category` ON `staticagent`(`category`);
CREATE INDEX `staticagent__idx__var_name` ON `staticagent`(`var_name`);

UPDATE `staticagent`
SET `var_name` = 'endcall', `var_val` = 'yes'
WHERE `filename` = 'agents.conf'
AND `category` = 'agents'
AND `var_name` = 'createlink';

UPDATE `staticagent`
SET `var_name` = 'autologoffunavail', `var_val` = 'no'
WHERE `filename` = 'agents.conf'
AND `category` = 'agents'
AND `var_name` = 'savecallsin';
