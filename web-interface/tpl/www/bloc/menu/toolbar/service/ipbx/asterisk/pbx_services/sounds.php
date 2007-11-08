<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');
	
	$act = $this->vars('act');
	$dir = $this->vars('dir');

	$param = array('act' => 'add');

	if($act !== 'listdir' && $act !== 'adddir'):
		$param['dir'] = $dir;
	else:
		$dir = '';
	endif;
?>

<form action="#" method="post" id="fm-sounds-toolbar" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => 'listdir'));?>
	<div class="fm-field"><?=$form->select(array('name' => 'dir','id' => 'it-dir','key' => false,'field' => false,'empty' => $this->bbf('toolbar_fm_directory'),'value' => $dir),$this->vars('list_dirs'),'onchange="this.form[\'act\'].value = this.value == \'\' ? \'listdir\' : \'list\'; return(this.form.submit());"');?></div>
</form>
<a href="#" onmouseover="xivo_eid('add-menu').style.display = 'block';" onmouseout="xivo_eid('add-menu').style.display = 'none';"><?=$url->img_html('img/menu/top/toolbar/bt-add.gif',$this->bbf('toolbar_opt_add'),'border="0"');?></a><?php

if($act === 'list'):
	echo '<a href="#" onclick="this.tmp = xivo_fm[\'fm-files-list\'][\'act\'].value; xivo_fm[\'fm-files-list\'][\'act\'].value = \'deletes\'; return(confirm(\'',xivo_stript($this->bbf('toolbar_adv_menu_delete_confirm')),'\') ? xivo_fm[\'fm-files-list\'].submit() : xivo_fm[\'fm-files-list\'][\'act\'] = this.tmp);">',$url->img_html('img/menu/top/toolbar/bt-delete.gif',$this->bbf('toolbar_opt_delete'),'border="0"'),'</a>';
endif;

?>
<div class="sb-advanced-menu">
	<ul id="add-menu" onmouseover="this.style.display = 'block';" onmouseout="this.style.display = 'none';">	
		<li><?=$url->href_html($this->bbf('toolbar_adv_menu_add-directory'),'service/ipbx/pbx_services/sounds','act=adddir');?></li>
		<li><?=$url->href_html($this->bbf('toolbar_adv_menu_add-file'),'service/ipbx/pbx_services/sounds',$param);?></li>
	</ul>
</div>
