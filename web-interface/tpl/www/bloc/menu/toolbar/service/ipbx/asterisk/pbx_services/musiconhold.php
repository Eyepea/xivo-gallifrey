<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');
	
	$act = $this->get_var('act');
	$cat = $this->get_var('cat');

	$param = array('act' => 'addfile');

	if($act !== 'list' && $act !== 'add'):
		$param['cat'] = $cat;
	else:
		$cat = '';
	endif;
?>

<form action="#" method="post" id="fm-musiconhold-toolbar" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => 'list'));?>
	<div class="fm-field"><?=$form->select(array('name' => 'cat','id' => 'it-cat','key' => true,'altkey' => 'category','field' => false,'empty' => $this->bbf('toolbar_fm_category'),'value' => $cat),$this->get_var('list_cats'),'onchange="this.form[\'act\'].value = this.value == \'\' ? \'list\' : \'listfile\'; return(this.form.submit());"');?></div>
</form>
<?=$url->img_html('img/menu/top/toolbar/bt-add.gif',$this->bbf('toolbar_opt_add'),'border="0" onmouseover="xivo_eid(\'add-menu\').style.display = \'block\';" onmouseout="xivo_eid(\'add-menu\').style.display = \'none\';"');?>
<div class="sb-advanced-menu">
	<ul id="add-menu" onmouseover="this.style.display = 'block';" onmouseout="this.style.display = 'none';">	
		<li><?=$url->href_html($this->bbf('toolbar_adv_menu_add-category'),'service/ipbx/pbx_services/musiconhold','act=add');?></li>
		<li><?=$url->href_html($this->bbf('toolbar_adv_menu_add-file'),'service/ipbx/pbx_services/musiconhold',$param);?></li>
	</ul>
</div>
