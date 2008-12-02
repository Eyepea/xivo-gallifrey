<?php

$form = &$this->get_module('form');
$element = $this->get_var('element');
$option = $this->get_var('option');
$info = $this->get_var('info');

?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center"><?=$this->bbf('title_content_name');?></span>
		<span class="span-right">&nbsp;</span>
	</h3>
	<div class="sb-content">
		<form action="#" enctype="multipart/form-data" method="post" accept-charset="utf-8">
<?php
	echo	$form->hidden(array('name'	=> 'max_file_size',
				    'value'	=> $option['file']['size'])),

		$form->hidden(array('name'	=> XIVO_SESS_NAME,
				    'value'	=> XIVO_SESS_ID)),

		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1)),

		$form->hidden(array('name'	=> 'act',
				    'value'	=> 'addfile')),

		$form->file(array('desc'	=> $this->bbf('fm_filename'),
				  'name'	=> 'filename',
				  'labelid'	=> 'filename',
				  'size'	=> 15,
				  'value'	=> $info['filename'])),

		$form->select(array('desc'	=> $this->bbf('fm_category'),
				    'name'	=> 'category',
				    'labelid'	=> 'it-category',
				    'key'	=> true,
				    'altkey'	=> 'category',
				    'value'	=> $info['category']),
			      $this->get_var('list_cats')),

		$form->submit(array('name'	=> 'submit',
				    'id'	=> 'it-submit',
				    'value'	=> $this->bbf('fm_bt-save')));
?>
		</form>
	</div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>
