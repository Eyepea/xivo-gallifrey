<div id="fd-ipbxapplication-gotoif" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_gotoif = $this->get_var('apparg_gotoif');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_gotoif-condition'),
		          'name'	=> 'ipbxapplication[gotoif][condition]',
		          'labelid'	=> 'ipbxapplication-gotoif-condition',
		          'size'	=> 15,
		          'default'	=> $apparg_gotoif['condition']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_gotoif-iftrue'),
		          'name'	=> 'ipbxapplication[gotoif][iftrue]',
		          'labelid'	=> 'ipbxapplication-gotoif-iftrue',
		          'size'	=> 15,
		          'default'	=> $apparg_gotoif['iftrue']['default'])),

	$form->text(array('desc'	=> $this->bbf('fmipbxapplication_gotoif-iffalse'),
		          'name'	=> 'ipbxapplication[gotoif][iffalse]',
		          'labelid'	=> 'ipbxapplication-gotoif-iffalse',
		          'size'	=> 15,
		          'default'	=> $apparg_gotoif['iffalse']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-gotoif',
			    'id'	=> 'it-add-ipbxapplication-gotoif',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_gotoif();"');

?>
</div>
