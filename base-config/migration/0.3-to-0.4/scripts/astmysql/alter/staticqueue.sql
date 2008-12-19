RENAME TABLE `generalqueue` TO `staticqueue`;

DROP INDEX `generalqueue__idx__commented` ON `staticqueue`;
DROP INDEX `generalqueue__idx__filename` ON `staticqueue`;
DROP INDEX `generalqueue__idx__category` ON `staticqueue`;
DROP INDEX `generalqueue__idx__var_name` ON `staticqueue`;

CREATE INDEX `staticqueue__idx__commented` ON `staticqueue`(`commented`);
CREATE INDEX `staticqueue__idx__filename` ON `staticqueue`(`filename`);
CREATE INDEX `staticqueue__idx__category` ON `staticqueue`(`category`);
CREATE INDEX `staticqueue__idx__var_name` ON `staticqueue`(`var_name`);
