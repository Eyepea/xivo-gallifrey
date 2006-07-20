#!/usr/bin/php
<?php

if(($path_conf = realpath('../conf/')) === false)
        die('ERR : XIVO CONF DIRECTORY NOT FOUND');

$conff_xivo = $path_conf.'/prepend.inc';

if(!(is_file($conff_xivo) === true && is_readable($conff_xivo) === true))
        die('ERR : XIVO PREPEND FILE NOT FOUND OR UNREADABLE');

require_once($conff_xivo);

if(!(is_dir(XIVO_PATH_I18N) === true && is_readable(XIVO_PATH_I18N) === true && is_writable(XIVO_PATH_I18N) === true))
	trigger_error('ERR : XIVO I18N DIRECTORY NOT FOUND OR UNREADABLE OR UNWRITABLE',E_USER_ERROR);

require_once(XIVO_PATH_LIBS.'/i18n.inc');

$tmp = array();

$fi = finfo_open(FILEINFO_MIME);

if(($di18n = opendir(XIVO_PATH_I18N)) !== false)
{
	while(($dlang = readdir($di18n)) !== false)
	{
		$meta_lang = $dlang;
		$dlang = XIVO_PATH_I18N.'/'.$dlang;

		if(is_dir($dlang) === true && isset($_CF['i18n']['locales'][$meta_lang]) === true)
		{
			if(is_readable($dlang) === false || is_writable($dlang) === false)
			{
				trigger_error('ERR : '.$dlang.' DIRECTORY UNREADBLE OR UNWRITABLE',E_USER_ERROR);
				continue;
			}

			$finc_lang = $dlang.'/'.$meta_lang.'.inc';

			if(is_file($dlang.'/'.$meta_lang.'.inc') === false)
			{
				touch($finc_lang);
			}

			if(is_readable($finc_lang) === false || is_writable($finc_lang) === false)
			{
				trigger_error('ERR : '.$finc_lang.' FILE UNREADBLE OR UNWRITABLE',E_USER_ERROR);
				continue;
			}

			if(($dfi18n = opendir($dlang)) !== false)
			{
				while(($fi18n = readdir($dfi18n)) !== false)
				{
					$fdlang = $dlang.'/'.$fi18n;

					if(is_file($fdlang) === true && is_readable($fdlang) === true
					&& preg_match('/^([a-z0-9-_]+)\.i18n$/',$fi18n,$match) === 1)
					{
						if(!isset($tmp[$fdlang]))
							$tmp[$fdlang] = array('lang' => $meta_lang,'namespace' => $match[1],'file' => file($fdlang),'finfo' => finfo_file($fi,$fdlang));
					}	
				}
				closedir($dfi18n);
			}
		}
	}

	closedir($di18n);
}

finfo_close($fi);
print_r($tmp);

if(empty($tmp) === false)
{
	$tmp_lang = array();
	$k = array_keys($tmp);
	$nb_files = count($k);
	$key = '';
	$txt = false;
	$_CF['i18n']['lang'] = array();

	for($i = 0;$i < $nb_files;$i++)
	{
		$a = &$tmp[$k[$i]];

		if(is_array($a) === true && isset($a['namespace'],$a['file'],$a['lang']) && is_array($a['file']) === true
		&& ($nb_lines = count($a['file'])) > 0)
		{
			print_r($a);
			if(!isset($_CF['i18n']['lang'][$a['lang']]))
				$_CF['i18n']['lang'][$a['lang']] = array();

			$_CF['i18n']['lang'][$a['lang']][$a['namespace']] = array();

			$ref_namespace = &$_CF['i18n']['lang'][$a['lang']][$a['namespace']];

			for($j = 0;$j < $nb_lines;$j++)
			{
				$line = trim($a['file'][$j]);

				if(!isset($line{0}))
				{
					$txt = false;
					continue;
				}
				
				switch($line{0})
				{
					case '#':
						$txt = false;
						break;
					case ';':
						if(preg_match('/^;(\t*| *)([a-z0-9-_]+)$/',$line,$match) === 1)
						{
							$txt = true;
							$key = $match[2];
						}
						break;
					default:
						if($txt === true && $k !== '')
						{
							if(!isset($ref_namespace[$key]))
								$ref_namespace[$key] = $line;
							else
								$ref_namespace[$key] .= "\n".$line;
						}
						else $txt = false;
				}
			}
		}
	}
}

print_r($_CF['i18n']['lang']);

?>
