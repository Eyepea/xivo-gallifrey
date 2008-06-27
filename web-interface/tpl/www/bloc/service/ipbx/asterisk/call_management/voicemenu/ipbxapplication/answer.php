<div id="fd-ipbxapplication-answer" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_answer = $this->get_var('apparg_answer');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_answer-delay'),
			  'name'	=> 'ipbxapplication[answer][delay]',
			  'labelid'	=> 'ipbxapplication-answer-delay',
			  'size'	=> 10,
			  'default'	=> $apparg_answer['delay']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-answer',
			    'id'	=> 'it-add-ipbxapplication-answer',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_answer();"');

?>
</div>
