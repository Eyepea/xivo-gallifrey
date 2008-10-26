<?php

$form = &$this->get_module('form');
$dhtml = &$this->get_module('dhtml');

$element = $this->get_var('element');
$event = $this->get_var('event');

echo	'<div id="fd-dialaction-',$event,'-extension-actionarg1" class="b-nodisplay">',
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_extension-actionarg1'),
			  'name'	=> 'dialaction['.$event.'][actionarg1]',
			  'labelid'	=> 'dialaction-'.$event.'-extension-actionarg1',
			  'size'	=> 15,
			  'value'	=> $this->get_varra('dialaction',array($event,'extension','actionarg1')))),
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_extension-actionarg2'),
			  'name'	=> 'dialaction['.$event.'][actionarg2]',
			  'labelid'	=> 'dialaction-'.$event.'-extension-actionarg2',
			  'size'	=> 15,
			  'value'	=> $this->get_varra('dialaction',array($event,'extension','actionarg2'))));

	if($event === 'voicemenuflow'):
		echo	$form->button(array('name'	=> 'add-defapplication-extension',
					    'id'	=> 'it-add-defapplication-extension',
					    'value'	=> $this->bbf('fm_bt-add')),
				      'onclick="xivo_ast_defapplication_extension(\''.$dhtml->escape($event).'\',\'it-voicemenu-flow\');"');
	elseif($event === 'voicemenuevent'):
		echo	$form->button(array('name'	=> 'select-defapplication-extension',
					    'id'	=> 'it-select-defapplication-extension',
					    'value'	=> $this->bbf('fm_bt-select')),
				      'onclick="xivo_ast_voicemenuevent_defapplication(\'extension\');"');
	endif;

echo	'</div>';

?>
