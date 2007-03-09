<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');
	
	$act = $this->vars('act');
?>

<form action="#" method="post" id="fm-sounds-toolbar" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => 'listdir'));?>
	<div class="fm-field"><?=$form->slt(array('name' => 'dir','id' => 'it-dir','key' => false,'field' => false,'empty' => $this->bbf('toolbar_fm_directory-main'),'value' => $this->vars('dir')),$this->vars('list_dirs'),($act !== 'listdir' && $act !== 'list' ? 'onclick="xivo_fm_unshift_pop_opt_select(this.id);"' : '').'onchange="this.form[\'act\'].value = this.value == \'\' ? \'listdir\' : \'list\'; return(this.value != \'null\' ? this.form.submit() : false);"')?></div>
</form>
<a href="#" onmouseover="xivo_eid('advanced-menu').style.display = 'block';" onmouseout="xivo_eid('advanced-menu').style.display = 'none';"><?=$url->img_html('img/menu/top/toolbar/bt-add.gif',$this->bbf('toolbar_opt_add'),'border="0"')?></a>
<div class="sb-advanced-menu">
	<ul id="advanced-menu" onmouseover="xivo_eid('advanced-menu').style.display = 'block';" onmouseout="xivo_eid('advanced-menu').style.display = 'none';">	
		<li><a href="#" onclick="xivo_fm['fm-sounds-toolbar']['act'].value = 'adddir'; xivo_fm['fm-sounds-toolbar'].submit();"><?=$this->bbf('toolbar_adv_menu_add_directory');?></a></li>
		<li><a href="#" onclick="xivo_fm['fm-sounds-toolbar']['act'].value = 'add'; xivo_fm['fm-sounds-toolbar'].submit();"><?=$this->bbf('toolbar_adv_menu_add_file');?></a></li>
	</ul>
</div>
