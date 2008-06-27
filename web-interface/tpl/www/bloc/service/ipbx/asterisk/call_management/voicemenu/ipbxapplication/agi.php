<div id="fd-ipbxapplication-agi" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_agi = $this->get_var('apparg_agi');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_agi-command'),
		          'name'	=> 'ipbxapplication[agi][command]',
		          'labelid'	=> 'ipbxapplication-agi-command',
		          'size'	=> 15,
		          'default'	=> $apparg_agi['command']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_agi-args'),
		          'name'	=> 'ipbxapplication[agi][args]',
		          'labelid'	=> 'ipbxapplication-agi-args',
		          'size'	=> 15,
		          'default'	=> $apparg_agi['args']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-agi',
			    'id'	=> 'it-add-ipbxapplication-agi',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_agi();"');

?>
</div>
