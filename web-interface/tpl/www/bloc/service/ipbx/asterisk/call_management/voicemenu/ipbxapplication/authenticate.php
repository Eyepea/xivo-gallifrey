<div id="fd-ipbxapplication-authenticate" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_authenticate = $this->get_var('apparg_authenticate');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_authenticate-password'),
			  'name'	=> 'ipbxapplication[authenticate][password]',
			  'labelid'	=> 'ipbxapplication-authenticate-password',
			  'size'	=> 15,
			  'default'	=> $apparg_authenticate['password']['default'])),

	$form->select(array('desc'	=> $this->bbf('fm_ipbxapplication_authenticate-passwordinterpreter'),
			    'name'	=> 'ipbxapplication[authenticate][passwordinterpreter]',
			    'labelid'	=> 'ipbxapplication-authenticate-passwordinterpreter',
			    'empty'	=> true,
			    'key'	=> false,
			    'bbf'	=> 'fm_ipbxapplication_authenticate-passwordinterpreter-opt-',
			    'default'	=> $apparg_authenticate['passwordinterpreter']['default']),
		      $apparg_authenticate['passwordinterpreter']['value']),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_authenticate-a'),
			      'name'	=> 'ipbxapplication[authenticate][a]',
			      'labelid'	=> 'ipbxapplication-authenticate-a',
			      'default'	=> $apparg_authenticate['a']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_authenticate-j'),
			      'name'	=> 'ipbxapplication[authenticate][j]',
			      'labelid'	=> 'ipbxapplication-authenticate-j',
			      'default'	=> $apparg_authenticate['j']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_authenticate-r'),
			      'name'	=> 'ipbxapplication[authenticate][r]',
			      'labelid'	=> 'ipbxapplication-authenticate-r',
			      'default'	=> $apparg_authenticate['r']['default'])),

	$form->button(array('name'	=> 'ipbxapplication-authenticate',
			    'id'	=> 'it-add-ipbxapplication-authenticate',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_authenticate();"');

?>
</div>
