<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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
		<li id="dwsm-tab-1"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-first');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_general');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-2"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-network');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_network');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-3"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-signalling');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_signalling');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-4"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-t38');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_t38');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-5"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-jitterbuffer');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_jitterbuffer');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-6"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-default');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_default');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-7"
		    class="dwsm-blur-last"
		    onclick="dwho_submenu.select(this,'sb-part-last',1);"
		    onmouseout="dwho_submenu.blur(this,1);"
		    onmouseover="dwho_submenu.focus(this,1);">
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
<form action="#" method="post" accept-charset="utf-8" onsubmit="dwho.form.select('it-localnet'); dwho.form.select('it-codec');">

<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),
		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1));
?>

<div id="sb-part-first">

<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_bindport'),
				  'name'	=> 'bindport',
				  'labelid'	=> 'bindport',
				  'value'	=> $this->get_var('info','bindport','var_val'),
				  'default'	=> $element['bindport']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_bindaddr'),
				  'name'	=> 'bindaddr',
				  'labelid'	=> 'bindaddr',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info','bindaddr','var_val'),
				  'default'	=> $element['bindaddr']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_videosupport'),
				      'name'	=> 'videosupport',
				      'labelid'	=> 'videosupport',
				      'checked'	=> $this->get_var('info','videosupport','var_val'),
				      'default'	=> $element['videosupport']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_autocreatepeer'),
				      'name'	=> 'autocreatepeer',
				      'labelid'	=> 'autocreatepeer',
				      'checked'	=> $this->get_var('info','autocreatepeer','var_val'),
				      'default'	=> $element['autocreatepeer']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_allowguest'),
				      'name'	=> 'allowguest',
				      'labelid'	=> 'allowguest',
				      'checked'	=> $this->get_var('info','allowguest','var_val'),
				      'default'	=> $element['allowguest']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_allowsubscribe'),
				      'name'	=> 'allowsubscribe',
				      'labelid'	=> 'allowsubscribe',
				      'checked'	=> $this->get_var('info','allowsubscribe','var_val'),
				      'default'	=> $element['allowsubscribe']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_allowoverlap'),
				      'name'	=> 'allowoverlap',
				      'labelid'	=> 'allowoverlap',
				      'checked'	=> $this->get_var('info','allowoverlap','var_val'),
				      'default'	=> $element['allowoverlap']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_promiscredir'),
				      'name'	=> 'promiscredir',
				      'labelid'	=> 'promiscredir',
				      'checked'	=> $this->get_var('info','promiscredir','var_val'),
				      'default'	=> $element['promiscredir']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_autodomain'),
				      'name'	=> 'autodomain',
				      'labelid'	=> 'autodomain',
				      'checked'	=> $this->get_var('info','autodomain','var_val'),
				      'default'	=> $element['autodomain']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_domain'),
				  'name'	=> 'domain',
				  'labelid'	=> 'domain',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info','domain','var_val'),
				  'default'	=> $element['domain']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_allowexternaldomains'),
				      'name'	=> 'allowexternaldomains',
				      'labelid'	=> 'allowexternaldomains',
				      'checked'	=> $this->get_var('info','allowexternaldomains','var_val'),
				      'default'	=> $element['allowexternaldomains']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_usereqphone'),
				      'name'	=> 'usereqphone',
				      'labelid'	=> 'usereqphone',
				      'checked'	=> $this->get_var('info','usereqphone','var_val'),
				      'default'	=> $element['usereqphone']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_realm'),
				  'name'	=> 'realm',
				  'labelid'	=> 'realm',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info','realm','var_val'),
				  'default'	=> $element['realm']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_alwaysauthreject'),
				      'name'	=> 'alwaysauthreject',
				      'labelid'	=> 'alwaysauthreject',
				      'checked'	=> $this->get_var('info','alwaysauthreject','var_val'),
				      'default'	=> $element['alwaysauthreject']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_limitonpeer'),
				      'name'	=> 'limitonpeer',
				      'labelid'	=> 'limitonpeer',
				      'checked'	=> $this->get_var('info','limitonpeer','var_val'),
				      'default'	=> $element['limitonpeer']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_useragent'),
				  'name'	=> 'useragent',
				  'labelid'	=> 'useragent',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info','useragent','var_val'),
				  'default'	=> $element['useragent']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_checkmwi'),
				    'name'	=> 'checkmwi',
				    'labelid'	=> 'checkmwi',
				    'key'	=> false,
				    'bbf'	=> 'fm_checkmwi-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('info','checkmwi','var_val'),
				    'default'	=> $element['checkmwi']['default']),
			      $element['checkmwi']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_buggymwi'),
				      'name'	=> 'buggymwi',
				      'labelid'	=> 'buggymwi',
				      'checked'	=> $this->get_var('info','buggymwi','var_val'),
				      'default'	=> $element['buggymwi']['default']));

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_regcontext'),
				    'name'	=> 'regcontext',
				    'labelid'	=> 'regcontext',
				    'empty'	=> true,
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['regcontext']['default'],
				    'selected'	=> $this->get_var('info','regcontext','var_val')),
			      $context_list);
endif;

	echo	$form->text(array('desc'	=> $this->bbf('fm_callerid'),
				  'name'	=> 'callerid',
				  'labelid'	=> 'callerid',
				  'size'	=> 15,
				  'notag'	=> false,
				  'value'	=> $this->get_var('info','callerid','var_val'),
				  'default'	=> $element['callerid']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_fromdomain'),
				  'name'	=> 'fromdomain',
				  'labelid'	=> 'fromdomain',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info','fromdomain','var_val'),
				  'default'	=> $element['fromdomain']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_sipdebug'),
				      'name'	=> 'sipdebug',
				      'labelid'	=> 'sipdebug',
				      'checked'	=> $this->get_var('info','sipdebug','var_val'),
				      'default'	=> $element['sipdebug']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_dumphistory'),
				      'name'	=> 'dumphistory',
				      'labelid'	=> 'dumphistory',
				      'checked'	=> $this->get_var('info','dumphistory','var_val'),
				      'default'	=> $element['dumphistory']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_recordhistory'),
				      'name'	=> 'recordhistory',
				      'labelid'	=> 'recordhistory',
				      'checked'	=> $this->get_var('info','recordhistory','var_val'),
				      'default'	=> $element['recordhistory']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_callevents'),
				      'name'	=> 'callevents',
				      'labelid'	=> 'callevents',
				      'checked'	=> $this->get_var('info','callevents','var_val'),
				      'default'	=> $element['callevents']['default']),
				'disabled="disabled"'),

		$form->select(array('desc'	=> $this->bbf('fm_tos-sip'),
				    'name'	=> 'tos_sip',
				    'labelid'	=> 'tos-sip',
				    'empty'	=> true,
				    'key'	=> false,
				    'selected'	=> $this->get_var('info','tos_sip','var_val'),
				    'default'	=> $element['tos_sip']['default']),
			      $element['tos_sip']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_tos-audio'),
				    'name'	=> 'tos_audio',
				    'labelid'	=> 'tos-audio',
				    'empty'	=> true,
				    'key'	=> false,
				    'selected'	=> $this->get_var('info','tos_audio','var_val'),
				    'default'	=> $element['tos_audio']['default']),
			      $element['tos_audio']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_tos-video'),
				    'name'	=> 'tos_video',
				    'labelid'	=> 'tos-video',
				    'empty'	=> true,
				    'key'	=> false,
				    'selected'	=> $this->get_var('info','tos_video','var_val'),
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
				  'value'	=> $this->get_var('info','externip','var_val'),
				  'default'	=> $element['externip']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_externhost'),
				  'name'	=> 'externhost',
				  'labelid'	=> 'externhost',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info','externhost','var_val'),
				  'default'	=> $element['externhost']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_externrefresh'),
				    'name'	=> 'externrefresh',
				    'labelid'	=> 'externrefresh',
				    'key'	=> false,
				    'bbf'	=> 'fm_externrefresh-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'selected'	=> $this->get_var('info','externrefresh','var_val'),
				    'default'	=> $element['externrefresh']['default']),
			      $element['externrefresh']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_matchexterniplocally'),
				      'name'	=> 'matchexterniplocally',
				      'labelid'	=> 'matchexterniplocally',
				      'checked'	=> $this->get_var('info','matchexterniplocally','var_val'),
				      'default'	=> $element['matchexterniplocally']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_outboundproxy'),
				  'name'	=> 'outboundproxy',
				  'labelid'	=> 'outboundproxy',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info','outboundproxy','var_val'),
				  'default'	=> $element['outboundproxy']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_outboundproxyport'),
				  'name'	=> 'outboundproxyport',
				  'labelid'	=> 'outboundproxyport',
				  'value'	=> $this->get_var('info','outboundproxyport','var_val'),
				  'default'	=> $element['outboundproxyport']['default']));
?>

<div id="localnetlist" class="fm-paragraph fm-multilist">
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
				       'paragraph'	=> false),
				 $this->get_var('info','localnet'));?>
		<div class="bt-adddelete">
			<a href="#"
			   onclick="xivo_form_select_add_host_ipv4_subnet('it-localnet',
									  prompt('<?=$dhtml->escape($this->bbf('localnet_add'));?>'));
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_add-localnet');?>">
				<?=$url->img_html('img/site/button/mini/blue/add.gif',
						  $this->bbf('bt_add-localnet'),
						  'class="bt-addlist" id="bt-add-localnet" border="0"');?></a><br />
			<a href="#"
			   onclick="dwho.form.select_delete_entry('it-localnet');
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_delete-localnet');?>">
				<?=$url->img_html('img/site/button/mini/orange/delete.gif',
						  $this->bbf('bt_delete-localnet'),
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
				    'bbf'	=> 'fm_t1min-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('info','t1min','var_val'),
				    'default'	=> $element['t1min']['default']),
			      $element['t1min']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_relaxdtmf'),
				      'name'	=> 'relaxdtmf',
				      'labelid'	=> 'relaxdtmf',
				      'checked'	=> $this->get_var('info','relaxdtmf','var_val'),
				      'default'	=> $element['relaxdtmf']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_rfc2833compensate'),
				      'name'	=> 'rfc2833compensate',
				      'labelid'	=> 'rfc2833compensate',
				      'checked'	=> $this->get_var('info','rfc2833compensate','var_val'),
				      'default'	=> $element['rfc2833compensate']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_compactheaders'),
				      'name'	=> 'compactheaders',
				      'labelid'	=> 'compactheaders',
				      'checked'	=> $this->get_var('info','compactheaders','var_val'),
				      'default'	=> $element['compactheaders']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_rtptimeout'),
				    'name'	=> 'rtptimeout',
				    'labelid'	=> 'rtptimeout',
				    'key'	=> false,
				    'bbf'	=> 'fm_rtptimeout-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('info','rtptimeout','var_val'),
				    'default'	=> $element['rtptimeout']['default']),
			      $element['rtptimeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_rtpholdtimeout'),
				    'name'	=> 'rtpholdtimeout',
				    'labelid'	=> 'rtpholdtimeout',
				    'key'	=> false,
				    'bbf'	=> 'fm_rtptimeout-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('info','rtpholdtimeout','var_val'),
				    'default'	=> $element['rtpholdtimeout']['default']),
			      $element['rtpholdtimeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_rtpkeepalive'),
				    'name'	=> 'rtpkeepalive',
				    'labelid'	=> 'rtpkeepalive',
				    'key'	=> false,
				    'bbf'	=> 'fm_rtpkeepalive-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('info','rtpkeepalive','var_val'),
				    'default'	=> $element['rtpkeepalive']['default']),
			      $element['rtpkeepalive']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_directrtpsetup'),
				      'name'	=> 'directrtpsetup',
				      'labelid'	=> 'directrtpsetup',
				      'checked'	=> $this->get_var('info','directrtpsetup','var_val'),
				      'default'	=> $element['directrtpsetup']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_notifymimetype'),
				  'name'	=> 'notifymimetype',
				  'labelid'	=> 'notifymimetype',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info','notifymimetype','var_val'),
				  'default'	=> $element['notifymimetype']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_srvlookup'),
				      'name'	=> 'srvlookup',
				      'labelid'	=> 'srvlookup',
				      'checked'	=> $this->get_var('info','srvlookup','var_val'),
				      'default'	=> $element['srvlookup']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_pedantic'),
				      'name'	=> 'pedantic',
				      'labelid'	=> 'pedantic',
				      'checked'	=> $this->get_var('info','pedantic','var_val'),
				      'default'	=> $element['pedantic']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_minexpiry'),
				    'name'	=> 'minexpiry',
				    'labelid'	=> 'minexpiry',
				    'key'	=> false,
				    'bbf'	=> 'fm_expiry-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%H%M%s')),
				    'selected'	=> $this->get_var('info','minexpiry','var_val'),
				    'default'	=> $element['minexpiry']['default']),
			      $element['minexpiry']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_maxexpiry'),
				    'name'	=> 'maxexpiry',
				    'labelid'	=> 'maxexpiry',
				    'key'	=> false,
				    'bbf'	=> 'fm_expiry-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%H%M%s')),
				    'selected'	=> $this->get_var('info','maxexpiry','var_val'),
				    'default'	=> $element['maxexpiry']['default']),
			      $element['maxexpiry']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_defaultexpiry'),
				    'name'	=> 'defaultexpiry',
				    'labelid'	=> 'defaultexpiry',
				    'key'	=> false,
				    'bbf'	=> 'fm_expiry-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%H%M%s')),
				    'selected'	=> $this->get_var('info','defaultexpiry','var_val'),
				    'default'	=> $element['defaultexpiry']['default']),
			      $element['defaultexpiry']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_registertimeout'),
				    'name'	=> 'registertimeout',
				    'labelid'	=> 'registertimeout',
				    'key'	=> false,
				    'bbf'	=> 'fm_registertimeout-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'selected'	=> $this->get_var('info','registertimeout','var_val'),
				    'default'	=> $element['registertimeout']['default']),
			      $element['registertimeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_registerattempts'),
				    'name'	=> 'registerattempts',
				    'labelid'	=> 'registerattempts',
				    'key'	=> false,
				    'bbf'	=> 'fm_registerattempts-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('info','registerattempts','var_val'),
				    'default'	=> $element['registerattempts']['default']),
			      $element['registerattempts']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_notifyringing'),
				      'name'	=> 'notifyringing',
				      'labelid'	=> 'notifyringing',
				      'checked'	=> $this->get_var('info','notifyringing','var_val'),
				      'default'	=> $element['notifyringing']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_notifyhold'),
				      'name'	=> 'notifyhold',
				      'labelid'	=> 'notifyhold',
				      'checked'	=> $this->get_var('info','notifyhold','var_val'),
				      'default'	=> $element['notifyhold']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_allowtransfer'),
				      'name'	=> 'allowtransfer',
				      'labelid'	=> 'allowtransfer',
				      'checked'	=> $this->get_var('info','allowtransfer','var_val'),
				      'default'	=> $element['allowtransfer']['default'])),

		$form->text(array('desc'	=> $this->bbf('fm_maxcallbitrate'),
				  'name'	=> 'maxcallbitrate',
				  'labelid'	=> 'maxcallbitrate',
				  'size'	=> 10,
				  'value'	=> $this->get_var('info','maxcallbitrate','var_val'),
				  'default'	=> $element['maxcallbitrate']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_autoframing'),
				      'name'	=> 'autoframing',
				      'labelid'	=> 'autoframing',
				      'checked'	=> $this->get_var('info','autoframing','var_val'),
				      'default'	=> $element['autoframing']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_g726nonstandard'),
				      'name'	=> 'g726nonstandard',
				      'labelid'	=> 'g726nonstandard',
				      'checked'	=> $this->get_var('info','g726nonstandard','var_val'),
				      'default'	=> $element['g726nonstandard']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_codec-disallow'),
				    'name'	=> 'disallow',
				    'labelid'	=> 'disallow',
				    'key'	=> false,
				    'bbf'	=> 'fm_codec-disallow-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue')),
			      $element['disallow']['value']);
?>

<div id="codeclist" class="fm-paragraph fm-multilist">
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
				    'paragraph'	=> false,
				    'key'	=> false,
				    'bbf'	=> 'ast_codec_name_type',
				    'bbfopt'	=> array('argmode' => 'paramvalue')),
			       $element['allow']['value']);
?>
	</div>
	<div class="inout-list">
		<a href="#"
		   onclick="dwho.form.move_selected('it-codeclist',
						  'it-codec');
			    return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_incodec');?>">
			<?=$url->img_html('img/site/button/arrow-left.gif',
					  $this->bbf('bt_incodec'),
					  'class="bt-inlist" id="bt-incodec" border="0"');?></a><br />
		<a href="#"
		   onclick="dwho.form.move_selected('it-codec',
						  'it-codeclist');
			    return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_outcodec');?>">
			<?=$url->img_html('img/site/button/arrow-right.gif',
					  $this->bbf('bt_outcodec'),
					  'class="bt-outlist" id="bt-outcodec" border="0"');?></a>
	</div>
	<div class="slt-inlist">
		<?=$form->select(array('name'		=> 'allow[]',
				       'label'		=> false,
				       'id'		=> 'it-codec',
				       'multiple'	=> true,
				       'size'		=> 5,
				       'paragraph'		=> false,
				       'key'		=> false,
				       'bbf'		=> 'ast_codec_name_type',
				       'bbfopt'		=> array('argmode' => 'paramvalue')),
				 $this->get_var('info','allow','var_val'));?>
		<div class="bt-updown">
			<a href="#"
			   onclick="dwho.form.order_selected('it-codec',1);
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_upcodec');?>">
				<?=$url->img_html('img/site/button/arrow-up.gif',
						  $this->bbf('bt_upcodec'),
						  'class="bt-uplist" id="bt-upcodec" border="0"');?></a><br />
			<a href="#"
			   onclick="dwho.form.order_selected('it-codec',-1);
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_downcodec');?>">
				<?=$url->img_html('img/site/button/arrow-down.gif',
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
				      'checked'	=> $this->get_var('info','t38pt_udptl','var_val'),
				      'default'	=> $element['t38pt_udptl']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_t38pt-rtp'),
				      'name'	=> 't38pt_rtp',
				      'labelid'	=> 't38pt-rtp',
				      'checked'	=> $this->get_var('info','t38pt_rtp','var_val'),
				      'default'	=> $element['t38pt_rtp']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_t38pt-tcp'),
				      'name'	=> 't38pt_tcp',
				      'labelid'	=> 't38pt-tcp',
				      'checked'	=> $this->get_var('info','t38pt_tcp','var_val'),
				      'default'	=> $element['t38pt_tcp']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_t38pt-usertpsource'),
				      'name'	=> 't38pt_usertpsource',
				      'labelid'	=> 't38pt-usertpsource',
				      'checked'	=> $this->get_var('info','t38pt_usertpsource','var_val'),
				      'default'	=> $element['t38pt_usertpsource']['default']));
?>
</div>

<div id="sb-part-jitterbuffer" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_jbenable'),
				      'name'	=> 'jbenable',
				      'labelid'	=> 'jbenable',
				      'checked'	=> $this->get_var('info','jbenable','var_val'),
				      'default'	=> $element['jbenable']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_jbforce'),
				      'name'	=> 'jbforce',
				      'labelid'	=> 'jbforce',
				      'checked'	=> $this->get_var('info','jbforce','var_val'),
				      'default'	=> $element['jbforce']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_jbmaxsize'),
				    'name'	=> 'jbmaxsize',
				    'labelid'	=> 'jbmaxsize',
				    'key'	=> false,
				    'bbf'	=> 'fm_jbmaxsize-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('info','jbmaxsize','var_val'),
				    'default'	=> $element['jbmaxsize']['default']),
			      $element['jbmaxsize']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_jbresyncthreshold'),
				    'name'	=> 'jbresyncthreshold',
				    'labelid'	=> 'jbresyncthreshold',
				    'key'	=> false,
				    'bbf'	=> 'fm_jbresyncthreshold-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('info','jbresyncthreshold','var_val'),
				    'default'	=> $element['jbresyncthreshold']['default']),
			      $element['jbresyncthreshold']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_jbimpl'),
				    'name'	=> 'jbimpl',
				    'labelid'	=> 'jbimpl',
				    'key'	=> false,
				    'bbf'	=> 'fm_jbimpl-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('info','jbimpl','var_val'),
				    'default'	=> $element['jbimpl']['default']),
			      $element['jbimpl']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_jblog'),
				      'name'	=> 'jblog',
				      'labelid'	=> 'jblog',
				      'checked'	=> $this->get_var('info','jblog','var_val'),
				      'default'	=> $element['jblog']['default']));
?>
</div>

<div id="sb-part-default" class="b-nodisplay">
<?php

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_context'),
				    'name'	=> 'context',
				    'labelid'	=> 'context',
				    'empty'	=> true,
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['context']['default'],
				    'selected'	=> $this->get_var('info','context','var_val')),
			      $context_list);
endif;

	echo	$form->select(array('desc'	=> $this->bbf('fm_nat'),
				    'name'	=> 'nat',
				    'labelid'	=> 'nat',
				    'key'	=> false,
				    'bbf'	=> 'fm_nat-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('info','nat','var_val'),
				    'default'	=> $element['nat']['default']),
			      $element['nat']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_dtmfmode'),
				    'name'	=> 'dtmfmode',
				    'labelid'	=> 'dtmfmode',
				    'key'	=> false,
				    'bbf'	=> 'fm_dtmfmode-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('info','dtmfmode','var_val'),
				    'default'	=> $element['dtmfmode']['default']),
			      $element['dtmfmode']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_qualify'),
				    'name'	=> 'qualify',
				    'labelid'	=> 'qualify',
				    'key'	=> false,
				    'bbf'	=> 'fm_qualify-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('info','qualify','var_val'),
				    'default'	=> $element['qualify']['default']),
			      $element['qualify']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_useclientcode'),
				      'name'	=> 'useclientcode',
				      'labelid'	=> 'useclientcode',
				      'checked'	=> $this->get_var('info','useclientcode','var_val'),
				      'default'	=> $element['useclientcode']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_progressinband'),
				    'name'	=> 'progressinband',
				    'labelid'	=> 'progressinband',
				    'key'	=> false,
				    'bbf'	=> 'fm_progressinband-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('info','progressinband','var_val'),
				    'default'	=> $element['progressinband']['default']),
			      $element['progressinband']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_language'),
				    'name'	=> 'language',
				    'labelid'	=> 'language',
				    'key'	=> false,
				    'selected'	=> $this->get_var('info','language','var_val'),
				    'default'	=> $element['language']['default']),
			      $element['language']['value']);

if($moh_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_mohinterpret'),
				    'name'	=> 'mohinterpret',
				    'labelid'	=> 'mohinterpret',
				    'key'	=> 'category',
				    'selected'	=> $this->get_var('info','mohinterpret','var_val'),
				    'default'	=> $element['mohinterpret']['default']),
			      $moh_list),

		$form->select(array('desc'	=> $this->bbf('fm_mohsuggest'),
				    'name'	=> 'mohsuggest',
				    'labelid'	=> 'mohsuggest',
				    'empty'	=> true,
				    'key'	=> 'category',
				    'selected'	=> $this->get_var('info','mohsuggest','var_val'),
				    'default'	=> $element['mohsuggest']['default']),
			      $moh_list);
endif;

	echo	$form->text(array('desc'	=> $this->bbf('fm_vmexten'),
				  'name'	=> 'vmexten',
				  'labelid'	=> 'vmexten',
				  'value'	=> $this->get_var('info','vmexten','var_val'),
				  'default'	=> $element['vmexten']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_trustrpid'),
				      'name'	=> 'trustrpid',
				      'labelid'	=> 'trustrpid',
				      'checked'	=> $this->get_var('info','trustrpid','var_val'),
				      'default'	=> $element['trustrpid']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_sendrpid'),
				      'name'	=> 'sendrpid',
				      'labelid'	=> 'sendrpid',
				      'checked'	=> $this->get_var('info','sendrpid','var_val'),
				      'default'	=> $element['sendrpid']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_assertedidentity'),
				      'name'	=> 'assertedidentity',
				      'labelid'	=> 'assertedidentity',
				      'checked'	=> $this->get_var('info','assertedidentity','var_val'),
				      'default'	=> $element['assertedidentity']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_canreinvite'),
				    'name'	=> 'canreinvite',
				    'labelid'	=> 'canreinvite',
				    'bbf'	=> 'fm_canreinvite-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('info','canreinvite','var_val'),
				    'default'	=> $element['canreinvite']['default']),
			      $element['canreinvite']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_insecure'),
				    'name'	=> 'insecure',
				    'labelid'	=> 'insecure',
				    'bbf'	=> 'fm_insecure-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('info','insecure','var_val'),
				    'default'	=> $element['insecure']['default']),
			      $element['insecure']['value']);
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_rtcachefriends'),
				      'name'	=> 'rtcachefriends',
				      'labelid'	=> 'rtcachefriends',
				      'checked'	=> $this->get_var('info','rtcachefriends','var_val'),
				      'default'	=> $element['rtcachefriends']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_rtupdate'),
				      'name'	=> 'rtupdate',
				      'labelid'	=> 'rtupdate',
				      'checked'	=> $this->get_var('info','rtupdate','var_val'),
				      'default'	=> $element['rtupdate']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_ignoreregexpire'),
				      'name'	=> 'ignoreregexpire',
				      'labelid'	=> 'ignoreregexpire',
				      'checked'	=> $this->get_var('info','ignoreregexpire','var_val'),
				      'default'	=> $element['ignoreregexpire']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_rtsavesysname'),
				      'name'	=> 'rtsavesysname',
				      'labelid'	=> 'rtsavesysname',
				      'checked'	=> $this->get_var('info','rtsavesysname','var_val'),
				      'default'	=> $element['rtsavesysname']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_rtautoclear'),
				    'name'	=> 'rtautoclear',
				    'labelid'	=> 'rtautoclear',
				    'key'	=> false,
				    'bbf'	=> 'fm_rtautoclear-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'selected'	=> $this->get_var('info','rtautoclear','var_val'),
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
