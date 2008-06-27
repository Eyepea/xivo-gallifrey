<?php

$form = &$this->get_module('form');
$url = &$this->get_module('url');
$dhtml = &$this->get_module('dhtml');

$element = $this->get_var('element');
$list = $this->get_varra('destination_list','queues');
$event = $this->get_var('event');

$linked = $this->get_varra('dialaction',array($event,'linked'));
$action = $this->get_varra('dialaction',array($event,'action'));

if(empty($list) === false):
	echo	'<div id="fd-dialaction-'.$event.'-queue-actiontype" class="b-nodisplay">',
		$form->select(array('desc'	=> $this->bbf('fm_dialaction_queue-actionarg1'),
				    'name'	=> 'dialaction['.$event.'][actionarg1]',
				    'labelid'	=> 'dialaction-'.$event.'-queue-actionarg1',
				    'key'	=> 'identity',
				    'altkey'	=> 'id',
				    'invalid'	=> ($linked === false && $action === 'queue'),
				    'default'	=> $element['dialaction']['actionarg1']['default'],
				    'value'	=> $this->get_varra('dialaction',array($event,'queue','actionarg1'))),
			      $list),
		$form->text(array('desc'	=> $this->bbf('fm_dialaction_queue-actionarg2'),
				  'name'	=> 'dialaction['.$event.'][actionarg2]',
				  'labelid'	=> 'dialaction-'.$event.'-queue-actionarg2',
				  'size'	=> 10,
				  'value'	=> $this->get_varra('dialaction',array($event,'queue','actionarg2'))));

	if($event === 'voicemenuflow'):
		echo	$form->button(array('name'	=> 'add-defapplication-queue',
					    'id'	=> 'it-add-defapplication-queue',
					    'value'	=> $this->bbf('fm_bt-add')),
				      'onclick="xivo_ast_defapplication_queue(\''.$dhtml->escape($event).'\',\'it-voicemenu-flow\');"');
	elseif($event === 'voicemenuevent'):
		echo	$form->button(array('name'	=> 'select-defapplication-queue',
					    'id'	=> 'it-select-defapplication-queue',
					    'value'	=> $this->bbf('fm_bt-select')),
				      'onclick="xivo_ast_voicemenuevent_defapplication(\'queue\');"');
	endif;
	echo	'</div>';
else:
	echo	'<div id="fd-dialaction-'.$event.'-queue-actiontype" class="txt-center b-nodisplay">';
	if($this->get_var('dialaction_from') === 'queue'):
		echo	$this->bbf('dialaction_no-queue');
	else:
		echo	$url->href_html($this->bbf('create_queue'),'service/ipbx/pbx_settings/queues','act=add');
	endif;
	echo	'</div>';
endif;

?>
