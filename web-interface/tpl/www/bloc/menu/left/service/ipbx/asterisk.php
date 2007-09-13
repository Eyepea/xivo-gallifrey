<?php
	$url = &$this->get_module('url');

	echo '<dl><dt><span class="span-left">&nbsp;</span><span class="span-center">'.$this->bbf('mn_left_name').'</span><span class="span-right">&nbsp;</span></dt><dd><dl>';

	if($this->chk_policy('general_settings') === true):
		echo '<dt>',$this->bbf('mn_left_ti_generalsettings'),'</dt>';
		if($this->chk_policy('general_settings','sip') === true):
			echo '<dd id="mn-general-settings--sip">',$url->href_html($this->bbf('mn_left_generalsettings-sip'),'service/ipbx/general_settings/sip'),'</dd>';
		endif;
		if($this->chk_policy('general_settings','iax') === true):
			echo '<dd id="mn-general-settings--iax">',$url->href_html($this->bbf('mn_left_generalsettings-iax'),'service/ipbx/general_settings/iax'),'</dd>';
		endif;
		if($this->chk_policy('general_settings','voicemail') === true):
			echo '<dd id="mn-general-settings--voicemail">',$url->href_html($this->bbf('mn_left_generalsettings-voicemail'),'service/ipbx/general_settings/voicemail'),'</dd>';
		endif;
		if($this->chk_policy('general_settings','agents') === true):
			echo '<dd id="mn-general-settings--agents">',$url->href_html($this->bbf('mn_left_generalsettings-agents'),'service/ipbx/general_settings/agents'),'</dd>';
		endif;
		if($this->chk_policy('general_settings','queues') === true):
			echo '<dd id="mn-general-settings--queues">',$url->href_html($this->bbf('mn_left_generalsettings-queues'),'service/ipbx/general_settings/queues'),'</dd>';
		endif;
		if($this->chk_policy('general_settings','sounds') === true):
			echo '<dd id="mn-general-settings--sounds">',$url->href_html($this->bbf('mn_left_generalsettings-sounds'),'service/ipbx/general_settings/sounds','act=listdir'),'</dd>';
		endif;
		if($this->chk_policy('general_settings','musiconhold') === true):
			echo '<dd id="mn-general-settings--musiconhold">',$url->href_html($this->bbf('mn_left_generalsettings-musiconhold'),'service/ipbx/general_settings/musiconhold','act=list'),'</dd>';
		endif;
		if($this->chk_policy('general_settings','extenfeatures') === true):
			echo '<dd id="mn-general-settings--extenfeatures">',$url->href_html($this->bbf('mn_left_generalsettings-extenfeatures'),'service/ipbx/general_settings/extenfeatures'),'</dd>';
		endif;
		if($this->chk_policy('general_settings','outcall') === true):
			echo '<dd id="mn-general-settings--outcall">',$url->href_html($this->bbf('mn_left_generalsettings-outcall'),'service/ipbx/general_settings/outcall'),'</dd>';
		endif;
	endif;

	if($this->chk_policy('pbx_settings') === true):
		echo '<dt>',$this->bbf('mn_left_ti_pbxsettings'),'</dt>';
		if($this->chk_policy('pbx_settings','agents') === true):
			echo '<dd id="mn-pbx-settings--agents">',$url->href_html($this->bbf('mn_left_pbxsettings-agents'),'service/ipbx/pbx_settings/agents','act=list'),'</dd>';
		endif;
		if($this->chk_policy('pbx_settings','users') === true):
			echo '<dd id="mn-pbx-settings--users">',$url->href_html($this->bbf('mn_left_pbxsettings-users'),'service/ipbx/pbx_settings/users','act=list'),'</dd>';
		endif;
		if($this->chk_policy('pbx_settings','groups') === true):
			echo '<dd id="mn-pbx-settings--groups">',$url->href_html($this->bbf('mn_left_pbxsettings-groups'),'service/ipbx/pbx_settings/groups','act=list'),'</dd>';
		endif;
		if($this->chk_policy('pbx_settings','queues') === true):
			echo '<dd id="mn-pbx-settings--queues">',$url->href_html($this->bbf('mn_left_pbxsettings-queues'),'service/ipbx/pbx_settings/queues','act=list'),'</dd>';
		endif;
		if($this->chk_policy('pbx_settings','meetme') === true):
			echo '<dd id="mn-pbx-settings--meetme">',$url->href_html($this->bbf('mn_left_pbxsettings-meetme'),'service/ipbx/pbx_settings/meetme','act=list'),'</dd>';
		endif;
	endif;
	
	if($this->chk_policy('call_management') === true):
		echo '<dt>',$this->bbf('mn_left_ti_callmanagement'),'</dt>';
		if($this->chk_policy('call_management','incall') === true):
			echo '<dd id="mn-call-management--incall">',$url->href_html($this->bbf('mn_left_callmanagement-incall'),'service/ipbx/call_management/incall','act=list'),'</dd>';
		endif;
		if($this->chk_policy('call_management','outcall') === true):
			echo '<dd id="mn-call-management--outcall">',$url->href_html($this->bbf('mn_left_callmamagement-outcall'),'service/ipbx/call_management/outcall'),'</dd>';
		endif;
		if($this->chk_policy('call_management','rightcall') === true):
			echo '<dd id="mn-call-management--rightcall">',$url->href_html($this->bbf('mn_left_callmamagement-rightcall'),'service/ipbx/call_management/rightcall'),'</dd>';
		endif;
		if($this->chk_policy('call_management','schedule') === true):
			echo '<dd id="mn-call-management--schedule">',$url->href_html($this->bbf('mn_left_callmamagement-schedule'),'service/ipbx/call_management/schedule'),'</dd>';
		endif;
		if($this->chk_policy('call_management','ivr') === true):
			echo '<dd id="mn-call-management--ivr"><a href="#">',$this->bbf('mn_left_callmanagement-ivr'),'</a></dd>';
		endif;
		if($this->chk_policy('call_management','cdr') === true):
			echo '<dd id="mn-call-management--cdr">',$url->href_html($this->bbf('mn_left_callmamagement-cdr'),'service/ipbx/call_management/cdr'),'</dd>';
		endif;
	endif;

	if($this->chk_policy('trunk_management') === true):
		echo '<dt>',$this->bbf('mn_left_ti_trunkmanagement'),'</dt>';
		if($this->chk_policy('trunk_management','sip') === true):
			echo '<dd id="mn-trunk-management--sip">',$url->href_html($this->bbf('mn_left_trunkmanagement-sip'),'service/ipbx/trunk_management/sip','act=list'),'</dd>';
		endif;
		if($this->chk_policy('trunk_management','iax') === true):
			echo '<dd id="mn-trunk-management--iax">',$url->href_html($this->bbf('mn_left_trunkmanagement-iax'),'service/ipbx/trunk_management/iax','act=list'),'</dd>';
		endif;
		if($this->chk_policy('trunk_management','custom') === true):
			echo '<dd id="mn-trunk-management--custom">',$url->href_html($this->bbf('mn_left_trunkmanagement-custom'),'service/ipbx/trunk_management/custom','act=list'),'</dd>';
		endif;
	endif;

	if($this->chk_policy('system_management') === true):
		echo '<dt>',$this->bbf('mn_left_ti_systemmanagement'),'</dt>';
		if($this->chk_policy('system_management','configfiles') === true):
			echo '<dd id="mn-system-management--configfiles">',$url->href_html($this->bbf('mn_left_systemmanagement-configfiles'),'service/ipbx/system_management/configfiles','act=list'),'</dd>';
		endif;
		if($this->chk_policy('system_management','isdncard') === true):
			echo '<dd id="mn-system-management--isdncard">',$url->href_html($this->bbf('mn_left_systemmanagement-systemhardware'),'service/ipbx/system_management/isdncard','act=list'),'</dd>';
		endif;
		if($this->chk_policy('system_management','modules') === true):
			echo '<dd id="mn-system-management--modules">',$url->href_html($this->bbf('mn_left_systemmanagement-modulessettings'),'service/ipbx/system_management/modules','act=list'),'</dd>';
		endif;
		if($this->chk_policy('system_management','manager') === true):
			echo '<dd id="mn-system-management--manager">',$url->href_html($this->bbf('mn_left_systemmanagement-manager'),'service/ipbx/system_management/manager','act=list'),'</dd>';
		endif;
	endif;

	if($this->chk_policy('control_system') === true):
		echo '<dt>',$this->bbf('mn_left_ti_controlsystem'),'</dt>';
		if($this->chk_policy('control_system','reload') === true):
			echo '<dd id="mn-control-system--reload">',$url->href_html($this->bbf('mn_left_controlsystem-ipbxreload',XIVO_SRE_IPBX_LABEL),'service/ipbx/control_system/reload'),'</dd>';
		endif;
	endif;


	echo '</dl></dd><dd class="b-nosize"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></dd></dl>';
?>
