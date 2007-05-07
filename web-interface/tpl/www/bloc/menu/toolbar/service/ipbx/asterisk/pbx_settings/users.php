<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');
	
	$act = $this->vars('act');
	$ract = $this->vars('ract');

	if($ract !== 'search' || ($search = $this->vars('search')) === ''):
		$search = $this->bbf('toolbar_fm_search');
	endif;

	$contexts = $this->vars('contexts');

	if(is_array($contexts) === true && isset($contexts['#main']) === true)
		$contexts['#main'] = $this->bbf('toolbar_fm_context-opt-main');
?>
<form action="#" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => 'search'));?>
	<div class="fm-field"><?=$form->text(array('name' => 'search','id' => 'it-search','size' => 20,'field' => false,'value' => $search),'onfocus="this.value = this.value == \''.xivo_stript($this->bbf('toolbar_fm_search')).'\' ? \'\' : this.value; this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?><?=$form->bimg(array('name' => 'submit','id' => 'it-subsearch','src' => $url->img('img/menu/top/toolbar/bt-search.gif'),'field' => false,'alt' => $this->bbf('toolbar_fm_search')));?><?=$form->slt(array('name' => 'context','id' => 'it-context','field' => false,'empty' => $this->bbf('toolbar_fm_context'),'value' => $this->vars('context')),$contexts,'style="margin-left: 20px;" onchange="this.form[\'act\'].value = \'context\'; this.form.submit();" onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"')?></div>
</form>
	<?=$url->href_html($url->img_html('img/menu/top/toolbar/bt-add.gif',$this->bbf('toolbar_opt_add'),'border="0"'),'service/ipbx/pbx_settings/users','act=add',null,$this->bbf('toolbar_opt_add'));?>
<?php
	if($act === 'list'):
?>
<div class="sb-advanced-menu">
	<ul id="advanced-menu" onmouseover="xivo_eid('advanced-menu').style.display = 'block';" onmouseout="xivo_eid('advanced-menu').style.display = 'none';">	
		<li><a href="#" onclick="xivo_fm['fm-users-list']['act'].value = 'enables'; xivo_fm['fm-users-list'].submit();"><?=$this->bbf('toolbar_adv_menu_enable');?></a></li>
		<li><a href="#" onclick="xivo_fm['fm-users-list']['act'].value = 'disables'; xivo_fm['fm-users-list'].submit();"><?=$this->bbf('toolbar_adv_menu_disable');?></a></li>
		<li><a href="#" onclick="this.tmp = xivo_fm['fm-users-list']['act'].value; xivo_fm['fm-users-list']['act'].value = 'deletes'; return(confirm('<?=xivo_stript($this->bbf('toolbar_adv_menu_delete_confirm'));?>') ? xivo_fm['fm-users-list'].submit() : xivo_fm['fm-users-list']['act'] = this.tmp); "><?=$this->bbf('toolbar_adv_menu_delete');?></a></li>
	</ul>
</div><a href="#" onmouseover="xivo_eid('advanced-menu').style.display = 'block';" onmouseout="xivo_eid('advanced-menu').style.display = 'none';"><?=$url->img_html('img/menu/top/toolbar/bt-more.gif',$this->bbf('toolbar_opt_advanced'),'border="0"')?></a>

<?php
	endif;
?>
