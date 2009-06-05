<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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
$pos = (int) $this->get_var('pos');
$prevpos = $this->get_var('prevpos');

$tagmenu = $xmlvendor->tag_menu();
$tagdirectory = $xmlvendor->tag_directory();

$argseparator = $xmlvendor->arg_separator();
$has_softkeys = $xmlvendor->has_softkeys();

if($prevpos > 0):
	$prevparam = array();
	$prevparam['node'] = 1;
	$prevparam['pos'] = floor($pos / $prevpos) * $prevpos;
	$prevparam['name'] = $this->get_var('name');

	$previous = $url->href('service/ipbx/web_services/phonebook/search',
			       $prevparam,
			       true,
			       $argseparator,
			       false);
else:
	$previous = '';
endif;

if($has_softkeys === true):
	$tagmain = $tagdirectory;

	echo	'<',$tagdirectory,
		($previous === '' ? '' : 'previous="'.$previous.'"'),
		' destroyOnExit="yes">',"\n";
else:
	$tagmain = $tagmenu;

	echo	'<',$tagmenu,' style="none" destroyOnExit="yes">',"\n";
endif;

if(is_array($list) === false || ($nb = count($list)) === 0):
	if($has_softkeys === true):
		echo	'<MenuItem>',"\n",
			'<Prompt>',$xmlvendor->escape($this->bbf('phone_noentries')),'</Prompt>',"\n",
			'<URI></URI>',"\n",
			'</MenuItem>',"\n";
	else:
		echo	'<MenuItem>',"\n",
			'<Prompt>',$xmlvendor->escape($this->bbf('phone_noentries')),'</Prompt>',"\n",
			'<URI></URI>',"\n",
			'</MenuItem>',"\n",
			'<MenuItem>',"\n",
			'<Prompt>[',$xmlvendor->escape($this->bbf('phone_back')),']</Prompt>',"\n",
			'<URI>',$previous,'</URI>',"\n",
			'</MenuItem>',"\n";
	endif;
else:
	if($has_softkeys === false):
		echo	'<MenuItem>',"\n",
			'<Prompt>[',$xmlvendor->escape($this->bbf('phone_back')),']</Prompt>',"\n",
			'<URI>',$previous,'</URI>',"\n",
			'</MenuItem>',"\n";
	endif;

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

		echo	'<MenuItem>',"\n",
			'<Prompt>',$xmlvendor->escape($name),'</Prompt>',"\n",
			'<URI>',($has_softkeys === false ? 'Dial:' : ''),
				$xmlvendor->escape($ref['phone']),'</URI>',"\n",
			'</MenuItem>',"\n";
	endfor;
endif;

echo '</',$tagmain,'>';

?>
