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

$list = $this->get_var('list');
$pos = (int) $this->get_var('pos');
$prevpos = $this->get_var('prevpos');

echo '<ol>',"\n";

if($prevpos > 0):
	$prevparam = array();
	$prevparam['node'] = 1;
	$prevparam['pos'] = floor($pos / $prevpos) * $prevpos;
	$prevparam['name'] = $this->get_var('name');

	echo	'<li>[',$url->href_html(dwho_htmlsc($this->bbf('phone_back')),
					'service/ipbx/web_services/phonebook/search',
					$prevparam,
					null,
					null,
					true,
					null,
					false,true,true,true),']<br /></li>',"\n";
endif;

if(is_array($list) === false || ($nb = count($list)) === 0):
	echo	'<li>',dwho_htmlsc($this->bbf('phone_noentries')),'<br /></li>',"\n";
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

		echo	'<li><a href="tel://',$ref['phone'],'">',dwho_htmlsc($name),'</a><br /></li>',"\n";
	endfor;
endif;

echo '</ol>';

?>
