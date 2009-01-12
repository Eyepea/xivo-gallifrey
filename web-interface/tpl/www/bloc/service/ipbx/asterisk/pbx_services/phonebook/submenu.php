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

?>
	<div class="sb-smenu">
		<ul>
			<li id="smenu-tab-1" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-first');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
				<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_general');?></a></span></div><span class="span-right">&nbsp;</span>
			</li>
			<li id="smenu-tab-2" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-office');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
				<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_office');?></a></span></div><span class="span-right">&nbsp;</span>
			</li>
			<li id="smenu-tab-3" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-home');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
				<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_home');?></a></span></div><span class="span-right">&nbsp;</span>
			</li>
			<li id="smenu-tab-4" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-last',1);" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
				<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_other');?></a></span></div><span class="span-right">&nbsp;</span>
			</li>
		</ul>
	</div>
