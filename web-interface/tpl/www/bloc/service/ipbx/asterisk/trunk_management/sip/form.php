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
$url = $this->get_module('url');

$info = $this->get_var('info');
$element = $this->get_var('element');
$context_list = $this->get_var('context_list');

if(isset($info['protocol']) === true):
	if(xivo_issa('allow',$info['protocol']) === true):
		$allow = $info['protocol']['allow'];
	else:
		$allow = array();
	endif;

	$host = (string) xivo_ak('host',$info['protocol'],true);
	$protocol_disable = (bool) xivo_ak('commented',$info['protocol'],true);
else:
	$allow = array();
	$host = '';
	$protocol_disable = false;
endif;

$codec_active = empty($allow) === false;
$host_static = ($host !== '' && $host !== 'dynamic');

if(($reg_active = $this->get_varra('info',array('register','commented'))) !== null):
	$reg_active = xivo_bool($reg_active) === false;
endif;

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_protocol_name'),
				  'name'	=> 'protocol[name]',
				  'labelid'	=> 'protocol-name',
				  'size'	=> 15,
				  'default'	=> $element['protocol']['name']['default'],
				  'value'	=> $info['protocol']['name'])),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_username'),
				  'name'	=> 'protocol[username]',
				  'labelid'	=> 'protocol-username',
				  'size'	=> 15,
				  'default'	=> $element['protocol']['username']['default'],
				  'value'	=> $info['protocol']['username'])),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_secret'),
				  'name'	=> 'protocol[secret]',
				  'labelid'	=> 'protocol-secret',
				  'size'	=> 15,
				  'default'	=> $element['protocol']['secret']['default'],
				  'value'	=> $info['protocol']['secret'])),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_callerid'),
				  'name'	=> 'protocol[callerid]',
				  'labelid'	=> 'protocol-callerid',
				  'size'	=> 15,
				  'notag'	=> false,
				  'default'	=> $element['protocol']['callerid']['default'],
				  'value'	=> $info['protocol']['callerid'])),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_calllimit'),
				    'name'	=> 'protocol[call-limit]',
				    'labelid'	=> 'protocol-calllimit',
				    'key'	=> false,
				    'bbf'	=> array('mixkey','fm_protocol_calllimit-opt'),
				    'default'	=> $element['protocol']['call-limit']['default'],
				    'value'	=> $info['protocol']['call-limit']),
			      $element['protocol']['call-limit']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_host-type'),
				    'name'	=> 'protocol[host-type]',
				    'labelid'	=> 'protocol-host-type',
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_host-type-opt',
				    'bbf_opt'	=> array('argmode' => 'paramkey'),
				    'default'	=> $element['protocol']['host']['default'],
				    'value'	=> ($host_static === true ? 'static' : $host)),
			      $element['protocol']['host-type']['value'],
			      'onchange="xivo_chg_attrib(\'ast_fm_trunk_host\',
							 \'fd-protocol-host-static\',
							 Number((this.value === \'static\')));"'),

		$form->text(array('desc'	=> '&nbsp;',
				  'name'	=> 'protocol[host-static]',
				  'labelid'	=> 'protocol-host-static',
				  'size'	=> 15,
				  'default'	=> $element['protocol']['host-static']['default'],
				  'value'	=> ($host_static === true ? $host : ''))),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_type'),
				    'name'	=> 'protocol[type]',
				    'labelid'	=> 'protocol-type',
				    'key'	=> false,
				    'bbf'	=> 'fm_protocol_type-',
				    'default'	=> $element['protocol']['type']['default'],
				    'value'	=> $info['protocol']['type']),
			      $element['protocol']['type']['value'],
			      'onchange="xivo_ast_chg_trunk_type(this.value);"');

	if($context_list !== false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_protocol_context'),
					    'name'	=> 'protocol[context]',
					    'labelid'	=> 'protocol-context',
					    'key'	=> 'identity',
					    'altkey'	=> 'name',
					    'empty'	=> true,
					    'default'	=> $element['protocol']['context']['default'],
					    'value'	=> $info['protocol']['context']),
				      $context_list);
	endif;

	echo	$form->select(array('desc'	=> $this->bbf('fm_protocol_language'),
				    'name'	=> 'protocol[language]',
				    'labelid'	=> 'protocol-language',
				    'key'	=> false,
				    'empty'	=> true,
				    'default'	=> $element['protocol']['language']['default'],
				    'value'	=> $info['protocol']['language']),
			      $element['protocol']['language']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_nat'),
				    'name'	=> 'protocol[nat]',
				    'labelid'	=> 'protocol-nat',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('concatvalue','fm_protocol_nat-opt-'),
				    'default'	=> $element['protocol']['nat']['default'],
				    'value'	=> $info['protocol']['nat']),
			      $element['protocol']['nat']['value']);
?>
</div>

<div id="sb-part-register" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'		=> $this->bbf('fm_register'),
				      'name'		=> 'register-active',
				      'labelid'		=> 'register-active',
				      'checked'		=> $reg_active,
				      'disabled'	=> $protocol_disable),
				      'onclick="xivo_chg_attrib(\'ast_fm_trunk_register\',
								\'it-register-username\',
								Number((this.checked === false)));"'),

		$form->text(array('desc'	=> $this->bbf('fm_register_username'),
				  'name'	=> 'register[username]',
				  'labelid'	=> 'register-username',
				  'size'	=> 15,
				  'default'	=> $element['register']['username']['default'],
				  'value'	=> $this->get_varra('info',array('register','username')))),

		$form->text(array('desc'	=> $this->bbf('fm_register_password'),
				  'name'	=> 'register[password]',
				  'labelid'	=> 'register-password',
				  'size'	=> 15,
				  'default'	=> $element['register']['password']['default'],
				  'value'	=> $this->get_varra('info',array('register','password')))),

		$form->text(array('desc'	=> $this->bbf('fm_register_authuser'),
				  'name'	=> 'register[authuser]',
				  'labelid'	=> 'register-authuser',
				  'size'	=> 15,
				  'default'	=> $element['register']['authuser']['default'],
				  'value'	=> $this->get_varra('info',array('register','authuser')))),

		$form->text(array('desc'	=> $this->bbf('fm_register_host'),
				  'name'	=> 'register[host]',
				  'labelid'	=> 'register-host',
				  'size'	=> 15,
				  'default'	=> $element['register']['host']['default'],
				  'value'	=> $this->get_varra('info',array('register','host')))),

		$form->text(array('desc'	=> $this->bbf('fm_register_port'),
				  'name'	=> 'register[port]',
				  'labelid'	=> 'register-port',
				  'size'	=> 15,
				  'default'	=> $element['register']['port']['default'],
				  'value'	=> $this->get_varra('info',array('register','port')))),

		$form->text(array('desc'	=> $this->bbf('fm_register_contact'),
				  'name'	=> 'register[contact]',
				  'labelid'	=> 'register-contact',
				  'size'	=> 15,
				  'default'	=> $element['register']['contact']['default'],
				  'value'	=> $this->get_varra('info',array('register','contact'))));
?>
</div>

<div id="sb-part-signalling" class="b-nodisplay">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_protocol_progressinband'),
				    'name'	=> 'protocol[progressinband]',
				    'labelid'	=> 'protocol-progressinband',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('concatvalue','fm_protocol_progressinband-opt-'),
				    'default'	=> $element['protocol']['progressinband']['default'],
				    'value'	=> $info['protocol']['progressinband']),
			      $element['protocol']['progressinband']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_dtmfmode'),
				    'name'	=> 'protocol[dtmfmode]',
				    'labelid'	=> 'protocol-dtmfmode',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('concatvalue','fm_protocol_dtmfmode-opt-'),
				    'default'	=> $element['protocol']['dtmfmode']['default'],
				    'value'	=> $info['protocol']['dtmfmode']),
			      $element['protocol']['dtmfmode']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_rfc2833compensate'),
				    'name'	=> 'protocol[rfc2833compensate]',
				    'labelid'	=> 'protocol-rfc2833compensate',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['rfc2833compensate']['default'],
				    'value'	=> $info['protocol']['rfc2833compensate']),
			      $element['protocol']['rfc2833compensate']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_qualify'),
				    'name'	=> 'protocol[qualify]',
				    'labelid'	=> 'protocol-qualify',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('mixkey','fm_protocol_qualify-opt'),
				    'default'	=> $element['protocol']['qualify']['default'],
				    'value'	=> $info['protocol']['qualify']),
			      $element['protocol']['qualify']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_rtptimeout'),
				    'name'	=> 'protocol[rtptimeout]',
				    'labelid'	=> 'protocol-rtptimeout',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('mixkey','fm_protocol_rtptimeout-opt'),
				    'default'	=> $element['protocol']['rtptimeout']['default'],
				    'value'	=> $info['protocol']['rtptimeout']),
			      $element['protocol']['rtptimeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_rtpholdtimeout'),
				    'name'	=> 'protocol[rtpholdtimeout]',
				    'labelid'	=> 'protocol-rtpholdtimeout',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('mixkey','fm_protocol_rtpholdtimeout-opt'),
				    'default'	=> $element['protocol']['rtpholdtimeout']['default'],
				    'value'	=> $info['protocol']['rtpholdtimeout']),
			      $element['protocol']['rtpholdtimeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_rtpkeepalive'),
				    'name'	=> 'protocol[rtpkeepalive]',
				    'labelid'	=> 'protocol-rtpkeepalive',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> array('mixkey','fm_protocol_rtpkeepalive-opt'),
				    'default'	=> $element['protocol']['rtpkeepalive']['default'],
				    'value'	=> $info['protocol']['rtpkeepalive']),
			      $element['protocol']['rtpkeepalive']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_allowtransfer'),
				    'name'	=> 'protocol[allowtransfer]',
				    'labelid'	=> 'protocol-allowtransfer',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['allowtransfer']['default'],
				    'value'	=> $info['protocol']['allowtransfer']),
			      $element['protocol']['allowtransfer']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_autoframing'),
				    'name'	=> 'protocol[autoframing]',
				    'labelid'	=> 'protocol-autoframing',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['autoframing']['default'],
				    'value'	=> $info['protocol']['autoframing']),
			      $element['protocol']['autoframing']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_videosupport'),
				    'name'	=> 'protocol[videosupport]',
				    'labelid'	=> 'protocol-videosupport',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['videosupport']['default'],
				    'value'	=> $info['protocol']['videosupport']),
			      $element['protocol']['videosupport']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_maxcallbitrate'),
				  'name'	=> 'protocol[maxcallbitrate]',
				  'labelid'	=> 'protocol-maxcallbitrate',
				  'size'	=> 10,
				  'default'	=> $element['protocol']['maxcallbitrate']['default'],
				  'value'	=> $info['protocol']['maxcallbitrate'])),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_g726nonstandard'),
				    'name'	=> 'protocol[g726nonstandard]',
				    'labelid'	=> 'protocol-g726nonstandard',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['g726nonstandard']['default'],
				    'value'	=> $info['protocol']['g726nonstandard']),
			      $element['protocol']['g726nonstandard']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_codec-custom'),
				      'name'	=> 'codec-active',
				      'labelid'	=> 'codec-active',
				      'checked'	=> $codec_active),
				'onclick="xivo_chg_attrib(\'ast_fm_trunk_codec\',
							  \'it-protocol-disallow\',
							  Number((this.checked === false)));"'),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_codec-disallow'),
				    'name'	=> 'protocol[disallow]',
				    'labelid'	=> 'protocol-disallow',
				    'key'	=> false,
				    'bbf'	=> array('concatvalue','fm_protocol_codec-disallow-opt-')),
			      $element['protocol']['disallow']['value']);
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
				      $element['protocol']['allow']['value']);
?>
	</div>

	<div class="inout-list">
		<a href="#"
		   onclick="xivo_fm_move_selected('it-codeclist',
						  'it-codec');
			    return(xivo.dom.free_focus());"
		   title="<?=$this->bbf('bt_incodec');?>">
			<?=$url->img_html('img/site/button/row-left.gif',
					  $this->bbf('bt_incodec'),
					  'class="bt-inlist" id="bt-incodec" border="0"');?></a><br />
		<a href="#"
		   onclick="xivo_fm_move_selected('it-codec',
						  'it-codeclist');
			    return(xivo.dom.free_focus());"
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
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_upcodec');?>">
				<?=$url->img_html('img/site/button/row-up.gif',
						  $this->bbf('bt_upcodec'),
						  'class="bt-uplist" id="bt-upcodec" border="0"');?></a><br />
			<a href="#"
			   onclick="xivo_fm_order_selected('it-codec',-1);
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_downcodec');?>">
				<?=$url->img_html('img/site/button/row-down.gif',
						  $this->bbf('bt_downcodec'),
						  'class="bt-downlist" id="bt-downcodec" border="0"');?></a>
		</div>
	</div>
</div>
<div class="clearboth"></div>

</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_protocol_insecure'),
				    'name'	=> 'protocol[insecure]',
				    'labelid'	=> 'protocol-insecure',
				    'empty'	=> true,
				    'bbf'	=> array('concatvalue','fm_protocol_insecure-opt-'),
				    'default'	=> $element['protocol']['insecure']['default'],
				    'value'	=> $info['protocol']['insecure']),
			      $element['protocol']['insecure']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_port'),
				  'name'	=> 'protocol[port]',
				  'labelid'	=> 'protocol-port',
				  'size'	=> 10,
				  'default'	=> $element['protocol']['port']['default'],
				  'value'	=> $info['protocol']['port'])),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_permit'),
				  'name'	=> 'protocol[permit]',
				  'labelid'	=> 'protocol-permit',
				  'size'	=> 20,
				  'default'	=> $element['protocol']['permit']['default'],
				  'value'	=> $info['protocol']['permit'])),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_deny'),
				  'name'	=> 'protocol[deny]',
				  'labelid'	=> 'protocol-deny',
				  'size'	=> 20,
				  'default'	=> $element['protocol']['deny']['default'],
				  'value'	=> $info['protocol']['deny'])),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_trustrpid'),
				    'name'	=> 'protocol[trustrpid]',
				    'labelid'	=> 'protocol-trustrpid',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['trustrpid']['default'],
				    'value'	=> $info['protocol']['trustrpid']),
			      $element['protocol']['trustrpid']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_sendrpid'),
				    'name'	=> 'protocol[sendrpid]',
				    'labelid'	=> 'protocol-sendrpid',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['sendrpid']['default'],
				    'value'	=> $info['protocol']['sendrpid']),
			      $element['protocol']['sendrpid']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_allowsubscribe'),
				    'name'	=> 'protocol[allowsubscribe]',
				    'labelid'	=> 'protocol-allowsubscribe',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['allowsubscribe']['default'],
				    'value'	=> $info['protocol']['allowsubscribe']),
			      $element['protocol']['allowsubscribe']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_allowoverlap'),
				    'name'	=> 'protocol[allowoverlap]',
				    'labelid'	=> 'protocol-allowoverlap',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['allowoverlap']['default'],
				    'value'	=> $info['protocol']['allowoverlap']),
			      $element['protocol']['allowoverlap']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_promiscredir'),
				    'name'	=> 'protocol[promiscredir]',
				    'labelid'	=> 'protocol-promiscredir',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['promiscredir']['default'],
				    'value'	=> $info['protocol']['promiscredir']),
			      $element['protocol']['promiscredir']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_usereqphone'),
				    'name'	=> 'protocol[usereqphone]',
				    'labelid'	=> 'protocol-usereqphone',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['usereqphone']['default'],
				    'value'	=> $info['protocol']['usereqphone']),
			      $element['protocol']['usereqphone']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_canreinvite'),
				    'name'	=> 'protocol[canreinvite]',
				    'labelid'	=> 'protocol-canreinvite',
				    'empty'	=> true,
				    'bbf'	=> array('concatvalue','fm_protocol_canreinvite-opt-'),
				    'default'	=> $element['protocol']['canreinvite']['default'],
				    'value'	=> $info['protocol']['canreinvite']),
			      $element['protocol']['canreinvite']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_fromuser'),
				  'name'	=> 'protocol[fromuser]',
				  'labelid'	=> 'protocol-fromuser',
				  'size'	=> 15,
				  'default'	=> $element['protocol']['fromuser']['default'],
				  'value'	=> $info['protocol']['fromuser'])),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_fromdomain'),
				  'name'	=> 'protocol[fromdomain]',
				  'labelid'	=> 'protocol-fromdomain',
				  'size'	=> 15,
				  'default'	=> $element['protocol']['fromdomain']['default'],
				  'value'	=> $info['protocol']['fromdomain'])),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_amaflags'),
				    'name'	=> 'protocol[amaflags]',
				    'labelid'	=> 'sip-protocol-amaflags',
				    'key'	=> false,
				    'bbf'	=> 'ast_amaflag_name_info',
				    'bbf_opt'	=> array('argmode' => 'paramkey'),
				    'default'	=> $element['protocol']['amaflags']['default'],
				    'value'	=> $info['protocol']['amaflags']['default']),
			      $element['protocol']['amaflags']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_protocol_accountcode'),
				  'name'	=> 'protocol[accountcode]',
				  'labelid'	=> 'protocol-accountcode',
				  'size'	=> 15,
				  'default'	=> $element['protocol']['accountcode']['default'],
				  'value'	=> $info['protocol']['accountcode'])),

		$form->select(array('desc'	=> $this->bbf('fm_protocol_useclientcode'),
				    'name'	=> 'protocol[useclientcode]',
				    'labelid'	=> 'protocol-useclientcode',
				    'key'	=> false,
				    'empty'	=> true,
				    'bbf'	=> 'fm_bool-opt-',
				    'default'	=> $element['protocol']['useclientcode']['default'],
				    'value'	=> $info['protocol']['useclientcode']),
			      $element['protocol']['useclientcode']['value']);
?>
	<div class="fm-field fm-description">
		<p>
			<label id="lb-trunkfeatures-description" for="it-trunkfeatures-description">
				<?=$this->bbf('fm_trunkfeatures_description');?>
			</label>
		</p>
		<?=$form->textarea(array('field'	=> false,
					 'label'	=> false,
					 'name'		=> 'trunkfeatures[description]',
					 'id'		=> 'it-trunkfeatures-description',
					 'cols'		=> 60,
					 'rows'		=> 5,
					 'default'	=> $element['trunkfeatures']['description']['default']),
				   $info['trunkfeatures']['description']);?>
	</div>
</div>
