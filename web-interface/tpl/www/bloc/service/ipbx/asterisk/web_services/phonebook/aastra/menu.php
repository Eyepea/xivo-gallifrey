<?php

$url = &$this->get_module('url');
$xmlphone = &$this->get_module('xmlphone',array('vendor' => $this->get_var('vendor')));

$list = $this->get_var('list');
$node = $this->get_var('node');

$tagmenu = $xmlphone->get_tag('menu');
$argseparator = $xmlphone->get_argseparator();

if(is_array($list) === false || ($nb = count($list)) === 0):
	$tagdirectory = $xmlphone->get_tag('directory');
	$previous = ' previous="'.$this->url('service/ipbx/web_services/phonebook/search',true).'"';

	echo	'<',$tagdirectory,$previous,' destroyOnExit="yes" style="none">',"\n",
		'<MenuItem>',"\n",
		'<Prompt>',$xmlphone->escape($this->bbf('phone_noentries')),'</Prompt>',"\n",
		'<URI></URI>',"\n",
		'</MenuItem>',"\n",
		'</',$tagdirectory,'>';
else:
	echo '<',$tagmenu,' style="none" destroyOnExit="yes">',"\n";

	$param = array();
	$param['name'] = $this->get_var('name');
	$param['node'] = $node > 1 ? $node - 1 : $node;

	if($node < $this->get_var('maxnode')):
		$prevparam = $param;
		$prevparam['node'] = $node + 1;
		$prevparam['prevpos'] = $this->get_var('prevpos');

		echo	'<MenuItem>',"\n",
			'<Prompt>[',$xmlphone->escape($this->bbf('phone_back')),']</Prompt>',"\n",
			'<URI>',$url->href('service/ipbx/web_services/phonebook/search',$prevparam,true,$argseparator,false),'</URI>',"\n",
			'</MenuItem>',"\n";
	endif;

	if($node === 1):
		$param['directory'] = true;
	endif;

	for($i = 0;$i < $nb;$i++):
		$ref = &$list[$i];

		if(isset($ref[0]['additionaltype']) === true && $ref[0]['additionaltype'] === 'custom'):
			if($ref[0]['additionaltext'] === ''):
				$name1 = $this->bbf('phone_name-empty',$ref[0]['name']);
			else:
				$name1 = $this->bbf('phone_name-custom',array($ref[0]['name'],$ref[0]['type']));
			endif;
		else:
			$name1 = $this->bbf('phone_name-'.$ref[0]['type'],$ref[0]['name']);
		endif;

		if(isset($ref[1]['additionaltype']) === true && $ref[1]['additionaltype'] === 'custom'):
			if($ref[1]['additionaltext'] === ''):
				$name2 = $this->bbf('phone_name-empty',$ref[1]['name']);
			else:
				$name2 = $this->bbf('phone_name-custom',array($ref[1]['name'],$ref[1]['type']));
			endif;
		else:
			$name2 = $this->bbf('phone_name-'.$ref[1]['type'],$ref[1]['name']);
		endif;

		$param['pos'] = $ref[2];

		echo	'<MenuItem>',"\n",
			'<Prompt>',$xmlphone->escape(xivo_trunc($name1,8,'.','',true).' > '.xivo_trunc($name2,8,'.','',true)),'</Prompt>',"\n",
			'<URI>',$url->href('service/ipbx/web_services/phonebook/search',$param,true,$argseparator,false),'</URI>',"\n",
			'</MenuItem>',"\n";
	endfor;

	echo '</',$tagmenu,'>';
endif;
?>
