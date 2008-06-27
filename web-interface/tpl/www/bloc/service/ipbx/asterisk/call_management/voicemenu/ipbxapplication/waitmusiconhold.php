<div id="fd-ipbxapplication-waitmusiconhold" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_waitmusiconhold = $this->get_var('apparg_waitmusiconhold');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_waitmusiconhold-delay'),
		          'name'	=> 'ipbxapplication[waitmusiconhold][delay]',
		          'labelid'	=> 'ipbxapplication-waitmusiconhold-delay',
		          'size'	=> 10,
		          'default'	=> $apparg_waitmusiconhold['delay']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-waitmusiconhold',
			    'id'	=> 'it-add-ipbxapplication-waitmusiconhold',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_waitmusiconhold();"');

?>
</div>
