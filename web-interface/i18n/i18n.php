#!/usr/bin/php
<?php

if(($path_conf = realpath('../conf/')) === false)
        die('ERR : CONF DIRECTORY NOT FOUND');

$conff_xivo = $path_conf.'/prepend.inc';

if(!(is_file($conff_xivo) && is_readable($conff_xivo)))
        die('ERR : CONF LOAD FILE XIVO NOT FOUND OR UNREADABLE');

require_once($conff_xivo);

if(!(is_dir(XIVO_PATH_I18N) && is_writable(XIVO_PATH_I18N)))
	die('ERR : I18N DIRECTORY NOT FOUND OR UNWRITABLE');

require_once(XIVO_PATH_LIBS.'/locales.inc');

if(($di18n = opendir(XIVO_PATH_I18N)) !== false)
{
	while(($dlang = readdir($di18n)) !== false)
	{
		if($dlang != '.' && $dlang != '..' && is_dir($dlang))
		{
			echo $dlang."\n";
		}
	}

	closedir($di18n);
}

?>
