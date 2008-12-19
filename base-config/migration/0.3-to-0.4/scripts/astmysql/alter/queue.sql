UPDATE `queue`
SET
	`context` = NULLIF(`context`,''),
	`monitor-join` = IF(`monitor-join` IN(0,'no'),0,2),
	`queue-youarenext` = IF(`queue-youarenext` IS NULL,
				'queue-youarenext',
				CONCAT('/var/lib/pf-xivo/sounds/acd/',SUBSTRING(`queue-youarenext`,46))),
	`queue-thereare` = IF(`queue-thereare` IS NULL,
			      'queue-thereare',
			      CONCAT('/var/lib/pf-xivo/sounds/acd/',SUBSTRING(`queue-thereare`,46))),
	`queue-callswaiting` = IF(`queue-callswaiting` IS NULL,
				  'queue-callswaiting',
				  CONCAT('/var/lib/pf-xivo/sounds/acd/',SUBSTRING(`queue-callswaiting`,46))),
	`queue-holdtime` = IF(`queue-holdtime` IS NULL,
			      'queue-holdtime',
			      CONCAT('/var/lib/pf-xivo/sounds/acd/',SUBSTRING(`queue-holdtime`,46))),
	`queue-minutes` = IF(`queue-minutes` IS NULL,
			     'queue-minutes',
			     CONCAT('/var/lib/pf-xivo/sounds/acd/',SUBSTRING(`queue-minutes`,46))),
	`queue-seconds` = IF(`queue-seconds` IS NULL,
			     'queue-seconds',
			     CONCAT('/var/lib/pf-xivo/sounds/acd/',SUBSTRING(`queue-seconds`,46))),
	`queue-lessthan` = IF(`queue-lessthan` IS NULL,
			      'queue-lessthan',
			      CONCAT('/var/lib/pf-xivo/sounds/acd/',SUBSTRING(`queue-lessthan`,46))),
	`queue-thankyou` = IF(`queue-thankyou` IS NULL,
			      'queue-thankyou',
			      CONCAT('/var/lib/pf-xivo/sounds/acd/',SUBSTRING(`queue-thankyou`,46))),
	`queue-reporthold` = IF(`queue-reporthold` IS NULL,
				'queue-reporthold',
				CONCAT('/var/lib/pf-xivo/sounds/acd/',SUBSTRING(`queue-reporthold`,46))),
	`periodic-announce` = IF(`periodic-announce` IS NULL,
				 'queue-periodic-announce',
				 CONCAT('/var/lib/pf-xivo/sounds/acd/',SUBSTRING(`periodic-announce`,46))),
	`eventmemberstatus` = 1,
	`eventwhencalled` = 1,
	`monitor-join` = IF(`monitor-join` = 1,2,1);

ALTER TABLE `queue` CHANGE COLUMN `monitor-join` `monitor-type` enum('no','mixmonitor');
ALTER TABLE `queue` MODIFY COLUMN `wrapuptime` tinyint(2) unsigned;

UPDATE `queue` SET `monitor-type` = NULL WHERE `monitor-type` = 'no';
