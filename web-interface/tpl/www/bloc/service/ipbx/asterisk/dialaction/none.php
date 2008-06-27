<?php

$form = &$this->get_module('form');
$event = $this->get_var('event');

if($event === 'voicemenuevent'):
	echo	'<div id="fd-dialaction-',$event,'-none-actiontype" class="b-nodisplay">',
		$form->button(array('name'	=> 'select-defapplication-none',
				    'id'	=> 'it-select-defapplication-none',
				    'value'	=> $this->bbf('fm_bt-select')),
			      'onclick="xivo_ast_voicemenuevent_defapplication(\'none\');"'),
		'</div>';
endif;

?>
