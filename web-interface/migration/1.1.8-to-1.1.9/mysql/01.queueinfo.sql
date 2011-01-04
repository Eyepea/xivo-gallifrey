
CREATE TABLE IF NOT EXISTS `queue_info` (
	`id` int(10) unsigned auto_increment,
	`call_time_t` int unsigned,
	`queue_name` varchar(255) NOT NULL,
	`caller` varchar(255) NOT NULL,
	`caller_uniqueid` varchar(255) NOT NULL,
	`call_picker` varchar(255),
	`hold_time` int, 
	`talk_time` int,
	PRIMARY KEY(`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
 
CREATE INDEX `queue_info_call_time_t_index` ON `queue_info`(`call_time_t`);
CREATE INDEX `queue_info_queue_name_index` ON `queue_info`(`queue_name`);

