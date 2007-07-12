	<div class="sb-smenu">
		<ul>
			<li id="smenu-tab-1" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-general');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-general'); return(false);"><?=$this->bbf('smenu_general');?></a></span></div><span class="span-right">&nbsp;</span>
			</li>
			<li id="smenu-tab-2" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-queue');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
				<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-queue'); return(false);"><?=$this->bbf('smenu_queues');?></a></span></div><span class="span-right">&nbsp;</span>
			</li>
			<li id="smenu-tab-3" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-advanced',1);" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
				<div><span class="span-center"><a href="#" onclick="xivo_smenu_click(this,'moc','sb-part-advanced'); return(false);"><?=$this->bbf('smenu_advanced');?></a></span></div><span class="span-right">&nbsp;</span>
			</li>
		</ul>
	</div>
