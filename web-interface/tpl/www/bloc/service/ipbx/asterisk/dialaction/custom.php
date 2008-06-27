<?php

$form = &$this->get_module('form');
$event = $this->get_var('event');

echo	'<div id="fd-dialaction-',$event,'-custom-actiontype" class="b-nodisplay">',
	$form->text(array('desc'	=> $this->bbf('fm_dialaction_custom-actionarg1'),
		          'name'	=> 'dialaction['.$event.'][actionarg1]',
		          'labelid'	=> 'dialaction-'.$event.'-custom-actionarg1',
		          'size'	=> 20,
		          'value'	=> $this->get_varra('dialaction',array($event,'custom','actionarg1'))));

	if($event === 'voicemenuflow'):
		echo	$form->button(array('name'	=> 'add-defapplication-custom',
					    'id'	=> 'it-add-defapplication-custom',
					    'value'	=> $this->bbf('fm_bt-add')),
				      'onclick="xivo_ast_defapplication_custom();"');
	endif;
	echo	'</div>';

?>
