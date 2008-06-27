<?php

$form = &$this->get_module('form');
$dhtml = &$this->get_module('dhtml');

$element = $this->get_var('element');
$event = $this->get_var('event');

echo	'<div id="fd-dialaction-',$event,'-application-actiontype" class="b-nodisplay">',
	$form->select(array('desc'	=> $this->bbf('fm_dialaction_application-action'),
			    'name'	=> 'dialaction['.$event.'][action]',
			    'labelid'	=> 'dialaction-'.$event.'-application-action',
			    'bbf'	=> 'fm_dialaction_application-action-opt-',
			    'key'	=> false,
			    'default'	=> $element['dialaction']['application']['default'],
			    'value'	=> $this->get_varra('dialaction',array($event,'application','action'))),
		      $element['dialaction']['application']['value'],
		      'onchange="xivo_ast_chg_dialaction_actionarg(\''.$dhtml->escape($event).'\',\'application\');"'),
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_application-callback-actionarg1'),
			  'name'	=> 'dialaction['.$event.'][actionarg1]',
			  'labelid'	=> 'dialaction-'.$event.'-application-callback-actionarg1',
			  'size'	=> 15,
			  'value'	=> $this->get_varra('dialaction',array($event,'callback','actionarg1')))),
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_application-callback-actionarg2'),
			  'name'	=> 'dialaction['.$event.'][actionarg2]',
			  'labelid'	=> 'dialaction-'.$event.'-application-callback-actionarg2',
			  'size'	=> 15,
			  'value'	=> $this->get_varra('dialaction',array($event,'callback','actionarg2')))),
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_application-callbackdisa-actionarg1'),
			  'name'	=> 'dialaction['.$event.'][actionarg1]',
			  'labelid'	=> 'dialaction-'.$event.'-application-callbackdisa-actionarg1',
			  'size'	=> 15,
			  'value'	=> $this->get_varra('dialaction',array($event,'callbackdisa','actionarg1')))),
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_application-callbackdisa-actionarg2'),
			  'name'	=> 'dialaction['.$event.'][actionarg2]',
			  'labelid'	=> 'dialaction-'.$event.'-application-callbackdisa-actionarg2',
			  'size'	=> 15,
			  'value'	=> $this->get_varra('dialaction',array($event,'callbackdisa','actionarg2')))),
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_application-directory-actionarg1'),
			  'name'	=> 'dialaction['.$event.'][actionarg1]',
			  'labelid'	=> 'dialaction-'.$event.'-application-directory-actionarg1',
			  'size'	=> 15,
			  'value'	=> $this->get_varra('dialaction',array($event,'directory','actionarg1')))),
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_application-faxtomail-actionarg1'),
			  'name'	=> 'dialaction['.$event.'][actionarg1]',
			  'labelid'	=> 'dialaction-'.$event.'-application-faxtomail-actionarg1',
			  'size'	=> 15,
			  'value'	=> $this->get_varra('dialaction',array($event,'faxtomail','actionarg1')))),
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_application-voicemailmain-actionarg1'),
			  'name'	=> 'dialaction['.$event.'][actionarg1]',
			  'labelid'	=> 'dialaction-'.$event.'-application-voicemailmain-actionarg1',
			  'size'	=> 15,
			  'value'	=> $this->get_varra('dialaction',array($event,'voicemailmain','actionarg1'))));

	if($event === 'voicemenuflow'):
		echo	$form->button(array('name'	=> 'add-defapplication-application',
					    'id'	=> 'it-add-defapplication-application',
					    'value'	=> $this->bbf('fm_bt-add')),
				      'onclick="xivo_ast_defapplication_application(\''.$dhtml->escape($event).'\',\'it-voicemenu-flow\');"');
	elseif($event === 'voicemenuevent'):
		echo	$form->button(array('name'	=> 'select-defapplication-application',
					    'id'	=> 'it-select-defapplication-application',
					    'value'	=> $this->bbf('fm_bt-select')),
				      'onclick="xivo_ast_voicemenuevent_defapplication(\'application\');"');
	endif;

echo	'</div>';
?>
