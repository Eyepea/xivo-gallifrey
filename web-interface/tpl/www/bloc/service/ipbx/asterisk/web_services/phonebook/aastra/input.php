<?php

$xmlphone = &$this->get_module('xmlphone',array('vendor' => $this->get_var('vendor')));
$taginput = $xmlphone->get_tag('input');

?>
<<?=$taginput?> type="string" editable="yes">
	<Title><?=$xmlphone->escape($this->bbf('phone_search-title'));?></Title>
	<Prompt><?=$xmlphone->escape($this->bbf('phone_search-prompt'));?></Prompt>
	<URL><?=$xmlphone->escape($this->url('service/ipbx/web_services/phonebook/search',true));?></URL>
	<Parameter>name</Parameter>
</<?=$taginput?>>
