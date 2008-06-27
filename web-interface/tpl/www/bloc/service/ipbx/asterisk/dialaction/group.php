<?php

$form = &$this->get_module('form');
$url = &$this->get_module('url');
$dhtml = &$this->get_module('dhtml');

$element = $this->get_var('element');
$list = $this->get_varra('destination_list','groups');
$event = $this->get_var('event');

$linked = $this->get_varra('dialaction',array($event,'linked'));
$action = $this->get_varra('dialaction',array($event,'action'));

if(empty($list) === false):
	echo	'<div id="fd-dialaction-'.$event.'-group-actiontype" class="b-nodisplay">',
		$form->select(array('desc'	=> $this->bbf('fm_dialaction_group-actionarg1'),
				    'name'	=> 'dialaction['.$event.'][actionarg1]',
				    'labelid'	=> 'dialaction-'.$event.'-group-actionarg1',
				    'key'	=> 'identity',
				    'altkey'	=> 'id',
				    'invalid'	=> ($linked === false && $action === 'group'),
				    'default'	=> $element['dialaction']['actionarg1']['default'],
				    'value' 	=> $this->get_varra('dialaction',array($event,'group','actionarg1'))),
			      $list),
		$form->text(array('desc'	=> $this->bbf('fm_dialaction_group-actionarg2'),
				  'name'	=> 'dialaction['.$event.'][actionarg2]',
				  'labelid'	=> 'dialaction-'.$event.'-group-actionarg2',
				  'size'	=> 10,
				  'value'	=> $this->get_varra('dialaction',array($event,'group','actionarg2'))));

	if($event === 'voicemenuflow'):
		echo	$form->button(array('name'	=> 'add-defapplication-group',
					    'id'	=> 'it-add-defapplication-group',
					    'value'	=> $this->bbf('fm_bt-add')),
				      'onclick="xivo_ast_defapplication_group(\''.$dhtml->escape($event).'\',\'it-voicemenu-flow\');"');
	elseif($event === 'voicemenuevent'):
		echo	$form->button(array('name'	=> 'select-defapplication-group',
					    'id'	=> 'it-select-defapplication-group',
					    'value'	=> $this->bbf('fm_bt-select')),
				      'onclick="xivo_ast_voicemenuevent_defapplication(\'group\');"');
	endif;
	echo	'</div>';
else:
	echo	'<div id="fd-dialaction-'.$event.'-group-actiontype" class="txt-center b-nodisplay">';
	if($this->get_var('dialaction_from') === 'group'):
		echo	$this->bbf('dialaction_no-group');
	else:
		echo	$url->href_html($this->bbf('create_group'),'service/ipbx/pbx_settings/groups','act=add');
	endif;
	echo	'</div>';
endif;

?>
