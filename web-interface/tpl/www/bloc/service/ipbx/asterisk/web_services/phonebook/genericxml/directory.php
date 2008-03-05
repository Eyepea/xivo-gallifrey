<?php

header('Content-Type: text/xml; charset=utf-8');

$url = &$this->get_module('url');

$list = $this->get_var('list');
$pos = (int) $this->get_var('pos');
$prevpos = $this->get_var('prevpos');

$tagmenu = $this->get_var('tagmenu');
$tagdirectory = $this->get_var('tagdirectory');

echo '<',$tagdirectory,'>',"\n";
/*
 * A day maybe...
 *

if($prevpos > 0):

	$prevparam = array();
	$prevparam['node'] = 1;
	$prevparam['pos'] = floor($pos / $prevpos) * $prevpos;
	$prevparam['name'] = $this->get_var('name');

	echo	'<',$tagmenu,'>',"\n",
		'<MenuItem>',"\n",
		'<Name>[',xivo_htmlsc($this->bbf('phone_back'),ENT_NOQUOTES),']</Name>',"\n",
		'<URL>',$url->href('service/ipbx/web_services/phonebook/search',$prevparam,true,$this->get_var('argseparator'),false),'</URL>',"\n",
		'</MenuItem>',"\n",
		'</',$tagmenu,'>',"\n";
endif;

*/
if(is_array($list) === false || ($nb = count($list)) === 0):
	echo	'<DirectoryEntry>',"\n",
		'<Name>',$this->bbf('phone_noentry'),'</Name>',"\n",
		'<Telephone></Telephone>',"\n",
		'</DirectoryEntry>',"\n";
else:
	for($i = 0;$i < $nb;$i++):
		$ref = &$list[$i];

		if(isset($ref['additionaltype']) === true && $ref['additionaltype'] === 'custom'):
			if($ref['additionaltext'] === ''):
				$name = $this->bbf('phone_name-empty',$ref['name']);
			else:
				$name = $this->bbf('phone_name-custom',array($ref['name'],$ref['type']));
			endif;
		else:
			$name = $this->bbf('phone_name-'.$ref['type'],$ref['name']);
		endif;

		echo	'<DirectoryEntry>',"\n",
			'<Name>',xivo_htmlsc($name,ENT_NOQUOTES),'</Name>',"\n",
			'<Telephone>',xivo_htmlsc($ref['phone'],ENT_NOQUOTES),'</Telephone>',"\n",
			'</DirectoryEntry>',"\n";
	endfor;
endif;

echo '</',$tagdirectory,'>';

?>
