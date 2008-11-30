<?php

$form = &$this->get_module('form');
$element = $this->get_var('element');
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

		$form->text(array('desc'	=> $this->bbf('fm_category'),
				  'name'	=> 'category',
				  'labelid'	=> 'category',
				  'size'	=> 15,
				  'default'	=> $element['category']['default'],
				  'value'	=> $info['category'])),

		$form->select(array('desc'	=> $this->bbf('fm_mode'),
				    'name'	=> 'mode',
				    'labelid'	=> 'mode',
				    'key'	=> false,
				    'default'	=> $element['mode']['default'],
				    'value'	=> $info['mode']),
			      $element['mode']['value'],
			      'onchange="xivo_chg_attrib(\'fm_musiconhold\',
							 \'fd-application\',
							 Number(this.value === \'custom\'));"'),

		$form->text(array('desc'	=> $this->bbf('fm_application'),
				  'name'	=> 'application',
				  'labelid'	=> 'application',
				  'size'	=> 15,
				  'default'	=> $element['application']['default'],
				  'value'	=> $info['application'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_random'),
				      'name'	=> 'random',
				      'labelid'	=> 'random',
				      'default'	=> $element['random']['default'],
				      'checked'	=> $info['random'])),

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
