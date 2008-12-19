INSERT INTO `contextnummember` (
	`context`,
	`type`,
	`typeval`,
	`number`)
SELECT
	`userfeatures`.`context`,
	'user',
	`userfeatures`.`id`,
	`userfeatures`.`number`
FROM `userfeatures`
WHERE NOT `internal`;

INSERT INTO `contextnummember` (
	`context`,
	`type`,
	`typeval`,
	`number`)
SELECT
	`groupfeatures`.`context`,
	'group',
	`groupfeatures`.`id`,
	`groupfeatures`.`number`
FROM `groupfeatures`
WHERE NOT `deleted`;

INSERT INTO `contextnummember` (
	`context`,
	`type`,
	`typeval`,
	`number`)
SELECT
	`queuefeatures`.`context`,
	'queue',
	`queuefeatures`.`id`,
	`queuefeatures`.`number`
FROM `queuefeatures`;

INSERT INTO `contextnummember` (
	`context`,
	`type`,
	`typeval`,
	`number`)
SELECT
	`meetmefeatures`.`context`,
	'meetme',
	`meetmefeatures`.`id`,
	`meetmefeatures`.`number`
FROM `meetmefeatures`;

INSERT INTO `contextnummember` (
	`context`,
	`type`,
	`typeval`,
	`number`)
SELECT
	`incall`.`context`,
	'incall',
	`incall`.`id`,
	`incall`.`exten`
FROM `incall`;
