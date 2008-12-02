<?php

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$info = $this->get_var('info');
$element = $this->get_var('element');
$list = $this->get_var('list');
$context_list = $this->get_var('context_list');

echo	$form->text(array('desc'	=> $this->bbf('fm_schedule_name'),
			  'name'	=> 'schedule[name]',
			  'labelid'	=> 'schedule-name',
			  'size'	=> 15,
			  'default'	=> $element['schedule']['name']['default'],
			  'value'	=> $info['schedule']['name']));

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_schedule_context'),
				    'name'	=> 'schedule[context]',
				    'labelid'	=> 'schedule-context',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['schedule']['context']['default'],
				    'value'	=> $info['schedule']['context']),
				    $context_list);
else:
	echo	'<div id="fd-schedule-context" class="txt-center">',
		$url->href_html($this->bbf('create_context'),
				'service/ipbx/system_management/context',
				'act=add'),
		'</div>';
endif;

echo	$form->checkbox(array('desc'	=> $this->bbf('fm_schedule_publicholiday'),
			      'name'	=> 'schedule[publicholiday]',
			      'labelid'	=> 'publicholiday',
			      'checked'	=> $info['schedule']['publicholiday'],
			      'default'	=> $element['schedule']['publicholiday']['default']));
?>
<div class="sb-list">
<table cellspacing="0" cellpadding="0" border="0" class="fm-field">
	<thead>
	<tr class="sb-top">
		<th class="th-left"><?=$this->bbf('col_schedule-time');?></th>
		<th class="th-center"><?=$this->bbf('col_schedule-begin');?></th>
		<th class="th-right"><?=$this->bbf('col_schedule-end');?></th>
	</tr>
	</thead>
	<tbody>
	<tr>
		<td class="txt-left"><?=$this->bbf('schedule_hour');?></td>
		<td>
<?php
			echo	$form->select(array('field'	=> false,
						    'name'	=> 'schedule[timebeg][hour]',
						    'labelid'	=> 'schedule-timehourbeg',
						    'empty'	=> true,
						    'key'	=> false,
						    'default'	=> $element['schedule']['timehourbeg']['default'],
						    'value'	=> $info['schedule']['timehourbeg']),
						    $element['schedule']['timehourbeg']['value']),
				$form->select(array('field'	=> false,
						    'name'	=> 'schedule[timebeg][min]',
						    'labelid'	=> 'schedule-timeminbeg',
						    'empty'	=> true,
						    'key'	=> false,
						    'default'	=> $element['schedule']['timeminbeg']['default'],
						    'value'	=> $info['schedule']['timeminbeg']),
						    $element['schedule']['timeminbeg']['value']);
?>
		</td>
		<td class="td-right">
<?php
			echo	$form->select(array('field'	=> false,
						    'name'	=> 'schedule[timeend][hour]',
						    'labelid'	=> 'schedule-timehourend',
						    'empty'	=> true,
						    'key'	=> false,
						    'default'	=> $element['schedule']['timehourend']['default'],
						    'value'	=> $info['schedule']['timehourend']),
						    $element['schedule']['timehourend']['value']),
				$form->select(array('field'	=> false,
						    'name'	=> 'schedule[timeend][min]',
						    'labelid'	=> 'schedule-timeminend',
						    'empty'	=> true,
						    'key'	=> false,
						    'default'	=> $element['schedule']['timeminend']['default'],
						    'value'	=> $info['schedule']['timeminend']),
						    $element['schedule']['timeminend']['value']);
?>
		</td>
	</tr>
	<tr>
		<td class="txt-left"><?=$this->bbf('schedule_dayname');?></td>
		<td>
			<?=$form->select(array('field'		=> false,
					       'name'		=> 'schedule[daynamebeg]',
					       'labelid'	=> 'schedule-daynamebeg',
					       'empty'		=> true,
					       'key'		=> false,
					       'bbf'		=> 'date_Day_',
					       'default'	=> $element['schedule']['daynamebeg']['default'],
					       'value'		=> $info['schedule']['daynamebeg']),
					       $element['schedule']['daynamebeg']['value']);?>
		</td>
		<td class="td-right">
			<?=$form->select(array('field'		=> false,
					       'name'		=> 'schedule[daynameend]',
					       'labelid'	=> 'schedule-daynameend',
					       'empty'		=> true,
					       'key'		=> false,
					       'bbf'		=> 'date_Day_',
					       'default'	=> $element['schedule']['daynameend']['default'],
					       'value'		=> $info['schedule']['daynameend']),
					       $element['schedule']['daynameend']['value']);?>
		</td>
	</tr>
	<tr>
		<td class="txt-left"><?=$this->bbf('schedule_daynum');?></td>
		<td>
			<?=$form->select(array('field'		=> false,
					       'name'		=> 'schedule[daynumbeg]',
					       'labelid'	=> 'schedule-daynumbeg',
					       'empty'		=> true,
					       'key'		=> false,
					       'default'	=> $element['schedule']['daynumbeg']['default'],
					       'value'		=> $info['schedule']['daynumbeg']),
					       $element['schedule']['daynumbeg']['value']);?>
		</td>
		<td class="td-right">
			<?=$form->select(array('field'		=> false,
					       'name'		=> 'schedule[daynumend]',
					       'labelid'	=> 'schedule-daynumend',
					       'empty'		=> true,
					       'key'		=> false,
					       'default'	=> $element['schedule']['daynumend']['default'],
					       'value'		=> $info['schedule']['daynumend']),
					       $element['schedule']['daynumend']['value']);?>
		</td>
	</tr>
	<tr>
		<td class="txt-left"><?=$this->bbf('schedule_month');?></td>
		<td>
			<?=$form->select(array('field'		=> false,
					       'name'		=> 'schedule[monthbeg]',
					       'labelid'	=> 'schedule-monthbeg',
					       'empty'		=> true,
					       'key'		=> false,
					       'bbf'		=> 'date_Month_',
					       'default'	=> $element['schedule']['monthbeg']['default'],
					       'value'		=> $info['schedule']['monthbeg']),
					       $element['schedule']['monthbeg']['value']);?>
		</td>
		<td class="td-right">
			<?=$form->select(array('field'		=> false,
					       'name'		=> 'schedule[monthend]',
					       'labelid'	=> 'schedule-monthend',
					       'empty'		=> true,
					       'key'		=> false,
					       'bbf'		=> 'date_Month_',
					       'default'	=> $element['schedule']['monthend']['default'],
					       'value'		=> $info['schedule']['monthend']),
					       $element['schedule']['monthend']['value']);?>
		</td>
	</tr>
	</tbody>
</table>
</div>

<fieldset id="fld-dialaction-inschedule">
	<legend><?=$this->bbf('fld-dialaction-inschedule');?></legend>
<?php
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/all',
			    array('event'	=> 'inschedule'));
?>
</fieldset>

<fieldset id="fld-dialaction-outchedule">
	<legend><?=$this->bbf('fld-dialaction-outschedule');?></legend>
<?php
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/all',
			    array('event'	=> 'outschedule'));
?>
</fieldset>
