<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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
<html lang="<?=DWHO_I18N_BABELFISH_LANGUAGE?>">
	<head id="t-head">
		<title><?=dwho_htmlsc($this->bbf('page_title',php_uname('n')));?></title>

		<script type="text/javascript">
			var dwho_i18n_lang		= '<?=$dhtml->escape(DWHO_I18N_BABELFISH_LANGUAGE);?>';
			var dwho_sess_name		= '<?=$dhtml->escape(DWHO_SESS_NAME);?>';
			var dwho_sess_id		= '<?=$dhtml->escape(DWHO_SESS_ID);?>';
			var dwho_sess_str		= '<?=$dhtml->escape(DWHO_SESS_STR);?>';
			var dwho_location_root_url	= '<?=$dhtml->escape(DWHO_LOCATION_ROOT_URL);?>';
			var dwho_location_app_path	= '<?=$dhtml->escape(dwho_constant('DWHO_LOCATION_APP_PATH'));?>';
			var dwho_form_class_onfocus	= 'it-mfocus';
			var dwho_form_class_onblur	= 'it-mblur';
			var dwho_form_class_error	= 'fm-error';
			var dwho_form_class_enabled	= 'it-enabled';
			var dwho_form_class_disabled	= 'it-disabled';
			var dwho_form_class_readonly	= 'it-readonly';

			document.title			= '<?=$dhtml->escape($this->bbf('page_title',php_uname('n')));?>';
			var xivo_tooltips		= '&nbsp;';
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
