<div id="fd-ipbxapplication-waitexten" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_waitexten = $this->get_var('apparg_waitexten');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_waitexten-seconds'),
		          'name'	=> 'ipbxapplication[waitexten][seconds]',
		          'labelid'	=> 'ipbxapplication-waitexten-seconds',
		          'size'	=> 10,
		          'default'	=> $apparg_waitexten['seconds']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_waitexten-m'),
			      'name'	=> 'ipbxapplication[waitexten][m]',
			      'labelid'	=> 'ipbxapplication-waitexten-m',
			      'default'	=> $apparg_waitexten['m']['default']));

if(($moh_list = $this->get_var('moh_list')) !== false):
	echo $form->select(array('desc'		=> $this->bbf('fm_ipbxapplication_waitexten-musiconhold'),
				 'name'		=> 'ipbxapplication[waitexten][musiconhold]',
				 'labelid'	=> 'ipbxapplication-waitexten-musiconhold',
				 'key'		=> 'category',
				 'empty'	=> true,
				 'default'	=> $apparg_waitexten['musiconhold']['default']),
			   $moh_list);
endif;

echo $form->button(array('name'		=> 'add-ipbxapplication-waitexten',
			 'id'		=> 'it-add-ipbxapplication-waitexten',
			 'value'	=> $this->bbf('fm_bt-add')),
		   'onclick="xivo_ast_application_waitexten();"');

?>
</div>
