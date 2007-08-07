<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');
	
	$act = $this->vars('act');

	$dir = $this->vars('dir');

	$param = array('act' => 'add');

	if($act !== 'listdir' && $act !== 'adddir')
		$param['dir'] = $dir;
	else
		$dir = '';
?>

<form action="#" method="post" id="fm-sounds-toolbar" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => 'listdir'));?>
	<div class="fm-field"><?=$form->select(array('name' => 'dir','id' => 'it-dir','key' => false,'field' => false,'empty' => $this->bbf('toolbar_fm_directory'),'value' => $dir),$this->vars('list_dirs'),'onchange="this.form[\'act\'].value = this.value == \'\' ? \'listdir\' : \'list\'; return(this.form.submit());"');?></div>
</form>
<a href="#" onmouseover="xivo_eid('advanced-menu').style.display = 'block';" onmouseout="xivo_eid('advanced-menu').style.display = 'none';"><?=$url->img_html('img/menu/top/toolbar/bt-add.gif',$this->bbf('toolbar_opt_add'),'border="0"');?></a>
<div class="sb-advanced-menu">
	<ul id="advanced-menu" onmouseover="xivo_eid('advanced-menu').style.display = 'block';" onmouseout="xivo_eid('advanced-menu').style.display = 'none';">	
		<li><?=$url->href_html($this->bbf('toolbar_adv_menu_add_directory'),'service/ipbx/general_settings/sounds','act=adddir');?></li>
		<li><?=$url->href_html($this->bbf('toolbar_adv_menu_add_file'),'service/ipbx/general_settings/sounds',$param);?></li>
	</ul>
</div>
