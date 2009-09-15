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
$form = &$this->get_module('form');
$dhtml = &$this->get_module('dhtml');

$pager = $this->get_var('pager');
$act = $this->get_var('act');

$page = $url->pager($pager['pages'],
		    $pager['page'],
		    $pager['prev'],
		    $pager['next'],
		    'xivo/configuration/manage/accesswebservice',
		    array('act' => $act));

?>
<div id="sr-accesswebservice" class="b-list">
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
<form action="#" name="fm-accesswebservice-list" method="post" accept-charset="utf-8">
<?php
	echo	$form->hidden(array('name'	=> XIVO_SESS_NAME,
				    'value'	=> XIVO_SESS_ID)),

		$form->hidden(array('name'	=> 'act',
				    'value'	=> $act)),

		$form->hidden(array('name'	=> 'page',
				    'value'	=> $pager['page']));
?>
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_name');?></th>
		<th class="th-center"><?=$this->bbf('col_login');?></th>
		<th class="th-center"><?=$this->bbf('col_password');?></th>
		<th class="th-center"><?=$this->bbf('col_host');?></th>
		<th class="th-center col-action"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php
	if(($list = $this->get_var('list')) === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="7" class="td-single"><?=$this->bbf('no_accesswebservice');?></td>
	</tr>
<?php
	else:
		for($i = 0;$i < $nb;$i++):

			$ref = &$list[$i];

			if($ref['disable'] === true):
				$icon = 'disable';
			else:
				$icon = 'enable';
			endif;

			if(xivo_has_len($ref['login']) === false):
				$login = '-';
			else:
				$login = xivo_htmlen(xivo_trunc($ref['login'],20,'...',false));
			endif;

			if(xivo_has_len($ref['passwd']) === false):
				$passwd = '-';
			else:
				$passwd = xivo_htmlen(xivo_trunc($ref['passwd'],20,'...',false));
			endif;

			if(xivo_has_len($ref['host']) === false):
				$host = '-';
			else:
				$host = xivo_htmlen(xivo_trunc($ref['host'],20,'...',false));
			endif;
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';"
	    onmouseout="this.className = this.tmp;"
	    class="sb-content l-infos-<?=(($i % 2) + 1)?>on2">
		<td class="td-left">
			<?=$form->checkbox(array('name'		=> 'accesswebservice[]',
						 'value'	=> $ref['id'],
						 'label'	=> false,
						 'id'		=> 'it-accesswebservice-'.$i,
						 'checked'	=> false,
						 'field'	=> false));?>
		</td>
		<td class="txt-left">
			<label for="it-accesswebservice-<?=$i?>" id="lb-accesswebservice-<?=$i?>">
<?php
				echo	$url->img_html('img/site/flag/'.$icon.'.gif',null,'class="icons-list"'),
					xivo_trunc($ref['name'],20,'...',false);
?>
			</label>
		</td>
		<td><?=$login?></td>
		<td><?=$passwd?></td>
		<td><?=$host?></td>
		<td class="td-right" colspan="2">
<?php
			echo	$url->href_html($url->img_html('img/site/button/edit.gif',
							       $this->bbf('opt_modify'),
							       'border="0"'),
						'xivo/configuration/manage/accesswebservice',
						array('act'	=> 'edit',
						      'id' 	=> $ref['id']),
						null,
						$this->bbf('opt_modify')),"\n",
				$url->href_html($url->img_html('img/site/button/key.gif',
							       $this->bbf('opt_acl'),
							       'border="0"'),
						'xivo/configuration/manage/accesswebservice',
						array('act'	=> 'acl',
						      'id'	=> $ref['id']),
						null,
						$this->bbf('opt_acl')),"\n",
				$url->href_html($url->img_html('img/site/button/delete.gif',
							       $this->bbf('opt_delete'),
							       'border="0"'),
						'xivo/configuration/manage/accesswebservice',
						array('act'	=> 'delete',
						      'id'	=> $ref['id'],
						      'page'	=> $pager['page']),
						'onclick="return(confirm(\''.$dhtml->escape($this->bbf('opt_delete_confirm')).'\'));"',
						$this->bbf('opt_delete'));
?>
		</td>
	</tr>
<?php
		endfor;
	endif;
?>
	<tr class="sb-foot">
		<td class="td-left xspan b-nosize"><span class="span-left b-nosize">&nbsp;</span></td>
		<td class="td-center" colspan="5"><span class="b-nosize">&nbsp;</span></td>
		<td class="td-right xspan b-nosize"><span class="span-right b-nosize">&nbsp;</span></td>
	</tr>
</table>
</form>
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
</div>
