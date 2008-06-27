<?php

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$element = $this->get_var('element');
$list = $this->get_varra('destination_list','sounds');
$event = $this->get_var('event');

$action = $this->get_varra('dialaction',array($event,'action'));

if(empty($list) === false):
	echo	'<div id="fd-dialaction-'.$event.'-sound-actiontype" class="b-nodisplay">',
		$form->select(array('desc'	=> $this->bbf('fm_dialaction_sound-actionarg1'),
				    'name'	=> 'dialaction['.$event.'][actionarg1]',
				    'labelid'	=> 'dialaction-'.$event.'-sound-actionarg1',
				    'invalid'	=> ($this->get_var('act') === 'edit' && $action === 'sound'),
				    'key'	=> 'name',
				    'altkey'	=> 'pathnoext',
				    'default'	=> $element['dialaction']['actionarg1']['default'],
				    'value'	=> $this->get_varra('dialaction',array($event,'sound','actionarg1'))),
			      $list),

		$form->checkbox(array('desc'	=> $this->bbf('fm_dialaction_sound-actionarg2-skip'),
				      'name'	=> 'dialaction['.$event.'][actionarg2][skip]',
				      'labelid'	=> 'dialaction-'.$event.'-sound-actionarg2-skip',
				      'checked'	=> $this->get_varra('dialaction',array($event,'sound','actionarg2','skip')),
				      'value'	=> 'skip')),

		$form->checkbox(array('desc'	=> $this->bbf('fm_dialaction_sound-actionarg2-noanswer'),
				      'name'	=> 'dialaction['.$event.'][actionarg2][noanswer]',
				      'labelid'	=> 'dialaction-'.$event.'-sound-actionarg2-noanswer',
				      'checked'	=> $this->get_varra('dialaction',array($event,'sound','actionarg2','noanswer')),
				      'value'	=> 'noanswer')),

		$form->checkbox(array('desc'	=> $this->bbf('fm_dialaction_sound-actionarg2-j'),
				      'name'	=> 'dialaction['.$event.'][actionarg2][j]',
				      'labelid'	=> 'dialaction-'.$event.'-sound-actionarg2-j',
				      'checked'	=> $this->get_varra('dialaction',array($event,'sound','actionarg2','j')),
				      'value'	=> 'j'));

	if($event === 'voicemenuevent'):
		echo	$form->button(array('name'	=> 'select-defapplication-sound',
					    'id'	=> 'it-select-defapplication-sound',
					    'value'	=> $this->bbf('fm_bt-select')),
				      'onclick="xivo_ast_voicemenuevent_defapplication(\'sound\');"');
	endif;
	echo	'</div>';
else:
	echo	'<div id="fd-dialaction-'.$event.'-sound-actiontype" class="txt-center b-nodisplay">',
		$url->href_html($this->bbf('add_playback-sound'),
				'service/ipbx/pbx_services/sounds',
				array('act' => 'list','dir' => 'playback')),
		'</div>';
endif;

?>
