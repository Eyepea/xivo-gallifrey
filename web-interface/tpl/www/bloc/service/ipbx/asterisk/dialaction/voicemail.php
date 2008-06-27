<?php

$form = &$this->get_module('form');
$url = &$this->get_module('url');
$dhtml = &$this->get_module('dhtml');

$element = $this->get_var('element');
$list = $this->get_varra('destination_list','voicemail');
$event = $this->get_var('event');

$linked = $this->get_varra('dialaction',array($event,'linked'));
$action = $this->get_varra('dialaction',array($event,'action'));

if(empty($list) === false):
	echo	'<div id="fd-dialaction-'.$event.'-voicemail-actiontype" class="b-nodisplay">',
		$form->select(array('desc'	=> $this->bbf('fm_dialaction_voicemail-actionarg1'),
				    'name'	=> 'dialaction['.$event.'][actionarg1]',
				    'labelid'	=> 'dialaction-'.$event.'-voicemail-actionarg1',
				    'key'	=> 'identity',
				    'altkey'	=> 'uniqueid',
				    'invalid'	=> ($linked === false && $action === 'voicemail'),
				    'default'	=> $element['dialaction']['actionarg1']['default'],
				    'value'	=> $this->get_varra('dialaction',array($event,'voicemail','actionarg1'))),
			      $list);

	if($event === 'voicemenuflow'):
		echo	$form->button(array('name'	=> 'add-defapplication-voicemail',
					    'id'	=> 'it-add-defapplication-voicemail',
					    'value'	=> $this->bbf('fm_bt-add')),
				      'onclick="xivo_ast_defapplication_voicemail(\''.$dhtml->escape($event).'\',\'it-voicemenu-flow\');"');
	elseif($event === 'voicemenuevent'):
		echo	$form->button(array('name'	=> 'select-defapplication-voicemail',
					    'id'	=> 'it-select-defapplication-voicemail',
					    'value'	=> $this->bbf('fm_bt-select')),
				      'onclick="xivo_ast_voicemenuevent_defapplication(\'voicemail\');"');
	endif;
	echo	'</div>';
else:
	echo	'<div id="fd-dialaction-'.$event.'-voicemail-actiontype" class="txt-center b-nodisplay">',
		$url->href_html($this->bbf('create_voicemail'),'service/ipbx/pbx_settings/voicemail','act=add'),
		'</div>';
endif;

?>
