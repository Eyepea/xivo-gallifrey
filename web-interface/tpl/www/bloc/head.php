<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

$dhtml = &$this->get_module('dhtml');

?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html lang="<?=XIVO_I18N_BABELFISH_LANGUAGE?>">
	<head id="t-head">
		<title><?=xivo_htmlsc($this->bbf('page_title',php_uname('n')));?></title>

		<script type="text/javascript">
			document.title = '<?=$dhtml->escape($this->bbf('page_title',php_uname('n')));?>';
			var xivo_i18n_lang = '<?=$dhtml->escape(XIVO_I18N_BABELFISH_LANGUAGE);?>';
			var xivo_sess_name = '<?=$dhtml->escape(XIVO_SESS_NAME);?>';
			var xivo_sess_id = '<?=$dhtml->escape(XIVO_SESS_ID);?>';
			var xivo_sess_str = '<?=$dhtml->escape(XIVO_SESS_STR);?>';
			var xivo_script_root = '<?=$dhtml->escape($this->get_option('script_root'));?>';
			var xivo_api_path_info = '<?=$dhtml->escape($this->get_option('api_path_info'));?>';
			var xivo_tooltips = '&nbsp;';
			var xivo_fm_onfocus_class = 'it-mfocus';
			var xivo_fm_onblur_class = 'it-mblur';
			var xivo_fm_error_class = 'fm-error';
			var xivo_fm_enabled_class = 'it-enabled';
			var xivo_fm_disabled_class = 'it-disabled';
			var xivo_fm_readonly_class = 'it-readonly';
		</script>
		<!-- Date: <?=gmstrftime('%Y-%m-%d %H:%M:%S %Z');?> -->

<?php

$this->file_include('bloc/meta');

$dhtml->load_css();
$dhtml->load_js();

?>
	</head>
	<body>
	<a style="display: none;" name="xivo-free-focus"></a>
	<a name="xivo-top"></a>
