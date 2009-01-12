<?php

#
# XiVO Web-Interface
# Copyright (C) 2006, 2007, 2008  Proformatique <technique@proformatique.com>
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

$menu = &$this->get_module('menu');
$this->file_include('bloc/head');

?>
<div id="bc-body">

<div id="bc-head">
	<div id="b-tmenu">
<?php
	$menu->mk_top();
?>
	</div>
</div>
<div id="bc-main">
	<div id="b-lmenu">
<?php
	$menu->mk_left();
?>
	</div>
	<div id="bc-content">
		<div id="b-content">
<?php
	$this->mk_struct();
?>
		</div>
	</div>
</div>

<div id="bc-foot">
	<div id="b-bmenu">
<?php
	$menu->mk_bottom();
?>
	</div>
</div>
</div>
<?php

$this->file_include('bloc/foot');

?>
