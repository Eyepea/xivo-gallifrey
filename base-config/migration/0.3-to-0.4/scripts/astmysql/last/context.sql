INSERT INTO `context` (
	`name`,
	`displayname`,
	`entity`,
	`commented`,
	`description`)
SELECT
	`contextmember`.`context`,
	`contextmember`.`context`,
	NULL,
	0,
	''
FROM `contextmember`
GROUP BY `context`;

INSERT INTO `context` (
	`name`,
	`displayname`,
	`entity`,
	`commented`,
	`description`)
SELECT
	`contextnummember`.`context`,
	`contextnummember`.`context`,
	NULL,
	0,
	''
FROM `contextnummember`
LEFT JOIN `context`
ON `contextnummember`.`context` = `context`.`name`
WHERE `context`.`name` IS NULL
GROUP BY `contextnummember`.`context`;
