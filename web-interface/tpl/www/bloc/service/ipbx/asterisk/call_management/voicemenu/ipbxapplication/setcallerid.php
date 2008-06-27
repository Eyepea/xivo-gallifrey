<div id="fd-ipbxapplication-setcallerid" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_setcallerid = $this->get_var('apparg_setcallerid');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_setcallerid-callerid'),
		          'name'	=> 'ipbxapplication[setcallerid][callerid]',
		          'labelid'	=> 'ipbxapplication-setcallerid-callerid',
		          'size'	=> 15,
		          'default'	=> $apparg_setcallerid['callerid']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-setcallerid',
			    'id'	=> 'it-add-ipbxapplication-setcallerid',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_setcallerid();"');

?>
</div>
