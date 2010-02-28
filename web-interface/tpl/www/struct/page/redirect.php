<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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

$url = &$this->get_module('url');

$seconds = $this->get_var('redirect_seconds');

?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html lang="<?=DWHO_I18N_BABELFISH_LANGUAGE?>">
	<head id="t-head">
		<title><?=dwho_htmlsc($this->bbf('page_title',php_uname('n')));?></title>
		<!-- Date: <?=gmstrftime('%Y-%m-%d %H:%M:%S %Z');?> -->

		<meta http-equiv="refresh" content="<?=$seconds?>;
						    url=<?=$url->href($this->get_var('redirect_url'),
								      $this->get_var('redirect_url_query'),
								      true,
								      null,
								      true,
								      false);?>" />
	</head>
	<body>
		<p><?=nl2br($this->bbf('redirect_message',$seconds));?></p>
	</body>
</html>
