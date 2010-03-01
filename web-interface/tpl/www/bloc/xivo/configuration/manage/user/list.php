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
$form = &$this->get_module('form');
$dhtml = &$this->get_module('dhtml');

$pager = $this->get_var('pager');
$act = $this->get_var('act');

$page = $url->pager($pager['pages'],
		    $pager['page'],
		    $pager['prev'],
		    $pager['next'],
		    'xivo/configuration/manage/user',
		    array('act' => $act));

?>
<div class="b-list">
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
<form action="#" name="fm-user-list" method="post" accept-charset="utf-8">
<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),

		$form->hidden(array('name'	=> 'act',
				    'value'	=> $act)),

		$form->hidden(array('name'	=> 'page',
				    'value'	=> $pager['page']));
?>
<table id="table-main-listing" cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_login');?></th>
		<th class="th-center"><?=$this->bbf('col_password');?></th>
		<th class="th-center"><?=$this->bbf('col_type');?></th>
		<th class="th-center"><?=$this->bbf('col_dcreate');?></th>
		<th class="th-center"><?=$this->bbf('col_valid');?></th>
		<th class="th-center col-action"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php
	if(($list = $this->get_var('list')) === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="8" class="td-single"><?=$this->bbf('no_user');?></td>
	</tr>
<?php
	else:
		for($i = 0;$i < $nb;$i++):

			$ref  = &$list[$i];
			$icon = $ref['valid']?'enable':'disable';

?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';"
	    onmouseout="this.className = this.tmp;"
	    class="sb-content l-infos-<?=(($i % 2) + 1)?>on2">
		<td class="td-left">
			<?=$form->checkbox(array('name'		=> 'userselection[]',
						 'value'	=> $ref['id'],
						 'label'	=> false,
						 'id'		=> 'it-user-'.$i,
						 'checked'	=> false,
						 'paragraph'	=> false));?>
		</td>
		<td class="txt-left" title="<?=dwho_alttitle($ref['login']);?>">
			<label for="it-user-<?=$i?>" id="lb-user-<?=$i?>">
<?php
				echo	$url->img_html('img/site/flag/'.$icon.'.gif',null,'class="icons-list"'),
					dwho_trunc($ref['login'],20,'...',false);
?>
			</label>
		</td>

		<td><?=dwho_htmlen($ref['passwd']);?></td>

		<td><?=$ref['meta']?></td>
		<td><?=dwho_i18n::strftime_l($this->bbf('date_format_yymmdd'),
					     null,
					     $ref['dcreate']);?></td>
		<td><?=$this->bbf('valid_'.intval((bool) $ref['valid']));?></td>
		<td class="td-right" colspan="2">
<?php
			echo	$url->href_html($url->img_html('img/site/button/edit.gif',
							       $this->bbf('opt_modify'),
							       'border="0"'),
						'xivo/configuration/manage/user',
						array('act'	=> 'edit',
						      'id'	=> $ref['id']),
						null,
						$this->bbf('opt_modify'));

			if(xivo_user::chk_authorize('admin',$ref['meta']) === true):
				echo	$url->href_html($url->img_html('img/site/button/key.gif',
								       $this->bbf('opt_acl'),
								       'border="0"'),
							'xivo/configuration/manage/user',
							array('act'	=> 'acl',
							      'id'	=> $ref['id']),
							null,
							$this->bbf('opt_acl')),


					$url->href_html($url->img_html('img/site/button/delete.gif',
				 				       $this->bbf('opt_delete'),
								       'border="0"'),
							'xivo/configuration/manage/user',
							array('act'	=> 'delete',
				      			      'id'	=> $ref['id'],
				      			      'page'	=> $pager['page']),
							'onclick="return(confirm(\''.$dhtml->escape($this->bbf('opt_delete_confirm')).'\'));"',
							$this->bbf('opt_delete'));

			endif;
?>
		</td>
	</tr>
<?php
		endfor;
	endif;
?>
	<tr class="sb-foot">
		<td class="td-left xspan b-nosize"><span class="span-left b-nosize">&nbsp;</span></td>
		<td class="td-center" colspan="6"><span class="b-nosize">&nbsp;</span></td>
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
