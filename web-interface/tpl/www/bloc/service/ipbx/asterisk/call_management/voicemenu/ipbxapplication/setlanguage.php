<div id="fd-ipbxapplication-setlanguage" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_setlanguage = $this->get_var('apparg_setlanguage');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_setlanguage-language'),
		          'name'	=> 'ipbxapplication[setlanguage][language]',
		          'labelid'	=> 'ipbxapplication-setlanguage-language',
		          'size'	=> 10,
		          'default'	=> $apparg_setlanguage['language']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-setlanguage',
			    'id'	=> 'it-add-ipbxapplication-setlanguage',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_setlanguage();"');

?>
</div>
