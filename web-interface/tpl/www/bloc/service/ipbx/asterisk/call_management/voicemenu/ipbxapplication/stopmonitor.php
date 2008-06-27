<div id="fd-ipbxapplication-stopmonitor" class="b-nodisplay">
<?php

$form = &$this->get_module('form');

echo	$form->button(array('name'	=> 'add-ipbxapplication-stopmonitor',
			    'id'	=> 'it-add-ipbxapplication-stopmonitor',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_stopmonitor();"');

?>
</div>
