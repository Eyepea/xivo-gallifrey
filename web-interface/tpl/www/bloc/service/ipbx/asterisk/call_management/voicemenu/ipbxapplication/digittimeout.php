<div id="fd-ipbxapplication-digittimeout" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_digittimeout = $this->get_var('apparg_digittimeout');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_digittimeout-timeout'),
		          'name'	=> 'ipbxapplication[digittimeout][timeout]',
		          'labelid'	=> 'ipbxapplication-digittimeout-timeout',
		          'size'	=> 10,
		          'default'	=> $apparg_digittimeout['timeout']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-digittimeout',
			    'id'	=> 'it-add-ipbxapplication-digittimeout',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_digittimeout();"');

?>
</div>
