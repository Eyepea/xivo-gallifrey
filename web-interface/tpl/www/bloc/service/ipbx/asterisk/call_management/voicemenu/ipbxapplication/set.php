<div id="fd-ipbxapplication-set" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_set = $this->get_var('apparg_set');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_set-name'),
		          'name'	=> 'ipbxapplication[set][name]',
		          'labelid'	=> 'ipbxapplication-set-name',
		          'size'	=> 15,
		          'default'	=> $apparg_set['name']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_set-value'),
		          'name'	=> 'ipbxapplication[set][value]',
		          'labelid'	=> 'ipbxapplication-set-value',
		          'size'	=> 15,
		          'default'	=> $apparg_set['value']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_set-g'),
			      'name'	=> 'ipbxapplication[set][g]',
			      'labelid'	=> 'ipbxapplication-set-g',
			      'default'	=> $apparg_set['g']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-set',
			    'id'	=> 'it-add-ipbxapplication-set',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_set();"');

?>
</div>
