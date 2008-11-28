<?php

$url = &$this->get_module('url');

?>
<h1 id="loginbox">
<?php
	echo	$url->img_html('img/menu/top/login/left.gif'),
		'<span>',
		$this->bbf('info_top_login'),
		'&nbsp;<b>',xivo_htmlen(xivo_user::get_info('login')),'</b>',
		'</span>',
		$url->img_html('img/menu/top/login/sep.gif'),
		'<span>',
		$this->bbf('info_top_type'),
		'&nbsp;<b>',$this->bbf('usr_type__'.xivo_user::get_info('meta')),'</b>',
		'</span>',
		$url->href_html($url->img_html('img/menu/top/login/bt-logoff.gif',
					       $this->bbf('mn_top_logoff'),
					       'border="0"'),
				'xivo/logoff',
				null,
				null,
				$this->bbf('mn_top_logoff'));
?>
</h1>
