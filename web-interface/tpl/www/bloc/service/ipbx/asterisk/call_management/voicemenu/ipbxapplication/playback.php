<div id="fd-ipbxapplication-playback" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$apparg_playback = $this->get_var('apparg_playback');
$sound_list = $this->get_varra('destination_list','sounds');

if(empty($sound_list) === false):
	echo $form->select(array('desc'		=> $this->bbf('fm_ipbxapplication_playback-filename'),
				 'name'		=> 'ipbxapplication[playback][filename]',
				 'labelid'	=> 'ipbxapplication-playback-filename',
				 'key'		=> 'name',
				 'altkey'	=> 'filename',
				 'default'	=> $apparg_playback['filename']['default']),
			   $sound_list);
else:
	echo	'<div class="txt-center">',
		$url->href_html($this->bbf('add_playback-sound'),
				'service/ipbx/pbx_services/sounds',
				array('act' => 'list','dir' => 'playback')),
		'</div>';
endif;

echo	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_playback-skip'),
			      'name'	=> 'ipbxapplication[playback][skip]',
			      'labelid'	=> 'ipbxapplication-playback-skip',
			      'default'	=> $apparg_playback['skip']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_playback-noanswer'),
			      'name'	=> 'ipbxapplication[playback][noanswer]',
			      'labelid'	=> 'ipbxapplication-playback-noanswer',
			      'default'	=> $apparg_playback['noanswer']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_playback-j'),
			      'name'	=> 'ipbxapplication[playback][j]',
			      'labelid'	=> 'ipbxapplication-playback-j',
			      'default'	=> $apparg_playback['j']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-playback',
			    'id'	=> 'it-add-ipbxapplication-playback',
			    'value'	=> $this->bbf('fm_bt-add')),
		   'onclick="xivo_ast_application_playback();"');

?>
</div>
