<?php
	$url = &$this->get_module('url');
?>
<?=$url->href_html($url->img_html('img/menu/top/toolbar/bt-add.gif',$this->bbf('toolbar_opt_add'),'border="0"'),'service/ipbx/pbx_settings/groups','act=add');?>
