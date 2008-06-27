<?php

$form = &$this->get_module('form');
$url = &$this->get_module('url');
$dhtml = &$this->get_module('dhtml');

$element = $this->get_var('element');
$list = $this->get_varra('destination_list','meetme');
$event = $this->get_var('event');

$linked = $this->get_varra('dialaction',array($event,'linked'));
$action = $this->get_varra('dialaction',array($event,'action'));

if(empty($list) === false):
	echo	'<div id="fd-dialaction-'.$event.'-meetme-actiontype" class="b-nodisplay">',
		$form->select(array('desc'	=> $this->bbf('fm_dialaction_meetme-actionarg1'),
				    'name'	=> 'dialaction['.$event.'][actionarg1]',
				    'labelid'	=> 'dialaction-'.$event.'-meetme-actionarg1',
				    'key'	=> 'identity',
				    'altkey'	=> 'id',
				    'invalid'	=> ($linked === false && $action === 'meetme'),
				    'default'	=> $element['dialaction']['actionarg1']['default'],
				    'value'	=> $this->get_varra('dialaction',array($event,'meetme','actionarg1'))),
			      $list);

	if($event === 'voicemenuflow'):
		echo	$form->button(array('name'	=> 'add-defapplication-meetme',
					    'id'	=> 'it-add-defapplication-meetme',
					    'value'	=> $this->bbf('fm_bt-add')),
				      'onclick="xivo_ast_defapplication_meetme(\''.$dhtml->escape($event).'\',\'it-voicemenu-flow\');"');
	elseif($event === 'voicemenuevent'):
		echo	$form->button(array('name'	=> 'select-defapplication-meetme',
					    'id'	=> 'it-select-defapplication-meetme',
					    'value'	=> $this->bbf('fm_bt-select')),
				      'onclick="xivo_ast_voicemenuevent_defapplication(\'meetme\');"');
	endif;
	echo	'</div>';
else:
	echo	'<div id="fd-dialaction-'.$event.'-meetme-actiontype" class="txt-center b-nodisplay">',
		$url->href_html($this->bbf('create_meetme'),'service/ipbx/pbx_settings/meetme','act=add'),
		'</div>';
endif;

?>
