<?php
	$url = &$this->get_module('url');

	echo '<dl><dt><span class="span-left">&nbsp;</span><span class="span-center">'.$this->bbf('mn_left_name').'</span><span class="span-right">&nbsp;</span></dt><dd><dl>';

	if($this->chk_acl('general_settings') === true):
		echo '<dt>',$this->bbf('mn_left_ti_generalsettings'),'</dt>';
		if($this->chk_acl('general_settings','sip') === true):
			echo '<dd id="mn-general-settings--sip">',$url->href_html($this->bbf('mn_left_generalsettings-sip'),'service/ipbx/general_settings/sip'),'</dd>';
		endif;
		if($this->chk_acl('general_settings','iax') === true):
			echo '<dd id="mn-general-settings--iax">',$url->href_html($this->bbf('mn_left_generalsettings-iax'),'service/ipbx/general_settings/iax'),'</dd>';
		endif;
		if($this->chk_acl('general_settings','voicemail') === true):
			echo '<dd id="mn-general-settings--voicemail">',$url->href_html($this->bbf('mn_left_generalsettings-voicemail'),'service/ipbx/general_settings/voicemail'),'</dd>';
		endif;
		if($this->chk_acl('general_settings','advanced') === true):
			echo '<dd id="mn-general-settings--advanced">',$url->href_html($this->bbf('mn_left_generalsettings-advanced'),'service/ipbx/general_settings/advanced'),'</dd>';
		endif;
	endif;

	if($this->chk_acl('pbx_settings') === true):
		echo '<dt>',$this->bbf('mn_left_ti_pbxsettings'),'</dt>';
		if($this->chk_acl('pbx_settings','agents') === true):
			echo '<dd id="mn-pbx-settings--agents">',$url->href_html($this->bbf('mn_left_pbxsettings-agents'),'service/ipbx/pbx_settings/agents','act=list'),'</dd>';
		endif;
		if($this->chk_acl('pbx_settings','users') === true):
			echo '<dd id="mn-pbx-settings--users">',$url->href_html($this->bbf('mn_left_pbxsettings-users'),'service/ipbx/pbx_settings/users','act=list'),'</dd>';
		endif;
		if($this->chk_acl('pbx_settings','groups') === true):
			echo '<dd id="mn-pbx-settings--groups">',$url->href_html($this->bbf('mn_left_pbxsettings-groups'),'service/ipbx/pbx_settings/groups','act=list'),'</dd>';
		endif;
		if($this->chk_acl('pbx_settings','queues') === true):
			echo '<dd id="mn-pbx-settings--queues">',$url->href_html($this->bbf('mn_left_pbxsettings-queues'),'service/ipbx/pbx_settings/queues','act=list'),'</dd>';
		endif;
		if($this->chk_acl('pbx_settings','meetme') === true):
			echo '<dd id="mn-pbx-settings--meetme">',$url->href_html($this->bbf('mn_left_pbxsettings-meetme'),'service/ipbx/pbx_settings/meetme','act=list'),'</dd>';
		endif;
	endif;
	
	if($this->chk_acl('call_management') === true):
		echo '<dt>',$this->bbf('mn_left_ti_callmanagement'),'</dt>';
		if($this->chk_acl('call_management','incall') === true):
			echo '<dd id="mn-call-management--incall">',$url->href_html($this->bbf('mn_left_callmanagement-incall'),'service/ipbx/call_management/incall','act=list'),'</dd>';
		endif;
		if($this->chk_acl('call_management','outcall') === true):
			echo '<dd id="mn-call-management--outcall">',$url->href_html($this->bbf('mn_left_callmamagement-outcall'),'service/ipbx/call_management/outcall','act=list'),'</dd>';
		endif;
		if($this->chk_acl('call_management','rightcall') === true):
			echo '<dd id="mn-call-management--rightcall">',$url->href_html($this->bbf('mn_left_callmamagement-rightcall'),'service/ipbx/call_management/rightcall','act=list'),'</dd>';
		endif;
		if($this->chk_acl('call_management','schedule') === true):
			echo '<dd id="mn-call-management--schedule">',$url->href_html($this->bbf('mn_left_callmamagement-schedule'),'service/ipbx/call_management/schedule','act=list'),'</dd>';
		endif;
		if($this->chk_acl('call_management','ivr') === true):
			echo '<dd id="mn-call-management--ivr"><a href="#">',$this->bbf('mn_left_callmanagement-ivr'),'</a></dd>';
		endif;
		if($this->chk_acl('call_management','cdr') === true):
			echo '<dd id="mn-call-management--cdr">',$url->href_html($this->bbf('mn_left_callmamagement-cdr'),'service/ipbx/call_management/cdr'),'</dd>';
		endif;
	endif;

	if($this->chk_acl('trunk_management') === true):
		echo '<dt>',$this->bbf('mn_left_ti_trunkmanagement'),'</dt>';
		if($this->chk_acl('trunk_management','sip') === true):
			echo '<dd id="mn-trunk-management--sip">',$url->href_html($this->bbf('mn_left_trunkmanagement-sip'),'service/ipbx/trunk_management/sip','act=list'),'</dd>';
		endif;
		if($this->chk_acl('trunk_management','iax') === true):
			echo '<dd id="mn-trunk-management--iax">',$url->href_html($this->bbf('mn_left_trunkmanagement-iax'),'service/ipbx/trunk_management/iax','act=list'),'</dd>';
		endif;
		if($this->chk_acl('trunk_management','custom') === true):
			echo '<dd id="mn-trunk-management--custom">',$url->href_html($this->bbf('mn_left_trunkmanagement-custom'),'service/ipbx/trunk_management/custom','act=list'),'</dd>';
		endif;
	endif;

	if($this->chk_acl('pbx_services') === true):
		echo '<dt>',$this->bbf('mn_left_ti_pbxservices'),'</dt>';
		if($this->chk_acl('pbx_services','sounds') === true):
			echo '<dd id="mn-pbx-services--sounds">',$url->href_html($this->bbf('mn_left_pbx_services-sounds'),'service/ipbx/pbx_services/sounds','act=list'),'</dd>';
		endif;
		if($this->chk_acl('pbx_services','musiconhold') === true):
			echo '<dd id="mn-pbx-services--musiconhold">',$url->href_html($this->bbf('mn_left_pbx_services-musiconhold'),'service/ipbx/pbx_services/musiconhold','act=list'),'</dd>';
		endif;
		if($this->chk_acl('pbx_services','extenfeatures') === true):
			echo '<dd id="mn-pbx-services--extenfeatures">',$url->href_html($this->bbf('mn_left_pbx_services-extenfeatures'),'service/ipbx/pbx_services/extenfeatures','act=list'),'</dd>';
		endif;
		if($this->chk_acl('pbx_services','outcall') === true):
			echo '<dd id="mn-pbx-services--outcall">',$url->href_html($this->bbf('mn_left_pbx_services-outcall'),'service/ipbx/pbx_services/outcall','act=list'),'</dd>';
		endif;
		if($this->chk_acl('pbx_services','phonebook') === true):
			echo '<dd id="mn-pbx-services--phonebook">',$url->href_html($this->bbf('mn_left_pbx_services-phonebook'),'service/ipbx/pbx_services/phonebook','act=list'),'</dd>';
		endif;
	endif;

	if($this->chk_acl('system_management') === true):
		echo '<dt>',$this->bbf('mn_left_ti_systemmanagement'),'</dt>';
		if($this->chk_acl('system_management','configfiles') === true):
			echo '<dd id="mn-system-management--configfiles">',$url->href_html($this->bbf('mn_left_systemmanagement-configfiles'),'service/ipbx/system_management/configfiles','act=list'),'</dd>';
		endif;
		if($this->chk_acl('system_management','isdncard') === true):
			echo '<dd id="mn-system-management--isdncard">',$url->href_html($this->bbf('mn_left_systemmanagement-systemhardware'),'service/ipbx/system_management/isdncard','act=list'),'</dd>';
		endif;
		if($this->chk_acl('system_management','modules') === true):
			echo '<dd id="mn-system-management--modules">',$url->href_html($this->bbf('mn_left_systemmanagement-modulessettings'),'service/ipbx/system_management/modules','act=list'),'</dd>';
		endif;
		if($this->chk_acl('system_management','manager') === true):
			echo '<dd id="mn-system-management--manager">',$url->href_html($this->bbf('mn_left_systemmanagement-manager'),'service/ipbx/system_management/manager','act=list'),'</dd>';
		endif;
	endif;

	if($this->chk_acl('control_system') === true):
		echo '<dt>',$this->bbf('mn_left_ti_controlsystem'),'</dt>';
		if($this->chk_acl('control_system','reload') === true):
			echo '<dd id="mn-control-system--reload">',$url->href_html($this->bbf('mn_left_controlsystem-ipbxreload',XIVO_SRE_IPBX_LABEL),'service/ipbx/control_system/reload'),'</dd>';
		endif;
		if($this->chk_acl('control_system','restart') === true):
			echo '<dd id="mn-control-system--restart">',$url->href_html($this->bbf('mn_left_controlsystem-ipbxrestart',XIVO_SRE_IPBX_LABEL),'service/ipbx/control_system/restart'),'</dd>';
		endif;
	endif;


	echo '</dl></dd><dd class="b-nosize"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></dd></dl>';
?>
