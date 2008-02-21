<?php
	$url = &$this->get_module('url');
	$form = &$this->get_module('form');
	$dhtml = &$this->get_module('dhtml');

	$pager = $this->get_var('pager');
	$list = $this->get_var('list');
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
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
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
		<th class="th-center" id="col-action" colspan="2"><?=$this->bbf('col_action');?></th>
		<th class="th-right xspan"><span class="span-right">&nbsp;</span></th>
	</tr>
<?php

	if($list === false || ($nb = count($list)) === 0):
?>
	<tr class="sb-content">
		<td colspan="11" class="td-single"><?=$this->bbf('no_schedule');?></td>
	</tr>
<?php
	else:
		for($i = $pager['beg'],$j = 0;$i < $pager['end'] && $i < $pager['total'];$i++,$j++):

			$ref = &$list[$i]['schedule'];

			$typetrue = $this->bbf('schedule_typetrue-'.$ref['typetrue']);

			if($ref['linked'] === false):
				$icon = 'unavailable';
				$typetrue = '-';
			elseif($ref['commented'] === true):
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
				$ref['dayname'] = $this->bbf('date_Day_'.$ref['daynamebeg']);
			else:
				$daynamebeg = $this->bbf('date_Day_'.$ref['daynamebeg']);
				$daynameend = $this->bbf('date_Day_'.$ref['daynameend']);

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
				$ref['month'] = $this->bbf('date_Month_'.$ref['monthbeg']);
			else:
				$monthbeg = $this->bbf('date_Month_'.$ref['monthbeg']);
				$monthend = $this->bbf('date_Month_'.$ref['monthend']);

				$ref['month'] = $this->bbf('schedule_rangemonth',
							   array($monthbeg,$monthend));
			endif;

			$ref['publicholiday'] = intval((bool) $ref['publicholiday']);

			$mod = $j % 2 === 0 ? 1 : 2;
?>
	<tr onmouseover="this.tmp = this.className; this.className = 'sb-content l-infos-over';" onmouseout="this.className = this.tmp;" class="sb-content l-infos-<?=$mod?>on2">
		<td class="td-left"><?=$form->checkbox(array('name' => 'schedules[]','value' => $ref['id'],'label' => false,'id' => 'it-schedules-'.$i,'checked' => false,'field' => false));?></td>
		<td class="txt-left"><label for="it-schedules-<?=$i?>" id="lb-schedules-<?=$i?>"><?=$url->img_html('img/site/flag/'.$icon.'.gif',null,'class="icons-list"');?><?=$ref['name']?></label></td>
		<td><?=$ref['time']?></td>
		<td><?=$ref['dayname']?></td>
		<td><?=$ref['daynum']?></td>
		<td><?=$ref['month']?></td>
		<td><?=$typetrue?></td>
		<td><?=$this->bbf('schedule_publicholiday-'.$ref['publicholiday']);?></td>
		<td class="td-right" colspan="3">
		<?=$url->href_html($url->img_html('img/site/button/edit.gif',$this->bbf('opt_modify'),'border="0"'),'service/ipbx/call_management/schedule',array('act' => 'edit','id' => $ref['id']),null,$this->bbf('opt_modify'));?>
		<?=$url->href_html($url->img_html('img/site/button/delete.gif',$this->bbf('opt_delete'),'border="0"'),'service/ipbx/call_management/schedule',array('act' => 'delete','id' => $ref['id'],'page' => $pager['page']),'onclick="return(confirm(\''.$dhtml->escape($this->bbf('opt_delete_confirm')).'\'));"',$this->bbf('opt_delete'));?>
		</td>
	</tr>
<?php
		endfor;
	endif;
?>
	<tr class="sb-foot">
		<td class="td-left xspan b-nosize"><span class="span-left b-nosize">&nbsp;</span></td>
		<td class="td-center" colspan="9"><span class="b-nosize">&nbsp;</span></td>
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
