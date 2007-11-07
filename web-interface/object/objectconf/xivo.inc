<?php

define('XIVO_LABEL','XIVO-WEBI');

$array = array();

$array['ini'] = array();

$array['sys'] = array();

$array['sys']['general'] = array(
				'soft_name'	=> 'xivo',
				'soft_label'	=> 'XIVO',
				'soft_version'	=> '0.2',
				'soft_url'	=> 'www.xivo.fr',
				'corp_address'	=> '67 rue Voltaire',
				'corp_zipcode'	=> '92800',
				'corp_city'	=> 'Puteaux',
				'corp_country'	=> 'France',
				'corp_phone'	=> '+33 (0) 1 41 38 99 60',
				'corp_fax'	=> '+33 (0) 1 41 38 99 70',
				'corp_name'	=> 'proformatique',
				'corp_label'	=> 'Proformatique',
				'corp_email'	=> 'contact@proformatique.com',
				'corp_url'	=> 'www.proformatique.com');

$array['sys']['php_version'] = array(
				'min'		=> '4.4',
				'max'		=> '5.0');

$array['sys']['extension'] = array(
				'prefix'	=> '',
				'suffix'	=> '',
				'const'		=> 'XIVO_EXT__');

$array['sys']['separator'] = array(
				'dir'		=> '/',
				'path'		=> ':',
				'arg_in'	=> '&',
				'arg_out'	=> '&');

$array['sys']['session'] = array(
				'name'		=> '_eid',
				'time'		=> 7200,
				'path'		=> '/',
				'max_per_user'	=> 1);

$array['sys']['locale'] = array(
				'language'	=> 'fr',
				'territory'	=> 'FR',
				'charset'	=> 'UTF-8');
?>
