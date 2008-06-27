<div id="fd-ipbxapplication-setcidname" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_setcidname = $this->get_var('apparg_setcidname');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_setcidname-name'),
		          'name'	=> 'ipbxapplication[setcidname][name]',
		          'labelid'	=> 'ipbxapplication-setcidname-name',
		          'size'	=> 15,
		          'default'	=> $apparg_setcidname['name']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-setcidname',
			    'id'	=> 'it-add-ipbxapplication-setcidname',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_setcidname();"');

?>
</div>
