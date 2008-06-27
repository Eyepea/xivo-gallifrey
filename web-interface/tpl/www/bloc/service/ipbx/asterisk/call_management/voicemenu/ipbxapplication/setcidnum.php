<div id="fd-ipbxapplication-setcidnum" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_setcidnum = $this->get_var('apparg_setcidnum');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_setcidnum-number'),
		          'name'	=> 'ipbxapplication[setcidnum][number]',
		          'labelid'	=> 'ipbxapplication-setcidnum-number',
		          'size'	=> 10,
		          'default'	=> $apparg_setcidnum['number']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-setcidnum',
			    'id'	=> 'it-add-ipbxapplication-setcidnum',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_setcidnum();"');

?>
</div>
