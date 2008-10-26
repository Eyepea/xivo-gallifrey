<?php

$form = &$this->get_module('form');
$info = $this->get_var('info');

?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center"><?=$this->bbf('title_content_name');?></span>
		<span class="span-right">&nbsp;</span>
	</h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">
<?php
	echo	$form->hidden(array('name'	=> XIVO_SESS_NAME,
				    'value'	=> XIVO_SESS_ID)),

		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1)),

		$form->hidden(array('name'	=> 'act',
				    'value'	=> 'edit')),

		$form->hidden(array('name'	=> 'id',
				    'value'	=> $this->get_var('id'))),

		$form->hidden(array('name'	=> 'dir',
				    'value'	=> $this->get_var('dir'))),

		$form->text(array('desc'	=> $this->bbf('fm_filename'),
				  'name'	=> 'filename',
				  'labelid'	=> 'filename',
				  'size'	=> 15,
				  'value'	=> $info['filename'])),

		$form->select(array('desc'	=> $this->bbf('fm_dirname'),
				    'name'	=> 'dirname',
				    'labelid'	=> 'it-dirname',
				    'key'	=> false,
				    'value'	=> $info['dirname']),
			      $this->get_var('list_dirs')),

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
