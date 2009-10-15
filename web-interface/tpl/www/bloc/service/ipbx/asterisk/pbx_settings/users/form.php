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

if(empty($info['userfeatures']['voicemailid']) === true):
	$voicemail_identity = false;
elseif(($voicemail_identity = (string) $this->get_var('voicemail','identity')) === ''):
	$voicemail_identity = $this->get_var('voicemail','fullname');
endif;

if(($outcallerid = (string) $info['userfeatures']['outcallerid']) === ''
|| in_array($outcallerid,$element['userfeatures']['outcallerid']['value'],true) === true):
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
	if(dwho_issa('allow',$info['protocol']) === true):
		$allow = $info['protocol']['allow'];
	else:
		$allow = array();
	endif;

	$context = (string) dwho_ak('context',$info['protocol'],true);
	$amaflags = (string) dwho_ak('amaflags',$info['protocol'],true);
	$qualify = (string) dwho_ak('qualify',$info['protocol'],true);
	$host = (string) dwho_ak('host',$info['protocol'],true);
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
				  'name'	=> 'userfeatures[firstname]',
				  'labelid'	=> 'userfeatures-firstname',
				  'size'	=> 15,
				  'default'	=> $element['userfeatures']['firstname']['default'],
				  'value'	=> $info['userfeatures']['firstname'])),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_lastname'),
				  'name'	=> 'userfeatures[lastname]',
				  'labelid'	=> 'userfeatures-lastname',
				  'size'	=> 15,
				  'default'	=> $element['userfeatures']['lastname']['default'],
				  'value'	=> $info['userfeatures']['lastname'])),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_name'),
				  'name'	=> 'protocol[name]',
				  'labelid'	=> 'protocol-name',
				  'size'	=> 15,
				  'value'	=> $info['protocol']['name'])),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_secret'),
				  'name'	=> 'protocol[secret]',
				  'labelid'	=> 'protocol-secret',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info','protocol','secret'))),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_interface'),
				  'name'	=> 'protocol[interface]',
				  'labelid'	=> 'protocol-interface',
				  'size'	=> 15,
				  'default'	=> $element['protocol']['custom']['interface']['default'],
				  'value'	=> $this->get_var('info','protocol','interface'))),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_number'),
				  'name'	=> 'userfeatures[number]',
				  'labelid'	=> 'userfeatures-number',
				  'size'	=> 15,
				  'default'	=> $element['userfeatures']['number']['default'],
				  'value'	=> $info['userfeatures']['number'])),

		$form->select(array('desc'	=> $this->bbf('fm_userfeatures_ringseconds'),
				    'name'	=> 'userfeatures[ringseconds]',
				    'labelid'	=> 'userfeatures-ringseconds',
				    'key'	=> false,
				    'bbf'	=> 'fm_userfeatures_ringseconds-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['userfeatures']['ringseconds']['default'],
				    'selected'	=> $info['userfeatures']['ringseconds']),
			      $element['userfeatures']['ringseconds']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_userfeatures_simultcalls'),
				    'name'	=> 'userfeatures[simultcalls]',
				    'labelid'	=> 'userfeatures-simultcalls',
				    'key'	=> false,
				    'default'	=> $element['userfeatures']['simultcalls']['default'],
				    'selected'	=> $info['userfeatures']['simultcalls']),
			      $element['userfeatures']['simultcalls']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_protocol'),
				    'name'	=> 'protocol[protocol]',
				    'labelid'	=> 'protocol-protocol',
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_protocol-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['userfeatures']['protocol']['default'],
				    'selected'	=> $info['userfeatures']['protocol']),
			      $element['userfeatures']['protocol']['value']);

	if($context_list !== false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_protocol_context'),
					    'name'	=> 'protocol[context]',
					    'labelid'	=> 'protocol-context',
					    'key'	=> 'identity',
					    'altkey'	=> 'name',
					    'selected'	=> $context),
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
					    'name'	=> 'userfeatures[musiconhold]',
					    'labelid'	=> 'userfeatures-musiconhold',
					    'empty'	=> true,
					    'key'	=> 'category',
					    'invalid'	=> ($this->get_var('act') === 'edit'),
					    'default'	=> ($this->get_var('act') === 'add' ? $element['userfeatures']['musiconhold']['default'] : null),
					    'selected'	=> $info['userfeatures']['musiconhold']),
				      $moh_list);
	endif;

	echo	$form->select(array('desc'	=> $this->bbf('fm_protocol_language'),
				    'name'	=> 'protocol[language]',
				    'labelid'	=> 'protocol-language',
				    'empty'	=> true,
				    'key'	=> false,
				    'default'	=> $element['protocol']['sip']['language']['default'],
				    'selected'	=> $this->get_var('info','protocol','language')),
			      $element['protocol']['sip']['language']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_nat'),
				    'name'	=> 'protocol[nat]',
				    'labelid'	=> 'protocol-nat',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_nat-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['nat']['default'],
				    'selected'	=> $this->get_var('info','protocol','nat')),
			      $element['protocol']['sip']['nat']['value']);
?>
</div>

<div id="sb-part-voicemail" class="b-nodisplay">
<?php
	echo	$form->hidden(array('name'	=> 'userfeatures[voicemailid]',
				    'id'	=> 'it-userfeatures-voicemailid',
				    'value'	=> $info['userfeatures']['voicemailid'])),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail_option'),
				    'name'	=> 'voicemail-option',
				    'labelid'	=> 'voicemail-option',
				    'empty'	=> $voicemail_identity,
				    'key'	=> false,
				    'bbf'	=> 'fm_voicemail_option-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('info','voicemail-option')),
			      $element['voicemail']['option']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail_suggest'),
				  'name'	=> 'voicemail-suggest',
				  'labelid'	=> 'voicemail-suggest',
				  'size'	=> 20)),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail_fullname'),
				  'name'	=> 'voicemail[fullname]',
				  'labelid'	=> 'voicemail-fullname',
				  'size'	=> 15,
				  'default'	=> $element['voicemail']['fullname']['default'],
				  'value'	=> $this->get_var('voicemail','fullname'))),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail_mailbox'),
				  'name'	=> 'voicemail[mailbox]',
				  'labelid'	=> 'voicemail-mailbox',
				  'size'	=> 10,
				  'default'	=> $element['voicemail']['mailbox']['default'],
				  'value'	=> $this->get_var('voicemail','mailbox'))),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail_password'),
				  'name'	=> 'voicemail[password]',
				  'labelid'	=> 'voicemail-password',
				  'size'	=> 10,
				  'default'	=> $element['voicemail']['password']['default'],
				  'value'	=> $this->get_var('voicemail','password'))),

		$form->text(array('desc'	=> $this->bbf('fm_voicemail_email'),
				  'name'	=> 'voicemail[email]',
				  'labelid'	=> 'voicemail-email',
				  'size'	=> 15,
				  'value'	=> $this->get_var('voicemail','email'),
				  'default'	=> $element['voicemail']['email']['default']));

	if(($tz_list = $this->get_var('tz_list')) !== false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_voicemail_tz'),
					    'name'	=> 'voicemail[tz]',
					    'labelid'	=> 'voicemail-tz',
					    'key'	=> 'name',
					    'default'	=> $element['voicemail']['tz']['default'],
					    'selected'	=> $this->get_var('voicemail','tz')),
				      $tz_list);
	endif;

	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_voicemailfeatures_skipcheckpass'),
				      'name'	=> 'voicemailfeatures[skipcheckpass]',
				      'labelid'	=> 'voicemailfeatures-skipcheckpass',
				      'default'	=> $element['voicemailfeatures']['skipcheckpass']['default'],
				      'checked'	=> $this->get_var('info','voicemailfeatures','skipcheckpass'))),

		$form->select(array('desc'	=> $this->bbf('fm_voicemail_attach'),
				    'name'	=> 'voicemail[attach]',
				    'labelid'	=> 'voicemail-attach',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['voicemail']['attach']['default'],
				    'selected'	=> $this->get_var('voicemail','attach')),
			      $element['voicemail']['attach']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_voicemail_deletevoicemail'),
				      'name'	=> 'voicemail[deletevoicemail]',
				      'labelid'	=> 'voicemail-deletevoicemail',
				      'default'	=> $element['voicemail']['deletevoicemail']['default'],
				      'checked'	=> $this->get_var('voicemail','deletevoicemail'))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_protocol_subscribemwi'),
				      'name'	=> 'protocol[subscribemwi]',
				      'labelid'	=> 'protocol-subscribemwi',
				      'default'	=> $element['protocol']['sip']['subscribemwi']['default'],
				      'checked'	=> $this->get_var('info','protocol','subscribemwi'))),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_buggymwi'),
				    'name'	=> 'protocol[buggymwi]',
				    'labelid'	=> 'protocol-buggymwi',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['buggymwi']['default'],
				    'selected'	=> $this->get_var('info','protocol','buggymwi')),
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
				      'name'	=> 'userfeatures[enableclient]',
				      'labelid'	=> 'userfeatures-enableclient',
				      'default'	=> $element['userfeatures']['enableclient']['default'],
				      'checked'	=> $info['userfeatures']['enableclient'])),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_loginclient'),
				  'name'	=> 'userfeatures[loginclient]',
				  'labelid'	=> 'userfeatures-loginclient',
				  'size'	=> 15,
				  'default'	=> $element['userfeatures']['loginclient']['default'],
				  'value'	=> $info['userfeatures']['loginclient'])),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_passwdclient'),
				  'name'	=> 'userfeatures[passwdclient]',
				  'labelid'	=> 'userfeatures-passwdclient',
				  'size'	=> 15,
				  'default'	=> $element['userfeatures']['passwdclient']['default'],
				  'value'	=> $info['userfeatures']['passwdclient']));

	if(is_array($profileclient_list) === true && empty($profileclient_list) === false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_userfeatures_profileclient'),
					    'name'	=> 'userfeatures[profileclient]',
					    'labelid'	=> 'userfeatures-profileclient',
					    'default'	=> $element['userfeatures']['profileclient']['default'],
					    'selected'	=> $info['userfeatures']['profileclient']),
				      $profileclient_list);
	endif;
?>
	</fieldset>

	<fieldset id="fld-services">
		<legend><?=$this->bbf('fld-services');?></legend>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_enablehint'),
				      'name'	=> 'userfeatures[enablehint]',
				      'labelid'	=> 'userfeatures-enablehint',
				      'default'	=> $element['userfeatures']['enablehint']['default'],
				      'checked'	=> $info['userfeatures']['enablehint'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_enablevoicemail'),
				      'name'	=> 'userfeatures[enablevoicemail]',
				      'labelid'	=> 'userfeatures-enablevoicemail',
				      'default'	=> $element['userfeatures']['enablevoicemail']['default'],
				      'checked'	=> $info['userfeatures']['enablevoicemail'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_enablexfer'),
				      'name'	=> 'userfeatures[enablexfer]',
				      'labelid'	=> 'userfeatures-enablexfer',
				      'default'	=> $element['userfeatures']['enablexfer']['default'],
				      'checked'	=> $info['userfeatures']['enablexfer'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_enableautomon'),
				      'name'	=> 'userfeatures[enableautomon]',
				      'labelid'	=> 'userfeatures-enableautomon',
				      'default'	=> $element['userfeatures']['enableautomon']['default'],
				      'checked'	=> $info['userfeatures']['enableautomon'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_callrecord'),
				      'name'	=> 'userfeatures[callrecord]',
				      'labelid'	=> 'userfeatures-callrecord',
				      'default'	=> $element['userfeatures']['callrecord']['default'],
				      'checked'	=> $info['userfeatures']['callrecord'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_incallfilter'),
				      'name'	=> 'userfeatures[incallfilter]',
				      'labelid'	=> 'userfeatures-incallfilter',
				      'default'	=> $element['userfeatures']['incallfilter']['default'],
				      'checked'	=> $info['userfeatures']['incallfilter'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_enablednd'),
				      'name'	=> 'userfeatures[enablednd]',
				      'labelid'	=> 'userfeatures-enablednd',
				      'default'	=> $element['userfeatures']['enablednd']['default'],
				      'checked'	=> $info['userfeatures']['enablednd'])),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_mobilephonenumber'),
				  'name'	=> 'userfeatures[mobilephonenumber]',
				  'labelid'	=> 'userfeatures-mobilephonenumber',
				  'size'	=> 15,
				  'default'	=> $element['userfeatures']['mobilephonenumber']['default'],
				  'value'	=> $info['userfeatures']['mobilephonenumber'])),

		$form->select(array('desc'	=> $this->bbf('fm_userfeatures_bsfilter'),
				    'name'	=> 'userfeatures[bsfilter]',
				    'labelid'	=> 'userfeatures-bsfilter',
				    'key'	=> false,
				    'bbf'	=> 'fm_userfeatures_bsfilter-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['userfeatures']['bsfilter']['default'],
				    'selected'	=> $info['userfeatures']['bsfilter']),
			      $element['userfeatures']['bsfilter']['value']);

	if($agent_list !== false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_userfeatures_agentid'),
					    'name'	=> 'userfeatures[agentid]',
					    'labelid'	=> 'userfeatures-agentid',
					    'empty'	=> true,
					    'key'	=> 'identity',
					    'altkey'	=> 'id',
					    'default'	=> $element['userfeatures']['agentid']['default'],
					    'selected'	=> $info['userfeatures']['agentid']),
				      $agent_list);
	else:
		echo	'<div id="fd-userfeatures-agentid" class="txt-center">',
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
				      'name'	=> 'userfeatures[enablerna]',
				      'labelid'	=> 'userfeatures-enablerna',
				      'default'	=> $element['userfeatures']['enablerna']['default'],
				      'checked'	=> $info['userfeatures']['enablerna'])),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_destrna'),
				  'name'	=> 'userfeatures[destrna]',
				  'labelid'	=> 'userfeatures-destrna',
				  'size'	=> 15,
				  'default'	=> $element['userfeatures']['destrna']['default'],
				  'value'	=> $info['userfeatures']['destrna'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_enablebusy'),
				      'name'	=> 'userfeatures[enablebusy]',
				      'labelid'	=> 'userfeatures-enablebusy',
				      'default'	=> $element['userfeatures']['enablebusy']['default'],
				      'checked'	=> $info['userfeatures']['enablebusy'])),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_destbusy'),
				  'name'	=> 'userfeatures[destbusy]',
				  'labelid'	=> 'userfeatures-destbusy',
				  'size'	=> 15,
				  'default'	=> $element['userfeatures']['destbusy']['default'],
				  'value'	=> $info['userfeatures']['destbusy'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_userfeatures_enableunc'),
				      'name'	=> 'userfeatures[enableunc]',
				      'labelid'	=> 'userfeatures-enableunc',
				      'default'	=> $element['userfeatures']['enableunc']['default'],
				      'checked'	=> $info['userfeatures']['enableunc'])),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_destunc'),
				  'name'	=> 'userfeatures[destunc]',
				  'labelid'	=> 'userfeatures-destunc',
				  'size'	=> 15,
				  'default'	=> $element['userfeatures']['destunc']['default'],
				  'value'	=> $info['userfeatures']['destunc']));
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
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_progressinband-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['progressinband']['default'],
				    'selected'	=> $this->get_var('info','protocol','progressinband')),
			      $element['protocol']['sip']['progressinband']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_dtmfmode'),
				    'name'	=> 'protocol[dtmfmode]',
				    'labelid'	=> 'protocol-dtmfmode',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_dtmfmode-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['dtmfmode']['default'],
				    'selected'	=> $this->get_var('info','protocol','dtmfmode')),
			      $element['protocol']['sip']['dtmfmode']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_rfc2833compensate'),
				    'name'	=> 'protocol[rfc2833compensate]',
				    'labelid'	=> 'protocol-rfc2833compensate',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['rfc2833compensate']['default'],
				    'selected'	=> $this->get_var('info','protocol','rfc2833compensate')),
			      $element['protocol']['sip']['rfc2833compensate']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_qualify'),
				    'name'	=> 'protocol[qualify]',
				    'labelid'	=> 'sip-protocol-qualify',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_qualify-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['qualify']['default'],
				    'selected'	=> $qualify),
			      $element['protocol']['sip']['qualify']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_qualify'),
				    'name'	=> 'protocol[qualify]',
				    'labelid'	=> 'iax-protocol-qualify',
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_qualify-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['iax']['qualify']['default'],
				    'selected'	=> $qualify),
			      $element['protocol']['iax']['qualify']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_qualifysmoothing'),
				    'name'	=> 'protocol[qualifysmoothing]',
				    'labelid'	=> 'protocol-qualifysmoothing',
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['iax']['qualifysmoothing']['default'],
				    'selected'	=> $this->get_var('info','protocol','qualifysmoothing')),
			      $element['protocol']['iax']['qualifysmoothing']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_qualifyfreqok'),
				    'name'	=> 'protocol[qualifyfreqok]',
				    'labelid'	=> 'protocol-qualifyfreqok',
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_qualifyfreq-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'millisecond',
									'format'	=> '%M%s%ms')),
				    'default'	=> $element['protocol']['iax']['qualifyfreqok']['default'],
				    'selected'	=> $this->get_var('info','protocol','qualifyfreqok')),
			      $element['protocol']['iax']['qualifyfreqok']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_qualifyfreqnotok'),
				    'name'	=> 'protocol[qualifyfreqnotok]',
				    'labelid'	=> 'protocol-qualifyfreqnotok',
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_qualifyfreq-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'millisecond',
									'format'	=> '%M%s%ms')),
				    'default'	=> $element['protocol']['iax']['qualifyfreqnotok']['default'],
				    'selected'	=> $this->get_var('info','protocol','qualifyfreqnotok')),
			      $element['protocol']['iax']['qualifyfreqnotok']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_rtptimeout'),
				    'name'	=> 'protocol[rtptimeout]',
				    'labelid'	=> 'protocol-rtptimeout',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_rtptimeout-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['rtptimeout']['default'],
				    'selected'	=> $this->get_var('info','protocol','rtptimeout')),
			      $element['protocol']['sip']['rtptimeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_rtpholdtimeout'),
				    'name'	=> 'protocol[rtpholdtimeout]',
				    'labelid'	=> 'protocol-rtpholdtimeout',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_rtpholdtimeout-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['rtpholdtimeout']['default'],
				    'selected'	=> $this->get_var('info','protocol','rtpholdtimeout')),
			      $element['protocol']['sip']['rtpholdtimeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_rtpkeepalive'),
				    'name'	=> 'protocol[rtpkeepalive]',
				    'labelid'	=> 'protocol-rtpkeepalive',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_rtpkeepalive-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['rtpkeepalive']['default'],
				    'selected'	=> $this->get_var('info','protocol','rtpkeepalive')),
			      $element['protocol']['sip']['rtpkeepalive']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_allowtransfer'),
				    'name'	=> 'protocol[allowtransfer]',
				    'labelid'	=> 'protocol-allowtransfer',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['allowtransfer']['default'],
				    'selected'	=> $this->get_var('info','protocol','allowtransfer')),
			      $element['protocol']['sip']['allowtransfer']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_autoframing'),
				    'name'	=> 'protocol[autoframing]',
				    'labelid'	=> 'protocol-autoframing',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['autoframing']['default'],
				    'selected'	=> $this->get_var('info','protocol','autoframing')),
			      $element['protocol']['sip']['autoframing']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_videosupport'),
				    'name'	=> 'protocol[videosupport]',
				    'labelid'	=> 'protocol-videosupport',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['videosupport']['default'],
				    'selected'	=> $this->get_var('info','protocol','videosupport')),
			      $element['protocol']['sip']['videosupport']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_maxcallbitrate'),
				  'name'	=> 'protocol[maxcallbitrate]',
				  'labelid'	=> 'protocol-maxcallbitrate',
				  'size'	=> 10,
				  'default'	=> $element['protocol']['sip']['maxcallbitrate']['default'],
				  'value'	=> $this->get_var('info','protocol','maxcallbitrate'))),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_g726nonstandard'),
				    'name'	=> 'protocol[g726nonstandard]',
				    'labelid'	=> 'protocol-g726nonstandard',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['g726nonstandard']['default'],
				    'selected'	=> $this->get_var('info','protocol','g726nonstandard')),
			      $element['protocol']['sip']['g726nonstandard']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_jitterbuffer'),
				    'name'	=> 'protocol[jitterbuffer]',
				    'labelid'	=> 'protocol-jitterbuffer',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['iax']['jitterbuffer']['default'],
				    'selected'	=> $this->get_var('info','protocol','jitterbuffer')),
			      $element['protocol']['iax']['jitterbuffer']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_forcejitterbuffer'),
				    'name'	=> 'protocol[forcejitterbuffer]',
				    'labelid'	=> 'protocol-forcejitterbuffer',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['iax']['forcejitterbuffer']['default'],
				    'selected'	=> $this->get_var('info','protocol','forcejitterbuffer')),
			      $element['protocol']['iax']['forcejitterbuffer']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_codecpriority'),
				    'name'	=> 'protocol[codecpriority]',
				    'labelid'	=> 'protocol-codecpriority',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_codecpriority-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['iax']['codecpriority']['default'],
				    'selected'	=> $this->get_var('info','protocol','codecpriority')),
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
				    'bbf'	=> 'fm_protocol_codec-disallow-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue')),
			      $element['protocol']['sip']['disallow']['value']);
?>

<div id="codeclist" class="fm-paragraph fm-multilist">
	<p>
		<label id="lb-codeclist" for="it-codeclist" onclick="dwho_eid('it-codeclist').focus();">
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
					    'paragraph'	=> false,
					    'key'	=> false,
					    'bbf'	=> 'ast_codec_name_type',
					    'bbfopt'	=> array('argmode' => 'paramvalue')),
				      $element['protocol']['sip']['allow']['value']);
?>
	</div>

	<div class="inout-list">
		<a href="#"
		   onclick="dwho.form.move_selected('it-codeclist',
						  'it-codec');
			    return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_incodec');?>">
			<?=$url->img_html('img/site/button/row-left.gif',
					  $this->bbf('bt_incodec'),
					  'class="bt-inlist" id="bt-incodec" border="0"');?></a><br />
		<a href="#"
		   onclick="dwho.form.move_selected('it-codec',
						  'it-codeclist');
			    return(dwho.dom.free_focus());"
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
					    'paragraph'	=> false,
					    'key'	=> false,
					    'bbf'	=> 'ast_codec_name_type',
					    'bbfopt'	=> array('argmode' => 'paramvalue')),
				      $allow);
?>
		<div class="bt-updown">
			<a href="#"
			   onclick="dwho.form.order_selected('it-codec',1);
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_upcodec');?>">
				<?=$url->img_html('img/site/button/row-up.gif',
						  $this->bbf('bt_upcodec'),
						  'class="bt-uplist" id="bt-upcodec" border="0"');?></a><br />
			<a href="#"
			   onclick="dwho.form.order_selected('it-codec',-1);
				    return(dwho.dom.free_focus());"
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
					    'empty'	=> true,
					    'key'	=> false,
					    'bbf'	=> 'fm_autoprov_modact-opt',
					    'bbfopt'	=> array('argmode'	=> 'paramvalue')),
				      $element['autoprov']['modact']['value']);
	endif;

	if(is_array($info['autoprov']) === false
	|| $vendormodel === ''
	|| (int) dwho_ak('iduserfeatures',$info['autoprov'],true) === 0):
		echo	$form->select(array('desc'	=> $this->bbf('fm_autoprov_vendormodel'),
					    'name'	=> 'autoprov[vendormodel]',
					    'labelid'	=> 'autoprov-vendormodel',
					    'empty'	=> true,
					    'key'	=> 'label',
					    'altkey'	=> 'path',
					    'selected'	=> $vendormodel,
					    'optgroup'	=> array('key'	=> 'name')),
				      $autoprov_list),
			$form->text(array('desc'	=> $this->bbf('fm_autoprov_macaddr'),
					  'name'	=> 'autoprov[macaddr]',
					  'labelid'	=> 'autoprov-macaddr',
					  'size'	=> 15,
					  'value'	=> $info['autoprov']['macaddr']));
	elseif(isset($autoprov_list[$info['autoprov']['vendor']]) === true):
		echo	'<p id="fd-autoprov-vendormodel" class="fm-paragraph">',
				'<label id="lb-autoprov-vendormodel"><span class="fm-desc">',
				$this->bbf('fm_autoprov_vendormodel'),
				'</span>&nbsp;',
				$autoprov_list[$info['autoprov']['vendor']]['name'],
				' ',
				$autoprov_list[$info['autoprov']['vendor']]['model'][$info['autoprov']['model']]['label'],
				'</label>',
			'</p>',

			'<p id="fd-autoprov-macaddr" class="fm-paragraph">',
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
	<div id="rightcalllist" class="fm-paragraph fm-multilist">
		<div class="slt-outlist">
			<?=$form->select(array('name'		=> 'rightcalllist',
					       'label'		=> false,
					       'id'		=> 'it-rightcalllist',
					       'browse'		=> 'rightcall',
					       'key'		=> 'identity',
					       'altkey'		=> 'id',
					       'multiple'	=> true,
					       'size'		=> 5,
					       'paragraph'		=> false),
					 $rightcall['list']);?>
		</div>
		<div class="inout-list">
			<a href="#"
			   onclick="dwho.form.move_selected('it-rightcalllist','it-rightcall');
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_inrightcall');?>">
				<?=$url->img_html('img/site/button/row-left.gif',
						  $this->bbf('bt_inrightcall'),
						  'class="bt-inlist" id="bt-inrightcall" border="0"');?></a><br />
			<a href="#"
			   onclick="dwho.form.move_selected('it-rightcall','it-rightcalllist');
				    return(dwho.dom.free_focus());"
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
					       'paragraph'		=> false),
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
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['t38pt_udptl']['default'],
				    'selected'	=> $this->get_var('info','protocol','t38pt_udptl')),
			      $element['protocol']['sip']['t38pt_udptl']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_t38pt-rtp'),
				    'name'	=> 'protocol[t38pt_rtp]',
				    'labelid'	=> 'protocol-t38pt-rtp',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['t38pt_rtp']['default'],
				    'selected'	=> $this->get_var('info','protocol','t38pt_rtp')),
			      $element['protocol']['sip']['t38pt_rtp']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_t38pt-tcp'),
				    'name'	=> 'protocol[t38pt_tcp]',
				    'labelid'	=> 'protocol-t38pt-tcp',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['t38pt_tcp']['default'],
				    'selected'	=> $this->get_var('info','protocol','t38pt_tcp')),
			      $element['protocol']['sip']['t38pt_tcp']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_t38pt-usertpsource'),
				    'name'	=> 'protocol[t38pt_usertpsource]',
				    'labelid'	=> 'protocol-t38pt-usertpsource',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['t38pt_usertpsource']['default'],
				    'selected'	=> $this->get_var('info','protocol','t38pt_usertpsource')),
			      $element['protocol']['sip']['t38pt_usertpsource']['value']);
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_protocol_callerid'),
				  'name'	=> 'protocol[callerid]',
				  'labelid'	=> 'protocol-callerid',
				  'value'	=> $this->get_var('info','protocol','callerid'),
				  'size'	=> 15,
				  'notag'	=> false)),

		$form->select(array('desc'	=> $this->bbf('fm_userfeatures_outcallerid'),
				    'name'	=> 'userfeatures[outcallerid-type]',
				    'labelid'	=> 'userfeatures-outcallerid-type',
				    'key'	=> false,
				    'bbf'	=> 'fm_userfeatures_outcallerid-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> ($outcallerid_custom === true ? 'custom' : $outcallerid)),
			      $element['userfeatures']['outcallerid-type']['value']),

		$form->text(array('desc'	=> '&nbsp;',
				  'name'	=> 'userfeatures[outcallerid-custom]',
				  'labelid'	=> 'userfeatures-outcallerid-custom',
				  'value'	=> ($outcallerid_custom === true ? $outcallerid : ''),
				  'size'	=> 15,
				  'notag'	=> false)),

		$form->checkbox(array('desc'	=> $this->bbf('fm_protocol_sendani'),
				      'name'	=> 'protocol[sendani]',
				      'labelid'	=> 'protocol-sendani',
				      'default'	=> $element['protocol']['iax']['sendani']['default'],
				      'checked'	=> $this->get_var('info','protocol','sendani'))),

		$form->text(array('desc'	=> $this->bbf('fm_userfeatures_preprocess-subroutine'),
				  'name'	=> 'userfeatures[preprocess_subroutine]',
				  'labelid'	=> 'userfeatures-preprocess-subroutine',
				  'size'	=> 15,
				  'default'	=> $element['userfeatures']['preprocess_subroutine']['default'],
				  'value'	=> $info['userfeatures']['preprocess_subroutine'])),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_insecure'),
				    'name'	=> 'protocol[insecure]',
				    'labelid'	=> 'protocol-insecure',
				    'empty'	=> true,
				    'bbf'	=> 'fm_protocol_insecure-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['insecure']['default'],
				    'selected'	=> $this->get_var('info','protocol','insecure')),
			      $element['protocol']['sip']['insecure']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_host-type'),
				    'name'	=> 'protocol[host-type]',
				    'labelid'	=> 'protocol-host-type',
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_host-type-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> ($host_static === true ? 'static' : $host)),
			      $element['protocol']['sip']['host-type']['value']),

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
				  'value'	=> $this->get_var('info','protocol','mask'))),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_permit'),
				  'name'	=> 'protocol[permit]',
				  'labelid'	=> 'protocol-permit',
				  'size'	=> 20,
				  'value'	=> $this->get_var('info','protocol','permit'))),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_deny'),
				  'name'	=> 'protocol[deny]',
				  'labelid'	=> 'protocol-deny',
				  'size'	=> 20,
				  'value'	=> $this->get_var('info','protocol','deny'))),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_trustrpid'),
				    'name'	=> 'protocol[trustrpid]',
				    'labelid'	=> 'protocol-trustrpid',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['trustrpid']['default'],
				    'selected'	=> $this->get_var('info','protocol','trustrpid')),
			      $element['protocol']['sip']['trustrpid']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_sendrpid'),
				    'name'	=> 'protocol[sendrpid]',
				    'labelid'	=> 'protocol-sendrpid',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['sendrpid']['default'],
				    'selected'	=> $this->get_var('info','protocol','sendrpid')),
			      $element['protocol']['sip']['sendrpid']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_allowsubscribe'),
				    'name'	=> 'protocol[allowsubscribe]',
				    'labelid'	=> 'protocol-allowsubscribe',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['allowsubscribe']['default'],
				    'selected'	=> $this->get_var('info','protocol','allowsubscribe')),
			      $element['protocol']['sip']['allowsubscribe']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_allowoverlap'),
				    'name'	=> 'protocol[allowoverlap]',
				    'labelid'	=> 'protocol-allowoverlap',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['allowoverlap']['default'],
				    'selected'	=> $this->get_var('info','protocol','allowoverlap')),
			      $element['protocol']['sip']['allowoverlap']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_promiscredir'),
				    'name'	=> 'protocol[promiscredir]',
				    'labelid'	=> 'protocol-promiscredir',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['promiscredir']['default'],
				    'selected'	=> $this->get_var('info','protocol','promiscredir')),
			      $element['protocol']['sip']['promiscredir']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_usereqphone'),
				    'name'	=> 'protocol[usereqphone]',
				    'labelid'	=> 'protocol-usereqphone',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['usereqphone']['default'],
				    'selected'	=> $this->get_var('info','protocol','usereqphone')),
			      $element['protocol']['sip']['usereqphone']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_canreinvite'),
				    'name'	=> 'protocol[canreinvite]',
				    'labelid'	=> 'protocol-canreinvite',
				    'empty'	=> true,
				    'bbf'	=> 'fm_protocol_canreinvite-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['canreinvite']['default'],
				    'selected'	=> $this->get_var('info','protocol','canreinvite')),
			      $element['protocol']['sip']['canreinvite']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_fromuser'),
				  'name'	=> 'protocol[fromuser]',
				  'labelid'	=> 'protocol-fromuser',
				  'size'	=> 15,
				  'default'	=> $element['protocol']['sip']['fromuser']['default'],
				  'value'	=> $this->get_var('info','protocol','fromuser'))),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_fromdomain'),
				  'name'	=> 'protocol[fromdomain]',
				  'labelid'	=> 'protocol-fromdomain',
				  'size'	=> 15,
				  'default'	=> $element['protocol']['sip']['fromdomain']['default'],
				  'value'	=> $this->get_var('info','protocol','fromdomain'))),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_maxauthreq'),
				    'name'	=> 'protocol[maxauthreq]',
				    'labelid'	=> 'protocol-maxauthreq',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_maxauthreq-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['iax']['maxauthreq']['default'],
				    'selected'	=> $this->get_var('info','protocol','maxauthreq')),
			      $element['protocol']['iax']['maxauthreq']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_adsi'),
				    'name'	=> 'protocol[adsi]',
				    'labelid'	=> 'protocol-adsi',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['iax']['adsi']['default'],
				    'selected'	=> $this->get_var('info','protocol','adsi')),
			      $element['protocol']['iax']['adsi']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_amaflags'),
				    'name'	=> 'protocol[amaflags]',
				    'labelid'	=> 'sip-protocol-amaflags',
				    'key'	=> false,
				    'bbf'	=> 'ast_amaflag_name_info',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['amaflags']['default'],
				    'selected'	=> $amaflags),
			      $element['protocol']['sip']['amaflags']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_amaflags'),
				    'name'	=> 'protocol[amaflags]',
				    'labelid'	=> 'iax-protocol-amaflags',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'ast_amaflag_name_info',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['iax']['amaflags']['default'],
				    'selected'	=> $amaflags),
			      $element['protocol']['iax']['amaflags']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_accountcode'),
				  'name'	=> 'protocol[accountcode]',
				  'labelid'	=> 'protocol-accountcode',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info','protocol','accountcode'))),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_useclientcode'),
				    'name'	=> 'protocol[useclientcode]',
				    'labelid'	=> 'protocol-useclientcode',
				    'empty'	=> true,
				    'key'	=> false,
				    'bbf'	=> 'fm_bool-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['protocol']['sip']['useclientcode']['default'],
				    'selected'	=> $this->get_var('info','protocol','useclientcode')),
			      $element['protocol']['sip']['useclientcode']['value']);
?>
	<div class="fm-paragraph fm-description">
		<p>
			<label id="lb-userfeatures-description" for="it-userfeatures-description"><?=$this->bbf('fm_userfeatures_description');?></label>
		</p>
		<?=$form->textarea(array('paragraph'	=> false,
					 'label'	=> false,
					 'name'		=> 'userfeatures[description]',
					 'id'		=> 'it-userfeatures-description',
					 'cols'		=> 60,
					 'rows'		=> 5,
					 'default'	=> $element['userfeatures']['description']['default']),
				   $info['userfeatures']['description']);?>
	</div>
</div>
