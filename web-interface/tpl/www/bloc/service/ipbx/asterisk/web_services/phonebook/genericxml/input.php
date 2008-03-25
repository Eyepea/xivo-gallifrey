<?php

$xmlphone = &$this->get_module('xmlphone',array('vendor' => $this->get_var('vendor')));
$taginput = $xmlphone->get_tag('input');

?>
<<?=$taginput?>>
	<Title><?=$xmlphone->escape($this->bbf('phone_search-title'));?></Title>
	<Prompt><?=$xmlphone->escape($this->bbf('phone_search-prompt'));?></Prompt>
	<URL><?=$this->url('service/ipbx/web_services/phonebook/search',true);?></URL>
	<InputItem>
		<DisplayName><?=$xmlphone->escape($this->bbf('phone_search-prompt'));?></DisplayName>
		<QueryStringParam>name</QueryStringParam>
		<DefaultValue />
		<InputFlags>a</InputFlags>
	</InputItem>
</<?=$taginput?>>
