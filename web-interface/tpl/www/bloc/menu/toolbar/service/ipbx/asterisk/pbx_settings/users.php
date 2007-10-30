<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');
	
	$act = $this->vars('act');

	if(($search = (string) $this->vars('search')) === ''):
		$searchjs = '';
	else:
		$searchjs = ' xivo_fm[\'fm-users-list\'][\'search\'].value = \''.xivo_stript($search).'\';';
	endif;

	if(($context = $this->vars('context')) === '' && $search === ''):
		$contextjs = '';
	else:
		$contextjs = ' xivo_fm[\'fm-users-list\'][\'context\'].value = \''.xivo_stript($context).'\';';
	endif;

?>
<form action="#" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => 'list'));?>
	<div class="fm-field"><?=$form->text(array('name' => 'search','id' => 'it-search','size' => 20,'field' => false,'value' => $search,'default' => $this->bbf('toolbar_fm_search')),'onfocus="this.value = this.value == \''.xivo_stript($this->bbf('toolbar_fm_search')).'\' ? \'\' : this.value; xivo_fm_set_onfocus(this);"');?><?=$form->image(array('name' => 'submit','id' => 'it-subsearch','src' => $url->img('img/menu/top/toolbar/bt-search.gif'),'field' => false,'alt' => $this->bbf('toolbar_fm_search')));?><?=$form->select(array('name' => 'context','id' => 'it-context','field' => false,'empty' => $this->bbf('toolbar_fm_context'),'value' => $context),$this->vars('contexts'),'style="margin-left: 20px;" onchange="this.form[\'search\'].value = \'\'; this.form.submit()"');?></div>
</form>
	<?=$url->href_html($url->img_html('img/menu/top/toolbar/bt-add.gif',$this->bbf('toolbar_opt_add'),'border="0"'),'service/ipbx/pbx_settings/users','act=add',null,$this->bbf('toolbar_opt_add'));?>
<?php
	if($act === 'list'):
?>
<div class="sb-advanced-menu">
	<ul id="advanced-menu" onmouseover="this.style.display = 'block';" onmouseout="this.style.display = 'none';">	
		<li><a href="#" onclick="xivo_fm['fm-users-list']['act'].value = 'enables';<?=$searchjs,$contextjs?> xivo_fm['fm-users-list'].submit();"><?=$this->bbf('toolbar_adv_menu_enable');?></a></li>
		<li><a href="#" onclick="xivo_fm['fm-users-list']['act'].value = 'disables';<?=$searchjs,$contextjs?> xivo_fm['fm-users-list'].submit();"><?=$this->bbf('toolbar_adv_menu_disable');?></a></li>
		<li><a href="#" onclick="this.tmp = xivo_fm['fm-users-list']['act'].value; xivo_fm['fm-users-list']['act'].value = 'deletes';<?=$searchjs,$contextjs?> return(confirm('<?=xivo_stript($this->bbf('toolbar_adv_menu_delete_confirm'));?>') ? xivo_fm['fm-users-list'].submit() : xivo_fm['fm-users-list']['act'] = this.tmp);"><?=$this->bbf('toolbar_adv_menu_delete');?></a></li>
	</ul>
</div><a href="#" onmouseover="xivo_eid('advanced-menu').style.display = 'block';" onmouseout="xivo_eid('advanced-menu').style.display = 'none';"><?=$url->img_html('img/menu/top/toolbar/bt-more.gif',$this->bbf('toolbar_opt_advanced'),'border="0"');?></a>

<?php
	endif;
?>
