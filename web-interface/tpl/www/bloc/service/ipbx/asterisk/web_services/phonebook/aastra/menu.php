<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

$url = &$this->get_module('url');
$xmlphone = &$this->get_module('xmlphone');
$xmlvendor = $xmlphone->factory($this->get_var('vendor'));

$list = $this->get_var('list');
$node = $this->get_var('node');

$tagmenu = $xmlvendor->tag_menu();
$argseparator = $xmlvendor->arg_separator();

if(is_array($list) === false || ($nb = count($list)) === 0):
	$previous = $this->url('service/ipbx/web_services/phonebook/search',true);

	if($xmlvendor->has_softkeys() === true):
		$tagdirectory = $xmlvendor->tag_directory();

		echo	'<',$tagdirectory,' previous="',$previous,'" destroyOnExit="yes">',"\n",
			'<MenuItem>',"\n",
			'<Prompt>',$xmlvendor->escape($this->bbf('phone_noentries')),'</Prompt>',"\n",
			'<URI></URI>',"\n",
			'</MenuItem>',"\n",
			'</',$tagdirectory,'>';
	else:
		echo	'<',$tagmenu,' style="none" destroyOnExit="yes">',"\n",
			'<MenuItem>',"\n",
			'<Prompt>',$xmlvendor->escape($this->bbf('phone_noentries')),'</Prompt>',"\n",
			'<URI></URI>',"\n",
			'</MenuItem>',"\n",
			'<MenuItem>',"\n",
			'<Prompt>[',$xmlvendor->escape($this->bbf('phone_back')),']</Prompt>',"\n",
			'<URI>',$previous,'</URI>',"\n",
			'</MenuItem>',"\n",
			'</',$tagmenu,'>',"\n";
	endif;
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
			'<Prompt>[',$xmlvendor->escape($this->bbf('phone_back')),']</Prompt>',"\n",
			'<URI>',$url->href('service/ipbx/web_services/phonebook/search',
					   $prevparam,
					   true,
					   $argseparator,
					   false,true,true,true),
			'</URI>',"\n",
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
			'<Prompt>',
			$xmlvendor->escape(dwho_trunc($name1,8,'.','',true).' > '.dwho_trunc($name2,8,'.','',true)),
			'</Prompt>',"\n",
			'<URI>',$url->href('service/ipbx/web_services/phonebook/search',
					   $param,
					   true,
					   $argseparator,
					   false,true,true,true),
			'</URI>',"\n",
			'</MenuItem>',"\n";
	endfor;

	echo '</',$tagmenu,'>';
endif;
?>
