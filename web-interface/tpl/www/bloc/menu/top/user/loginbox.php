<?php
	$url = &$this->get_module('url');
?>
<h1 id="loginbox"><?=$url->img_html('img/menu/top/login/left.gif');?><span><?=xivo_user::get_infos('login');?></span><?=$url->img_html('img/menu/top/login/sep.gif');?><span><?=$this->bbf('usr_type__'.xivo_user::get_infos('meta'));?></span><?=$url->href_html($url->img_html('img/menu/top/login/bt-logout.gif',$this->bbf('mn_top_logout'),'border="0"'),'xivo/logout',null,null,$this->bbf('mn_top_logout'));?></h1>
