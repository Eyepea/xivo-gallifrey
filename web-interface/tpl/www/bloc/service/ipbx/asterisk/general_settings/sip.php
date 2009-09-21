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
$dhtml = &$this->get_module('dhtml');

$element = $this->get_var('element');
$moh_list = $this->get_var('moh_list');
$context_list = $this->get_var('context_list');

if(($fm_save = $this->get_var('fm_save')) === true):
	$dhtml->write_js('xivo_form_result(true,\''.$dhtml->escape($this->bbf('fm_success-save')).'\');');
elseif($fm_save === false):
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>
<div class="b-infos b-form">
<h3 class="sb-top xspan">
	<span class="span-left">&nbsp;</span>
	<span class="span-center"><?=$this->bbf('title_content_name');?></span>
	<span class="span-right">&nbsp;</span>
</h3>
<div class="sb-smenu">
	<ul>
		<li id="smenu-tab-1"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-first');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_general');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-network');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_network');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-signalling');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_signalling');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-4"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-t38');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_t38');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-5"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-jitterbuffer');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_jitterbuffer');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-6"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-default');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_default');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-7"
		    class="moo-last"
		    onclick="xivo_smenu_click(this,'moc','sb-part-last',1);"
		    onmouseout="xivo_smenu_out(this,'moo',1);"
		    onmouseover="xivo_smenu_over(this,'mov',1);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_realtime');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_fm_select('it-localnet'); xivo_fm_select('it-codec');">

<?php
	echo	$form->hidden(array('name'	=> XIVO_SESS_NAME,
				    'value'	=> XIVO_SESS_ID)),
		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1));
?>

<div id="sb-part-first">

<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_bindport'),
				  'name'	=> 'bindport',
				  'labelid'	=> 'bindport',
				  'value'	=> $this->get_varra('info',array('bindport','var_val')),
				  'default'	=> $element['bindport']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_bindaddr'),
				  'name'	=> 'bindaddr',
				  'labelid'	=> 'bindaddr',
				  'size'	=> 15,
				  'value'	=> $this->get_varra('info',array('bindaddr','var_val')),
				  'default'	=> $element['bindaddr']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_videosupport'),
				      'name'	=> 'videosupport',
				      'labelid'	=> 'videosupport',
				      'checked'	=> $this->get_varra('info',array('videosupport','var_val')),
				      'default'	=> $element['videosupport']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_autocreatepeer'),
				      'name'	=> 'autocreatepeer',
				      'labelid'	=> 'autocreatepeer',
				      'checked'	=> $this->get_varra('info',array('autocreatepeer','var_val')),
				      'default'	=> $element['autocreatepeer']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_allowguest'),
				      'name'	=> 'allowguest',
				      'labelid'	=> 'allowguest',
				      'checked'	=> $this->get_varra('info',array('allowguest','var_val')),
				      'default'	=> $element['allowguest']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_allowsubscribe'),
				      'name'	=> 'allowsubscribe',
				      'labelid'	=> 'allowsubscribe',
				      'checked'	=> $this->get_varra('info',array('allowsubscribe','var_val')),
				      'default'	=> $element['allowsubscribe']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_allowoverlap'),
				      'name'	=> 'allowoverlap',
				      'labelid'	=> 'allowoverlap',
				      'checked'	=> $this->get_varra('info',array('allowoverlap','var_val')),
				      'default'	=> $element['allowoverlap']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_promiscredir'),
				      'name'	=> 'promiscredir',
				      'labelid'	=> 'promiscredir',
				      'checked'	=> $this->get_varra('info',array('promiscredir','var_val')),
				      'default'	=> $element['promiscredir']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_autodomain'),
				      'name'	=> 'autodomain',
				      'labelid'	=> 'autodomain',
				      'checked'	=> $this->get_varra('info',array('autodomain','var_val')),
				      'default'	=> $element['autodomain']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_domain'),
				  'name'	=> 'domain',
				  'labelid'	=> 'domain',
				  'size'	=> 15,
				  'value'	=> $this->get_varra('info',array('domain','var_val')),
				  'default'	=> $element['domain']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_allowexternaldomains'),
				      'name'	=> 'allowexternaldomains',
				      'labelid'	=> 'allowexternaldomains',
				      'checked'	=> $this->get_varra('info',array('allowexternaldomains','var_val')),
				      'default'	=> $element['allowexternaldomains']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_usereqphone'),
				      'name'	=> 'usereqphone',
				      'labelid'	=> 'usereqphone',
				      'checked'	=> $this->get_varra('info',array('usereqphone','var_val')),
				      'default'	=> $element['usereqphone']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_realm'),
				  'name'	=> 'realm',
				  'labelid'	=> 'realm',
				  'size'	=> 15,
				  'value'	=> $this->get_varra('info',array('realm','var_val')),
				  'default'	=> $element['realm']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_alwaysauthreject'),
				      'name'	=> 'alwaysauthreject',
				      'labelid'	=> 'alwaysauthreject',
				      'checked'	=> $this->get_varra('info',array('alwaysauthreject','var_val')),
				      'default'	=> $element['alwaysauthreject']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_limitonpeer'),
				      'name'	=> 'limitonpeer',
				      'labelid'	=> 'limitonpeer',
				      'checked'	=> $this->get_varra('info',array('limitonpeer','var_val')),
				      'default'	=> $element['limitonpeer']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_useragent'),
				  'name'	=> 'useragent',
				  'labelid'	=> 'useragent',
				  'size'	=> 15,
				  'value'	=> $this->get_varra('info',array('useragent','var_val')),
				  'default'	=> $element['useragent']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_checkmwi'),
				    'name'	=> 'checkmwi',
				    'labelid'	=> 'checkmwi',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_checkmwi-opt'),
				    'value'	=> $this->get_varra('info',array('checkmwi','var_val')),
				    'default'	=> $element['checkmwi']['default']),
			      $element['checkmwi']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_buggymwi'),
				      'name'	=> 'buggymwi',
				      'labelid'	=> 'buggymwi',
				      'checked'	=> $this->get_varra('info',array('buggymwi','var_val')),
				      'default'	=> $element['buggymwi']['default']));

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_regcontext'),
				    'name'	=> 'regcontext',
				    'labelid'	=> 'regcontext',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'empty'	=> true,
				    'default'	=> $element['regcontext']['default'],
				    'value'	=> $this->get_varra('info',array('regcontext','var_val'))),
			      $context_list);
endif;

	echo	$form->text(array('desc'	=> $this->bbf('fm_callerid'),
				  'name'	=> 'callerid',
				  'labelid'	=> 'callerid',
				  'size'	=> 15,
				  'notag'	=> false,
				  'value'	=> $this->get_varra('info',array('callerid','var_val')),
				  'default'	=> $element['callerid']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_fromdomain'),
				  'name'	=> 'fromdomain',
				  'labelid'	=> 'fromdomain',
				  'size'	=> 15,
				  'value'	=> $this->get_varra('info',array('fromdomain','var_val')),
				  'default'	=> $element['fromdomain']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_sipdebug'),
				      'name'	=> 'sipdebug',
				      'labelid'	=> 'sipdebug',
				      'checked'	=> $this->get_varra('info',array('sipdebug','var_val')),
				      'default'	=> $element['sipdebug']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_dumphistory'),
				      'name'	=> 'dumphistory',
				      'labelid'	=> 'dumphistory',
				      'checked'	=> $this->get_varra('info',array('dumphistory','var_val')),
				      'default'	=> $element['dumphistory']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_recordhistory'),
				      'name'	=> 'recordhistory',
				      'labelid'	=> 'recordhistory',
				      'checked'	=> $this->get_varra('info',array('recordhistory','var_val')),
				      'default'	=> $element['recordhistory']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_callevents'),
				      'name'	=> 'callevents',
				      'labelid'	=> 'callevents',
				      'checked'	=> $this->get_varra('info',array('callevents','var_val')),
				      'default'	=> $element['callevents']['default']),
				'disabled="disabled"'),

		$form->select(array('desc'	=> $this->bbf('fm_tos-sip'),
				    'name'	=> 'tos_sip',
				    'labelid'	=> 'tos-sip',
				    'key'	=> false,
				    'empty'	=> true,
				    'value'	=> $this->get_varra('info',array('tos_sip','var_val')),
				    'default'	=> $element['tos_sip']['default']),
			      $element['tos_sip']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_tos-audio'),
				    'name'	=> 'tos_audio',
				    'labelid'	=> 'tos-audio',
				    'key'	=> false,
				    'empty'	=> true,
				    'value'	=> $this->get_varra('info',array('tos_audio','var_val')),
				    'default'	=> $element['tos_audio']['default']),
			      $element['tos_audio']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_tos-video'),
				    'name'	=> 'tos_video',
				    'labelid'	=> 'tos-video',
				    'key'	=> false,
				    'empty'	=> true,
				    'value'	=> $this->get_varra('info',array('tos_video','var_val')),
				    'default'	=> $element['tos_video']['default']),
			      $element['tos_video']['value']);
?>
</div>

<div id="sb-part-network" class="b-nodisplay">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_externip'),
				  'name'	=> 'externip',
				  'labelid'	=> 'externip',
				  'size'	=> 15,
				  'value'	=> $this->get_varra('info',array('externip','var_val')),
				  'default'	=> $element['externip']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_externhost'),
				  'name'	=> 'externhost',
				  'labelid'	=> 'externhost',
				  'size'	=> 15,
				  'value'	=> $this->get_varra('info',array('externhost','var_val')),
				  'default'	=> $element['externhost']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_externrefresh'),
				    'name'	=> 'externrefresh',
				    'labelid'	=> 'externrefresh',
				    'key'	=> false,
				    'bbf'	=> 'fm_externrefresh-opt',
				    'bbf_opt'	=> array('argmode'	=> 'paramkey',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'value'	=> $this->get_varra('info',array('externrefresh','var_val')),
				    'default'	=> $element['externrefresh']['default']),
			      $element['externrefresh']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_matchexterniplocally'),
				      'name'	=> 'matchexterniplocally',
				      'labelid'	=> 'matchexterniplocally',
				      'checked'	=> $this->get_varra('info',array('matchexterniplocally','var_val')),
				      'default'	=> $element['matchexterniplocally']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_outboundproxy'),
				  'name'	=> 'outboundproxy',
				  'labelid'	=> 'outboundproxy',
				  'size'	=> 15,
				  'value'	=> $this->get_varra('info',array('outboundproxy','var_val')),
				  'default'	=> $element['outboundproxy']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_outboundproxyport'),
				  'name'	=> 'outboundproxyport',
				  'labelid'	=> 'outboundproxyport',
				  'value'	=> $this->get_varra('info',array('outboundproxyport','var_val')),
				  'default'	=> $element['outboundproxyport']['default']));
?>

<div id="localnetlist" class="fm-field fm-multilist">
	<p>
		<label id="lb-localnetlist" for="it-localnet">
			<?=$this->bbf('fm_localnet');?>
		</label>
	</p>
	<div class="slt-list">
		<?=$form->select(array('name'		=> 'localnet[]',
				       'label'		=> false,
				       'id'		=> 'it-localnet',
				       'key'		=> 'var_val',
				       'altkey'		=> 'var_val',
				       'multiple'	=> true,
				       'size'		=> 5,
				       'field'		=> false),$this->get_varra('info','localnet'));?>
		<div class="bt-adddelete">
			<a href="#"
			   onclick="xivo_fm_select_add_host_ipv4_subnet('it-localnet',
									prompt('<?=$dhtml->escape($this->bbf('localnet_add'));?>'));
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_add-localnet');?>">
				<?=$url->img_html('img/site/button/mini/blue/add.gif',
						  $this->bbf('bt_add-localnet'),
						  'class="bt-addlist" id="bt-add-localnet" border="0"');?></a><br />
			<a href="#"
			   onclick="xivo_fm_select_delete_entry('it-localnet');
				    return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_delete-localnet');?>">
				<?=$url->img_html('img/site/button/mini/orange/delete.gif',
						  $this->bbf('bt_del-localnet'),
						  'class="bt-deletelist" id="bt-del-localnet" border="0"');?></a>
		</div>
	</div>
</div>
<div class="clearboth"></div>
</div>

<div id="sb-part-signalling" class="b-nodisplay">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_t1min'),
				    'name'	=> 't1min',
				    'labelid'	=> 't1min',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_t1min-opt'),
				    'value'	=> $this->get_varra('info',array('t1min','var_val')),
				    'default'	=> $element['t1min']['default']),
			      $element['t1min']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_relaxdtmf'),
				      'name'	=> 'relaxdtmf',
				      'labelid'	=> 'relaxdtmf',
				      'checked'	=> $this->get_varra('info',array('relaxdtmf','var_val')),
				      'default'	=> $element['relaxdtmf']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_rfc2833compensate'),
				      'name'	=> 'rfc2833compensate',
				      'labelid'	=> 'rfc2833compensate',
				      'checked'	=> $this->get_varra('info',array('rfc2833compensate','var_val')),
				      'default'	=> $element['rfc2833compensate']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_compactheaders'),
				      'name'	=> 'compactheaders',
				      'labelid'	=> 'compactheaders',
				      'checked'	=> $this->get_varra('info',array('compactheaders','var_val')),
				      'default'	=> $element['compactheaders']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_rtptimeout'),
				    'name'	=> 'rtptimeout',
				    'labelid'	=> 'rtptimeout',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_rtptimeout-opt'),
				    'value'	=> $this->get_varra('info',array('rtptimeout','var_val')),
				    'default'	=> $element['rtptimeout']['default']),
			      $element['rtptimeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_rtpholdtimeout'),
				    'name'	=> 'rtpholdtimeout',
				    'labelid'	=> 'rtpholdtimeout',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_rtptimeout-opt'),
				    'value'	=> $this->get_varra('info',array('rtpholdtimeout','var_val')),
				    'default'	=> $element['rtpholdtimeout']['default']),
			      $element['rtpholdtimeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_rtpkeepalive'),
				    'name'	=> 'rtpkeepalive',
				    'labelid'	=> 'rtpkeepalive',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_rtpkeepalive-opt'),
				    'value'	=> $this->get_varra('info',array('rtpkeepalive','var_val')),
				    'default'	=> $element['rtpkeepalive']['default']),
			      $element['rtpkeepalive']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_directrtpsetup'),
				      'name'	=> 'directrtpsetup',
				      'labelid'	=> 'directrtpsetup',
				      'checked'	=> $this->get_varra('info',array('directrtpsetup','var_val')),
				      'default'	=> $element['directrtpsetup']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_notifymimetype'),
				  'name'	=> 'notifymimetype',
				  'labelid'	=> 'notifymimetype',
				  'size'	=> 15,
				  'value'	=> $this->get_varra('info',array('notifymimetype','var_val')),
				  'default'	=> $element['notifymimetype']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_srvlookup'),
				      'name'	=> 'srvlookup',
				      'labelid'	=> 'srvlookup',
				      'checked'	=> $this->get_varra('info',array('srvlookup','var_val')),
				      'default'	=> $element['srvlookup']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_pedantic'),
				      'name'	=> 'pedantic',
				      'labelid'	=> 'pedantic',
				      'checked'	=> $this->get_varra('info',array('pedantic','var_val')),
				      'default'	=> $element['pedantic']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_minexpiry'),
				    'name'	=> 'minexpiry',
				    'labelid'	=> 'minexpiry',
				    'key'	=> false,
				    'bbf'	=> 'fm_expiry-opt',
				    'bbf_opt'	=> array('argmode'	=> 'paramkey',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%H%M%s')),
				    'value'	=> $this->get_varra('info',array('minexpiry','var_val')),
				    'default'	=> $element['minexpiry']['default']),
			      $element['minexpiry']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_maxexpiry'),
				    'name'	=> 'maxexpiry',
				    'labelid'	=> 'maxexpiry',
				    'key'	=> false,
				    'bbf'	=> 'fm_expiry-opt',
				    'bbf_opt'	=> array('argmode'	=> 'paramkey',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%H%M%s')),
				    'value'	=> $this->get_varra('info',array('maxexpiry','var_val')),
				    'default'	=> $element['maxexpiry']['default']),
			      $element['maxexpiry']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_defaultexpiry'),
				    'name'	=> 'defaultexpiry',
				    'labelid'	=> 'defaultexpiry',
				    'key'	=> false,
				    'bbf'	=> 'fm_expiry-opt',
				    'bbf_opt'	=> array('argmode'	=> 'paramkey',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%H%M%s')),
				    'value'	=> $this->get_varra('info',array('defaultexpiry','var_val')),
				    'default'	=> $element['defaultexpiry']['default']),
			      $element['defaultexpiry']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_registertimeout'),
				    'name'	=> 'registertimeout',
				    'labelid'	=> 'registertimeout',
				    'key'	=> false,
				    'bbf'	=> 'fm_registertimeout-opt',
				    'bbf_opt'	=> array('argmode'	=> 'paramkey',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'value'	=> $this->get_varra('info',array('registertimeout','var_val')),
				    'default'	=> $element['registertimeout']['default']),
			      $element['registertimeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_registerattempts'),
				    'name'	=> 'registerattempts',
				    'labelid'	=> 'registerattempts',
				    'key'	=> false,
				    'bbf'	=> 'fm_registerattempts-opt',
				    'bbf_opt'	=> array('argmode' => 'paramkey'),
				    'value'	=> $this->get_varra('info',array('registerattempts','var_val')),
				    'default'	=> $element['registerattempts']['default']),
			      $element['registerattempts']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_notifyringing'),
				      'name'	=> 'notifyringing',
				      'labelid'	=> 'notifyringing',
				      'checked'	=> $this->get_varra('info',array('notifyringing','var_val')),
				      'default'	=> $element['notifyringing']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_notifyhold'),
				      'name'	=> 'notifyhold',
				      'labelid'	=> 'notifyhold',
				      'checked'	=> $this->get_varra('info',array('notifyhold','var_val')),
				      'default'	=> $element['notifyhold']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_allowtransfer'),
				      'name'	=> 'allowtransfer',
				      'labelid'	=> 'allowtransfer',
				      'checked'	=> $this->get_varra('info',array('allowtransfer','var_val')),
				      'default'	=> $element['allowtransfer']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_maxcallbitrate'),
				  'name'	=> 'maxcallbitrate',
				  'labelid'	=> 'maxcallbitrate',
				  'size'	=> 10,
				  'value'	=> $this->get_varra('info',array('maxcallbitrate','var_val')),
				  'default'	=> $element['maxcallbitrate']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_autoframing'),
				      'name'	=> 'autoframing',
				      'labelid'	=> 'autoframing',
				      'checked'	=> $this->get_varra('info',array('autoframing','var_val')),
				      'default'	=> $element['autoframing']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_g726nonstandard'),
				      'name'	=> 'g726nonstandard',
				      'labelid'	=> 'g726nonstandard',
				      'checked'	=> $this->get_varra('info',array('g726nonstandard','var_val')),
				      'default'	=> $element['g726nonstandard']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_codec-disallow'),
				    'name'	=> 'disallow',
				    'labelid'	=> 'disallow',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_codec-disallow-opt')),
			      $element['disallow']['value']);
?>

<div id="codeclist" class="fm-field fm-multilist">
	<p>
		<label id="lb-codeclist" for="it-codeclist">
			<?=$this->bbf('fm_codec-allow');?>
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
			       $element['allow']['value']);
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
		<?=$form->select(array('name'		=> 'allow[]',
				       'label'		=> false,
				       'id'		=> 'it-codec',
				       'multiple'	=> true,
				       'size'		=> 5,
				       'field'		=> false,
				       'key'		=> false,
				       'bbf'		=> 'ast_codec_name_type-'),
				 $this->get_varra('info',array('allow','var_val')));?>
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

<div id="sb-part-t38" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_t38pt-udptl'),
				      'name'	=> 't38pt_udptl',
				      'labelid'	=> 't38pt-udptl',
				      'checked'	=> $this->get_varra('info',array('t38pt_udptl','var_val')),
				      'default'	=> $element['t38pt_udptl']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_t38pt-rtp'),
				      'name'	=> 't38pt_rtp',
				      'labelid'	=> 't38pt-rtp',
				      'checked'	=> $this->get_varra('info',array('t38pt_rtp','var_val')),
				      'default'	=> $element['t38pt_rtp']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_t38pt-tcp'),
				      'name'	=> 't38pt_tcp',
				      'labelid'	=> 't38pt-tcp',
				      'checked'	=> $this->get_varra('info',array('t38pt_tcp','var_val')),
				      'default'	=> $element['t38pt_tcp']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_t38pt-usertpsource'),
				      'name'	=> 't38pt_usertpsource',
				      'labelid'	=> 't38pt-usertpsource',
				      'checked'	=> $this->get_varra('info',array('t38pt_usertpsource','var_val')),
				      'default'	=> $element['t38pt_usertpsource']['default']));
?>
</div>

<div id="sb-part-jitterbuffer" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_jbenable'),
				      'name'	=> 'jbenable',
				      'labelid'	=> 'jbenable',
				      'checked'	=> $this->get_varra('info',array('jbenable','var_val')),
				      'default'	=> $element['jbenable']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_jbforce'),
				      'name'	=> 'jbforce',
				      'labelid'	=> 'jbforce',
				      'checked'	=> $this->get_varra('info',array('jbforce','var_val')),
				      'default'	=> $element['jbforce']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_jbmaxsize'),
				    'name'	=> 'jbmaxsize',
				    'labelid'	=> 'jbmaxsize',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_jbmaxsize-opt'),
				    'value'	=> $this->get_varra('info',array('jbmaxsize','var_val')),
				    'default'	=> $element['jbmaxsize']['default']),
			      $element['jbmaxsize']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_jbresyncthreshold'),
				    'name'	=> 'jbresyncthreshold',
				    'labelid'	=> 'jbresyncthreshold',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_jbresyncthreshold-opt'),
				    'value'	=> $this->get_varra('info',array('jbresyncthreshold','var_val')),
				    'default'	=> $element['jbresyncthreshold']['default']),
			      $element['jbresyncthreshold']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_jbimpl'),
				    'name'	=> 'jbimpl',
				    'labelid'	=> 'jbimpl',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_jbimpl-opt'),
				    'value'	=> $this->get_varra('info',array('jbimpl','var_val')),
				    'default'	=> $element['jbimpl']['default']),
			      $element['jbimpl']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_jblog'),
				      'name'	=> 'jblog',
				      'labelid'	=> 'jblog',
				      'checked'	=> $this->get_varra('info',array('jblog','var_val')),
				      'default'	=> $element['jblog']['default']));
?>
</div>

<div id="sb-part-default" class="b-nodisplay">
<?php

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_context'),
				    'name'	=> 'context',
				    'labelid'	=> 'context',
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'empty'	=> true,
				    'default'	=> $element['context']['default'],
				    'value'	=> $this->get_varra('info',array('context','var_val'))),
			      $context_list);
endif;

	echo	$form->select(array('desc'	=> $this->bbf('fm_nat'),
				    'name'	=> 'nat',
				    'labelid'	=> 'nat',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_nat-opt'),
				    'value'	=> $this->get_varra('info',array('nat','var_val')),
				    'default'	=> $element['nat']['default']),
			      $element['nat']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_dtmfmode'),
				    'name'	=> 'dtmfmode',
				    'labelid'	=> 'dtmfmode',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_dtmfmode-opt'),
				    'value'	=> $this->get_varra('info',array('dtmfmode','var_val')),
				    'default'	=> $element['dtmfmode']['default']),
			      $element['dtmfmode']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_qualify'),
				    'name'	=> 'qualify',
				    'labelid'	=> 'qualify',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_qualify-opt'),
				    'value'	=> $this->get_varra('info',array('qualify','var_val')),
				    'default'	=> $element['qualify']['default']),
			      $element['qualify']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_useclientcode'),
				      'name'	=> 'useclientcode',
				      'labelid'	=> 'useclientcode',
				      'checked'	=> $this->get_varra('info',array('useclientcode','var_val')),
				      'default'	=> $element['useclientcode']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_progressinband'),
				    'name'	=> 'progressinband',
				    'labelid'	=> 'progressinband',
				    'key'	=> false,
				    'bbf'	=> array('paramkey','fm_progressinband-opt'),
				    'value'	=> $this->get_varra('info',array('progressinband','var_val')),
				    'default'	=> $element['progressinband']['default']),
			      $element['progressinband']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_language'),
				    'name'	=> 'language',
				    'labelid'	=> 'language',
				    'key'	=> false,
				    'value'	=> $this->get_varra('info',array('language','var_val')),
				    'default'	=> $element['language']['default']),
			      $element['language']['value']);

if($moh_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_mohinterpret'),
				    'name'	=> 'mohinterpret',
				    'labelid'	=> 'mohinterpret',
				    'key'	=> 'category',
				    'value'	=> $this->get_varra('info',array('mohinterpret','var_val')),
				    'default'	=> $element['mohinterpret']['default']),
			      $moh_list),

		$form->select(array('desc'	=> $this->bbf('fm_mohsuggest'),
				    'name'	=> 'mohsuggest',
				    'labelid'	=> 'mohsuggest',
				    'empty'	=> true,
				    'key'	=> 'category',
				    'value'	=> $this->get_varra('info',array('mohsuggest','var_val')),
				    'default'	=> $element['mohsuggest']['default']),
			      $moh_list);
endif;

	echo	$form->text(array('desc'	=> $this->bbf('fm_vmexten'),
				  'name'	=> 'vmexten',
				  'labelid'	=> 'vmexten',
				  'value'	=> $this->get_varra('info',array('vmexten','var_val')),
				  'default'	=> $element['vmexten']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_trustrpid'),
				      'name'	=> 'trustrpid',
				      'labelid'	=> 'trustrpid',
				      'checked'	=> $this->get_varra('info',array('trustrpid','var_val')),
				      'default'	=> $element['trustrpid']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_sendrpid'),
				      'name'	=> 'sendrpid',
				      'labelid'	=> 'sendrpid',
				      'checked'	=> $this->get_varra('info',array('sendrpid','var_val')),
				      'default'	=> $element['sendrpid']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_assertedidentity'),
				      'name'	=> 'assertedidentity',
				      'labelid'	=> 'assertedidentity',
				      'checked'	=> $this->get_varra('info',array('assertedidentity','var_val')),
				      'default'	=> $element['assertedidentity']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_canreinvite'),
				    'name'	=> 'canreinvite',
				    'labelid'	=> 'canreinvite',
				    'bbf'	=> array('paramvalue','fm_canreinvite-opt'),
				    'value'	=> $this->get_varra('info',array('canreinvite','var_val')),
				    'default'	=> $element['canreinvite']['default']),
			      $element['canreinvite']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_insecure'),
				    'name'	=> 'insecure',
				    'labelid'	=> 'insecure',
				    'bbf'	=> array('paramvalue','fm_insecure-opt'),
				    'value'	=> $this->get_varra('info',array('insecure','var_val')),
				    'default'	=> $element['insecure']['default']),
			      $element['insecure']['value']);
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_rtcachefriends'),
				      'name'	=> 'rtcachefriends',
				      'labelid'	=> 'rtcachefriends',
				      'checked'	=> $this->get_varra('info',array('rtcachefriends','var_val')),
				      'default'	=> $element['rtcachefriends']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_rtupdate'),
				      'name'	=> 'rtupdate',
				      'labelid'	=> 'rtupdate',
				      'checked'	=> $this->get_varra('info',array('rtupdate','var_val')),
				      'default'	=> $element['rtupdate']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_ignoreregexpire'),
				      'name'	=> 'ignoreregexpire',
				      'labelid'	=> 'ignoreregexpire',
				      'checked'	=> $this->get_varra('info',array('ignoreregexpire','var_val')),
				      'default'	=> $element['ignoreregexpire']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_rtsavesysname'),
				      'name'	=> 'rtsavesysname',
				      'labelid'	=> 'rtsavesysname',
				      'checked'	=> $this->get_varra('info',array('rtsavesysname','var_val')),
				      'default'	=> $element['rtsavesysname']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_rtautoclear'),
				    'name'	=> 'rtautoclear',
				    'labelid'	=> 'rtautoclear',
				    'key'	=> false,
				    'bbf'	=> 'fm_rtautoclear-opt',
				    'bbf_opt'	=> array('argmode'	=> 'paramkey',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'value'	=> $this->get_varra('info',array('rtautoclear','var_val')),
				    'default'	=> $element['rtautoclear']['default']),
			      $element['rtautoclear']['value']);
?>
</div>
	<?=$form->submit(array('name'	=> 'submit',
			       'id'	=> 'it-submit',
			       'value'	=> $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>
