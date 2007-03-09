<?php
	$form = &$this->get_module('form');
	$info = $this->vars('info');
?>

<div id="cat-policy" class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'cat','value' => 'policy'));?>
<?=$form->hidden(array('name' => 'fm_send','value' => 1));?>
<?=$form->hidden(array('name' => 'id','value' => $info['id']));?>
<table cellspacing="0" cellpadding="0" border="0">
<?php
	$tree = $this->vars('tree');
	$tree = &$tree['service'];

	if(isset($tree['child']) === true && ($arr = xivo_get_aks($tree['child'])) !== false):
		$tree = &$tree['child'];
		for($i = 0;$i < $arr['cnt'];$i++):
			$k = &$arr['keys'][$i];
			$v = &$tree[$k];

			echo '<tr><th>',$form->checkbox(array('desc' => array('%s%s',$this->bbf('ply_'.$v['id']),1),'name' => 'tree[]','label' => 'lb-'.$v['id'],'id' => $v['id'],'field' => false,'value' => $v['path'],'checked' => $v['access']),'onclick="xivo_fm_mk_policy(this);"'),'</th></tr>';

			if(isset($v['child']) === true):
				$this->file_include('bloc/xivo/configuration/policy/tree',array('tree' => $v['child']));
			endif;
		endfor;
	endif;
?>
</table>
<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
