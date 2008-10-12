<?php

$array = array();

$array['ini'] = array();

$array['sys'] = array();

$array['sys']['php_version'] = array(
				'min'		=> '4.4',
				'max'		=> '5.0');

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
