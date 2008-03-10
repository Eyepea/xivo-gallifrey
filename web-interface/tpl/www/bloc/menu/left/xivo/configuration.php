<?php
	$url = &$this->get_module('url');
?>
	<dl><dt><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('mn_left_name');?></span><span class="span-right">&nbsp;</span></dt><dd><dl>

	<dt><?=$this->bbf('mn_left_ti_manage');?></dt>
	<dd id="mn-manage--user"><?=$url->href_html($this->bbf('mn_left_manage-user'),'xivo/configuration/manage/user','act=list');?></dd>
	<dd id="mn-manage--server"><?=$url->href_html($this->bbf('mn_left_manage-server'),'xivo/configuration/manage/server','act=list');?></dd>
	<dd id="mn-manage--ldapserver"><?=$url->href_html($this->bbf('mn_left_manage-ldapserver'),'xivo/configuration/manage/ldapserver','act=list');?></dd>
	<dd id="mn-manage--entity"><?=$url->href_html($this->bbf('mn_left_manage-entity'),'xivo/configuration/manage/entity','act=list');?></dd>

	</dl></dd><dd class="b-nosize"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></dd></dl>
