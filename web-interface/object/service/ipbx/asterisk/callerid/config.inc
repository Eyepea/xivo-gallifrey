<?php

$array = array();
$array['element'] = array();

$array['element']['mode'] = array();
$array['element']['mode']['value'] = array('prepend','overwrite','append');
$array['element']['mode']['default'] = '';
$array['element']['mode']['null'] = true;

$array['element']['callerdisplay'] = array();
$array['element']['callerdisplay']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('callfilter','incall','group','queue');
$array['element']['type']['default'] = '';

$array['element']['typeval'] = array();
$array['element']['typeval']['default'] = '';

$array['filter'] = array();
$array['filter']['mode'] = array('set' => false,'chk' => 2,'key' => array('prepend','overwrite','append'));
$array['filter']['callerdisplay'] = array('set'		=> false,
					  'chk'		=> 2,
					  'maxlen'	=> 80,
					  'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_CALLERID.'$/u',
					  'notag'	=> false);
$array['filter']['type'] = array('key' => array('callfilter','incall','group','queue'));
$array['filter']['typeval'] = array('cast' => 'ulongint');

?>
