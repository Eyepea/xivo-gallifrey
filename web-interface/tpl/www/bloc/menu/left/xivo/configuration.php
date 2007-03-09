<?php
	$url = &$this->get_module('url');

	echo '<dl><dt><span class="span-left">&nbsp;</span><span class="span-center">'.$this->bbf('mn_left_name').'</span><span class="span-right">&nbsp;</span></dt><dd><dl>';

	echo '<dt>',$this->bbf('mn_left_ti_manage'),'</dt>';
	echo '<dd>',$url->href_html($this->bbf('mn_left_users'),'xivo/configuration','cat=list'),'</dd>';

	echo '</dl></dd><dd class="b-nosize"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></dd></dl>';
?>
