<?php

$url = &$this->get_module('url');
$dhtml = &$this->get_module('dhtml');

echo	$url->href_html($url->img_html('img/menu/top/toolbar/bt-add.gif',
				       $this->bbf('toolbar_opt_add'),
				       'border="0"'),
			'service/ipbx/pbx_settings/voicemail',
			'act=add',
			null,
			$this->bbf('toolbar_opt_add'));

if($this->get_var('act') === 'list'):
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
			   onclick="xivo_fm['fm-voicemail-list']['act'].value = 'enables';
				    xivo_fm['fm-voicemail-list'].submit();">
				<?=$this->bbf('toolbar_adv_menu_enable');?></a>
		</li>
		<li>
			<a href="#"
			   onclick="xivo_fm['fm-voicemail-list']['act'].value = 'disables';
				    xivo_fm['fm-voicemail-list'].submit();">
				<?=$this->bbf('toolbar_adv_menu_disable');?></a>
		</li>
		<li>
			<a href="#"
			   onclick="xivo_fm_checked_all('fm-voicemail-list','voicemails[]');
				    return(false);">
				<?=$this->bbf('toolbar_adv_menu_select-all');?></a>
		</li>
		<li>
			<a href="#"
			   onclick="this.tmp = xivo_fm['fm-voicemail-list']['act'].value;
				    xivo_fm['fm-voicemail-list']['act'].value = 'deletes';
				    return(confirm('<?=$dhtml->escape($this->bbf('toolbar_adv_menu_delete_confirm'));?>')
					   ? xivo_fm['fm-voicemail-list'].submit()
					   : xivo_fm['fm-voicemail-list']['act'] = this.tmp);">
				<?=$this->bbf('toolbar_adv_menu_delete');?></a>
		</li>
	</ul>
</div>
<?php

endif;

?>
