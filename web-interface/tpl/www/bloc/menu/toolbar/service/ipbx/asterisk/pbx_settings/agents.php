<?php
	$form = &$this->get_module('form');
	$url = &$this->get_module('url');
	
	$act = $this->vars('act');
	$group = $this->vars('group');

	$param = array('act' => 'addagent');

	if($act !== 'list' && $act !== 'add')
		$param['group'] = $group;
	else
		$group = '';
?>
<form action="#" method="post" id="fm-agent-toolbar" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => 'list'));?>
	<div class="fm-field"><?=$form->slt(array('name' => 'group','id' => 'it-group','key' => true,'key_val' => 'group','browse' => 'agroup','key' => 'name','key_val' => 'id','field' => false,'empty' => $this->bbf('toolbar_fm_group'),'value' => $group),$this->vars('list_grps'),'onchange="this.form[\'act\'].value = this.value == \'\' ? \'list\' : \'listagent\'; return(this.form.submit());"');?></div>
</form>

<a href="#" onmouseover="xivo_eid('advanced-menu').style.display = 'block';" onmouseout="xivo_eid('advanced-menu').style.display = 'none';"><?=$url->img_html('img/menu/top/toolbar/bt-add.gif',$this->bbf('toolbar_opt_add'),'border="0"');?></a>
<div class="sb-advanced-menu">
	<ul id="advanced-menu" onmouseover="xivo_eid('advanced-menu').style.display = 'block';" onmouseout="xivo_eid('advanced-menu').style.display = 'none';">	
		<li><?=$url->href_html($this->bbf('toolbar_adv_menu_add_group'),'service/ipbx/pbx_settings/agents','act=add');?></li>
		<li><?=$url->href_html($this->bbf('toolbar_adv_menu_add_agent'),'service/ipbx/pbx_settings/agents',$param);?></li>
	</ul>
</div>

