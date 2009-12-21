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

$form = &$this->get_module('form');
$dhtml = &$this->get_module('dhtml');

$step = $this->get_var('step');
$msg = dwho_has_len($this->get_var('wz-message')) ? $this->get_var('wz-message') : '&nbsp;';

?>
<div class="b-infos">
	<h3 class="sb-top xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">
			<!-- ?=$this->bbf('title_content_name');?> -->
			<?=$this->bbf('wz-title')?>
		</span>
		<span class="span-right">&nbsp;</span>
	</h3>
	<div class="sb-content">
		<form action="#" method="post" enctype="multipart/form-data" accept-charset="utf-8">
			<div class="b-field">
				<div id="wz-message"><?=$msg?></div>
<?php
		$this->file_include('bloc/wizard/nav');
		$this->file_include('bloc/wizard/'.$step);
?>
			</div>
		</form>
	</div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>
<script type="text/javascript">
dwho.dom.set_onload(function ()
{
	dwho.dom.add_cssclass(dwho_eid('mn-wizard--<?=$dhtml->escape($step);?>'),
								   'wz-active');
});
</script>
