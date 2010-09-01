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

$error = $this->get_var('error');
$element = $this->get_var('element');

$info = $this->get_var('info');
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
		<li id="dwsm-tab-5"
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
		<li id="dwsm-tab-6"
		    class="dwsm-blur-last"
		    onclick="dwho_submenu.select(this,'sb-part-last',1);"
		    onmouseout="dwho_submenu.blur(this,1);"
		    onmouseover="dwho_submenu.focus(this,1);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_hotline');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="dwho.form.select('it-localnet'); dwho.form.select('it-codec');">

<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,'value'	=> DWHO_SESS_ID)),
			$form->hidden(array('name'	=> 'fm_send','value'	=> 1));
?>

<div id="sb-part-first">
<?php

	echo	$form->text(array('desc'	=> $this->bbf('fm_servername'),
					'name'		=> 'servername',
					'labelid'	=> 'servername',
					'value'		=> $this->get_var('info','servername','var_val'),
					'help'		=> $this->bbf('hlp_fm_servername'),
					'comment'	=> $this->bbf('cmt_fm_servername'),
				    'default'	=> $element['servername']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'servername')) )),

		$form->select(array('desc'	=> $this->bbf('fm_keepalive'),
					'name'		=> 'keepalive',
					'labelid'	=> 'keepalive',
				    'empty'		=> false,
				    'key'		=> false,
					'bbf'		=> 'fm_keepalive-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 		'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
					'help'		=> $this->bbf('hlp_fm_keepalive'),
					'comment'	=> $this->bbf('cmt_fm_keepalive'),
					'selected'	=> $this->get_var('info','keepalive','var_val'),
					'default'	=> $element['keepalive']['default']),$element['keepalive']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_debug'),
					'name'		=> 'debug',
					'labelid'	=> 'debug',
					'value'		=> $this->get_var('info','debug','var_val'),
					'help'		=> $this->bbf('hlp_fm_debug'),
					'comment'	=> $this->bbf('cmt_fm_debug'),
					'default'	=> $element['debug']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'debug')) ));

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_context'),
				    'name'		=> 'context',
				    'labelid'	=> 'context',
				    'empty'		=> true,
				    'key'		=> 'identity',
				    'altkey'	=> 'name',
					'help'		=> $this->bbf('hlp_fm_context'),
					'comment'	=> $this->bbf('cmt_fm_context'),
				    'selected'	=> $this->get_var('info','context','var_val'),
				    'default'	=> $element['context']['default']),$context_list);

	echo	$form->select(array('desc'	=> $this->bbf('fm_regcontext'),
				    'name'		=> 'regcontext',
				    'labelid'	=> 'regcontext',
				    'empty'		=> true,
				    'key'		=> 'identity',
				    'altkey'	=> 'name',
					'help'		=> $this->bbf('hlp_fm_regcontext'),
					'comment'	=> $this->bbf('cmt_fm_regcontext'),
				    'selected'	=> $this->get_var('info','regcontext','var_val'),
				    'default'	=> $element['regcontext']['default']),$context_list);
endif;

	echo	$form->text(array('desc'	=> $this->bbf('fm_dateFormat'),
					'name'		=> 'dateFormat',
					'labelid'	=> 'dateFormat',
					'value'		=> $this->get_var('info','dateFormat','var_val'),
					'help'		=> $this->bbf('hlp_fm_dateFormat'),
					'comment'	=> $this->bbf('cmt_fm_dateFormat'),
					'default'	=> $element['dateFormat']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'dateFormat')) )),

		$form->text(array('desc'	=> $this->bbf('fm_bindaddr'),
					'name'		=> 'bindaddr',
					'labelid'	=> 'bindaddr',
					'value'		=> $this->get_var('info','bindaddr','var_val'),
					'help'		=> $this->bbf('hlp_fm_bindaddr'),
					'comment'	=> $this->bbf('cmt_fm_bindaddr'),
					'default'	=> $element['bindaddr']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'bindaddr')) )),

		$form->text(array('desc'	=> $this->bbf('fm_port'),
					'size'		=> '5',
					'name'		=> 'port',
					'labelid'	=> 'port',
					'value'		=> $this->get_var('info','port','var_val'),
					'help'		=> $this->bbf('hlp_fm_port'),
					'comment'	=> $this->bbf('cmt_fm_port'),
					'default'	=> $element['port']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'port')) )),

		$form->text(array('desc'	=> $this->bbf('fm_firstdigittimeout'),
					'size'		=> '4',
					'name'		=> 'firstdigittimeout',
					'labelid'	=> 'firstdigittimeout',
					'value'		=> $this->get_var('info','firstdigittimeout','var_val'),
					'help'		=> $this->bbf('hlp_fm_firstdigittimeout'),
					'comment'	=> $this->bbf('cmt_fm_firstdigittimeout'),
					'default'	=> $element['firstdigittimeout']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'firstdigittimeout')) )),

		$form->text(array('desc'	=> $this->bbf('fm_digittimeout'),
					'size'		=> '4',
					'name'		=> 'digittimeout',
					'labelid'	=> 'digittimeout',
					'value'		=> $this->get_var('info','digittimeout','var_val'),
					'help'		=> $this->bbf('hlp_fm_digittimeout'),
					'comment'	=> $this->bbf('cmt_fm_digittimeout'),
					'default'	=> $element['digittimeout']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'digittimeout')) )),

		$form->text(array('desc'	=> $this->bbf('fm_digittimeoutchar'),
					'size'		=> '4',
					'name'		=> 'digittimeoutchar',
					'labelid'	=> 'digittimeoutchar',
				    'empty'		=> true,
					'value'		=> $this->get_var('info','digittimeoutchar','var_val'),
					'help'		=> $this->bbf('hlp_fm_digittimeoutchar'),
					'comment'	=> $this->bbf('cmt_fm_digittimeoutchar'),
					'default'	=> $element['digittimeoutchar']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'digittimeoutchar')) ));

if($moh_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_musicclass'),
				    'name'		=> 'musicclass',
				    'labelid'	=> 'musicclass',
				    'key'		=> 'category',
					'help'		=> $this->bbf('hlp_fm_musicclass'),
					'comment'	=> $this->bbf('cmt_fm_musicclass'),
					'required'	=> false,
				    'selected'	=> $this->get_var('info','musicclass','var_val'),
				    'default'	=> $element['musicclass']['default']),$moh_list);
endif;

	echo	$form->select(array('desc'	=> $this->bbf('fm_language'),
					'name'		=> 'language',
					'labelid'	=> 'language',
				    'empty'		=> true,
				    'key'		=> false,
					'help'		=> $this->bbf('hlp_fm_language'),
					'comment'	=> $this->bbf('cmt_fm_language'),
					'selected'	=> $this->get_var('info','language','var_val'),
					'default'	=> $element['language']['default']),$element['language']['value']);
?>
</div>

<div id="sb-part-network" class="b-nodisplay">
<?php

	echo	$form->text(array('desc'	=> $this->bbf('fm_address-deny'),
					'size'		=> '24',
					'name'		=> 'address-deny',
					'labelid'	=> 'address-deny',
					'value'		=> $this->get_var('info','deny','var_val'),
					'help'		=> $this->bbf('hlp_fm_address-deny'),
					'comment'	=> $this->bbf('cmt_fm_address-deny'),
				    'default'	=> $element['deny']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'deny')) )),

		$form->text(array('desc'	=> $this->bbf('fm_address-permit'),
					'size'		=> '24',
					'name'		=> 'address-permit',
					'labelid'	=> 'address-permit',
					'value'		=> $this->get_var('info','permit','var_val'),
					'help'		=> $this->bbf('hlp_fm_address-permit'),
					'comment'	=> $this->bbf('cmt_fm_address-permit'),
				    'default'	=> $element['permit']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'permit')) )),

		$form->text(array('desc'	=> $this->bbf('fm_address-localnet'),
					'size'		=> '24',
					'name'		=> 'address-localnet',
					'labelid'	=> 'address-localnet',
					'empty'		=> true,
					'value'		=> $this->get_var('info','localnet','var_val'),
					'help'		=> $this->bbf('hlp_fm_address-localnet'),
					'comment'	=> $this->bbf('cmt_fm_address-localnet'),
					'required'	=> false,
					'default'	=> $element['localnet']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'localnet')) )),

		$form->text(array('desc'	=> $this->bbf('fm_address-externip'),
					'size'		=> '24',
					'name'		=> 'address-externip',
					'labelid'	=> 'address-externip',
					'empty'		=> true,
					'value'		=> $this->get_var('info','externip','var_val'),
					'help'		=> $this->bbf('hlp_fm_address-externip'),
					'comment'	=> $this->bbf('cmt_fm_address-externip'),
					'required'	=> false,
					'default'	=> $element['externip']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'externip')) )),

		$form->text(array('desc'	=> $this->bbf('fm_address-externhost'),
					'size'		=> '24',
					'name'		=> 'address-externhost',
					'labelid'	=> 'address-externhost',
					'empty'		=> true,
					'value'		=> $this->get_var('info','externhost','var_val'),
					'help'		=> $this->bbf('hlp_fm_address-externhost'),
					'comment'	=> $this->bbf('cmt_fm_address-externhost'),
					'required'	=> false,
					'default'	=> $element['externhost']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'externhost')) )),

		$form->select(array('desc'	=> $this->bbf('fm_externrefresh'),
				    'name'		=> 'externrefresh',
				    'labelid'	=> 'externrefresh',
					'empty'		=> true,
				    'key'		=> false,
				    'bbf'		=> 'fm_externrefresh-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 		'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
					'help'		=> $this->bbf('hlp_fm_externrefresh'),
					'comment'	=> $this->bbf('cmt_fm_externrefresh'),
					'required'	=> false,
				    'selected'	=> $this->get_var('info','externrefresh','var_val'),
				    'default'	=> $element['externrefresh']['default']),$element['externrefresh']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_nat'),
					'name'		=> 'nat',
					'labelid'	=> 'nat',
				    'key'		=> false,
					'help'		=> $this->bbf('hlp_fm_nat'),
					'comment'	=> $this->bbf('cmt_fm_nat'),
					'selected'	=> $this->get_var('info','nat','var_val'),
					'default'	=> $element['nat']['default']),$element['nat']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_sccp_tos'),
					'size'		=> '4',
					'name'		=> 'sccp_tos',
					'labelid'	=> 'sccp_tos',
					'value'		=> $this->get_var('info','sccp_tos','var_val'),
					'help'		=> $this->bbf('hlp_fm_sccp_tos'),
					'comment'	=> $this->bbf('cmt_fm_sccp_tos'),
					'default'	=> $element['sccp_tos']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'sccp_tos')) )),

		$form->select(array('desc'	=> $this->bbf('fm_sccp_cos'),
					'name'		=> 'sccp_cos',
					'labelid'	=> 'sccp_cos',
				    'key'		=> false,
					'help'		=> $this->bbf('hlp_fm_sccp_cos'),
					'comment'	=> $this->bbf('cmt_fm_sccp_cos'),
					'selected'	=> $this->get_var('info','sccp_cos','var_val'),
					'default'	=> $element['sccp_cos']['default']),$element['sccp_cos']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_audio_tos'),
					'size'		=> '4',
					'name'		=> 'audio_tos',
					'labelid'	=> 'audio_tos',
					'value'		=> $this->get_var('info','audio_tos','var_val'),
					'help'		=> $this->bbf('hlp_fm_audio_tos'),
					'comment'	=> $this->bbf('cmt_fm_audio_tos'),
					'default'	=> $element['audio_tos']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'audio_tos')) )),

		$form->select(array('desc'	=> $this->bbf('fm_audio_cos'),
					'name'		=> 'audio_cos',
					'labelid'	=> 'audio_cos',
				    'key'		=> false,
					'help'		=> $this->bbf('hlp_fm_audio_cos'),
					'comment'	=> $this->bbf('cmt_fm_audio_cos'),
					'selected'	=> $this->get_var('info','audio_cos','var_val'),
					'default'	=> $element['audio_cos']['default']),$element['audio_cos']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_video_tos'),
					'size'		=> '4',
					'name'		=> 'video_tos',
					'labelid'	=> 'video_tos',
					'value'		=> $this->get_var('info','video_tos','var_val'),
					'help'		=> $this->bbf('hlp_fm_video_tos'),
					'comment'	=> $this->bbf('cmt_fm_video_tos'),
					'default'	=> $element['video_tos']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'video_tos')) )),

		$form->select(array('desc'	=> $this->bbf('fm_video_cos'),
					'name'		=> 'video_cos',
					'labelid'	=> 'video_cos',
				    'key'		=> false,
					'help'		=> $this->bbf('hlp_fm_video_cos'),
					'comment'	=> $this->bbf('cmt_fm_video_cos'),
					'selected'	=> $this->get_var('info','video_cos','var_val'),
					'default'	=> $element['video_cos']['default']),$element['video_cos']['value']);

?>
</div>

<div id="sb-part-signalling" class="b-nodisplay">
<?php

	echo	$form->select(array('desc'	=> $this->bbf('fm_dnd'),
					'name'		=> 'dnd',
					'labelid'	=> 'dnd',
				    'empty'		=> false,
				    'key'		=> false,
					'help'		=> $this->bbf('hlp_fm_dnd'),
					'comment'	=> $this->bbf('cmt_fm_dnd'),
					'selected'	=> $this->get_var('info','dnd','var_val'),
					'default'	=> $element['dnd']['default']),$element['dnd']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_echocancel'),
					'name'		=> 'echocancel',
					'labelid'	=> 'echocancel',
				    'empty'		=> false,
				    'key'		=> false,
					'help'		=> $this->bbf('hlp_fm_echocancel'),
					'comment'	=> $this->bbf('cmt_fm_echocancel'),
					'selected'	=> $this->get_var('info','echocancel','var_val'),
					'default'	=> $element['echocancel']['default']),$element['echocancel']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_silencesuppression'),
					'name'		=> 'silencesuppression',
					'labelid'	=> 'silencesuppression',
				    'empty'		=> false,
				    'key'		=> false,
					'help'		=> $this->bbf('hlp_fm_silencesuppression'),
					'comment'	=> $this->bbf('cmt_fm_silencesuppression'),
					'selected'	=> $this->get_var('info','silencesuppression','var_val'),
					'default'	=> $element['silencesuppression']['default']),$element['silencesuppression']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_autoanswer_ring_time'),
					'name'		=> 'autoanswer_ring_time',
					'labelid'	=> 'autoanswer_ring_time',
				    'empty'		=> false,
				    'key'		=> false,
					'bbf'		=> 'fm_autoanswer_ring_time-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 		'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
					'help'		=> $this->bbf('hlp_fm_autoanswer_ring_time'),
					'comment'	=> $this->bbf('cmt_fm_autoanswer_ring_time'),
					'selected'	=> $this->get_var('info','autoanswer_ring_time','var_val'),
					'default'	=> $element['autoanswer_ring_time']['default']),$element['autoanswer_ring_time']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_autoanswer_tone'),
					'name'		=> 'autoanswer_tone',
					'labelid'	=> 'autoanswer_tone',
				    'empty'		=> false,
				    'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_autoanswer_tone'),
					'comment'	=> $this->bbf('cmt_fm_autoanswer_tone'),
					'selected'	=> $this->get_var('info','autoanswer_tone','var_val'),
					'default'	=> $element['autoanswer_tone']['default']),$element['autoanswer_tone']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_remotehangup_tone'),
					'name'		=> 'remotehangup_tone',
					'labelid'	=> 'remotehangup_tone',
				    'empty'		=> false,
				    'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_remotehangup_tone'),
					'comment'	=> $this->bbf('cmt_fm_remotehangup_tone'),
					'selected'	=> $this->get_var('info','remotehangup_tone','var_val'),
					'default'	=> $element['remotehangup_tone']['default']),$element['remotehangup_tone']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_transfer_tone'),
					'name'		=> 'transfer_tone',
					'labelid'	=> 'transfer_tone',
				    'empty'		=> false,
				    'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_transfer_tone'),
					'comment'	=> $this->bbf('cmt_fm_transfer_tone'),
					'selected'	=> $this->get_var('info','transfer_tone','var_val'),
					'default'	=> $element['transfer_tone']['default']),$element['transfer_tone']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_callwaiting_tone'),
					'name'		=> 'callwaiting_tone',
					'labelid'	=> 'callwaiting_tone',
				    'empty'		=> false,
				    'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_callwaiting_tone'),
					'comment'	=> $this->bbf('cmt_fm_callwaiting_tone'),
					'selected'	=> $this->get_var('info','callwaiting_tone','var_val'),
					'default'	=> $element['callwaiting_tone']['default']),$element['callwaiting_tone']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_directrtp'),
					'name'		=> 'directrtp',
					'labelid'	=> 'directrtp',
				    'empty'		=> false,
				    'key'		=> false,
					'help'		=> $this->bbf('hlp_fm_directrtp'),
					'comment'	=> $this->bbf('cmt_fm_directrtp'),
					'selected'	=> $this->get_var('info','directrtp','var_val'),
					'default'	=> $element['directrtp']['default']),$element['directrtp']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_allowoverlap'),
					'name'		=> 'allowoverlap',
					'labelid'	=> 'allowoverlap',
				    'empty'		=> false,
				    'key'		=> false,
					'help'		=> $this->bbf('hlp_fm_allowoverlap'),
					'comment'	=> $this->bbf('cmt_fm_allowoverlap'),
					'selected'	=> $this->get_var('info','allowoverlap','var_val'),
					'default'	=> $element['allowoverlap']['default']),$element['allowoverlap']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_callanswerorder'),
					'name'		=> 'callanswerorder',
					'labelid'	=> 'callanswerorder',
				    'empty'		=> false,
				    'key'		=> false,
					'bbf'		=> 'fm_callanswerorder-opt',
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_callanswerorder'),
					'comment'	=> $this->bbf('cmt_fm_callanswerorder'),
					'selected'	=> $this->get_var('info','callanswerorder','var_val'),
					'default'	=> $element['callanswerorder']['default']),$element['callanswerorder']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_codec-disallow'),
				    'name'		=> 'codec-disallow',
				    'labelid'	=> 'codec-disallow',
				    'empty'		=> false,
				    'key'		=> false,
					'help'		=> $this->bbf('hlp_fm_codec-disallow'),
					'comment'	=> $this->bbf('cmt_fm_codec-disallow'),
				    'bbf'		=> 'fm_codec-disallow-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue')),$element['disallow']['value']);
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
				    'label'		=> false,
					'help'		=> $this->bbf('hlp_fm_codec-allow'),
					'comment'	=> $this->bbf('cmt_fm_codec-allow'),
				    'id'		=> 'it-codeclist',
				    'multiple'	=> true,
				    'key'		=> false,
				    'size'		=> 5,
				    'paragraph'	=> false,
				    'bbf'		=> 'ast_codec_name_type',
				    'bbfopt'	=> array('argmode' => 'paramvalue')),$element['allow']['value']);
?>
	</div>
	<div class="inout-list">
		<a href="#"
		   onclick="dwho.form.move_selected('it-codeclist','it-codec');return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_incodec');?>">
			<?=$url->img_html('img/site/button/arrow-left.gif',
					  $this->bbf('bt_incodec'),
					  'class="bt-inlist" id="bt-incodec" border="0"');?></a><br />
		<a href="#"
		   onclick="dwho.form.move_selected('it-codec','it-codeclist');return(dwho.dom.free_focus());"
		   title="<?=$this->bbf('bt_outcodec');?>">
			<?=$url->img_html('img/site/button/arrow-right.gif',
					  $this->bbf('bt_outcodec'),
					  'class="bt-outlist" id="bt-outcodec" border="0"');?></a>
	</div>
	<div class="slt-inlist">
		<?=$form->select(array('name'		=> 'allow[]',
						'label'		=> false,
						'id'			=> 'it-codec',
						'multiple'	=> true,
						'size'		=> 5,
						'key'       => false,
						'paragraph'	=> false,
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

<div id="sb-part-jitterbuffer" class="b-nodisplay">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_jbenable'),
					'name'		=> 'jbenable',
					'labelid'	=> 'jbenable',
				    'empty'		=> false,
				    'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_jbenable'),
					'comment'	=> $this->bbf('cmt_fm_jbenable'),
					'selected'	=> $this->get_var('info','jbenable','var_val'),
					'default'	=> $element['jbenable']['default']),$element['jbenable']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_jbforce'),
					'name'		=> 'jbforce',
					'labelid'	=> 'jbforce',
				    'empty'		=> false,
				    'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_jbforce'),
					'comment'	=> $this->bbf('cmt_fm_jbforce'),
					'selected'	=> $this->get_var('info','jbforce','var_val'),
					'default'	=> $element['jbforce']['default']),$element['jbforce']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_jbmaxsize'),
					'name'		=> 'jbmaxsize',
					'labelid'	=> 'jbmaxsize',
					'key'		=> false,
					'bbf'		=> 'fm_jbmaxsize-opt',
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_jbmaxsize'),
					'comment'	=> $this->bbf('cmt_fm_jbmaxsize'),
					'selected'	=> $this->get_var('info','jbmaxsize','var_val'),
					'default'	=> $element['jbmaxsize']['default']), $element['jbmaxsize']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_jbresyncthreshold'),
					'name'		=> 'jbresyncthreshold',
					'labelid'	=> 'jbresyncthreshold',
					'key'		=> false,
					'bbf'		=> 'fm_jbresyncthreshold-opt',
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_jbresyncthreshold'),
					'comment'	=> $this->bbf('cmt_fm_jbforce'),
					'selected'	=> $this->get_var('info','jbresyncthreshold','var_val'),
					'default'	=> $element['jbresyncthreshold']['default']),$element['jbresyncthreshold']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_jbimpl'),
					'name'		=> 'jbimpl',
					'labelid'	=> 'jbimpl',
					'key'		=> false,
					'bbf'		=> 'fm_jbimpl-opt',
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_jbimpl'),
					'comment'	=> $this->bbf('cmt_fm_jbimpl'),
					'selected'	=> $this->get_var('info','jbimpl','var_val'),
					'default'	=> $element['jbimpl']['default']),$element['jbimpl']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_jblog'),
					'name'		=> 'jblog',
					'labelid'	=> 'jblog',
				    'empty'		=> false,
				    'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_jblog'),
					'comment'	=> $this->bbf('cmt_fm_jblog'),
					'selected'	=> $this->get_var('info','jblog','var_val'),
					'default'	=> $element['jblog']['default']),$element['jblog']['value']);
?>
</div>

<div id="sb-part-default" class="b-nodisplay">
<?php

	echo	$form->select(array('desc'	=> $this->bbf('fm_callevents'),
					'name'		=> 'callevents',
					'labelid'	=> 'callevents',
				    'empty'		=> true,
				    'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_callevents'),
					'comment'	=> $this->bbf('cmt_fm_callevents'),
					'required'	=> false,
					'selected'	=> $this->get_var('info','callevents','var_val'),
					'default'	=> $element['callevents']['default']),$element['callevents']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_accountcode'),
					'name'		=> 'accountcode',
					'labelid'	=> 'accountcode',
				    'empty'		=> true,
					'value'		=> $this->get_var('info','accountcode','var_val'),
					'help'		=> $this->bbf('hlp_fm_accountcode'),
					'comment'	=> $this->bbf('cmt_fm_accountcode'),
					'required'	=> false,
				    'default'	=> $element['accountcode']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'accountcode')) )),

		$form->text(array('desc'	=> $this->bbf('fm_callgroup'),
					'name'		=> 'callgroup',
					'labelid'	=> 'callgroup',
					'value'		=> $this->get_var('info','callgroup','var_val'),
					'help'		=> $this->bbf('hlp_fm_callgroup'),
					'comment'	=> $this->bbf('cmt_fm_callgroup'),
					'default'	=> $element['callgroup']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'callgroup')) )),

		$form->text(array('desc'	=> $this->bbf('fm_pickupgroup'),
					'name'		=> 'pickupgroup',
					'labelid'	=> 'pickupgroup',
					'value'		=> $this->get_var('info','pickupgroup','var_val'),
					'help'		=> $this->bbf('hlp_fm_pickupgroup'),
					'comment'	=> $this->bbf('cmt_fm_pickupgroup'),
					'default'	=> $element['pickupgroup']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'pickupgroup')) )),

		$form->text(array('desc'	=> $this->bbf('fm_amaflags'),
					'name'		=> 'amaflags',
					'labelid'	=> 'amaflags',
					'empty'		=> true,
					'value'		=> $this->get_var('info','amaflags','var_val'),
					'help'		=> $this->bbf('hlp_fm_amaflags'),
					'comment'	=> $this->bbf('cmt_fm_amaflags'),
					'required'	=> false,
					'default'	=> $element['amaflags']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'amaflags')) )),

		$form->select(array('desc'	=> $this->bbf('fm_trustphoneip'),
					'name'		=> 'trustphoneip',
					'labelid'	=> 'trustphoneip',
					'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_trustphoneip'),
					'comment'	=> $this->bbf('cmt_fm_trustphoneip'),
					'required'	=> false,
					'selected'	=> $this->get_var('info','trustphoneip','var_val'),
					'default'	=> $element['trustphoneip']['default']),$element['trustphoneip']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_earlyrtp'),
					'name'		=> 'earlyrtp',
					'labelid'	=> 'earlyrtp',
					'empty'		=> true,
					'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_earlyrtp'),
					'comment'	=> $this->bbf('cmt_fm_earlyrtp'),
					'required'	=> false,
					'selected'	=> $this->get_var('info','earlyrtp','var_val'),
//					'default'	=> $element['earlyrtp']['default']
),$element['earlyrtp']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_private'),
					'name'		=> 'private',
					'labelid'	=> 'private',
					'empty'		=> true,
					'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_private'),
					'comment'	=> $this->bbf('cmt_fm_private'),
					'required'	=> false,
					'selected'	=> $this->get_var('info','private','var_val'),
					'default'	=> $element['private']['default']),$element['private']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_mwilamp'),
					'name'		=> 'mwilamp',
					'labelid'	=> 'mwilamp',
					'empty'		=> true,
					'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_mwilamp'),
					'comment'	=> $this->bbf('cmt_fm_mwilamp'),
					'required'	=> false,
					'selected'	=> $this->get_var('info','mwilamp','var_val'),
					'default'	=> $element['mwilamp']['default']
				),
			$element['mwilamp']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_mwioncall'),
					'name'		=> 'mwioncall',
					'labelid'	=> 'mwioncall',
					'empty'		=> true,
					'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_mwioncall'),
					'comment'	=> $this->bbf('cmt_fm_mwioncall'),
					'required'	=> false,
					'selected'	=> $this->get_var('info','mwioncall','var_val'),
					'default'	=> $element['mwioncall']['default']),$element['mwioncall']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_blindtransferindication'),
					'name'		=> 'blindtransferindication',
					'labelid'	=> 'blindtransferindication',
					'empty'		=> true,
					'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_blindtransferindication'),
					'comment'	=> $this->bbf('cmt_fm_blindtransferindication'),
					'selected'	=> $this->get_var('info','blindtransferindication','var_val'),
					'default'	=> $element['blindtransferindication']['default']),$element['blindtransferindication']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_protocolversion'),
					'size'		=> '4',
					'name'		=> 'protocolversion',
					'labelid'	=> 'protocolversion',
					'value'		=> $this->get_var('info','protocolversion','var_val'),
					'help'		=> $this->bbf('hlp_fm_protocolversion'),
					'comment'	=> $this->bbf('cmt_fm_protocolversion'),
					'default'	=> $element['protocolversion']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'protocolversion')) )),

		$form->select(array('desc'	=> $this->bbf('fm_cfwdall'),
					'name'		=> 'cfwdall',
					'labelid'	=> 'cfwdall',
					'empty'		=> true,
					'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_cfwdall'),
					'comment'	=> $this->bbf('cmt_fm_cfwdall'),
					'required'	=> false,
					'selected'	=> $this->get_var('info','cfwdall','var_val'),
					'default'	=> $element['cfwdall']['default']),$element['cfwdall']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_cfwdbusy'),
					'name'		=> 'cfwdbusy',
					'labelid'	=> 'cfwdbusy',
					'empty'		=> true,
					'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_cfwdbusy'),
					'comment'	=> $this->bbf('cmt_fm_cfwdbusy'),
					'required'	=> false,
					'selected'	=> $this->get_var('info','cfwdbusy','var_val'),
					'default'	=> $element['cfwdbusy']['default']),$element['cfwdbusy']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_cfwdnoanswer'),
					'name'		=> 'cfwdnoanswer',
					'labelid'	=> 'cfwdnoanswer',
					'empty'		=> true,
					'key'		=> false,
					'bbfopt'	=> array('argmode' => 'paramvalue'),
					'help'		=> $this->bbf('hlp_fm_cfwdnoanswer'),
					'comment'	=> $this->bbf('cmt_fm_cfwdnoanswer'),
					'required'	=> false,
					'selected'	=> $this->get_var('info','cfwdnoanswer','var_val'),
					'default'	=> $element['cfwdnoanswer']['default']),$element['cfwdnoanswer']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_devicetable'),
					'name'		=> 'devicetable',
					'labelid'	=> 'devicetable',
					'value'		=> $this->get_var('info','devicetable','var_val'),
					'help'		=> $this->bbf('hlp_fm_devicetable'),
					'comment'	=> $this->bbf('cmt_fm_devicetable'),
					'default'	=> $element['devicetable']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'devicetable')) )),

		$form->text(array('desc'	=> $this->bbf('fm_linetable'),
					'name'		=> 'linetable',
					'labelid'	=> 'linetable',
					'value'		=> $this->get_var('info','linetable','var_val'),
					'help'		=> $this->bbf('hlp_fm_linetable'),
					'comment'	=> $this->bbf('cmt_fm_linetable'),
					'default'	=> $element['linetable']['default'],
					'error'		=> $this->bbf_args('error',$this->get_var('error', 'linetable')) ));
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php

	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_hotline_enabled'),
					'name'		=> 'hotline_enabled',
					'labelid'	=> 'hotline_enabled',
					'help'		=> $this->bbf('hlp_hotline_part'),
					'comment'	=> $this->bbf('cmt_fm_hotline_enabled'),
					'disabled'	=> true,
					'checked'	=> $this->get_var('info','hotline_enabled','var_val'),
					'default'	=> $element['hotline_enabled']['default']));

	echo	$form->text(array('desc'	=> $this->bbf('fm_hotline_context'),
					'name'		=> 'hotline_context',
					'labelid'	=> 'hotline_context',
					'value'		=> $this->get_var('info','hotline_context','var_val'),
					'help'		=> $this->bbf('hlp_fm_hotline_context'),
					'comment'	=> $this->bbf('cmt_fm_hotline_context'),
					'disabled'	=> true,
					'default'	=> $element['hotline_context']['default'],
	));

	echo	$form->text(array('desc'	=> $this->bbf('fm_hotline_extension'),
					'name'		=> 'hotline_extension',
					'labelid'	=> 'hotline_extension',
					'value'		=> $this->get_var('info','hotline_extension','var_val'),
					'help'		=> $this->bbf('hlp_fm_hotline_extension'),
					'comment'	=> $this->bbf('cmt_fm_hotline_extension'),
					'disabled'	=> true,
					'default'	=> $element['hotline_extension']['default'],
	));
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
