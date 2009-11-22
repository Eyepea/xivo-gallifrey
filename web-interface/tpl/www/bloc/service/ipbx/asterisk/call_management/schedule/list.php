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
		    'service/ipbx/call_management/schedule',
		    array('act' => $act));

?>
<div class="b-list">
<?php
	if($page !== ''):
		echo '<div class="b-page">',$page,'</div>';
	endif;
?>
<form action="#" name="fm-schedule-list" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => DWHO_SESS_NAME,'value' => DWHO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => $act));?>
<?=$form->hidden(array('name' => 'page','value' => $pager['page']));?>
<table cellspacing="0" cellpadding="0" border="0">
	<tr class="sb-top">
		<th class="th-left xspan"><span class="span-left">&nbsp;</span></th>
		<th class="th-center"><?=$this->bbf('col_name');?></th>
		<th class="th-center"><?=$this->bbf('col_time');?></th>
		<th class="th-center"><?=$this->bbf('col_dayname');?></th>
		<th class="th-center"><?=$this->bbf('col_daynum');?></th>
		<th class="th-center"><?=$this->bbf('col_month');?></th>
		<th class="th-center"><?=$this->bbf('col_destination');?></th>
		<th class="th-center"><?=$this->bbf('col_publicholiday');?></th>
		<th class="th-center col-action"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php
	if(($list = $this->get_var('list')) === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="10" class="td-single"><?=$this->bbf('no_schedule');?></td>
	</tr>
<?php
	else:
		for($i = 0;$i < $nb;$i++):

			$ref = &$list[$i];

			if($ref['commented'] === true):
				$icon = 'disable';
			else:
				$icon = 'enable';
			endif;

			if($ref['timebeg'] === '*'):
				$ref['time'] = '-';
			elseif((string) $ref['timeend'] === ''):
				$ref['time'] = $this->bbf('schedule_time',
							  array($ref['timehourbeg'],$ref['timeminbeg']));
			else:
				$timebeg = $this->bbf('schedule_time',
						      array($ref['timehourbeg'],$ref['timeminbeg']));

				$timeend = $this->bbf('schedule_time',
						      array($ref['timehourend'],$ref['timeminend']));

				$ref['time'] = $this->bbf('schedule_rangetime',array($timebeg,$timeend));
			endif;

			if($ref['daynamebeg'] === '*'):
				$ref['dayname'] = '-';
			elseif((string) $ref['daynameend'] === ''):
				$ref['dayname'] = $this->bbf('date_Day',$ref['daynamebeg']);
			else:
				$daynamebeg = $this->bbf('date_Day',$ref['daynamebeg']);
				$daynameend = $this->bbf('date_Day',$ref['daynameend']);

				$ref['dayname'] = $this->bbf('schedule_rangedayname',
							     array($daynamebeg,$daynameend));
			endif;

			if($ref['daynumbeg'] === '*'):
				$ref['daynum'] = '-';
			elseif((string) $ref['daynumend'] === ''):
				$ref['daynum'] = $ref['daynumbeg'];
			else:
				$ref['daynum'] = $this->bbf('schedule_rangedaynum',
							    array($ref['daynumbeg'],$ref['daynumend']));
			endif;

			if($ref['monthbeg'] === '*'):
				$ref['month'] = '-';
			elseif((string) $ref['monthend'] === ''):
				$ref['month'] = $this->bbf('date_Month',$ref['monthbeg']);
			else:
				$monthbeg = $this->bbf('date_Month',$ref['monthbeg']);
				$monthend = $this->bbf('date_Month',$ref['monthend']);

				$ref['month'] = $this->bbf('schedule_rangemonth',
							   array($monthbeg,$monthend));
			endif;

			$ref['publicholiday'] = intval((bool) $ref['publicholiday']);

			$destination = $this->bbf('schedule_destination-'.$ref['destination']);

			if($ref['linked'] === false):
				$icon = 'unavailable';
				$destination = '-';
			elseif($ref['destination'] === 'endcall'):
				$destination = $this->bbf('schedule_destination-endcall-'.$ref['destidentity']);
			elseif($ref['destination'] === 'application'):
				$destination = $this->bbf('schedule_destination-application-'.$ref['destidentity']);
			else:
				$destination = $ref['destidentity'];
			endif;
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';"
	    onmouseout="this.className = this.tmp;"
	    class="sb-content l-infos-<?=(($i % 2) + 1)?>on2">
		<td class="td-left">
			<?=$form->checkbox(array('name'		=> 'schedules[]',
						 'value'	=> $ref['id'],
						 'label'	=> false,
						 'id'		=> 'it-schedules-'.$i,
						 'checked'	=> false,
						 'paragraph'	=> false));?>
		</td>
		<td class="txt-left" title="<?=dwho_alttitle($ref['name']);?>">
			<label for="it-schedules-<?=$i?>" id="lb-schedules-<?=$i?>">
<?php
				echo	$url->img_html('img/site/flag/'.$icon.'.gif',null,'class="icons-list"'),
					dwho_trunc($ref['name'],15,'...',false);
?>
			</label>
		</td>
		<td><?=$ref['time']?></td>
		<td><?=$ref['dayname']?></td>
		<td><?=$ref['daynum']?></td>
		<td><?=$ref['month']?></td>
		<td title="<?=dwho_alttitle($destination);?>">
			<?=dwho_htmlen(dwho_trunc($destination,15,'...',false));?>
		</td>
		<td><?=$this->bbf('schedule_publicholiday-'.$ref['publicholiday']);?></td>
		<td class="td-right" colspan="2">
<?php
			echo	$url->href_html($url->img_html('img/site/button/edit.gif',
							       $this->bbf('opt_modify'),
							       'border="0"'),
						'service/ipbx/call_management/schedule',
						array('act'	=> 'edit',
						      'id'	=> $ref['id']),
						null,
						$this->bbf('opt_modify')),"\n",
				$url->href_html($url->img_html('img/site/button/delete.gif',
							       $this->bbf('opt_delete'),
							       'border="0"'),
						'service/ipbx/call_management/schedule',
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
		<td class="td-center" colspan="8"><span class="b-nosize">&nbsp;</span></td>
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
