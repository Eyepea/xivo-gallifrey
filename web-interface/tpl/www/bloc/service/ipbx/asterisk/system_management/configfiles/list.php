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

	$pager = $this->get_var('pager');
	$act = $this->get_var('act');

	$page = $url->pager($pager['pages'],
			    $pager['page'],
			    $pager['prev'],
			    $pager['next'],
			    'service/ipbx/system_management/configfiles',
			    array('act' => $act));
?>
<div class="b-list">
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_file');?></th>
		<th class="th-center col-action"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php
	if(($list = $this->get_var('list')) === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="4" class="td-single"><?=$this->bbf('no_file');?></td>
	</tr>
<?php
	else:
		for($i = $pager['beg'],$j = 0;$i < $pager['end'] && $i < $pager['total'];$i++,$j++):

			$name = &$list[$i];
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';"
	    onmouseout="this.className = this.tmp;"
	    class="sb-content l-infos-<?=(($j % 2) + 1)?>on2">
		<td class="td-left txt-left curpointer"
		    colspan="2"
		    onclick="location.href = dwho_eid('ah-files-<?=$i?>').href;"><?=$name?></td>
		<td class="td-right" colspan="2">
			<?=$url->href_html($url->img_html('img/site/button/edit.gif',
							  $this->bbf('opt_modify'),
							  'border="0"'),
					   'service/ipbx/system_management/configfiles',
					   array('act'	=> 'edit',
						 'id'	=> $name),
					   'id="ah-files-'.$i.'"',
					   $this->bbf('opt_modify'));?>
		</td>
	</tr>
<?php
		endfor;
	endif;
?>
	<tr class="sb-foot">
		<td class="td-left xspan b-nosize"><span class="span-left b-nosize">&nbsp;</span></td>
		<td class="td-center" colspan="2"><span class="b-nosize">&nbsp;</span></td>
		<td class="td-right xspan b-nosize"><span class="span-right b-nosize">&nbsp;</span></td>
	</tr>
</table>
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
</div>
