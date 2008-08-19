<?php

$form = &$this->get_module('form');
$url = &$this->get_module('url');
$dhtml = &$this->get_module('dhtml');

$element = $this->get_var('element');
$list = $this->get_varra('destination_list','schedule');
$event = $this->get_var('event');

$linked = $this->get_varra('dialaction',array($event,'linked'));
$action = $this->get_varra('dialaction',array($event,'action'));

if(empty($list) === false):
	echo	'<div id="fd-dialaction-'.$event.'-schedule-actiontype" class="b-nodisplay">',
		$form->select(array('desc'	=> $this->bbf('fm_dialaction_schedule-actionarg1'),
				    'name'	=> 'dialaction['.$event.'][actionarg1]',
				    'labelid'	=> 'dialaction-'.$event.'-schedule-actionarg1',
				    'key'	=> 'identity',
				    'altkey'	=> 'id',
				    'invalid'	=> ($linked === false && $action === 'schedule'),
				    'default'	=> $element['dialaction']['actionarg1']['default'],
				    'value'	=> $this->get_varra('dialaction',array($event,'schedule','actionarg1'))),
			      $list);

	if($event === 'voicemenuflow'):
		echo	$form->button(array('name'	=> 'add-defapplication-schedule',
					    'id'	=> 'it-add-defapplication-schedule',
					    'value'	=> $this->bbf('fm_bt-add')),
				      'onclick="xivo_ast_defapplication_schedule(\''.$dhtml->escape($event).'\',\'it-voicemenu-flow\');"');
	elseif($event === 'voicemenuevent'):
		echo	$form->button(array('name'	=> 'select-defapplication-schedule',
					    'id'	=> 'it-select-defapplication-schedule',
					    'value'	=> $this->bbf('fm_bt-select')),
				      'onclick="xivo_ast_voicemenuevent_defapplication(\'schedule\');"');
	endif;
	echo	'</div>';
else:
	echo	'<div id="fd-dialaction-'.$event.'-schedule-actiontype" class="txt-center b-nodisplay">';
	if($this->get_var('dialaction_from') === 'schedule'):
		echo	$this->bbf('dialaction_no-schedule');
	else:
		echo	$url->href_html($this->bbf('create_schedule'),'service/ipbx/call_management/schedule','act=add');
	endif;
	echo	'</div>';
endif;

?>
