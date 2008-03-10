<?php
	$form = &$this->get_module('form');
	$info = $this->get_var('info');
	$tree = $this->get_var('tree');

	$tree = &$tree['service'];
?>

<div id="sr-acl" class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => 'acl'));?>
<?=$form->hidden(array('name' => 'fm_send','value' => 1));?>
<?=$form->hidden(array('name' => 'id','value' => $info['id']));?>
<table cellspacing="0" cellpadding="0" border="0">
<?php
	if(xivo_issa('child',$tree) === true && empty($tree['child']) === false):
		foreach($tree['child'] as $v):
			echo '<tr><th>',$form->checkbox(array('desc' => array('%s%s',$this->bbf('acl_'.$v['id']),1),'name' => 'tree[]','label' => 'lb-'.$v['id'],'id' => $v['id'],'field' => false,'value' => $v['path'],'checked' => $v['access']),'onclick="xivo_fm_mk_acl(this);"'),'</th></tr>';

			if(isset($v['child']) === true):
				$this->file_include('bloc/xivo/configuration/manage/user/acltree',array('tree' => $v['child']));
			endif;
		endforeach;
	endif;
?>
</table>
<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
