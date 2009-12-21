<?php
$url = &$this->get_module('url');
$form = &$this->get_module('form');

echo
	$form->hidden(array('name' => 'fm_send',
						'value' => 1)),

	$form->hidden(array('name' => 'step',
						'value' => $this->get_var('step'))),

	$form->hidden(array('name'	=> 'max_file_size',
						'value'	=> $this->get_var('import_file','size'))),

	$this->bbf('wz-import-format-hint'),

	$form->file(array('desc'    => $this->bbf('fm_import'),
					'name'    => 'import',
					'labelid' => 'import',
					'size'    => 15)),
	
	$form->submit(array('name' => 'upload',
						'value' => $this->bbf('fm_upload')));
?>
