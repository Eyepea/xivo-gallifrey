<div id="fd-ipbxapplication-background" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$apparg_background = $this->get_var('apparg_background');
$sound_list = $this->get_varra('destination_list','sounds');

if(empty($sound_list) === false):
	echo $form->select(array('desc'		=> $this->bbf('fm_ipbxapplication_background-filename'),
				 'name'		=> 'ipbxapplication[background][filename]',
				 'labelid'	=> 'ipbxapplication-background-filename',
				 'key'		=> 'name',
				 'altkey'	=> 'filename',
				 'default'	=> $apparg_background['filename']['default']),
			   $sound_list);
else:
	echo	'<div class="txt-center">',
		$url->href_html($this->bbf('add_playback-sound'),
				'service/ipbx/pbx_services/sounds',
				array('act' => 'list','dir' => 'playback')),
		'</div>';
endif;

echo	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_background-s'),
			      'name'	=> 'ipbxapplication[background][s]',
			      'labelid'	=> 'ipbxapplication-background-s',
			      'default'	=> $apparg_background['s']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_background-n'),
			      'name'	=> 'ipbxapplication[background][n]',
			      'labelid'	=> 'ipbxapplication-background-n',
			      'default'	=> $apparg_background['n']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_background-m'),
			      'name'	=> 'ipbxapplication[background][m]',
			      'labelid'	=> 'ipbxapplication-background-m',
			      'default'	=> $apparg_background['m']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_background-language'),
		          'name'	=> 'ipbxapplication[background][language]',
		          'labelid'	=> 'ipbxapplication-background-language',
		          'size'	=> 10,
		          'default'	=> $apparg_background['language']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_background-context'),
		          'name'	=> 'ipbxapplication[background][context]',
		          'labelid'	=> 'ipbxapplication-background-context',
		          'size'	=> 15,
		          'default'	=> $apparg_background['context']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-background',
			    'id'	=> 'it-add-ipbxapplication-background',
			    'value'	=> $this->bbf('fm_bt-add')),
		   'onclick="xivo_ast_application_background();"');

?>
</div>
