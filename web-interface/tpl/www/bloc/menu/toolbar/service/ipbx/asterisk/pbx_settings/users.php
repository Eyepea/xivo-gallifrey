<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');
	$dhtml = &$this->get_module('dhtml');
	
	$act = $this->get_var('act');

	if(($search = (string) $this->get_var('search')) === ''):
		$searchjs = '';
	else:
		$searchjs = ' xivo_fm[\'fm-users-list\'][\'search\'].value = \''.$dhtml->escape($search).'\';';
	endif;

	if(($context = $this->get_var('context')) === '' && $search === ''):
		$contextjs = '';
	else:
		$contextjs = ' xivo_fm[\'fm-users-list\'][\'context\'].value = \''.$dhtml->escape($context).'\';';
	endif;

?>
<form action="#" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => 'list'));?>
	<div class="fm-field"><?=$form->text(array('name' => 'search','id' => 'it-search','size' => 20,'field' => false,'value' => $search,'default' => $this->bbf('toolbar_fm_search')),'onfocus="this.value = this.value == \''.$dhtml->escape($this->bbf('toolbar_fm_search')).'\' ? \'\' : this.value; xivo_fm_set_onfocus(this);"');?><?=$form->image(array('name' => 'submit','id' => 'it-subsearch','src' => $url->img('img/menu/top/toolbar/bt-search.gif'),'field' => false,'alt' => $this->bbf('toolbar_fm_search')));?><?=$form->select(array('name' => 'context','id' => 'it-context','field' => false,'empty' => $this->bbf('toolbar_fm_context'),'value' => $context),$this->get_var('contexts'),'style="margin-left: 20px;" onchange="this.form[\'search\'].value = \'\'; this.form.submit()"');?></div>
</form>
<?=$url->img_html('img/menu/top/toolbar/bt-add.gif',$this->bbf('toolbar_opt_add'),'border="0" onmouseover="xivo_eid(\'add-menu\').style.display = \'block\';" onmouseout="xivo_eid(\'add-menu\').style.display = \'none\';"');?>
<div class="sb-advanced-menu">
	<ul id="add-menu" onmouseover="this.style.display = 'block';" onmouseout="this.style.display = 'none';">	
		<li><?=$url->href_html($this->bbf('toolbar_add_menu_add'),'service/ipbx/pbx_settings/users','act=add');?></li>
		<li><?=$url->href_html($this->bbf('toolbar_add_menu_import-file'),'service/ipbx/pbx_settings/users','act=import');?></li>
	</ul>
</div><?php
	if($act === 'list'):
?>
<?=$url->img_html('img/menu/top/toolbar/bt-more.gif',$this->bbf('toolbar_opt_advanced'),'border="0" onmouseover="xivo_eid(\'advanced-menu\').style.display = \'block\';" onmouseout="xivo_eid(\'advanced-menu\').style.display = \'none\';"');?>
<div class="sb-advanced-menu">
	<ul id="advanced-menu" onmouseover="this.style.display = 'block';" onmouseout="this.style.display = 'none';">	
		<li><a href="#" onclick="xivo_fm['fm-users-list']['act'].value = 'enables';<?=$searchjs,$contextjs?> xivo_fm['fm-users-list'].submit();"><?=$this->bbf('toolbar_adv_menu_enable');?></a></li>
		<li><a href="#" onclick="xivo_fm['fm-users-list']['act'].value = 'disables';<?=$searchjs,$contextjs?> xivo_fm['fm-users-list'].submit();"><?=$this->bbf('toolbar_adv_menu_disable');?></a></li>
		<li><a href="#" onclick="xivo_fm_checked_all('fm-users-list','users[]'); return(false);"><?=$this->bbf('toolbar_adv_menu_select-all');?></a></li>
		<li><a href="#" onclick="this.tmp = xivo_fm['fm-users-list']['act'].value; xivo_fm['fm-users-list']['act'].value = 'deletes';<?=$searchjs,$contextjs?> return(confirm('<?=$dhtml->escape($this->bbf('toolbar_adv_menu_delete_confirm'));?>') ? xivo_fm['fm-users-list'].submit() : xivo_fm['fm-users-list']['act'] = this.tmp);"><?=$this->bbf('toolbar_adv_menu_delete');?></a></li>
	</ul>
</div>
<?php
	endif;
?>
