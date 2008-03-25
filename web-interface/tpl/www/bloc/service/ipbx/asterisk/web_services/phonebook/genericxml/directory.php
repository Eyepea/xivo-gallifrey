<?php

$url = &$this->get_module('url');
$xmlphone = &$this->get_module('xmlphone',array('vendor' => $this->get_var('vendor')));

$list = $this->get_var('list');
$pos = (int) $this->get_var('pos');
$prevpos = $this->get_var('prevpos');

$tagdirectory = $xmlphone->get_tag('directory');

echo '<',$tagdirectory,'>',"\n";
/*
 * A day maybe...
 *

if($prevpos > 0):

	$prevparam = array();
	$prevparam['node'] = 1;
	$prevparam['pos'] = floor($pos / $prevpos) * $prevpos;
	$prevparam['name'] = $this->get_var('name');

	$tagmenu = $xmlphone->get_tag('menu');

	echo	'<',$tagmenu,'>',"\n",
		'<MenuItem>',"\n",
		'<Name>[',$xmlphone->escape($this->bbf('phone_back')),']</Name>',"\n",
		'<URL>',$url->href('service/ipbx/web_services/phonebook/search',$prevparam,true,$xmlphone->get_argseparator(),false),'</URL>',"\n",
		'</MenuItem>',"\n",
		'</',$tagmenu,'>',"\n";

endif;

*/
if(is_array($list) === false || ($nb = count($list)) === 0):
	echo	'<DirectoryEntry>',"\n",
		'<Name>',$xmlphone->escape($this->bbf('phone_noentries')),'</Name>',"\n",
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
			'<Name>',$xmlphone->escape($name),'</Name>',"\n",
			'<Telephone>',$xmlphone->escape($ref['phone']),'</Telephone>',"\n",
			'</DirectoryEntry>',"\n";
	endfor;
endif;

echo '</',$tagdirectory,'>';

?>
