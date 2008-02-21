<?php
	header('Content-Type: text/xml; charset=utf-8');
	$taginput = $this->get_var('taginput');
?>
<<?=$taginput?>>
	<Title><?=$this->bbf('phone_menu');?></Title>
	<Prompt><?=$this->bbf('phone_prompt');?></Prompt>
	<URL><?=$this->url('service/ipbx/web_services/phonebook/search',true);?></URL>
	<InputItem>
		<DisplayName><?=$this->bbf('phone_search');?></DisplayName>
		<QueryStringParam>name</QueryStringParam>
		<DefaultValue />
		<InputFlags>a</InputFlags>
	</InputItem>
</<?=$taginput?>>
