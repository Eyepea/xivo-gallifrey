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

$form = &$this->get_module('form');

?>
<div id="sr-agentgroup" class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-smenu">
		<ul>
			<li id="dwsm-tab-1"
				class="dwsm-blur"
				onclick="dwho_submenu.select(this,'sb-part-first');"
				onmouseout="dwho_submenu.blur(this);"
				onmouseover="dwho_submenu.focus(this);">
				<div class="tab">
					<span class="span-center">
						<a href="#" onclick="return(false);"><?=$this->bbf('smenu_general');?></a>
					</span>
				</div>
				<span class="span-right">&nbsp;</span>
			</li>
		   <li id="dwsm-tab-2"
				class="dwsm-blur"
				onclick="dwho_submenu.select(this,'sb-part-prefs');"
				onmouseout="dwho_submenu.blur(this);"
				onmouseover="dwho_submenu.focus(this);">
				<div class="tab">
					<span class="span-center">
						<a href="#" onclick="return(false);"><?=$this->bbf('smenu_prefs');?></a>
					</span>
				</div>
				<span class="span-right">&nbsp;</span>
			</li>
			<li id="dwsm-tab-3"
				class="dwsm-blur-last"
				onclick="dwho_submenu.select(this,'sb-part-last',1);"
				onmouseout="dwho_submenu.blur(this,1);"
				onmouseover="dwho_submenu.focus(this,1);">
				<div class="tab">
					<span class="span-center">
						<a href="#" onclick="return(false);"><?=$this->bbf('smenu_xlets');?></a>
					</span>
				</div>
				<span class="span-right">&nbsp;</span>
			</li>
		</ul>
	</div>

	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="dwho.form.select('it-services'); dwho.form.select('it-funcs');">

<?=$form->hidden(array('name' => DWHO_SESS_NAME,'value' => DWHO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => 'add'));?>
<?=$form->hidden(array('name' => 'fm_send','value' => 1));?>
<?=$form->hidden(array('name' => 'idprofiles','value' => $this->get_var('idprofiles')));?>

<?php
	$this->file_include('bloc/cti/profiles/form');
?>

<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>

</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
