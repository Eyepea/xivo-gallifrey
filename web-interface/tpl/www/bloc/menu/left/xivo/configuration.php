<?php
	$url = &$this->get_module('url');
?>
	<dl><dt><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('mn_left_name');?></span><span class="span-right">&nbsp;</span></dt><dd><dl>

	<dt><?=$this->bbf('mn_left_ti_manage');?></dt>
	<dd id="mn-manage--users"><?=$url->href_html($this->bbf('mn_left_manage-users'),'xivo/configuration/users','act=list');?></dd>
	<dd id="mn-manage--servers"><?=$url->href_html($this->bbf('mn_left_manage-servers'),'xivo/configuration/servers','act=list');?></dd>

	</dl></dd><dd class="b-nosize"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></dd></dl>
