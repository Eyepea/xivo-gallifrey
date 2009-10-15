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

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$tree = $this->get_var('tree');

if(($parent = $this->get_var('parent')) === null):
	$pid = '';
	$plevel = 0;
else:
	$pid = $parent['id'];
	$plevel = $parent['level'];
endif;

if(is_array($tree) === true && empty($tree) === false):
	if($pid === '' && $plevel === 0):
		echo	'<tr><td>';
	endif;

	$keys = array_keys($tree);
	$nb = count($keys);
	$cnt = $nb - 1;

	for($i = 0;$i < $nb;$i++):
		$v = &$tree[$keys[$i]];

		$mod9 = $i % 9;
		$mod3 = $i % 3;

		if($v['level'] === 3):
			echo	'<div class="acl-category"><div><h4>',
				$form->checkbox(array('desc'	=> array('format'	=> '%{formfield}$s%{description}$s',
									 'description'	=> $this->bbf('acl',$v['id'])),
						      'name'	=> 'tree[]',
						      'label'	=> 'lb-'.$v['id'],
						      'id'	=> $v['id'],
						      'paragraph'	=> false,
						      'value'	=> $v['path'],
						      'checked'	=> $v['access']),
						'onclick="xivo_form_mk_acl(this);"'),
				'</h4>';

			if(isset($v['child']) === true):
				echo	'<span><a href="#"
						  title="',$this->bbf('opt_browse'),'"
						  onclick="dwho_eid(\'table-',$v['id'],'\').style.display =
							   dwho_eid(\'table-',$v['id'],'\').style.display === \'block\'
							   ? \'none\'
							   : \'block\';
							   return(false);">',
					$url->img_html('img/site/button/more.gif',
						       $this->bbf('opt_browse'),
						       'border="0"'),
					'</a></span>';
			endif;

			echo	'</div>';
		else:
			if($i === 0):
				echo	'<table cellspacing="0" cellpadding="0" border="0" id="table-',
					$v['parent']['id'],
					'"><tr><td>',"\n";
			elseif($mod9 === 0):
				echo	'</td></tr><tr><td>',"\n";
			elseif($mod3 === 0):
				echo	'</td><td>';
			endif;

			echo	'<div class="acl-func">',
				$form->checkbox(array('desc'	=> array('format'	=> '%{formfield}$s%{description}$s',
									 'description'	=> $this->bbf('acl',$v['id'])),
						      'name'	=> 'tree[]',
						      'label'	=> 'lb-'.$v['id'],
						      'id'	=> $v['id'],
						      'paragraph'	=> false,
						      'value'	=> $v['path'],
						      'checked'	=> $v['access']),
						'onclick="xivo_form_mk_acl(this);"'),
				'</div>',"\n";

			if($cnt === $i):
				if($mod9 < 3):
					$repeat = 2;
				elseif($mod9 < 6):
					$repeat = 1;
				else:
					echo	'</td>';
					$repeat = 0;
				endif;
				echo	str_repeat('<td>&nbsp;</td>',$repeat),'</tr></table>',"\n";
			endif;

		endif;

		if(isset($v['child']) === true):

			if(isset($v['parent']) === true):
				$parent = $v['parent'];
			else:
				$parent = null;
			endif;

			$this->file_include('bloc/xivo/configuration/manage/acl/tree',
					    array('tree'	=> $v['child'],
						  'parent'	=> $parent));
		endif;
		if($v['level'] === 3):
			echo	'</div>';
		endif;
	endfor;
	if($pid === '' && $plevel === 0):
		echo	'</td></tr>';
	endif;
endif;

?>
