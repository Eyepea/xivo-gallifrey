<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');
	
	$act = $this->vars('act');
?>

<form action="#" method="post" id="fm-musiconhold-toolbar" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => 'list'));?>
	<div class="fm-field"><?=$form->slt(array('name' => 'cat','id' => 'it-cat','key' => true,'key_val' => 'category','field' => false,'empty' => $this->bbf('toolbar_fm_category-main'),'value' => $this->vars('cat')),$this->vars('list_cats'),($act !== 'list' && $act !== 'listfile' ? 'onclick="xivo_fm_unshift_pop_opt_select(this.id);"' : '').'onchange="this.form[\'act\'].value = this.value == \'\' ? \'list\' : \'listfile\'; return(this.value != \'null\' ? this.form.submit() : false);"')?></div>
</form>
<a href="#" onmouseover="xivo_eid('advanced-menu').style.display = 'block';" onmouseout="xivo_eid('advanced-menu').style.display = 'none';"><?=$url->img_html('img/menu/top/toolbar/bt-add.gif',$this->bbf('toolbar_opt_add'),'border="0"')?></a>
<div class="sb-advanced-menu">
	<ul id="advanced-menu" onmouseover="xivo_eid('advanced-menu').style.display = 'block';" onmouseout="xivo_eid('advanced-menu').style.display = 'none';">	
		<li><a href="#" onclick="xivo_fm['fm-musiconhold-toolbar']['act'].value = 'add'; xivo_fm['fm-musiconhold-toolbar'].submit();"><?=$this->bbf('toolbar_adv_menu_add_category');?></a></li>
		<li><a href="#" onclick="xivo_fm['fm-musiconhold-toolbar']['act'].value = 'addfile'; xivo_fm['fm-musiconhold-toolbar'].submit();"><?=$this->bbf('toolbar_adv_menu_add_file');?></a></li>
	</ul>
</div>
