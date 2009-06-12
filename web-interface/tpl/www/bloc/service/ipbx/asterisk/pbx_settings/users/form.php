<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$info = $this->get_var('info');
$element = $this->get_var('element');

$voicemail_list = $this->get_var('voicemail_list');
$autoprov_list = $this->get_var('autoprov_list');
$agent_list = $this->get_var('agent_list');
$context_list = $this->get_var('context_list');
$profileclient_list = $this->get_var('profileclient_list');
$rightcall = $this->get_var('rightcall');

if(($outcallerid = (string) $info['ufeatures']['outcallerid']) === ''
|| in_array($outcallerid,$element['ufeatures']['outcallerid']['value'],true) === true):
	$outcallerid_custom = false;
else:
	$outcallerid_custom = true;
endif;

if(empty($info['autoprov']) === true || $info['autoprov']['vendor'] === ''):
	$vendormodel = '';
else:
	$vendormodel = $info['autoprov']['vendor'].'.'.$info['autoprov']['model'];
endif;

if(isset($info['protocol']) === true):
	if(xivo_issa('allow',$info['protocol']) === true):
		$allow = $info['protocol']['allow'];
	else:
		$allow = array();
	endif;

	$context = (string) xivo_ak('context',$info['protocol'],true);
	$amaflags = (string) xivo_ak('amaflags',$info['protocol'],true);
	$qualify = (string) xivo_ak('qualify',$info['protocol'],true);
	$host = (string) xivo_ak('host',$info['protocol'],true);
else:
	$allow = array();
	$context = $amaflags = $qualify = $host = '';
endif;

$codec_active = empty($allow) === false;
$host_static = ($host !== '' && $host !== 'dynamic');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_userfeatures_firstname'),
				  'name'	=> 'ufeatures[firstname]',
				  'labelid'	=> 'ufeatures-firstname',
				  'size'	=> 15,
				  'default'	=> $element['ufeatures']['firstname']['default'],
				  'value'	=> $info['ufeatures']['firstname']),
			    'onchange="xivo_ast_chg_user_name();"
			     onfocus="xivo_ast_cpy_user_name(); xivo_fm_set_onfocus(this);"
			     onblur="xivo_ast_chg_user_name(); xivo_fm_set_onblur(this);"'),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_lastname'),
				  'name'	=> 'ufeatures[lastname]',
				  'labelid'	=> 'ufeatures-lastname',
				  'size'	=> 15,
				  'default'	=> $element['ufeatures']['lastname']['default'],
				  'value'	=> $info['ufeatures']['lastname']),
			    'onchange="xivo_ast_chg_user_name();"
			     onfocus="xivo_ast_cpy_user_name(); xivo_fm_set_onfocus(this);"
			     onblur="xivo_ast_chg_user_name(); xivo_fm_set_onblur(this);"'),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_name'),
				  'name'	=> 'protocol[name]',
				  'labelid'	=> 'protocol-name',
				  'size'	=> 15,
				  'value'	=> $info['protocol']['name'])),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_secret'),
				  'name'	=> 'protocol[secret]',
				  'labelid'	=> 'protocol-secret',
				  'size'	=> 15,
				  'value'	=> $this->get_varra('info',array('protocol','secret')))),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_interface'),
				  'name'	=> 'protocol[interface]',
				  'labelid'	=> 'protocol-interface',
				  'size'	=> 15,
				  'default'	=> $element['protocol']['custom']['interface']['default'],
				  'value'	=> $this->get_varra('info',array('protocol','interface')))),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_number'),
				  'name'	=> 'ufeatures[number]',
				  'labelid'	=> 'ufeatures-number',
				  'size'	=> 15,
				  'default'	=> $element['ufeatures']['number']['default'],
				  'value'	=> $info['ufeatures']['number']),
			    'onchange="xivo_ast_chg_user_name();"
			     onfocus="xivo_ast_cpy_user_name(); xivo_fm_set_onfocus(this);"'),

		$form->select(array('desc'	=> $this->bbf('fm_userfeatures_ringseconds'),
				    'name'	=> 'ufeatures[ringseconds]',
				    'labelid'	=> 'ufeatures-ringseconds',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_userfeatures_ringseconds-opt'),
				    'default'	=> $element['ufeatures']['ringseconds']['default'],
				    'value'	=> $info['ufeatures']['ringseconds']),
			      $element['ufeatures']['ringseconds']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_userfeatures_simultcalls'),
				    'name'	=> 'ufeatures[simultcalls]',
				    'labelid'	=> 'ufeatures-simultcalls',
				    'key'	=> false,
				    'default'	=> $element['ufeatures']['simultcalls']['default'],
				    'value'	=> $info['ufeatures']['simultcalls']),
			      $element['ufeatures']['simultcalls']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_protocol'),
				    'name'	=> 'protocol[protocol]',
				    'labelid'	=> 'protocol-protocol',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_protocol_protocol-opt'),
				    'default'	=> $element['ufeatures']['protocol']['default'],
				    'value'	=> $info['ufeatures']['protocol']),
			      $element['ufeatures']['protocol']['value'],
			      'onchange="xivo_ast_chg_user_protocol(this.value); xivo_ast_chg_user_name();"
			       onfocus="xivo_ast_cpy_user_name(); xivo_fm_set_onfocus(this);"');

	if($context_list !== false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_protocol_context'),
					    'name'	=> 'protocol[context]',
					    'labelid'	=> 'protocol-context',
					    'key'	=> 'identity',
					    'altkey'	=> 'name',
					    'value'	=> $context),
				      $context_list);
	else:
		echo	'<div id="fd-protocol-context" class="txt-center">',
			$url->href_html($this->bbf('create_context'),
					'service/ipbx/system_management/context',
					'act=add'),
			'</div>';
	endif;

	if(($moh_list = $this->get_var('moh_list')) !== false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_userfeatures_musiconhold'),
					    'name'	=> 'ufeatures[musiconhold]',
					    'labelid'	=> 'ufeatures-musiconhold',
					    'key'	=> 'category',
					    'empty'	=> true,
					    'invalid'	=> ($this->get_var('act') === 'edit'),
					    'default'	=> ($this->get_var('act') === 'add' ? $element['ufeatures']['musiconhold']['default'] : null),
					    'value'	=> $info['ufeatures']['musiconhold']),
				      $moh_list);
	endif;

	echo	$form->select(array('desc'	=> $this->bbf('fm_protocol_language'),
				    'name'	=> 'protocol[language]',
				    'labelid'	=> 'protocol-language',
				    'key'	=> false,
				    'empty'	=> true,
				    'default'	=> $element['protocol']['sip']['language']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','language'))),
			      $element['protocol']['sip']['language']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_nat'),
				    'name'	=> 'protocol[nat]',
				    'labelid'	=> 'protocol-nat',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('paramkey','fm_protocol_nat-opt'),
				    'default'	=> $element['protocol']['sip']['nat']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','nat'))),
			      $element['protocol']['sip']['nat']['value']);
?>
</div>

<div id="sb-part-voicemail" class="b-nodisplay">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_userfeatures_voicemailid'),
				    'name'	=> 'ufeatures[voicemailid]',
				    'labelid'	=> 'ufeatures-voicemailid',
				    'empty'	=> true,
				    'key'	=> 'identity',
				    'altkey'	=> 'uniqueid',
				    'value'	=> $info['ufeatures']['voicemailid']),
			      $voicemail_list,
			      'onchange="xivo_ast_user_voicemail_selection(this.value);"',
			      array('add'	=> $this->bbf('fm_voicemail-opt-add'))),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail_fullname'),
				  'name'	=> 'voicemail[fullname]',
				  'labelid'	=> 'voicemail-fullname',
				  'size'	=> 15,
				  'default'	=> $element['voicemail']['fullname']['default'],
				  'value'	=> $this->get_varra('voicemail','fullname'))),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail_mailbox'),
				  'name'	=> 'voicemail[mailbox]',
				  'labelid'	=> 'voicemail-mailbox',
				  'size'	=> 10,
				  'default'	=> $element['voicemail']['mailbox']['default'],
				  'value'	=> $this->get_varra('voicemail','mailbox'))),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail_password'),
				  'name'	=> 'voicemail[password]',
				  'labelid'	=> 'voicemail-password',
				  'size'	=> 10,
				  'default'	=> $element['voicemail']['password']['default'],
				  'value'	=> $this->get_varra('voicemail','password'))),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail_email'),
				  'name'	=> 'voicemail[email]',
				  'labelid'	=> 'voicemail-email',
				  'size'	=> 15,
				  'value'	=> $this->get_varra('voicemail','email'),
				  'default'	=> $element['voicemail']['email']['default']));

	if(($tz_list = $this->get_var('tz_list')) !== false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_voicemail_tz'),
					    'name'	=> 'voicemail[tz]',
					    'labelid'	=> 'voicemail-tz',
					    'key'	=> 'name',
					    'default'	=> $element['voicemail']['tz']['default'],
					    'value'	=> $this->get_varra('voicemail','tz')),
				      $tz_list);
	endif;

	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_voicemailfeatures_skipcheckpass'),
				      'name'	=> 'vmfeatures[skipcheckpass]',
				      'labelid'	=> 'vmfeatures-skipcheckpass',
				      'default'	=> $element['vmfeatures']['skipcheckpass']['default'],
				      'checked'	=> $this->get_varra('info',array('vmfeatures','skipcheckpass')))),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail_attach'),
				    'name'	=> 'voicemail[attach]',
				    'labelid'	=> 'voicemail-attach',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['voicemail']['attach']['default'],
				    'value'	=> $this->get_varra('voicemail','attach')),
			      $element['voicemail']['attach']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail_deletevoicemail'),
				      'name'	=> 'voicemail[deletevoicemail]',
				      'labelid'	=> 'voicemail-deletevoicemail',
				      'default'	=> $element['voicemail']['deletevoicemail']['default'],
				      'checked'	=> $this->get_varra('voicemail','deletevoicemail'))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_protocol_subscribemwi'),
				      'name'	=> 'protocol[subscribemwi]',
				      'labelid'	=> 'protocol-subscribemwi',
				      'default'	=> $element['protocol']['sip']['subscribemwi']['default'],
				      'checked'	=> $this->get_varra('info',array('protocol','subscribemwi')))),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_buggymwi'),
				    'name'	=> 'protocol[buggymwi]',
				    'labelid'	=> 'protocol-buggymwi',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['buggymwi']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','buggymwi'))),
			      $element['protocol']['sip']['buggymwi']['value']);
?>
</div>

<div id="sb-part-dialaction" class="b-nodisplay">
	<fieldset id="fld-dialaction-noanswer">
		<legend><?=$this->bbf('fld-dialaction-noanswer');?></legend>
<?php
		$this->file_include('bloc/service/ipbx/asterisk/dialaction/all',
				    array('event'	=> 'noanswer'));
?>
	</fieldset>

	<fieldset id="fld-dialaction-busy">
		<legend><?=$this->bbf('fld-dialaction-busy');?></legend>
<?php
		$this->file_include('bloc/service/ipbx/asterisk/dialaction/all',
				    array('event'	=> 'busy'));
?>
	</fieldset>

	<fieldset id="fld-dialaction-congestion">
		<legend><?=$this->bbf('fld-dialaction-congestion');?></legend>
<?php
		$this->file_include('bloc/service/ipbx/asterisk/dialaction/all',
				    array('event'	=> 'congestion'));
?>
	</fieldset>

	<fieldset id="fld-dialaction-chanunavail">
		<legend><?=$this->bbf('fld-dialaction-chanunavail');?></legend>
<?php
		$this->file_include('bloc/service/ipbx/asterisk/dialaction/all',
				    array('event'	=> 'chanunavail'));
?>
	</fieldset>
</div>

<div id="sb-part-service" class="b-nodisplay">
	<fieldset id="fld-client">
		<legend><?=$this->bbf('fld-client');?></legend>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_enableclient'),
				      'name'	=> 'ufeatures[enableclient]',
				      'labelid'	=> 'ufeatures-enableclient',
				      'default'	=> $element['ufeatures']['enableclient']['default'],
				      'checked'	=> $info['ufeatures']['enableclient']),
				'onchange="xivo_chg_attrib(\'ast_fm_user_enableclient\',
							   \'it-ufeatures-loginclient\',
							   Number(this.checked));"'),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_loginclient'),
				  'name'	=> 'ufeatures[loginclient]',
				  'labelid'	=> 'ufeatures-loginclient',
				  'size'	=> 15,
				  'default'	=> $element['ufeatures']['loginclient']['default'],
				  'value'	=> $info['ufeatures']['loginclient'])),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_passwdclient'),
				  'name'	=> 'ufeatures[passwdclient]',
				  'labelid'	=> 'ufeatures-passwdclient',
				  'size'	=> 15,
				  'default'	=> $element['ufeatures']['passwdclient']['default'],
				  'value'	=> $info['ufeatures']['passwdclient']));

	if(is_array($profileclient_list) === true && empty($profileclient_list) === false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_userfeatures_profileclient'),
					    'name'	=> 'ufeatures[profileclient]',
					    'labelid'	=> 'ufeatures-profileclient',
					    'default'	=> $element['ufeatures']['profileclient']['default'],
					    'value'	=> $info['ufeatures']['profileclient']),
				      $profileclient_list);
	endif;
?>
	</fieldset>

	<fieldset id="fld-services">
		<legend><?=$this->bbf('fld-services');?></legend>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_enablehint'),
				      'name'	=> 'ufeatures[enablehint]',
				      'labelid'	=> 'ufeatures-enablehint',
				      'default'	=> $element['ufeatures']['enablehint']['default'],
				      'checked'	=> $info['ufeatures']['enablehint'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_enablevoicemail'),
				      'name'	=> 'ufeatures[enablevoicemail]',
				      'labelid'	=> 'ufeatures-enablevoicemail',
				      'default'	=> $element['ufeatures']['enablevoicemail']['default'],
				      'checked'	=> $info['ufeatures']['enablevoicemail'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_enablexfer'),
				      'name'	=> 'ufeatures[enablexfer]',
				      'labelid'	=> 'ufeatures-enablexfer',
				      'default'	=> $element['ufeatures']['enablexfer']['default'],
				      'checked'	=> $info['ufeatures']['enablexfer'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_enableautomon'),
				      'name'	=> 'ufeatures[enableautomon]',
				      'labelid'	=> 'ufeatures-enableautomon',
				      'default'	=> $element['ufeatures']['enableautomon']['default'],
				      'checked'	=> $info['ufeatures']['enableautomon'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_callrecord'),
				      'name'	=> 'ufeatures[callrecord]',
				      'labelid'	=> 'ufeatures-callrecord',
				      'default'	=> $element['ufeatures']['callrecord']['default'],
				      'checked'	=> $info['ufeatures']['callrecord'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_callfilter'),
				      'name'	=> 'ufeatures[callfilter]',
				      'labelid'	=> 'ufeatures-callfilter',
				      'default'	=> $element['ufeatures']['callfilter']['default'],
				      'checked'	=> $info['ufeatures']['callfilter'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_enablednd'),
				      'name'	=> 'ufeatures[enablednd]',
				      'labelid'	=> 'ufeatures-enablednd',
				      'default'	=> $element['ufeatures']['enablednd']['default'],
				      'checked'	=> $info['ufeatures']['enablednd'])),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_mobilephonenumber'),
				  'name'	=> 'ufeatures[mobilephonenumber]',
				  'labelid'	=> 'ufeatures-mobilephonenumber',
				  'size'	=> 15,
				  'default'	=> $element['ufeatures']['mobilephonenumber']['default'],
				  'value'	=> $info['ufeatures']['mobilephonenumber'])),

		$form->select(array('desc'	=> $this->bbf('fm_userfeatures_bsfilter'),
				    'name'	=> 'ufeatures[bsfilter]',
				    'labelid'	=> 'ufeatures-bsfilter',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_userfeatures_bsfilter-opt'),
				    'default'	=> $element['ufeatures']['bsfilter']['default'],
				    'value'	=> $info['ufeatures']['bsfilter']),
			      $element['ufeatures']['bsfilter']['value']);

	if($agent_list !== false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_userfeatures_agentid'),
					    'name'	=> 'ufeatures[agentid]',
					    'labelid'	=> 'ufeatures-agentid',
					    'empty'	=> true,
					    'key'	=> 'identity',
					    'altkey'	=> 'id',
					    'default'	=> $element['ufeatures']['agentid']['default'],
					    'value'	=> $info['ufeatures']['agentid']),
				      $agent_list);
	else:
		echo	'<div id="fd-ufeatures-agentid" class="txt-center">',
			$url->href_html($this->bbf('create_agent'),
					'service/ipbx/pbx_settings/agents',
					array('act'	=> 'addagent',
					      'group'	=> 1)),
			'</div>';
	endif;
?>
	</fieldset>

	<fieldset id="fld-callforwards">
		<legend><?=$this->bbf('fld-callforwards');?></legend>
<?php

	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_enablerna'),
				      'name'	=> 'ufeatures[enablerna]',
				      'labelid'	=> 'ufeatures-enablerna',
				      'default'	=> $element['ufeatures']['enablerna']['default'],
				      'checked'	=> $info['ufeatures']['enablerna']),
				'onchange="xivo_chg_attrib(\'ast_fm_user_enablerna\',
							   \'it-ufeatures-destrna\',
							   Number(this.checked));"'),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_destrna'),
				  'name'	=> 'ufeatures[destrna]',
				  'labelid'	=> 'ufeatures-destrna',
				  'size'	=> 15,
				  'default'	=> $element['ufeatures']['destrna']['default'],
				  'value'	=> $info['ufeatures']['destrna'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_enablebusy'),
				      'name'	=> 'ufeatures[enablebusy]',
				      'labelid'	=> 'ufeatures-enablebusy',
				      'default'	=> $element['ufeatures']['enablebusy']['default'],
				      'checked'	=> $info['ufeatures']['enablebusy']),
				'onchange="xivo_chg_attrib(\'ast_fm_user_enablebusy\',
							   \'it-ufeatures-destbusy\',
							   Number(this.checked));"'),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_destbusy'),
				  'name'	=> 'ufeatures[destbusy]',
				  'labelid'	=> 'ufeatures-destbusy',
				  'size'	=> 15,
				  'default'	=> $element['ufeatures']['destbusy']['default'],
				  'value'	=> $info['ufeatures']['destbusy'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_enableunc'),
				      'name'	=> 'ufeatures[enableunc]',
				      'labelid'	=> 'ufeatures-enableunc',
				      'default'	=> $element['ufeatures']['enableunc']['default'],
				      'checked'	=> $info['ufeatures']['enableunc']),
				'onchange="xivo_chg_attrib(\'ast_fm_user_enableunc\',
							   \'it-ufeatures-destunc\',
							   Number(this.checked));"'),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_destunc'),
				  'name'	=> 'ufeatures[destunc]',
				  'labelid'	=> 'ufeatures-destunc',
				  'size'	=> 15,
				  'default'	=> $element['ufeatures']['destunc']['default'],
				  'value'	=> $info['ufeatures']['destunc']));
?>
	</fieldset>
</div>

<div id="sb-part-group" class="b-nodisplay">
<?php
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/groups');
?>
</div>

<div id="sb-part-signalling" class="b-nodisplay">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_protocol_progressinband'),
				    'name'	=> 'protocol[progressinband]',
				    'labelid'	=> 'protocol-progressinband',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('paramkey','fm_protocol_progressinband-opt'),
				    'default'	=> $element['protocol']['sip']['progressinband']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','progressinband'))),
			      $element['protocol']['sip']['progressinband']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_dtmfmode'),
				    'name'	=> 'protocol[dtmfmode]',
				    'labelid'	=> 'protocol-dtmfmode',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('paramkey','fm_protocol_dtmfmode-opt'),
				    'default'	=> $element['protocol']['sip']['dtmfmode']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','dtmfmode'))),
			      $element['protocol']['sip']['dtmfmode']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_rfc2833compensate'),
				    'name'	=> 'protocol[rfc2833compensate]',
				    'labelid'	=> 'protocol-rfc2833compensate',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['rfc2833compensate']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','rfc2833compensate'))),
			      $element['protocol']['sip']['rfc2833compensate']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_qualify'),
				    'name'	=> 'protocol[qualify]',
				    'labelid'	=> 'sip-protocol-qualify',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('paramkey','fm_protocol_qualify-opt'),
				    'default'	=> $element['protocol']['sip']['qualify']['default'],
				    'value'	=> $qualify),
			      $element['protocol']['sip']['qualify']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_qualify'),
				    'name'	=> 'protocol[qualify]',
				    'labelid'	=> 'iax-protocol-qualify',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_protocol_qualify-opt'),
				    'default'	=> $element['protocol']['iax']['qualify']['default'],
				    'value'	=> $qualify),
			      $element['protocol']['iax']['qualify']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_qualifysmoothing'),
				    'name'	=> 'protocol[qualifysmoothing]',
				    'labelid'	=> 'protocol-qualifysmoothing',
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['iax']['qualifysmoothing']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','qualifysmoothing'))),
			      $element['protocol']['iax']['qualifysmoothing']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_qualifyfreqok'),
				    'name'	=> 'protocol[qualifyfreqok]',
				    'labelid'	=> 'protocol-qualifyfreqok',
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_qualifyfreq-opt',
				    'bbf_opt'	=> array('argmode'	=> 'paramkey',
				    			 'time'		=> array(
							 		'from'		=> 'millisecond',
							 		'format'	=> '%M%s%ms')),
				    'default'	=> $element['protocol']['iax']['qualifyfreqok']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','qualifyfreqok'))),
			      $element['protocol']['iax']['qualifyfreqok']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_qualifyfreqnotok'),
				    'name'	=> 'protocol[qualifyfreqnotok]',
				    'labelid'	=> 'protocol-qualifyfreqnotok',
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_qualifyfreq-opt',
				    'bbf_opt'	=> array('argmode'	=> 'paramkey',
				    			 'time'		=> array(
							 		'from'		=> 'millisecond',
							 		'format'	=> '%M%s%ms')),
				    'default'	=> $element['protocol']['iax']['qualifyfreqnotok']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','qualifyfreqnotok'))),
			      $element['protocol']['iax']['qualifyfreqnotok']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_rtptimeout'),
				    'name'	=> 'protocol[rtptimeout]',
				    'labelid'	=> 'protocol-rtptimeout',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('paramkey','fm_protocol_rtptimeout-opt'),
				    'default'	=> $element['protocol']['sip']['rtptimeout']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','rtptimeout'))),
			      $element['protocol']['sip']['rtptimeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_rtpholdtimeout'),
				    'name'	=> 'protocol[rtpholdtimeout]',
				    'labelid'	=> 'protocol-rtpholdtimeout',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('paramkey','fm_protocol_rtpholdtimeout-opt'),
				    'default'	=> $element['protocol']['sip']['rtpholdtimeout']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','rtpholdtimeout'))),
			      $element['protocol']['sip']['rtpholdtimeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_rtpkeepalive'),
				    'name'	=> 'protocol[rtpkeepalive]',
				    'labelid'	=> 'protocol-rtpkeepalive',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('paramkey','fm_protocol_rtpkeepalive-opt'),
				    'default'	=> $element['protocol']['sip']['rtpkeepalive']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','rtpkeepalive'))),
			      $element['protocol']['sip']['rtpkeepalive']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_allowtransfer'),
				    'name'	=> 'protocol[allowtransfer]',
				    'labelid'	=> 'protocol-allowtransfer',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['allowtransfer']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','allowtransfer'))),
			      $element['protocol']['sip']['allowtransfer']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_autoframing'),
				    'name'	=> 'protocol[autoframing]',
				    'labelid'	=> 'protocol-autoframing',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['autoframing']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','autoframing'))),
			      $element['protocol']['sip']['autoframing']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_videosupport'),
				    'name'	=> 'protocol[videosupport]',
				    'labelid'	=> 'protocol-videosupport',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['videosupport']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','videosupport'))),
			      $element['protocol']['sip']['videosupport']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_maxcallbitrate'),
				  'name'	=> 'protocol[maxcallbitrate]',
				  'labelid'	=> 'protocol-maxcallbitrate',
				  'size'	=> 10,
				  'default'	=> $element['protocol']['sip']['maxcallbitrate']['default'],
				  'value'	=> $this->get_varra('info',array('protocol','maxcallbitrate')))),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_g726nonstandard'),
				    'name'	=> 'protocol[g726nonstandard]',
				    'labelid'	=> 'protocol-g726nonstandard',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['g726nonstandard']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','g726nonstandard'))),
			      $element['protocol']['sip']['g726nonstandard']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_jitterbuffer'),
				    'name'	=> 'protocol[jitterbuffer]',
				    'labelid'	=> 'protocol-jitterbuffer',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['iax']['jitterbuffer']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','jitterbuffer'))),
			      $element['protocol']['iax']['jitterbuffer']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_forcejitterbuffer'),
				    'name'	=> 'protocol[forcejitterbuffer]',
				    'labelid'	=> 'protocol-forcejitterbuffer',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['iax']['forcejitterbuffer']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','forcejitterbuffer'))),
			      $element['protocol']['iax']['forcejitterbuffer']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_codecpriority'),
				    'name'	=> 'protocol[codecpriority]',
				    'labelid'	=> 'protocol-codecpriority',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('paramkey','fm_protocol_codecpriority-opt'),
				    'default'	=> $element['protocol']['iax']['codecpriority']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','codecpriority'))),
			      $element['protocol']['iax']['codecpriority']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_codec-custom'),
				      'name'	=> 'codec-active',
				      'labelid'	=> 'codec-active',
				      'checked'	=> $codec_active),
				'onclick="xivo_chg_attrib(\'ast_fm_user_codec\',
							  \'it-protocol-disallow\',
							  Number((this.checked === false)));"'),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_codec-disallow'),
				    'name'	=> 'protocol[disallow]',
				    'labelid'	=> 'protocol-disallow',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_protocol_codec-disallow-opt')),
			      $element['protocol']['sip']['disallow']['value']);
?>

<div id="codeclist" class="fm-field fm-multilist">
	<p>
		<label id="lb-codeclist" for="it-codeclist" onclick="xivo_eid('it-codeclist').focus();">
			<?=$this->bbf('fm_protocol_codec-allow');?>
		</label>
	</p>
	<div class="slt-outlist">
<?php
		echo	$form->select(array('name'	=> 'codeclist',
					    'label'	=> false,
					    'id'	=> 'it-codeclist',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'field'	=> false,
					    'key'	=> false,
					    'bbf'	=> 'ast_codec_name_type-'),
				      $element['protocol']['sip']['allow']['value']);
?>
	</div>

	<div class="inout-list">
		<a href="#"
		   onclick="xivo_fm_move_selected('it-codeclist',
		   				  'it-codec');
			    return(xivo_free_focus());"
		   title="<?=$this->bbf('bt_incodec');?>">
		   	<?=$url->img_html('img/site/button/row-left.gif',
					  $this->bbf('bt_incodec'),
					  'class="bt-inlist" id="bt-incodec" border="0"');?></a><br />
		<a href="#"
		   onclick="xivo_fm_move_selected('it-codec',
		   				  'it-codeclist');
			    return(xivo_free_focus());"
		   title="<?=$this->bbf('bt_outcodec');?>">
			<?=$url->img_html('img/site/button/row-right.gif',
					  $this->bbf('bt_outcodec'),
					  'class="bt-outlist" id="bt-outcodec" border="0"');?></a>
	</div>

	<div class="slt-inlist">
<?php
		echo	$form->select(array('name'	=> 'protocol[allow][]',
					    'label'	=> false,
					    'id'	=> 'it-codec',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'field'	=> false,
					    'key'	=> false,
					    'bbf'	=> 'ast_codec_name_type-'),
				      $allow);
?>
		<div class="bt-updown">
			<a href="#"
			   onclick="xivo_fm_order_selected('it-codec',1);
			   	    return(xivo_free_focus());"
			   title="<?=$this->bbf('bt_upcodec');?>">
			   	<?=$url->img_html('img/site/button/row-up.gif',
						  $this->bbf('bt_upcodec'),
						  'class="bt-uplist" id="bt-upcodec" border="0"');?></a><br />
			<a href="#"
			   onclick="xivo_fm_order_selected('it-codec',-1);
			   	    return(xivo_free_focus());"
			   title="<?=$this->bbf('bt_downcodec');?>">
			   	<?=$url->img_html('img/site/button/row-down.gif',
						  $this->bbf('bt_downcodec'),
						  'class="bt-downlist" id="bt-downcodec" border="0"');?></a>
		</div>
	</div>
</div>
<div class="clearboth"></div>

</div>

<div id="sb-part-autoprov" class="b-nodisplay">
<?php
	if($this->get_var('act') === 'edit'):
		echo	$form->select(array('desc'	=> $this->bbf('fm_autoprov_modact'),
					    'name'	=> 'autoprov[modact]',
					    'labelid'	=> 'autoprov-modact',
					    'bbf'	=> 'fm_autoprov_modact-',
					    'key'	=> false,
					    'empty'	=> true),
				      $element['autoprov']['modact']['value'],
				      'onchange="xivo_chg_attrib(\'ast_fm_user_autoprov-\'+xivo_ast_user_protocol,
				      				 \'it-autoprov-modact\',
								 Number((this.value === \'\')));"');
	endif;

	if(is_array($info['autoprov']) === false
	|| $vendormodel === ''
	|| (int) xivo_ak('iduserfeatures',$info['autoprov'],true) === 0):
		echo	$form->select(array('desc'	=> $this->bbf('fm_autoprov_vendormodel'),
					    'name'	=> 'autoprov[vendormodel]',
					    'labelid'	=> 'autoprov-vendormodel',
					    'optgroup'	=> array('key'	=> 'name'),
					    'empty'	=> true,
					    'key'	=> 'label',
					    'altkey'	=> 'path',
					    'value'	=> $vendormodel),
				      $autoprov_list),
			$form->text(array('desc'	=> $this->bbf('fm_autoprov_macaddr'),
					  'name'	=> 'autoprov[macaddr]',
					  'labelid'	=> 'autoprov-macaddr',
					  'size'	=> 15,
					  'value'	=> $info['autoprov']['macaddr']));
	elseif(isset($autoprov_list[$info['autoprov']['vendor']]) === true):
		echo	'<p id="fd-autoprov-vendormodel" class="fm-field">',
				'<label id="lb-autoprov-vendormodel"><span class="fm-desc">',
				$this->bbf('fm_autoprov_vendormodel'),
				'</span>&nbsp;',
				$autoprov_list[$info['autoprov']['vendor']]['name'],
				' ',
				$autoprov_list[$info['autoprov']['vendor']]['model'][$info['autoprov']['model']]['label'],
				'</label>',
			'</p>',

			'<p id="fd-autoprov-macaddr" class="fm-field">',
				'<label id="lb-autoprov-macaddr"><span class="fm-desc">',
				$this->bbf('fm_autoprov_macaddr'),
				'</span>&nbsp;',
				$info['autoprov']['macaddr'],
				'</label>',
			'</p>';
	endif;

	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey');
?>
</div>

<div id="sb-part-rightcall" class="b-nodisplay">
<?php
	if($rightcall['list'] !== false):
?>
	<div id="rightcalllist" class="fm-field fm-multilist">
		<div class="slt-outlist">
			<?=$form->select(array('name'		=> 'rightcalllist',
					       'label'		=> false,
					       'id'		=> 'it-rightcalllist',
					       'browse'		=> 'rightcall',
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'field'		=> false),
					 $rightcall['list']);?>
		</div>
		<div class="inout-list">
			<a href="#"
			   onclick="xivo_fm_move_selected('it-rightcalllist','it-rightcall');
			            return(xivo_free_focus());"
			   title="<?=$this->bbf('bt_inrightcall');?>">
			   	<?=$url->img_html('img/site/button/row-left.gif',
						  $this->bbf('bt_inrightcall'),
						  'class="bt-inlist" id="bt-inrightcall" border="0"');?></a><br />
			<a href="#"
			   onclick="xivo_fm_move_selected('it-rightcall','it-rightcalllist');
			   	    return(xivo_free_focus());"
			   title="<?=$this->bbf('bt_outrightcall');?>">
			   	<?=$url->img_html('img/site/button/row-right.gif',
						  $this->bbf('bt_outrightcall'),
						  'class="bt-outlist" id="bt-outrightcall" border="0"');?></a>
		</div>
		<div class="slt-inlist">
			<?=$form->select(array('name'		=> 'rightcall[]',
					       'label'		=> false,
					       'id'		=> 'it-rightcall',
					       'browse'		=> 'rightcall',
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'field'		=> false),
					 $rightcall['slt']);?>
		</div>
	</div>
	<div class="clearboth"></div>
<?php
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_rightcall'),
					'service/ipbx/call_management/rightcall',
					'act=add'),
			'</div>';
	endif;
?>
</div>

<div id="sb-part-t38" class="b-nodisplay">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_protocol_t38pt-udptl'),
				    'name'	=> 'protocol[t38pt_udptl]',
				    'labelid'	=> 'protocol-t38pt-udptl',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['t38pt_udptl']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','t38pt_udptl'))),
			      $element['protocol']['sip']['t38pt_udptl']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_t38pt-rtp'),
				    'name'	=> 'protocol[t38pt_rtp]',
				    'labelid'	=> 'protocol-t38pt-rtp',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['t38pt_rtp']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','t38pt_rtp'))),
			      $element['protocol']['sip']['t38pt_rtp']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_t38pt-tcp'),
				    'name'	=> 'protocol[t38pt_tcp]',
				    'labelid'	=> 'protocol-t38pt-tcp',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['t38pt_tcp']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','t38pt_tcp'))),
			      $element['protocol']['sip']['t38pt_tcp']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_t38pt-usertpsource'),
				    'name'	=> 'protocol[t38pt_usertpsource]',
				    'labelid'	=> 'protocol-t38pt-usertpsource',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['t38pt_usertpsource']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','t38pt_usertpsource'))),
			      $element['protocol']['sip']['t38pt_usertpsource']['value']);
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_protocol_callerid'),
				  'name'	=> 'protocol[callerid]',
				  'labelid'	=> 'protocol-callerid',
				  'value'	=> $this->get_varra('info',array('protocol','callerid')),
				  'size'	=> 15,
				  'notag'	=> false)),

		$form->select(array('desc'	=> $this->bbf('fm_userfeatures_outcallerid'),
				    'name'	=> 'ufeatures[outcallerid-type]',
				    'labelid'	=> 'ufeatures-outcallerid-type',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_userfeatures_outcallerid-opt'),
				    'value'	=> ($outcallerid_custom === true ? 'custom' : $outcallerid)),
			      $element['ufeatures']['outcallerid-type']['value'],
			      'onchange="xivo_chg_attrib(\'ast_fm_user_outcallerid\',
			      				 \'fd-ufeatures-outcallerid-custom\',
							 Number((this.value === \'custom\')));"'),

		$form->text(array('desc'	=> '&nbsp;',
				  'name'	=> 'ufeatures[outcallerid-custom]',
				  'labelid'	=> 'ufeatures-outcallerid-custom',
				  'value'	=> ($outcallerid_custom === true ? $outcallerid : ''),
				  'size'	=> 15,
				  'notag'	=> false)),

		$form->checkbox(array('desc'	=> $this->bbf('fm_protocol_sendani'),
				      'name'	=> 'protocol[sendani]',
				      'labelid'	=> 'protocol-sendani',
				      'default'	=> $element['protocol']['iax']['sendani']['default'],
				      'checked'	=> $this->get_varra('info',array('protocol','sendani')))),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_preprocess-subroutine'),
				  'name'	=> 'ufeatures[preprocess_subroutine]',
				  'labelid'	=> 'ufeatures-preprocess-subroutine',
				  'size'	=> 15,
				  'default'	=> $element['ufeatures']['preprocess_subroutine']['default'],
				  'value'	=> $info['ufeatures']['preprocess_subroutine'])),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_insecure'),
				    'name'	=> 'protocol[insecure]',
				    'labelid'	=> 'protocol-insecure',
				    'empty'	=> true,
				    'bbf'	=> array('paramvalue','fm_protocol_insecure-opt'),
				    'default'	=> $element['protocol']['sip']['insecure']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','insecure'))),
			      $element['protocol']['sip']['insecure']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_host'),
				    'name'	=> 'protocol[host-dynamic]',
				    'labelid'	=> 'protocol-host-dynamic',
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_host-',
				    'value'	=> ($host_static === true ? 'static' : $host)),
			      $element['protocol']['sip']['host-dynamic']['value'],
			      'onchange="xivo_chg_attrib(\'ast_fm_user_host\',
			      				 \'fd-protocol-host-static\',
							 Number((this.value === \'static\')));"'),

		$form->text(array('desc'	=> '&nbsp;',
				  'name'	=> 'protocol[host-static]',
				  'labelid'	=> 'protocol-host-static',
				  'size'	=> 15,
				  'value'	=> ($host_static === true ? $host : ''))),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_mask'),
				  'name'	=> 'protocol[mask]',
				  'labelid'	=> 'protocol-mask',
				  'size'	=> 15,
				  'default'	=> $element['protocol']['iax']['mask']['default'],
				  'value'	=> $this->get_varra('info',array('protocol','mask')))),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_permit'),
				  'name'	=> 'protocol[permit]',
				  'labelid'	=> 'protocol-permit',
				  'size'	=> 20,
				  'value'	=> $this->get_varra('info',array('protocol','permit')))),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_deny'),
				  'name'	=> 'protocol[deny]',
				  'labelid'	=> 'protocol-deny',
				  'size'	=> 20,
				  'value'	=> $this->get_varra('info',array('protocol','deny')))),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_trustrpid'),
				    'name'	=> 'protocol[trustrpid]',
				    'labelid'	=> 'protocol-trustrpid',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['trustrpid']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','trustrpid'))),
			      $element['protocol']['sip']['trustrpid']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_sendrpid'),
				    'name'	=> 'protocol[sendrpid]',
				    'labelid'	=> 'protocol-sendrpid',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['sendrpid']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','sendrpid'))),
			      $element['protocol']['sip']['sendrpid']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_allowsubscribe'),
				    'name'	=> 'protocol[allowsubscribe]',
				    'labelid'	=> 'protocol-allowsubscribe',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['allowsubscribe']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','allowsubscribe'))),
			      $element['protocol']['sip']['allowsubscribe']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_allowoverlap'),
				    'name'	=> 'protocol[allowoverlap]',
				    'labelid'	=> 'protocol-allowoverlap',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['allowoverlap']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','allowoverlap'))),
			      $element['protocol']['sip']['allowoverlap']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_promiscredir'),
				    'name'	=> 'protocol[promiscredir]',
				    'labelid'	=> 'protocol-promiscredir',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['promiscredir']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','promiscredir'))),
			      $element['protocol']['sip']['promiscredir']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_usereqphone'),
				    'name'	=> 'protocol[usereqphone]',
				    'labelid'	=> 'protocol-usereqphone',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['usereqphone']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','usereqphone'))),
			      $element['protocol']['sip']['usereqphone']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_canreinvite'),
				    'name'	=> 'protocol[canreinvite]',
				    'labelid'	=> 'protocol-canreinvite',
				    'empty'	=> true,
				    'bbf'	=> array('paramvalue','fm_protocol_canreinvite-opt'),
				    'default'	=> $element['protocol']['sip']['canreinvite']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','canreinvite'))),
			      $element['protocol']['sip']['canreinvite']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_fromuser'),
				  'name'	=> 'protocol[fromuser]',
				  'labelid'	=> 'protocol-fromuser',
				  'size'	=> 15,
				  'default'	=> $element['protocol']['sip']['fromuser']['default'],
				  'value'	=> $this->get_varra('info',array('protocol','fromuser')))),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_fromdomain'),
				  'name'	=> 'protocol[fromdomain]',
				  'labelid'	=> 'protocol-fromdomain',
				  'size'	=> 15,
				  'default'	=> $element['protocol']['sip']['fromdomain']['default'],
				  'value'	=> $this->get_varra('info',array('protocol','fromdomain')))),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_maxauthreq'),
				    'name'	=> 'protocol[maxauthreq]',
				    'labelid'	=> 'protocol-maxauthreq',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('paramkey','fm_protocol_maxauthreq-opt'),
				    'default'	=> $element['protocol']['iax']['maxauthreq']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','maxauthreq'))),
			      $element['protocol']['iax']['maxauthreq']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_adsi'),
				    'name'	=> 'protocol[adsi]',
				    'labelid'	=> 'protocol-adsi',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['iax']['adsi']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','adsi'))),
			      $element['protocol']['iax']['adsi']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_amaflags'),
				    'name'	=> 'protocol[amaflags]',
				    'labelid'	=> 'sip-protocol-amaflags',
				    'key'	=> false,
				    'bbf'	=> 'ast_amaflag_name_info-',
				    'default'	=> $element['protocol']['sip']['amaflags']['default'],
				    'value'	=> $amaflags),
			      $element['protocol']['sip']['amaflags']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_amaflags'),
				    'name'	=> 'protocol[amaflags]',
				    'labelid'	=> 'iax-protocol-amaflags',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'ast_amaflag_name_info-',
				    'default'	=> $element['protocol']['iax']['amaflags']['default'],
				    'value'	=> $amaflags),
			      $element['protocol']['iax']['amaflags']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_accountcode'),
				  'name'	=> 'protocol[accountcode]',
				  'labelid'	=> 'protocol-accountcode',
				  'size'	=> 15,
				  'value'	=> $this->get_varra('info',array('protocol','accountcode')))),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_useclientcode'),
				    'name'	=> 'protocol[useclientcode]',
				    'labelid'	=> 'protocol-useclientcode',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sip']['useclientcode']['default'],
				    'value'	=> $this->get_varra('info',array('protocol','useclientcode'))),
			      $element['protocol']['sip']['useclientcode']['value']);
?>
	<div class="fm-field fm-description">
		<p>
			<label id="lb-ufeatures-description" for="it-ufeatures-description"><?=$this->bbf('fm_userfeatures_description');?></label>
		</p>
		<?=$form->textarea(array('field'	=> false,
					 'label'	=> false,
					 'name'		=> 'ufeatures[description]',
					 'id'		=> 'it-ufeatures-description',
					 'cols'		=> 60,
					 'rows'		=> 5,
					 'default'	=> $element['ufeatures']['description']['default']),
				   $info['ufeatures']['description']);?>
	</div>
</div>
