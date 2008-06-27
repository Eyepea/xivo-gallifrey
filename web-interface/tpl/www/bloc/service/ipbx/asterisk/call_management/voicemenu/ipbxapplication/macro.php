<div id="fd-ipbxapplication-macro" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_macro = $this->get_var('apparg_macro');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_macro-macroname'),
			  'name'	=> 'ipbxapplication[macro][macroname]',
			  'labelid'	=> 'ipbxapplication-macro-macroname',
			  'size'	=> 15,
			  'default'	=> $apparg_macro['macroname']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_macro-args'),
			  'name'	=> 'ipbxapplication[macro][args]',
			  'labelid'	=> 'ipbxapplication-macro-args',
			  'size'	=> 15,
			  'default'	=> $apparg_macro['args']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-macro',
			    'id'	=> 'it-add-ipbxapplication-macro',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_macro();"');

?>
</div>
