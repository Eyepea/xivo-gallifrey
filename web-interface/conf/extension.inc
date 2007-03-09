<?php

define('XIVO_EXT__ZLIB',1);
define('XIVO_EXT__MBSTRING',2);
define('XIVO_EXT__FILEINFO',4);
define('XIVO_EXT__MIME_MAGIC',8);
define('XIVO_EXT__MYSQL',16);
define('XIVO_EXT__SQLITE',32);
define('XIVO_EXT__PGSQL',64);

$array = array();

$array[XIVO_EXT__ZLIB] = array(
				'meta'		=> 'zlib',
				'label'		=> 'Zlib Compression',
				'load'		=> 1,
				'optionnal'	=> 2,
				'const'		=> 'XIVO_EXT__ZLIB');

$array[XIVO_EXT__MBSTRING] = array(
				'meta'		=> 'mbstring',
				'label'		=> 'Multibyte String',
				'load'		=> 1,
				'optionnal'	=> 2,
				'const'		=> 'XIVO_EXT__MBSTRING');

$array[XIVO_EXT__FILEINFO] = array(
				'meta'		=> 'fileinfo',
				'label'		=> 'Fileinfo',
				'load'		=> 1,
				'optionnal'	=> 2,
				'const'		=> 'XIVO_EXT__FILEINFO');

$array[XIVO_EXT__MIME_MAGIC] = array(
				'meta'		=> 'mime_magic',
				'label'		=> 'Mimetype',
				'load'		=> 1,
				'optionnal'	=> 2,
				'const'		=> 'XIVO_EXT__MIME_MAGIC');

$array[XIVO_EXT__MYSQL]	= array(
				'meta'		=> 'mysql',
				'label'		=> 'MySQL',
				'load'		=> 2,
				'optionnal'	=> 1,
				'const'		=> 'XIVO_EXT__MYSQL');

$array[XIVO_EXT__SQLITE] = array(
				'meta'		=> 'sqlite',
				'label'		=> 'SQLite',
				'load'		=> 2,
				'optionnal'	=> 1,
				'const'		=> 'XIVO_EXT__SQLITE');

$array[XIVO_EXT__PGSQL]	= array(
				'meta'		=> 'pgsql',
				'label'		=> 'PostgreSQL',
				'load'		=> 2,
				'optionnal'	=> 1,
				'const'		=> 'XIVO_EXT__PGSQL');

?>
