<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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
$url  = &$this->get_module('url');

$info = $this->get_var('info');
$data = array_key_exists('pf.ha', $info)?$info['pf.ha']:null;
$netifaces 	= $this->get_var('netifaces');

?>

<div class="sb-list">

<?php
	$type       = 'disp-virtnet';
	$count      = $data != null?count($data):0;
	$errdisplay = '';
?>
	<p>&nbsp;</p>
    <?= $this->bbf("fm_ha_virtual_network") ?>
	<div class="sb-list">
		<table cellspacing="0" cellpadding="0" border="0">
			<thead>
			<tr class="sb-top">

				<th class="th-left"><?=$this->bbf('fm_ha_virtnet_col_ipaddr');?></th>
				<th class="th-center"><?=$this->bbf('fm_ha_virtnet_col_netmask');?></th>
				<th class="th-center"><?=$this->bbf('fm_ha_virtnet_col_bcast');?></th>
				<th class="th-right th-rule">
					<?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
									  $this->bbf('col_add'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\'disp-virtnet\',this); return(dwho.dom.free_focus());"',
							   $this->bbf('col_add'));?>
				</th>
			</tr>
			</thead>
			<tbody id="disp-virtnet">
		<?php
		if($count > 0):
			for($i = 0;$i < $count;$i++):

		?>
			<tr class="fm-paragraph<?=$errdisplay?>">
				<td class="td-left">
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'	=> 'pf.ha.ipaddr[]',
								   'id'		=> false,
								   'label'	=> false,
								   'size'	=> 15,
								   'key'	=> false,
								   'default'	=> '',
								   'value'	=> $data[$i]['ipaddr'],
								   'error'      => $this->bbf_args('ipaddr',
								       $this->get_var('error', "virtnet[$i]", 'ipaddr'))));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'	=> 'pf.ha.netmask[]',
								   'id'		=> false,
								   'label'	=> false,
								   'size'	=> 15,
								   'key'	=> false,
								   'default'	=> '',
								   'value'	=> $data[$i]['netmask'],
								   'error'      => $this->bbf_args	('netmask', $this->get_var('error', "virtnet[$i]", 'netmask'))));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'	=> 'pf.ha.broadcast[]',
								   'id'		=> false,
								   'label'	=> false,
								   'size'	=> 15,
								   'key'	=> false,
								   'default'	=> '',
								   'value'	=> $data[$i]['broadcast'],
								   'error'      => $this->bbf_args	('broadcast', $this->get_var('error', "virtnet[$i]", 'broadcast'))));
	 ?>
				</td>
                <td class="td-right">
					<?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
									  $this->bbf('opt_'.$type.'-delete'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this,1); return(dwho.dom.free_focus());"',
							   $this->bbf('opt_'.$type.'-delete'));?>
				</td>
			</tr>

		<?php
			endfor;
		endif;
		?>
			</tbody>
			<tfoot>
			<tr id="no-<?=$type?>"<?=($count > 0 ? ' class="b-nodisplay"' : '')?>>
				<td colspan="5" class="td-single"><?=$this->bbf('no_'.$type);?></td>
			</tr>
			</tfoot>
		</table>
		<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
			<tbody id="ex-<?=$type?>">
			<tr class="fm-paragraph">
				<td class="td-left">
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'	=> 'pf.ha.ipaddr[]',
								   'id'		=> false,
								   'label'	=> false,
								   'size'	=> 15,
								   'key'	=> false,
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'	=> 'pf.ha.netmask[]',
								   'id'		=> false,
								   'label'	=> false,
								   'size'	=> 15,
								   'key'	=> false,
								   'default'	=> ''));
	 ?>
				</td>
				<td>
	<?php
					echo $form->text(array('paragraph'	=> false,
								   'name'	=> 'pf.ha.broadcast[]',
								   'id'		=> false,
								   'label'	=> false,
								   'size'	=> 15,
								   'key'	=> false,
								   'default'	=> ''));
	 ?>
				</td>

				<td class="td-right">
					<?=$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
									  $this->bbf('opt_'.$type.'-delete'),
									  'border="0"'),
							   '#',
							   null,
							   'onclick="dwho.dom.make_table_list(\''.$type.'\',this,1); return(dwho.dom.free_focus());"',
							   $this->bbf('opt_'.$type.'-delete'));?>
				</td>
			</tr>
			</tbody>
		</table>
	</div>
</div>
