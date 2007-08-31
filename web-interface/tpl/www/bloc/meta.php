<meta http-equiv="content-type" content="text/html; charset=utf-8">
<meta http-equiv="content-language" content="<?=xivo_user::get_infos('language');?>">

<meta name="charset" content="utf-8">
<meta name="robots" content="none">
<meta name="author" content="<?=XIVO_CORP_LABEL?>">
<meta name="publisher" content="<?=XIVO_CORP_LABEL?>">
<meta name="distribution" content="iu">
<meta name="copyright" content="Copyright <?=strftime('%Y'),' ',XIVO_CORP_LABEL?>">
<meta name="title" content="<?=$this->bbf('html_title');?>">

<link rel="icon" href="<?=$this->file_time($this->url('favicon.ico'));?>">
<link rel="shortcut icon" href="<?=$this->file_time($this->url('favicon.ico'));?>">

<link rel="stylesheet" type="text/css" href="<?=$this->file_time($this->url('css/xivo.css'));?>">
<script type="text/javascript" src="<?=$this->file_time($this->url('js/xivo.js'));?>"></script>
<script type="text/javascript" src="<?=$this->file_time($this->url('js/xivo_form.js'));?>"></script>

<?php
	$dhtml = &$this->get_module('dhtml');

	echo $dhtml->mk_js(),"\n",$dhtml->mk_css();
?>
