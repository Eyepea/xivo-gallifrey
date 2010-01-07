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
$pos = (int) $this->get_var('pos');
$prevpos = $this->get_var('prevpos');

$tagdirectory = $xmlvendor->tag_directory();

echo '<',$tagdirectory,'>',"\n";

if($xmlvendor->get_vendor() === 'thomson' && $prevpos > 0):
	$prevparam = array();
	$prevparam['node'] = 1;
	$prevparam['pos'] = floor($pos / $prevpos) * $prevpos;
	$prevparam['name'] = $this->get_var('name');

	echo	'<MenuItem>',"\n",
		'<Item>[',$xmlvendor->escape($this->bbf('phone_back')),']</Item>',"\n",
		'<URL>',$url->href('service/ipbx/web_services/phonebook/search',
				   $prevparam,
				   true,
				   $xmlvendor->arg_separator(),
				   false),
		'</URL>',"\n",
		'</MenuItem>',"\n";
endif;

if(is_array($list) === false || ($nb = count($list)) === 0):
	echo	'<DirectoryEntry>',"\n",
		'<Name>',$xmlvendor->escape($this->bbf('phone_noentries')),'</Name>',"\n",
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
			'<Name>',$xmlvendor->escape($name),'</Name>',"\n",
			'<Telephone>',$xmlvendor->escape($ref['phone']),'</Telephone>',"\n",
			'</DirectoryEntry>',"\n";
	endfor;
endif;

echo '</',$tagdirectory,'>';

?>
