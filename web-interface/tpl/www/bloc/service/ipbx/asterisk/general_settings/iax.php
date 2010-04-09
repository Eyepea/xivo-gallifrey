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

$element = $this->get_var('element');
$moh_list = $this->get_var('moh_list');
$context_list = $this->get_var('context_list');

if(($fm_save = $this->get_var('fm_save')) === true):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(true,\''.$dhtml->escape($this->bbf('fm_success-save')).'\');');
elseif($fm_save === false):
	$dhtml = &$this->get_module('dhtml');
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
		<li id="dwsm-tab-3"
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
		<li id="dwsm-tab-4"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-realtime');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_realtime');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-5"
		    class="dwsm-blur-last"
		    onclick="dwho_submenu.select(this,'sb-part-last',1);"
		    onmouseout="dwho_submenu.blur(this,1);"
		    onmouseover="dwho_submenu.focus(this,1);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_advanced');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8" onsubmit="dwho.form.select('it-codec');">

<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),
		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1));
?>

<div id="sb-part-first">

<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_bindport'),
				  'name'		=> 'bindport',
				  'labelid'		=> 'bindport',
				  'help'		=> $this->bbf('hlp_fm_bindport'),
				  'required'	=> true,
				  'value'		=> $this->get_var('info','bindport','var_val'),
				  'default'		=> $element['bindport']['default'],
				  'error'		=> $this->bbf_args('error',
					   $this->get_var('error', 'bindport')) )),

		$form->text(array('desc'	=> $this->bbf('fm_bindaddr'),
				  'name'		=> 'bindaddr',
				  'labelid'		=> 'bindaddr',
				  'size'		=> 15,
				  'help'		=> $this->bbf('hlp_fm_bindaddr'),
				  'required'	=> true,
				  'value'		=> $this->get_var('info','bindaddr','var_val'),
				  'default'		=> $element['bindaddr']['default'],
				  'error'		=> $this->bbf_args('error',
					   $this->get_var('error', 'bindaddr')) )),

		$form->select(array('desc'	=> $this->bbf('fm_iaxthreadcount'),
				    'name'		=> 'iaxthreadcount',
				    'labelid'	=> 'iaxthreadcount',
				    'key'		=> false,
				    'help'		=> $this->bbf('hlp_fm_iaxthreadcount'),
				    'selected'	=> $this->get_var('info','iaxthreadcount','var_val'),
				    'default'	=> $element['iaxthreadcount']['default']),
			      $element['iaxthreadcount']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_iaxmaxthreadcount'),
				    'name'		=> 'iaxmaxthreadcount',
				    'labelid'	=> 'iaxmaxthreadcount',
				    'key'		=> false,
				    'bbf'		=> 'fm_iaxmaxthreadcount-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'help'		=> $this->bbf('hlp_fm_iaxmaxthreadcount'),
				    'selected'	=> $this->get_var('info','iaxmaxthreadcount','var_val'),
				    'default'	=> $element['iaxmaxthreadcount']['default']),
			      $element['iaxmaxthreadcount']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_iaxcompat'),
				      'name'	=> 'iaxcompat',
				      'labelid'	=> 'iaxcompat',
				      'help'	=> $this->bbf('hlp_fm_iaxcompat'),
				      'checked'	=> $this->get_var('info','iaxcompat','var_val'),
				      'default'	=> $element['iaxcompat']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_authdebug'),
				      'name'	=> 'authdebug',
				      'labelid'	=> 'authdebug',
				      'help'	=> $this->bbf('hlp_fm_authdebug'),
				      'checked'	=> $this->get_var('info','authdebug','var_val'),
				      'default'	=> $element['authdebug']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_delayreject'),
				      'name'	=> 'delayreject',
				      'labelid'	=> 'delayreject',
				      'help'	=> $this->bbf('hlp_fm_delayreject'),
				      'checked'	=> $this->get_var('info','delayreject','var_val'),
				      'default'	=> $element['delayreject']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_trunkfreq'),
				    'name'		=> 'trunkfreq',
				    'labelid'	=> 'trunkfreq',
				    'key'		=> false,
				    'bbf'		=> 'fm_trunkfreq-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'help'		=> $this->bbf('hlp_fm_trunkfreq'),
				    'selected'	=> $this->get_var('info','trunkfreq','var_val'),
				    'default'	=> $element['trunkfreq']['default']),
			      $element['trunkfreq']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_trunktimestamps'),
				      'name'	=> 'trunktimestamps',
				      'labelid'	=> 'trunktimestamps',
				      'help'	=> $this->bbf('hlp_fm_trunktimestamps'),
				      'checked'	=> $this->get_var('info','trunktimestamps','var_val'),
				      'default'	=> $element['trunktimestamps']['default']));

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_regcontext'),
				    'name'		=> 'regcontext',
				    'labelid'	=> 'regcontext',
				    'empty'		=> true,
				    'key'		=> 'identity',
				    'altkey'	=> 'name',
				    'help'		=> $this->bbf('hlp_fm_regcontext'),
				    'default'	=> $element['regcontext']['default'],
				    'selected'	=> $this->get_var('info','regcontext','var_val')),
			      $context_list);
endif;

	echo	$form->select(array('desc'	=> $this->bbf('fm_minregexpire'),
				    'name'		=> 'minregexpire',
				    'labelid'	=> 'minregexpire',
				    'key'		=> false,
				    'bbf'		=> 'fm_regexpire-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'help'		=> $this->bbf('hlp_fm_minregexpire'),
				    'selected'	=> $this->get_var('info','minregexpire','var_val'),
				    'default'	=> $element['minregexpire']['default']),
			      $element['minregexpire']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_maxregexpire'),
				    'name'		=> 'maxregexpire',
				    'labelid'	=> 'maxregexpire',
				    'key'		=> false,
				    'bbf'		=> 'fm_regexpire-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'help'		=> $this->bbf('hlp_fm_maxregexpire'),
				    'selected'	=> $this->get_var('info','maxregexpire','var_val'),
				    'default'	=> $element['maxregexpire']['default']),
			      $element['maxregexpire']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_bandwidth'),
				    'name'		=> 'bandwidth',
				    'labelid'	=> 'bandwidth',
				    'key'		=> false,
				    'bbf'		=> 'fm_bandwidth-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'help'		=> $this->bbf('hlp_fm_bandwidth'),
				    'selected'	=> $this->get_var('info','bandwidth','var_val'),
				    'default'	=> $element['bandwidth']['default']),
			      $element['bandwidth']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_tos'),
				    'name'		=> 'tos',
				    'labelid'	=> 'tos',
				    'empty'		=> true,
				    'key'		=> false,
				    'help'		=> $this->bbf('hlp_fm_tos'),
				    'selected'	=> $this->get_var('info','tos','var_val'),
				    'default'	=> $element['tos']['default']),
			      $element['tos']['value']);
?>
</div>

<div id="sb-part-jitterbuffer" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_jitterbuffer'),
				      'name'	=> 'jitterbuffer',
				      'labelid'	=> 'jitterbuffer',
				      'help'	=> $this->bbf('hlp_fm_jitterbuffer'),
				      'checked'	=> $this->get_var('info','jitterbuffer','var_val'),
				      'default'	=> $element['jitterbuffer']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_forcejitterbuffer'),
				      'name'	=> 'forcejitterbuffer',
				      'labelid'	=> 'forcejitterbuffer',
				      'help'	=> $this->bbf('hlp_fm_forcejitterbuffer'),
				      'checked'	=> $this->get_var('info','forcejitterbuffer','var_val'),
				      'default'	=> $element['forcejitterbuffer']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_maxjitterbuffer'),
				    'name'		=> 'maxjitterbuffer',
				    'labelid'	=> 'maxjitterbuffer',
				    'key'		=> false,
				    'bbf'		=> 'fm_maxjitterbuffer-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'help'		=> $this->bbf('hlp_fm_maxjitterbuffer'),
				    'selected'	=> $this->get_var('info','maxjitterbuffer','var_val'),
				    'default'	=> $element['maxjitterbuffer']['default']),
			      $element['maxjitterbuffer']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_maxjitterinterps'),
				    'name'		=> 'maxjitterinterps',
				    'labelid'	=> 'maxjitterinterps',
				    'key'		=> false,
				    'bbf'		=> 'fm_maxjitterinterps-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'help'		=> $this->bbf('hlp_fm_maxjitterinterps'),
				    'selected'	=> $this->get_var('info','maxjitterinterps','var_val'),
				    'default'	=> $element['maxjitterinterps']['default']),
			      $element['maxjitterinterps']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_resyncthreshold'),
				    'name'		=> 'resyncthreshold',
				    'labelid'	=> 'resyncthreshold',
				    'key'		=> false,
				    'bbf'		=> 'fm_resyncthreshold-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'help'		=> $this->bbf('hlp_fm_resyncthreshold'),
				    'selected'	=> $this->get_var('info','resyncthreshold','var_val'),
				    'default'	=> $element['resyncthreshold']['default']),
			      $element['resyncthreshold']['value']);
?>
</div>

<div id="sb-part-default" class="b-nodisplay">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_accountcode'),
				  'name'	=> 'accountcode',
				  'labelid'	=> 'accountcode',
				  'size'	=> 15,
				  'help'	=> $this->bbf('hlp_fm_accountcode'),
				  'required'=> false,
				  'value'	=> $this->get_var('info','accountcode','var_val'),
				  'default'	=> $element['accountcode']['default'],
				  'error'	=> $this->bbf_args('error',
					   $this->get_var('error', 'accountcode')) )),

		$form->select(array('desc'	=> $this->bbf('fm_amaflags'),
				    'name'		=> 'amaflags',
				    'labelid'	=> 'amaflags',
				    'key'		=> false,
				    'bbf'		=> 'ast_amaflag_name_info',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'help'		=> $this->bbf('hlp_fm_amaflags'),
				    'selected'	=> $this->get_var('info','amaflags','var_val'),
				    'default'	=> $element['amaflags']['default']),
			      $element['amaflags']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_adsi'),
				      'name'	=> 'adsi',
				      'labelid'	=> 'adsi',
				      'help'	=> $this->bbf('hlp_fm_adsi'),
				      'checked'	=> $this->get_var('info','adsi','var_val'),
				      'default'	=> $element['adsi']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_transfer'),
				    'name'		=> 'transfer',
				    'labelid'	=> 'transfer',
				    'key'		=> false,
				    'bbf'		=> 'fm_transfer-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'help'		=> $this->bbf('hlp_fm_transfer'),
				    'selected'	=> $this->get_var('info','transfer','var_val'),
				    'default'	=> $element['transfer']['default']),
			      $element['transfer']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_language'),
				    'name'		=> 'language',
				    'labelid'	=> 'language',
				    'key'		=> false,
				    'help'		=> $this->bbf('hlp_fm_language'),
				    'selected'	=> $this->get_var('info','language','var_val'),
				    'default'	=> $element['language']['default']),
			      $element['language']['value']);

if($moh_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_mohinterpret'),
				    'name'		=> 'mohinterpret',
				    'labelid'	=> 'mohinterpret',
				    'key'		=> 'category',
				    'help'		=> $this->bbf('hlp_fm_language'),
				    'selected'	=> $this->get_var('info','mohinterpret','var_val'),
				    'default'	=> $element['mohinterpret']['default']),
			      $moh_list),

		$form->select(array('desc'	=> $this->bbf('fm_mohsuggest'),
				    'name'		=> 'mohsuggest',
				    'labelid'	=> 'mohsuggest',
				    'empty'		=> true,
				    'key'		=> 'category',
				    'help'		=> $this->bbf('hlp_fm_language'),
				    'selected'	=> $this->get_var('info','mohsuggest','var_val'),
				    'default'	=> $element['mohsuggest']['default']),
			      $moh_list);
endif;

	echo	$form->select(array('desc'	=> $this->bbf('fm_encryption'),
				    'name'		=> 'encryption',
				    'labelid'	=> 'encryption',
				    'key'		=> false,
				    'bbf'		=> 'fm_encryption-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'help'		=> $this->bbf('hlp_fm_encryption'),
				    'selected'	=> $this->get_var('info','encryption','var_val'),
				    'default'	=> $element['encryption']['default']),
			      $element['encryption']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_maxauthreq'),
				    'name'		=> 'maxauthreq',
				    'labelid'	=> 'maxauthreq',
				    'key'		=> false,
				    'bbf'		=> 'fm_maxauthreq-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'help'		=> $this->bbf('hlp_fm_.language'),
				    'selected'	=> $this->get_var('info','maxauthreq','var_val'),
				    'default'	=> $element['maxauthreq']['default']),
			      $element['maxauthreq']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_codecpriority'),
				    'name'		=> 'codecpriority',
				    'labelid'	=> 'codecpriority',
				    'key'		=> false,
				    'bbf'		=> 'fm_codecpriority-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'help'		=> $this->bbf('hlp_fm_codecpriority'),
				    'selected'	=> $this->get_var('info','codecpriority','var_val'),
				    'default'	=> $element['codecpriority']['default']),
			      $element['codecpriority']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_codec-disallow'),
				    'name'		=> 'disallow',
				    'labelid'	=> 'disallow',
				    'key'		=> false,
				    'help'		=> $this->bbf('hlp_fm_codec-disallow'),
				    'bbf'		=> 'fm_codec-disallow-opt',
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
				    'label'		=> false,
				    'id'		=> 'it-codeclist',
				    'multiple'	=> true,
				    'size'		=> 5,
				    'paragraph'	=> false,
				    'key'		=> false,
				    'bbf'		=> 'ast_codec_name_type',
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
				       'id'			=> 'it-codec',
				       'multiple'	=> true,
				       'size'		=> 5,
				       'paragraph'	=> false,
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

<div id="sb-part-realtime" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_rtcachefriends'),
				      'name'	=> 'rtcachefriends',
				      'labelid'	=> 'rtcachefriends',
				      'help'	=> $this->bbf('hlp_fm_rtcachefriends'),
				      'checked'	=> $this->get_var('info','rtcachefriends','var_val'),
				      'default'	=> $element['rtcachefriends']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_rtupdate'),
				      'name'	=> 'rtupdate',
				      'labelid'	=> 'rtupdate',
				      'help'	=> $this->bbf('hlp_fm_rtupdate'),
				      'checked'	=> $this->get_var('info','rtupdate','var_val'),
				      'default'	=> $element['rtupdate']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_rtignoreregexpire'),
				      'name'	=> 'rtignoreregexpire',
				      'labelid'	=> 'rtignoreregexpire',
				      'help'	=> $this->bbf('hlp_fm_rtignoreregexpire'),
				      'checked'	=> $this->get_var('info','rtignoreregexpire','var_val'),
				      'default'	=> $element['rtignoreregexpire']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_rtautoclear'),
				    'name'		=> 'rtautoclear',
				    'labelid'	=> 'rtautoclear',
				    'key'		=> false,
				    'bbf'		=> 'fm_rtautoclear-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'help'		=> $this->bbf('hlp_fm_rtautoclear'),
				    'selected'	=> $this->get_var('info','rtautoclear','var_val'),
				    'default'	=> $element['rtautoclear']['default']),
			      $element['rtautoclear']['value']);
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_pingtime'),
				    'name'		=> 'pingtime',
				    'labelid'	=> 'pingtime',
				    'key'		=> false,
				    'bbf'		=> 'fm_pingtime-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'help'		=> $this->bbf('hlp_fm_pingtime'),
				    'selected'	=> $this->get_var('info','pingtime','var_val'),
				    'default'	=> $element['pingtime']['default']),
			      $element['pingtime']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_lagrqtime'),
				    'name'		=> 'lagrqtime',
				    'labelid'	=> 'lagrqtime',
				    'key'		=> false,
				    'bbf'		=> 'fm_lagrqtime-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'help'		=> $this->bbf('hlp_fm_lagrqtime'),
				    'selected'	=> $this->get_var('info','lagrqtime','var_val'),
				    'default'	=> $element['lagrqtime']['default']),
			      $element['lagrqtime']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_nochecksums'),
				      'name'	=> 'nochecksums',
				      'labelid'	=> 'nochecksums',
				      'help'	=> $this->bbf('hlp_fm_nochecksums'),
				      'checked'	=> $this->get_var('info','nochecksums','var_val'),
				      'default'	=> $element['nochecksums']['default'])),

		$form->select(array('desc'	=> $this->bbf('fm_autokill'),
				    'name'		=> 'autokill',
				    'labelid'	=> 'autokill',
				    'key'		=> false,
				    'bbf'		=> 'fm_autokill-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'help'		=> $this->bbf('hlp_fm_autokill'),
				    'selected'	=> $this->get_var('info','autokill','var_val'),
				    'default'	=> $element['autokill']['default']),
			      $element['autokill']['value']),

		$form->checkbox(array('desc'	=> $this->bbf('fm_requiredcalltoken'),
				      'name'	=> 'requiredcalltoken',
				      'labelid'	=> 'requiredcalltoken',
				      'help'	=> $this->bbf('hlp_fm_requiredcalltoken'),
				      'checked'	=> $this->get_var('info','requiredcalltoken','var_val'),
				      'default'	=> $element['requiredcalltoken']['default'])),

        $form->text(array('desc'	=> $this->bbf('fm_calltokenoptional'),
				  'name'	=> 'calltokenoptional',
				  'labelid'	=> 'calltokenoptional',
				  'size'	=> 25,
				  'help'	=> $this->bbf('hlp_fm_calltokenoptional'),
				  'required'=> false,
				  'value'	=> $this->get_var('info','calltokenoptional','var_val'),
				  'default'	=> $element['calltokenoptional']['default'],
				  'error'	=> $this->bbf_args('error',
					   $this->get_var('error', 'calltokenoptional')) ));
?>
</div>
	<?=$form->submit(array('name'	=> 'submit',
			       'id'		=> 'it-submit',
			       'value'	=> $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>
