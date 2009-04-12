START TRANSACTION;

GRANT ALL PRIVILEGES ON `xivo`.* TO `xivo`@`localhost` IDENTIFIED BY PASSWORD '*DBA86DFECE903EB25FE460A66BDCDA790A1CA4A4';
CREATE DATABASE IF NOT EXISTS `xivo` DEFAULT CHARACTER SET utf8;

USE `xivo`;

DROP TABLE IF EXISTS `ctilog`;
CREATE TABLE `ctilog` (
 `eventdate` timestamp DEFAULT 0,
 `loginclient` varchar(64),
 `company` varchar(64),
 `status` varchar(32),
 `action` varchar(32),
 `arguments` varchar(255) NOT NULL,
 `callduration` int(10) unsigned
);

CREATE INDEX `ctilog__idx__eventdate` ON `ctilog`(`eventdate`);
CREATE INDEX `ctilog__idx__loginclient` ON `ctilog`(`loginclient`);
CREATE INDEX `ctilog__idx__company` ON `ctilog`(`company`);
CREATE INDEX `ctilog__idx__status` ON `ctilog`(`status`);
CREATE INDEX `ctilog__idx__action` ON `ctilog`(`action`);
CREATE INDEX `ctilog__idx__arguments` ON `ctilog`(`arguments`);
CREATE INDEX `ctilog__idx__callduration` ON `ctilog`(`callduration`);

COMMIT;
