<div id="fd-ipbxapplication-goto" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_goto = $this->get_var('apparg_goto');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_goto-context'),
		          'name'	=> 'ipbxapplication[goto][context]',
		          'labelid'	=> 'ipbxapplication-goto-context',
		          'size'	=> 15,
		          'default'	=> $apparg_goto['context']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_goto-exten'),
		          'name'	=> 'ipbxapplication[goto][exten]',
		          'labelid'	=> 'ipbxapplication-goto-exten',
		          'size'	=> 15,
		          'default'	=> $apparg_goto['exten']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_goto-priority'),
		          'name'	=> 'ipbxapplication[goto][priority]',
		          'labelid'	=> 'ipbxapplication-goto-priority',
		          'size'	=> 15,
		          'default'	=> $apparg_goto['priority']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-goto',
			    'id'	=> 'it-add-ipbxapplication-goto',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_goto();"');

?>
</div>
