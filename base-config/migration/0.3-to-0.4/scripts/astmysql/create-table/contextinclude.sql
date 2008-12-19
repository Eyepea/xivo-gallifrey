DROP TABLE IF EXISTS `contextinclude`;
CREATE TABLE `contextinclude` (
 `context` varchar(39) NOT NULL,
 `include` varchar(39) NOT NULL,
 `priority` tinyint(2) unsigned NOT NULL DEFAULT 0,
 PRIMARY KEY(`context`,`include`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

CREATE INDEX `contextinclude__idx__context` ON `contextinclude`(`context`);
CREATE INDEX `contextinclude__idx__include` ON `contextinclude`(`include`);
CREATE INDEX `contextinclude__idx__priority` ON `contextinclude`(`priority`);
