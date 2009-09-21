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

?>
<div class="sb-smenu">
	<ul>
		<li id="smenu-tab-1"
		    class="moo"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div onclick="xivo_smenu_click(xivo_eid('smenu-tab-1'),'moc','sb-part-first');">
				<div class="tab">
					<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_general');?></a></span>
				</div>
				<span class="span-right">&nbsp;</span>
			</div>
			<div class="stab">
				<ul>
					<li><a href="#"
					       onclick="xivo_smenu_click(xivo_eid('smenu-tab-1'),'moc','sb-part-voicemail');
							return(false);"><?=$this->bbf('smenu_voicemail');?></a></li>
					<li><a href="#"
					       onclick="xivo_smenu_click(xivo_eid('smenu-tab-1'),'moc','sb-part-dialaction');
							return(false);"><?=$this->bbf('smenu_dialaction');?></a></li>
					<li><a href="#"
					       onclick="xivo_smenu_click(xivo_eid('smenu-tab-1'),'moc','sb-part-service');
							return(false);"><?=$this->bbf('smenu_services');?></a></li>
				</ul>
			</div>
		</li>
		<li id="smenu-tab-2"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-signalling');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_signalling');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-group');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_groups');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-4"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-autoprov');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_autoprov');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-5"
		    class="moo-last"
		    onmouseout="xivo_smenu_out(this,'moo',1);"
		    onmouseover="xivo_smenu_over(this,'mov',1);">
			<div onclick="xivo_smenu_click(xivo_eid('smenu-tab-5'),'moc','sb-part-last',1);">
				<div class="tab">
					<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_advanced');?></a></span>
				</div>
				<span class="span-right">&nbsp;</span>
			</div>
			<div class="stab">
				<ul>
					<li><a href="#"
					       onclick="xivo_smenu_click(xivo_eid('smenu-tab-5'),'moc','sb-part-rightcall',1);
							return(false);"><?=$this->bbf('smenu_rightcalls');?></a></li>
					<li><a href="#"
					       onclick="xivo_smenu_click(xivo_eid('smenu-tab-5'),'moc','sb-part-t38',1);
							return(false);"><?=$this->bbf('smenu_t38');?></a></li>
				</ul>
			</div>
		</li>
	</ul>
</div>
