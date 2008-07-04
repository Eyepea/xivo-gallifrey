	<div class="sb-smenu">
		<ul>
			<li id="smenu-tab-1" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-first');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
				<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_general');?></a></span></div><span class="span-right">&nbsp;</span>
			</li>
			<li id="smenu-tab-2" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-user');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
				<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_users');?></a></span></div><span class="span-right">&nbsp;</span>
			</li>
			<li id="smenu-tab-3" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-rightcall');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
				<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_rightcalls');?></a></span></div><span class="span-right">&nbsp;</span>
			</li>
			<li id="smenu-tab-4" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-last',1);" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
				<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_dialaction');?></a></span></div><span class="span-right">&nbsp;</span>
			</li>
		</ul>
	</div>
