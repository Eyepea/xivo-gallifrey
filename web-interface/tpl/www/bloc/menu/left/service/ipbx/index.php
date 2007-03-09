<?php
	$url = &$this->get_module('url');

	echo '<dl><dt><span class="span-left">&nbsp;</span><span class="span-center">IPBX</span><span class="span-right">&nbsp;</span></dt><dd><dl>';

	if($this->chk_policy('settings') === true):
		echo '<dt>',$this->bbf('mn_ti_settings'),'</dt>';
		echo '<dd>',$url->href_html($this->bbf('mn_local_extension'),'service/ipbx','cat=list'),'</dd>';
	endif;
	
	if($this->chk_policy('call_management') === true):
		echo '<dt>',$this->bbf('mn_ti_call_management'),'</dt>';
		if($this->chk_policy('call_management','ivr') === true):
			echo '<dd>',$this->bbf('mn_ivr'),'</dd>';
		endif;
		if($this->chk_policy('call_management','cdr') === true):
			echo '<dd>',$this->bbf('mn_cdr'),'</dd>';
		endif;
	endif;

	echo '</dl></dd><dd class="b-nosize"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></dd></dl>';
?>
