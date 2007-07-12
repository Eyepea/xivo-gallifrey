<?php
	$url = &$this->get_module('url');

	echo '<dl><dt><span class="span-left">&nbsp;</span><span class="span-center">'.$this->bbf('mn_left_name').'</span><span class="span-right">&nbsp;</span></dt><dd><dl>';

	if($this->chk_policy('general_settings') === true):
		echo '<dt>',$this->bbf('mn_left_ti_generalsettings'),'</dt>';
		if($this->chk_policy('general_settings','sipprotocol') === true):
			echo '<dd id="mn-general-settings--sipprotocol">',$url->href_html($this->bbf('mn_left_sip_protocol'),'service/ipbx/general_settings/sipprotocol'),'</dd>';
		endif;
		if($this->chk_policy('general_settings','iaxprotocol') === true):
			echo '<dd id="mn-general-settings--iaxprotocol">',$url->href_html($this->bbf('mn_left_iax_protocol'),'service/ipbx/general_settings/iaxprotocol'),'</dd>';
		endif;
		if($this->chk_policy('general_settings','voicemail') === true):
			echo '<dd id="mn-general-settings--voicemail">',$url->href_html($this->bbf('mn_left_voicemail'),'service/ipbx/general_settings/voicemail'),'</dd>';
		endif;
		if($this->chk_policy('general_settings','queues') === true):
			echo '<dd id="mn-general-settings--queues">',$url->href_html($this->bbf('mn_left_queues'),'service/ipbx/general_settings/queues'),'</dd>';
		endif;
		if($this->chk_policy('general_settings','agents') === true):
			echo '<dd id="mn-general-settings--agents">',$url->href_html($this->bbf('mn_left_agents'),'service/ipbx/general_settings/agents'),'</dd>';
		endif;
		if($this->chk_policy('general_settings','sounds') === true):
			echo '<dd id="mn-general-settings--sounds">',$url->href_html($this->bbf('mn_left_sounds'),'service/ipbx/general_settings/sounds','act=listdir'),'</dd>';
		endif;
		if($this->chk_policy('general_settings','musiconhold') === true):
			echo '<dd id="mn-general-settings--musiconhold">',$url->href_html($this->bbf('mn_left_musiconhold'),'service/ipbx/general_settings/musiconhold','act=list'),'</dd>';
		endif;
		if($this->chk_policy('general_settings','extenfeatures') === true):
			echo '<dd id="mn-general-settings--extenfeatures">',$url->href_html($this->bbf('mn_left_extenfeatures'),'service/ipbx/general_settings/extenfeatures'),'</dd>';
		endif;
	endif;

	if($this->chk_policy('pbx_settings') === true):
		echo '<dt>',$this->bbf('mn_left_ti_pbxsettings'),'</dt>';
		if($this->chk_policy('pbx_settings','users') === true):
			echo '<dd id="mn-pbx-settings--users">',$url->href_html($this->bbf('mn_left_users'),'service/ipbx/pbx_settings/users','act=list'),'</dd>';
		endif;
		if($this->chk_policy('pbx_settings','groups') === true):
			echo '<dd id="mn-pbx-settings--groups">',$url->href_html($this->bbf('mn_left_groups'),'service/ipbx/pbx_settings/groups','act=list'),'</dd>';
		endif;
		if($this->chk_policy('pbx_settings','meetme') === true):
			echo '<dd id="mn-pbx-settings--meetme">',$url->href_html($this->bbf('mn_left_meetme'),'service/ipbx/pbx_settings/meetme','act=list'),'</dd>';
		endif;
		if($this->chk_policy('pbx_settings','queues') === true):
			echo '<dd id="mn-pbx-settings--queues">',$url->href_html($this->bbf('mn_left_queues'),'service/ipbx/pbx_settings/queues','act=list'),'</dd>';
		endif;
		if($this->chk_policy('pbx_settings','agents') === true):
			echo '<dd id="mn-pbx-settings--agents">',$url->href_html($this->bbf('mn_left_agents'),'service/ipbx/pbx_settings/agents','act=list'),'</dd>';
		endif;
	endif;
	
	if($this->chk_policy('call_management') === true):
		echo '<dt>',$this->bbf('mn_left_ti_call_management'),'</dt>';
		if($this->chk_policy('call_management','did') === true):
			echo '<dd id="mn-call-management--did">',$url->href_html($this->bbf('mn_left_did'),'service/ipbx/call_management/did','act=list'),'</dd>';
		endif;
		if($this->chk_policy('call_management','ivr') === true):
			echo '<dd id="mn-call-management--ivr"><a href="#">',$this->bbf('mn_left_ivr'),'</a></dd>';
		endif;
		if($this->chk_policy('call_management','cdr') === true):
			echo '<dd id="mn-call-management--cdr"><a href="#">',$this->bbf('mn_left_cdr'),'</a></dd>';
		endif;
		if($this->chk_policy('call_management','outcall') === true):
			echo '<dd id="mn-call-management--outcall"><a href="#">',$this->bbf('mn_left_outcall'),'</a></dd>';
		endif;
	endif;

	if($this->chk_policy('trunk_management') === true):
		echo '<dt>',$this->bbf('mn_left_ti_trunk_management'),'</dt>';
		if($this->chk_policy('trunk_management','sip') === true):
			echo '<dd id="mn-trunk-management--sip">',$url->href_html($this->bbf('mn_left_trunk_sip'),'service/ipbx/trunk_management/sip','act=list'),'</dd>';
		endif;
		if($this->chk_policy('trunk_management','iax') === true):
			echo '<dd id="mn-trunk-management--iax">',$url->href_html($this->bbf('mn_left_trunk_iax'),'service/ipbx/trunk_management/iax','act=list'),'</dd>';
		endif;
	endif;

	if($this->chk_policy('system_management') === true):
		echo '<dt>',$this->bbf('mn_left_ti_system_management'),'</dt>';
		if($this->chk_policy('system_management','configfiles') === true):
			echo '<dd id="mn-system-management--configfiles">',$url->href_html($this->bbf('mn_left_config_files'),'service/ipbx/system_management/configfiles','act=list'),'</dd>';
		endif;
		if($this->chk_policy('system_management','isdncard') === true):
			echo '<dd id="mn-system-management--isdncard">',$url->href_html($this->bbf('mn_left_system_hardware'),'service/ipbx/system_management/isdncard','act=list'),'</dd>';
		endif;
		if($this->chk_policy('system_management','modules') === true):
			echo '<dd id="mn-system-management--modules">',$url->href_html($this->bbf('mn_left_modules_settings'),'service/ipbx/system_management/modules','act=list'),'</dd>';
		endif;
		if($this->chk_policy('system_management','manager') === true):
			echo '<dd id="mn-system-management--manager">',$url->href_html($this->bbf('mn_left_manager'),'service/ipbx/system_management/manager','act=list'),'</dd>';
		endif;
	endif;

	if($this->chk_policy('control_system') === true):
		echo '<dt>',$this->bbf('mn_left_ti_control_system'),'</dt>';
		if($this->chk_policy('control_system','reload') === true):
			echo '<dd id="mn-control-system--reload">',$url->href_html($this->bbf('mn_left_ipbx_reload',XIVO_SRE_IPBX_LABEL),'service/ipbx/control_system/reload'),'</dd>';
		endif;
	endif;


	echo '</dl></dd><dd class="b-nosize"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></dd></dl>';
?>
