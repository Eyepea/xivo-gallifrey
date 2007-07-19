<meta http-equiv="content-type" content="text/html; charset=utf-8">
<meta http-equiv="content-language" content="<?=xivo_user::get_infos('language');?>">

<meta name="keywords" content="">
<meta name="description" lang="<?=xivo_user::get_infos('language');?>" content="">
<meta name="author" content="XIVO">
<meta name="publisher" content="XIVO">
<meta name="classification" content="Communication">
<meta name="distribution" content="global">
<meta name="revisit-after" content="5days">
<meta name="copyright" content="Copyright <?=strftime('%Y');?> Xivo">
<meta name="title" content="Xivo">

<link rel="shortcut icon" href="<?=$this->url('favicon.ico');?>">

<link rel="stylesheet" type="text/css" href="<?=$this->file_time($this->url('css/xivo.css'));?>">
<script type="text/javascript" src="<?=$this->file_time($this->url('js/xivo.js'));?>"></script>

<?php
	$dhtml = &$this->get_module('dhtml');

	echo $dhtml->mk_js(),"\n",$dhtml->mk_css();
?>
