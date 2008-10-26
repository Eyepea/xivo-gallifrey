<?php

$form = &$this->get_module('form');
$url = &$this->get_module('url');
$dhtml = &$this->get_module('dhtml');
	
$act = $this->get_var('act');
$group = $this->get_var('group');

$param = array('act' => 'addagent');

if($act !== 'list' && $act !== 'add'):
	$param['group'] = $group;
else:
	$group = '';
endif;

?>
<form action="#" method="post" accept-charset="utf-8">
<?php
	echo	$form->hidden(array('name'	=> XIVO_SESS_NAME,
				    'value'	=> XIVO_SESS_ID)),

		$form->hidden(array('name'	=> 'act',
				    'value'	=> $act));
?>
	<div class="fm-field">
<?php
		echo	$form->select(array('name'	=> 'group',
					    'id'	=> 'it-group',
					    'field'	=> false,
					    'browse'	=> 'agentgroup',
					    'key'	=> 'name',
					    'altkey'	=> 'id',
					    'empty'	=> $this->bbf('toolbar_fm_group'),
					    'value'	=> $group),
				      $this->get_var('agentgroup_list'),
				      'onchange="this.form[\'act\'].value = this.value === \'\'
				      					    ? \'list\'
									    : \'listagent\';
						 return(this.form.submit());"');
?>
	</div>
</form>
<?php
	echo	$url->img_html('img/menu/top/toolbar/bt-add.gif',
			       $this->bbf('toolbar_opt_add'),
			       'border="0"
			        onmouseover="xivo_eid(\'add-menu\').style.display = \'block\';"
				onmouseout="xivo_eid(\'add-menu\').style.display = \'none\';"');
?>
<div class="sb-advanced-menu">
	<ul id="add-menu"
	    onmouseover="this.style.display = 'block';"
	    onmouseout="this.style.display = 'none';">	
		<li><?=$url->href_html($this->bbf('toolbar_adv_menu_add-group'),
				       'service/ipbx/pbx_settings/agents',
				       'act=add');?></li>
		<li><?=$url->href_html($this->bbf('toolbar_adv_menu_add-agent'),
				       'service/ipbx/pbx_settings/agents',
				       $param);?></li>
	</ul>
</div><?php

if($act === 'list'):
	echo	$url->img_html('img/menu/top/toolbar/bt-more.gif',
			       $this->bbf('toolbar_opt_advanced'),
			       'border="0"
				onmouseover="xivo_eid(\'advanced-menu\').style.display = \'block\';"
				onmouseout="xivo_eid(\'advanced-menu\').style.display = \'none\';"');

?>
<div class="sb-advanced-menu">
	<ul id="advanced-menu"
	    onmouseover="this.style.display = 'block';"
	    onmouseout="this.style.display = 'none';">	
		<li>
			<a href="#"
			   onclick="xivo_fm['fm-agentgroups-list']['act'].value = 'enables';
				    xivo_fm['fm-agentgroups-list'].submit();">
			<?=$this->bbf('toolbar_adv_menu_enable');?></a>
		</li>
		<li>
			<a href="#"
			   onclick="xivo_fm['fm-agentgroups-list']['act'].value = 'disables';
				    xivo_fm['fm-agentgroups-list'].submit();">
			<?=$this->bbf('toolbar_adv_menu_disable');?></a>
		</li>
		<li>
			<a href="#"
			   onclick="xivo_fm_checked_all('fm-agentgroups-list','agentgroups[]');
				    return(false);">
			<?=$this->bbf('toolbar_adv_menu_select-all');?></a>
		</li>
		<li>
			<a href="#"
			   onclick="this.tmp = xivo_fm['fm-agentgroups-list']['act'].value;
				    xivo_fm['fm-agentgroups-list']['act'].value = 'deletes';
				    return(confirm('<?=$dhtml->escape($this->bbf('toolbar_adv_menu_delete_confirm-agentgroup'));?>')
					   ? xivo_fm['fm-agentgroups-list'].submit()
					   : xivo_fm['fm-agentgroups-list']['act'] = this.tmp);">
			<?=$this->bbf('toolbar_adv_menu_delete');?></a>
		</li>
	</ul>
</div><?php

elseif($act === 'listagent'):
	echo	$url->img_html('img/menu/top/toolbar/bt-more.gif',
			       $this->bbf('toolbar_opt_advanced'),
			       'border="0"
				onmouseover="xivo_eid(\'advanced-menu\').style.display = \'block\';"
				onmouseout="xivo_eid(\'advanced-menu\').style.display = \'none\';"');

?>
<div class="sb-advanced-menu">
	<ul id="advanced-menu"
	    onmouseover="this.style.display = 'block';"
	    onmouseout="this.style.display = 'none';">	
		<li>
			<a href="#"
			   onclick="xivo_fm['fm-agents-list']['act'].value = 'enableagents';
				    xivo_fm['fm-agents-list'].submit();">
			<?=$this->bbf('toolbar_adv_menu_enable');?></a>
		</li>
		<li>
			<a href="#"
			   onclick="xivo_fm['fm-agents-list']['act'].value = 'disableagents';
				    xivo_fm['fm-agents-list'].submit();">
			<?=$this->bbf('toolbar_adv_menu_disable');?></a>
		</li>
		<li>
			<a href="#"
			   onclick="xivo_fm_checked_all('fm-agents-list','agents[]');
				    return(false);">
			<?=$this->bbf('toolbar_adv_menu_select-all');?></a>
		</li>
		<li>
			<a href="#"
			   onclick="this.tmp = xivo_fm['fm-agents-list']['act'].value;
				    xivo_fm['fm-agents-list']['act'].value = 'deleteagents';
				    return(confirm('<?=$dhtml->escape($this->bbf('toolbar_adv_menu_delete_confirm-agent'));?>')
					   ? xivo_fm['fm-agents-list'].submit()
					   : xivo_fm['fm-agents-list']['act'] = this.tmp);">
			<?=$this->bbf('toolbar_adv_menu_delete');?></a>
		</li>
	</ul>
</div>
<?php

endif;

?>
