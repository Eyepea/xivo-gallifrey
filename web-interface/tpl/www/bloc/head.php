<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html lang="<?=xivo_user::get_infos('language');?>">
	<head>
		<title><?=$this->bbf('html_title');?></title>

		<script type="text/javascript">
			document.title = '<?=$this->bbf('html_title');?>';
			var xivo_i18n_lang = '<?=xivo_user::get_infos('language');?>';
			var xivo_sess_name = '<?=XIVO_SESS_NAME?>';
			var xivo_sess_id = '<?=XIVO_SESS_ID?>';
			var xivo_sess_str = '<?=XIVO_SESS_STR?>';
			var xivo_ctrl_path_info = '<?=$this->get_option('ctrl_path_info');?>';
		</script>
		<!-- Date: <?=strftime('%Y-%m-%d %H:%M')?> -->
<?php
	$this->file_include('bloc/meta');

	$dhtml = &$this->get_module('dhtml');

	$dhtml->load_css();
	$dhtml->load_js();
?>
	</head>
	<body>
