<div id="fd-ipbxapplication-vmauthenticate" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$apparg_vmauthenticate = $this->get_var('apparg_vmauthenticate');

if(($voicemail_list = $this->get_var('voicemail_list')) !== false):
	echo $form->select(array('desc'		=> $this->bbf('fm_ipbxapplication_vmauthenticate-mailbox'),
				 'name'		=> 'ipbxapplication[vmauthenticate][mailbox]',
				 'labelid'	=> 'ipbxapplication-vmauthenticate-mailbox',
				 'key'		=> 'identity',
				 'altkey'	=> 'uniqueid'),
			   $voicemail_list);
else:
	echo	'<div class="txt-center">',
		$url->href_html($this->bbf('create_voicemail'),
				'service/ipbx/pbx_settings/voicemail',
				'act=add'),
		'</div>';
endif;

	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_vmauthenticate-s'),
				      'name'	=> 'ipbxapplication[vmauthenticate][s]',
				      'labelid'	=> 'ipbxapplication-vmauthenticate-s',
				      'default'	=> $apparg_vmauthenticate['s']['default'])),

		$form->button(array('name'	=> 'ipbxapplication-vmauthenticate',
				    'id'	=> 'it-add-ipbxapplication-vmauthenticate',
				    'value'	=> $this->bbf('fm_bt-add')),
			      'onclick="xivo_ast_application_vmauthenticate();"');

?>
</div>
