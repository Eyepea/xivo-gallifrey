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
$dhtml = &$this->get_module('dhtml');

$element = $this->get_var('element');
$error = $this->get_var('error');
$sound_list = $this->get_var('sound_list');

if(($fm_save = $this->get_var('fm_save')) === true):
	$dhtml->write_js('xivo_form_result(true,\''.$dhtml->escape($this->bbf('fm_success-save')).'\');');
elseif($fm_save === false):
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

$invalid = array();
$invalid['extenfeatures'] = array();
$invalid['generalfeatures'] = array();
$invalid['featuremap'] = array();

$error_js = array();
$error_nb = count($error['extenfeatures']);

for($i = 0;$i < $error_nb;$i++):
	$error_js[] = 'dwho.form.error[\'it-extenfeatures-'.$error['extenfeatures'][$i].'\'] = true;';
	$invalid['extenfeatures'][$error['extenfeatures'][$i]] = true;
endfor;

$error_nb = count($error['generalfeatures']);

for($i = 0;$i < $error_nb;$i++):
	$error_js[] = 'dwho.form.error[\'it-generalfeatures-'.$error['generalfeatures'][$i].'\'] = true;';
	$invalid['generalfeatures'][$error['generalfeatures'][$i]] = true;
endfor;

$error_nb = count($error['featuremap']);

for($i = 0;$i < $error_nb;$i++):
	$error_js[] = 'dwho.form.error[\'it-featuremap-'.$error['featuremap'][$i].'\'] = true;';
	$invalid['featuremap'][$error['featuremap'][$i]] = true;
endfor;

if(isset($error_js[0]) === true)
	$dhtml->write_js($error_js);

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
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div onclick="dwho_submenu.select(dwho_eid('dwsm-tab-1'),'sb-part-first');">
				<div class="tab">
					<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_general');?></a></span>
				</div>
				<span class="span-right">&nbsp;</span>
			</div>
			<div class="stab">
				<ul>
					<li><a href="#"
					       onclick="dwho_submenu.select(dwho_eid('dwsm-tab-1'),'sb-part-call');
							return(false);"><?=$this->bbf('smenu_calls');?></a></li>
					<li><a href="#"
					       onclick="dwho_submenu.select(dwho_eid('dwsm-tab-1'),'sb-part-transfer');
							return(false);"><?=$this->bbf('smenu_transfers');?></a></li>
					<li><a href="#"
					       onclick="dwho_submenu.select(dwho_eid('dwsm-tab-1'),'sb-part-forward');
							return(false);"><?=$this->bbf('smenu_forwards');?></a></li>
				</ul>
			</div>
		</li>
		<li id="dwsm-tab-2"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-voicemail');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_voicemail');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-3"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-agent');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_agent');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-4"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-group');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_groups');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-5"
		    class="dwsm-blur-last"
		    onmouseout="dwho_submenu.blur(this,1);"
		    onmouseover="dwho_submenu.focus(this,1);">
			<div onclick="dwho_submenu.select(dwho_eid('dwsm-tab-5'),'sb-part-last',1);">
				<div class="tab">
					<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_advanced');?></a></span>
				</div>
				<span class="span-right">&nbsp;</span>
			</div>
			<div class="stab">
				<ul>
					<li><a href="#"
					       onclick="dwho_submenu.select(dwho_eid('dwsm-tab-5'),'sb-part-parking',1);
							return(false);"><?=$this->bbf('smenu_parking');?></a></li>
				</ul>
			</div>
		</li>
	</ul>
</div>

<div class="sb-content">
	<form action="#" method="post" accept-charset="utf-8">
<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),

		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1));
?>
	<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_featuremap_automon'),
				  'name'	=> 'featuremap[automon]',
				  'labelid'	=> 'featuremap-automon',
				  'size'	=> 15,
				  'value'	=> $this->get_var('featuremap','automon','var_val'),
				  'default'	=> $element['featuremap']['automon']['default'],
				  'invalid'	=> isset($invalid['featuremap']['automon']))),

		$form->text(array('desc'	=> $this->bbf('fm_featuremap_disconnect'),
				  'name'	=> 'featuremap[disconnect]',
				  'labelid'	=> 'featuremap-disconnect',
				  'size'	=> 15,
				  'value'	=> $this->get_var('featuremap','disconnect','var_val'),
				  'default'	=> $element['featuremap']['disconnect']['default'],
				  'invalid'	=> isset($invalid['featuremap']['disconnect']))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-recsnd'),
				      'name'	=> 'extenfeatures[recsnd][enable]',
				      'labelid'	=> 'extenfeatures-enable-recsnd',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','recsnd','commented') === false))),

		$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[recsnd][exten]',
				  'labelid'	=> 'extenfeatures-recsnd',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','recsnd','exten'),
				  'default'	=> $element['extenfeatures']['recsnd']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['recsnd']))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-phonestatus'),
				      'name'	=> 'extenfeatures[phonestatus][enable]',
				      'labelid'	=> 'extenfeatures-enable-phonestatus',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','phonestatus','commented') === false))),

		$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[phonestatus][exten]',
				  'labelid'	=> 'extenfeatures-phonestatus',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','phonestatus','exten'),
				  'default'	=> $element['extenfeatures']['phonestatus']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['phonestatus']))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-enablednd'),
				      'name'	=> 'extenfeatures[enablednd][enable]',
				      'labelid'	=> 'extenfeatures-enable-enablednd',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','enablednd','commented') === false))),

		$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[enablednd][exten]',
				  'labelid'	=> 'extenfeatures-enablednd',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','enablednd','exten'),
				  'default'	=> $element['extenfeatures']['enablednd']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['enablednd']))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-callrecord'),
				      'name'	=> 'extenfeatures[callrecord][enable]',
				      'labelid'	=> 'extenfeatures-enable-callrecord',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','callrecord','commented') === false))),

		$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[callrecord][exten]',
				  'labelid'	=> 'extenfeatures-callrecord',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','callrecord','exten'),
				  'default'	=> $element['extenfeatures']['callrecord']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['callrecord']))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-incallfilter'),
				      'name'	=> 'extenfeatures[incallfilter][enable]',
				      'labelid'	=> 'extenfeatures-enable-incallfilter',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','incallfilter','commented') === false))),

		$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[incallfilter][exten]',
				  'labelid'	=> 'extenfeatures-incallfilter',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','incallfilter','exten'),
				  'default'	=> $element['extenfeatures']['incallfilter']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['incallfilter']))),

		$form->text(array('desc'	=> $this->bbf('fm_generalfeatures_pickupexten'),
				  'name'	=> 'generalfeatures[pickupexten]',
				  'labelid'	=> 'generalfeatures-pickupexten',
				  'size'	=> 15,
				  'value'	=> $this->get_var('generalfeatures','pickupexten','var_val'),
				  'default'	=> $element['generalfeatures']['pickupexten']['default'],
				  'invalid'	=> isset($invalid['generalfeatures']['pickupexten']))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-pickup'),
				      'name'	=> 'extenfeatures[pickup][enable]',
				      'labelid'	=> 'extenfeatures-enable-pickup',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','pickup','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[pickup][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-pickup',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','pickup','exten'),
				  'default'	=> $element['extenfeatures']['pickup']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['pickup']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-pickup]',
				    'labelid'	=> 'extenfeatures-list-pickup',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-calllistening'),
				      'name'	=> 'extenfeatures[calllistening][enable]',
				      'labelid'	=> 'extenfeatures-enable-calllistening',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','calllistening','commented') === false))),

		$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[calllistening][exten]',
				  'labelid'	=> 'extenfeatures-calllistening',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','calllistening','exten'),
				  'default'	=> $element['extenfeatures']['calllistening']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['calllistening']))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-directoryaccess'),
				      'name'	=> 'extenfeatures[directoryaccess][enable]',
				      'labelid'	=> 'extenfeatures-enable-directoryaccess',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','directoryaccess','commented') === false))),

		$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[directoryaccess][exten]',
				  'labelid'	=> 'extenfeatures-directoryaccess',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','directoryaccess','exten'),
				  'default'	=> $element['extenfeatures']['directoryaccess']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['directoryaccess']))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-bsfilter'),
				      'name'	=> 'extenfeatures[bsfilter][enable]',
				      'labelid'	=> 'extenfeatures-enable-bsfilter',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','bsfilter','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[bsfilter][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-bsfilter',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','bsfilter','exten'),
				  'default'	=> $element['extenfeatures']['bsfilter']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['bsfilter']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-bsfilter]',
				    'labelid'	=> 'extenfeatures-list-bsfilter',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
	</div>

	<div id="sb-part-call" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-callgroup'),
				      'name'	=> 'extenfeatures[callgroup][enable]',
				      'labelid'	=> 'extenfeatures-enable-callgroup',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','callgroup','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[callgroup][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-callgroup',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','callgroup','exten'),
				  'default'	=> $element['extenfeatures']['callgroup']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['callgroup']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-callgroup]',
				    'labelid'	=> 'extenfeatures-list-callgroup',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-callmeetme'),
				      'name'	=> 'extenfeatures[callmeetme][enable]',
				      'labelid'	=> 'extenfeatures-enable-callmeetme',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','callmeetme','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[callmeetme][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-callmeetme',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','callmeetme','exten'),
				  'default'	=> $element['extenfeatures']['callmeetme']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['callmeetme']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-callmeetme]',
				    'labelid'	=> 'extenfeatures-list-callmeetme',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-callqueue'),
				      'name'	=> 'extenfeatures[callqueue][enable]',
				      'labelid'	=> 'extenfeatures-enable-callqueue',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','callqueue','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[callqueue][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-callqueue',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','callqueue','exten'),
				  'default'	=> $element['extenfeatures']['callqueue']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['callqueue']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-callqueue]',
				    'labelid'	=> 'extenfeatures-list-callqueue',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-calluser'),
				      'name'	=> 'extenfeatures[calluser][enable]',
				      'labelid'	=> 'extenfeatures-enable-calluser',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','calluser','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[calluser][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-calluser',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','calluser','exten'),
				  'default'	=> $element['extenfeatures']['calluser']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['calluser']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-calluser]',
				    'labelid'	=> 'extenfeatures-list-calluser',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
	</div>

	<div id="sb-part-transfer" class="b-nodisplay">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_featuremap_blindxfer'),
				  'name'	=> 'featuremap[blindxfer]',
				  'labelid'	=> 'featuremap-blindxfer',
				  'size'	=> 15,
				  'value'	=> $this->get_var('featuremap','blindxfer','var_val'),
				  'default'	=> $element['featuremap']['blindxfer']['default'],
				  'invalid'	=> isset($invalid['featuremap']['blindxfer']))),

		$form->text(array('desc'	=> $this->bbf('fm_featuremap_atxfer'),
				  'name'	=> 'featuremap[atxfer]',
				  'labelid'	=> 'featuremap-atxfer',
				  'size'	=> 15,
				  'value'	=> $this->get_var('featuremap','atxfer','var_val'),
				  'default'	=> $element['featuremap']['atxfer']['default'],
				  'invalid'	=> isset($invalid['featuremap']['atxfer']))),

		$form->select(array('desc'	=> $this->bbf('fm_generalfeatures_transferdigittimeout'),
				    'name'	=> 'generalfeatures[transferdigittimeout]',
				    'labelid'	=> 'generalfeatures-transferdigittimeout',
				    'key'	=> false,
				    'bbf'	=> 'fm_generalfeatures_transferdigittimeout-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('generalfeatures','transferdigittimeout','var_val'),
				    'default'	=> $element['generalfeatures']['transferdigittimeout']['default']),
			      $element['generalfeatures']['transferdigittimeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_generalfeatures_xfersound'),
				    'name'	=> 'generalfeatures[xfersound]',
				    'labelid'	=> 'generalfeatures-xfersound',
				    'empty'	=> $this->bbf('fm_generalfeatures_xfersound-opt','default'),
				    'default'	=> $element['generalfeatures']['xfersound']['default'],
				    'selected'	=> $this->get_var('generalfeatures','xfersound','var_val')),
			      $sound_list),

		$form->select(array('desc'	=> $this->bbf('fm_generalfeatures_xferfailsound'),
				    'name'	=> 'generalfeatures[xferfailsound]',
				    'labelid'	=> 'generalfeatures-xferfailsound',
				    'empty'	=> $this->bbf('fm_generalfeatures_xferfailsound-opt','default'),
				    'default'	=> $element['generalfeatures']['xferfailsound']['default'],
				    'selected'	=> $this->get_var('generalfeatures','xferfailsound','var_val')),
			      $sound_list);
?>
	</div>

	<div id="sb-part-forward" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-fwdundoall'),
				      'name'	=> 'extenfeatures[fwdundoall][enable]',
				      'labelid'	=> 'extenfeatures-enable-fwdundoall',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','fwdundoall','commented') === false))),

		$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[fwdundoall][exten]',
				  'labelid'	=> 'extenfeatures-fwdundoall',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','fwdundoall','exten'),
				  'default'	=> $element['extenfeatures']['fwdundoall']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['fwdundoall']))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-fwdrna'),
				      'name'	=> 'extenfeatures[fwdrna][enable]',
				      'labelid'	=> 'extenfeatures-enable-fwdrna',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','fwdrna','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[fwdrna][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-fwdrna',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','fwdrna','exten'),
				  'default'	=> $element['extenfeatures']['fwdrna']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['fwdrna']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-fwdrna]',
				    'labelid'	=> 'extenfeatures-list-fwdrna',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-fwdbusy'),
				      'name'	=> 'extenfeatures[fwdbusy][enable]',
				      'labelid'	=> 'extenfeatures-enable-fwdbusy',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','fwdbusy','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[fwdbusy][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-fwdbusy',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','fwdbusy','exten'),
				  'default'	=> $element['extenfeatures']['fwdbusy']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['fwdbusy']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-fwdbusy]',
				    'labelid'	=> 'extenfeatures-list-fwdbusy',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-fwdunc'),
				      'name'	=> 'extenfeatures[fwdunc][enable]',
				      'labelid'	=> 'extenfeatures-enable-fwdunc',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','fwdunc','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[fwdunc][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-fwdunc',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','fwdunc','exten'),
				  'default'	=> $element['extenfeatures']['fwdunc']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['fwdunc']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-fwdunc]',
				    'labelid'	=> 'extenfeatures-list-fwdunc',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
	</div>

	<div id="sb-part-voicemail" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-enablevm'),
				      'name'	=> 'extenfeatures[enablevm][enable]',
				      'labelid'	=> 'extenfeatures-enable-enablevm',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','enablevm','commented') === false))),

		$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[enablevm][exten]',
				  'labelid'	=> 'extenfeatures-enablevm',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','enablevm','exten'),
				  'default'	=> $element['extenfeatures']['enablevm']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['enablevm']))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-enablevmslt'),
				      'name'	=> 'extenfeatures[enablevmslt][enable]',
				      'labelid'	=> 'extenfeatures-enable-enablevmslt',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','enablevmslt','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[enablevmslt][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-enablevmslt',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','enablevmslt','exten'),
				  'default'	=> $element['extenfeatures']['enablevmslt']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['enablevmslt']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-enablevmslt]',
				    'labelid'	=> 'extenfeatures-list-enablevmslt',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-enablevmbox'),
				      'name'	=> 'extenfeatures[enablevmbox][enable]',
				      'labelid'	=> 'extenfeatures-enable-enablevmbox',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','enablevmbox','commented') === false))),

		$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[enablevmbox][exten]',
				  'labelid'	=> 'extenfeatures-enablevmbox',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','enablevmbox','exten'),
				  'default'	=> $element['extenfeatures']['enablevmbox']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['enablevmbox']))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-enablevmboxslt'),
				      'name'	=> 'extenfeatures[enablevmboxslt][enable]',
				      'labelid'	=> 'extenfeatures-enable-enablevmboxslt',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','enablevmboxslt','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[enablevmboxslt][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-enablevmboxslt',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','enablevmboxslt','exten'),
				  'default'	=> $element['extenfeatures']['enablevmboxslt']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['enablevmboxslt']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-enablevmboxslt]',
				    'labelid'	=> 'extenfeatures-list-enablevmboxslt',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-vmusermsg'),
				      'name'	=> 'extenfeatures[vmusermsg][enable]',
				      'labelid'	=> 'extenfeatures-enable-vmusermsg',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','vmusermsg','commented') === false))),

		$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[vmusermsg][exten]',
				  'labelid'	=> 'extenfeatures-vmusermsg',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','vmusermsg','exten'),
				  'default'	=> $element['extenfeatures']['vmusermsg']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['vmusermsg']))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-vmboxmsgslt'),
				      'name'	=> 'extenfeatures[vmboxmsgslt][enable]',
				      'labelid'	=> 'extenfeatures-enable-vmboxmsgslt',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','vmboxmsgslt','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[vmboxmsgslt][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-vmboxmsgslt',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','vmboxmsgslt','exten'),
				  'default'	=> $element['extenfeatures']['vmboxmsgslt']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['vmboxmsgslt']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-vmboxmsgslt]',
				    'labelid'	=> 'extenfeatures-list-vmboxmsgslt',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-vmuserslt'),
				      'name'	=> 'extenfeatures[vmuserslt][enable]',
				      'labelid'	=> 'extenfeatures-enable-vmuserslt',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','vmuserslt','commented') === false)));
?>

		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[vmuserslt][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-vmuserslt',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','vmuserslt','exten'),
				  'default'	=> $element['extenfeatures']['vmuserslt']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['vmuserslt']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-vmuserslt]',
				    'labelid'	=> 'extenfeatures-list-vmuserslt',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-vmboxslt'),
				      'name'	=> 'extenfeatures[vmboxslt][enable]',
				      'labelid'	=> 'extenfeatures-enable-vmboxslt',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','vmboxslt','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[vmboxslt][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-vmboxslt',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','vmboxslt','exten'),
				  'default'	=> $element['extenfeatures']['vmboxslt']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['vmboxslt']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-vmboxslt]',
				    'labelid'	=> 'extenfeatures-list-vmboxslt',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-vmuserpurge'),
				      'name'	=> 'extenfeatures[vmuserpurge][enable]',
				      'labelid'	=> 'extenfeatures-enable-vmuserpurge',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','vmuserpurge','commented') === false))),

		$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[vmuserpurge][exten]',
				  'labelid'	=> 'extenfeatures-vmuserpurge',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','vmuserpurge','exten'),
				  'default'	=> $element['extenfeatures']['vmuserpurge']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['vmuserpurge']))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-vmuserpurgeslt'),
				      'name'	=> 'extenfeatures[vmuserpurgeslt][enable]',
				      'labelid'	=> 'extenfeatures-enable-vmuserpurgeslt',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','vmuserpurgeslt','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[vmuserpurgeslt][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-vmuserpurgeslt',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','vmuserpurgeslt','exten'),
				  'default'	=> $element['extenfeatures']['vmuserpurgeslt']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['vmuserpurgeslt']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-vmuserpurgeslt]',
				    'labelid'	=> 'extenfeatures-list-vmuserpurgeslt',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-vmboxpurgeslt'),
				      'name'	=> 'extenfeatures[vmboxpurgeslt][enable]',
				      'labelid'	=> 'extenfeatures-enable-vmboxpurgeslt',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','vmboxpurgeslt','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[vmboxpurgeslt][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-vmboxpurgeslt',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','vmboxpurgeslt','exten'),
				  'default'	=> $element['extenfeatures']['vmboxpurgeslt']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['vmboxpurgeslt']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-vmboxpurgeslt]',
				    'labelid'	=> 'extenfeatures-list-vmboxpurgeslt',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
	</div>

	<div id="sb-part-agent" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-agentstaticlogtoggle'),
				      'name'	=> 'extenfeatures[agentstaticlogtoggle][enable]',
				      'labelid'	=> 'extenfeatures-enable-agentstaticlogtoggle',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','agentstaticlogtoggle','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[agentstaticlogtoggle][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-agentstaticlogtoggle',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','agentstaticlogtoggle','exten'),
				  'default'	=> $element['extenfeatures']['agentstaticlogtoggle']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['agentstaticlogtoggle']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-agentstaticlogtoggle]',
				    'labelid'	=> 'extenfeatures-list-agentstaticlogtoggle',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-agentstaticlogin'),
				      'name'	=> 'extenfeatures[agentstaticlogin][enable]',
				      'labelid'	=> 'extenfeatures-enable-agentstaticlogin',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','agentstaticlogin','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[agentstaticlogin][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-agentstaticlogin',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','agentstaticlogin','exten'),
				  'default'	=> $element['extenfeatures']['agentstaticlogin']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['agentstaticlogin']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-agentstaticlogin]',
				    'labelid'	=> 'extenfeatures-list-agentstaticlogin',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-agentstaticlogoff'),
				      'name'	=> 'extenfeatures[agentstaticlogoff][enable]',
				      'labelid'	=> 'extenfeatures-enable-agentstaticlogoff',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','agentstaticlogoff','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[agentstaticlogoff][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-agentstaticlogoff',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','agentstaticlogoff','exten'),
				  'default'	=> $element['extenfeatures']['agentstaticlogoff']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['agentstaticlogoff']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-agentstaticlogoff]',
				    'labelid'	=> 'extenfeatures-list-agentstaticlogoff',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-agentdynamiclogin'),
				      'name'	=> 'extenfeatures[agentdynamiclogin][enable]',
				      'labelid'	=> 'extenfeatures-enable-agentdynamiclogin',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','agentdynamiclogin','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[agentdynamiclogin][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-agentdynamiclogin',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','agentdynamiclogin','exten'),
				  'default'	=> $element['extenfeatures']['agentdynamiclogin']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['agentdynamiclogin']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-agentdynamiclogin]',
				    'labelid'	=> 'extenfeatures-list-agentdynamiclogin',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
	</div>

	<div id="sb-part-group" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-grouptogglemember'),
				      'name'	=> 'extenfeatures[grouptogglemember][enable]',
				      'labelid'	=> 'extenfeatures-enable-grouptogglemember',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','grouptogglemember','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[grouptogglemember][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-grouptogglemember',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','grouptogglemember','exten'),
				  'default'	=> $element['extenfeatures']['grouptogglemember']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['grouptogglemember']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-grouptogglemember]',
				    'labelid'	=> 'extenfeatures-list-grouptogglemember',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-groupaddmember'),
				      'name'	=> 'extenfeatures[groupaddmember][enable]',
				      'labelid'	=> 'extenfeatures-enable-groupaddmember',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','groupaddmember','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[groupaddmember][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-groupaddmember',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','groupaddmember','exten'),
				  'default'	=> $element['extenfeatures']['groupaddmember']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['groupaddmember']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-groupaddmember]',
				    'labelid'	=> 'extenfeatures-list-groupaddmember',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-groupremovemember'),
				      'name'	=> 'extenfeatures[groupremovemember][enable]',
				      'labelid'	=> 'extenfeatures-enable-groupremovemember',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','groupremovemember','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[groupremovemember][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-groupremovemember',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','groupremovemember','exten'),
				  'default'	=> $element['extenfeatures']['groupremovemember']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['groupremovemember']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-groupremovemember]',
				    'labelid'	=> 'extenfeatures-list-groupremovemember',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-queuetogglemember'),
				      'name'	=> 'extenfeatures[queuetogglemember][enable]',
				      'labelid'	=> 'extenfeatures-enable-queuetogglemember',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','queuetogglemember','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[queuetogglemember][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-queuetogglemember',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','queuetogglemember','exten'),
				  'default'	=> $element['extenfeatures']['queuetogglemember']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['queuetogglemember']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-queuetogglemember]',
				    'labelid'	=> 'extenfeatures-list-queuetogglemember',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-queueaddmember'),
				      'name'	=> 'extenfeatures[queueaddmember][enable]',
				      'labelid'	=> 'extenfeatures-enable-queueaddmember',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','queueaddmember','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[queueaddmember][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-queueaddmember',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','queueaddmember','exten'),
				  'default'	=> $element['extenfeatures']['queueaddmember']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['queueaddmember']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-queueaddmember]',
				    'labelid'	=> 'extenfeatures-list-queueaddmember',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-queueremovemember'),
				      'name'	=> 'extenfeatures[queueremovemember][enable]',
				      'labelid'	=> 'extenfeatures-enable-queueremovemember',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','queueremovemember','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[queueremovemember][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-queueremovemember',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','queueremovemember','exten'),
				  'default'	=> $element['extenfeatures']['queueremovemember']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['queueremovemember']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-queueremovemember]',
				    'labelid'	=> 'extenfeatures-list-queueremovemember',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
	</div>

	<div id="sb-part-parking" class="b-nodisplay">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_generalfeatures_parkext'),
				  'name'	=> 'generalfeatures[parkext]',
				  'labelid'	=> 'generalfeatures-parkext',
				  'size'	=> 15,
				  'value'	=> $this->get_var('generalfeatures','parkext','var_val'),
				  'default'	=> $element['generalfeatures']['parkext']['default'],
				  'invalid'	=> isset($invalid['generalfeatures']['parkext']))),

		$form->text(array('desc'	=> $this->bbf('fm_generalfeatures_context'),
				  'name'	=> 'generalfeatures[context]',
				  'labelid'	=> 'generalfeatures-context',
				  'size'	=> 15,
				  'value'	=> $this->get_var('generalfeatures','context','var_val'),
				  'default'	=> $element['generalfeatures']['context']['default'],
				  'invalid'	=> isset($invalid['generalfeatures']['context'])),
			    'class="it-readonly" readonly="readonly"'),

		$form->select(array('desc'	=> $this->bbf('fm_generalfeatures_parkingtime'),
				    'name'	=> 'generalfeatures[parkingtime]',
				    'labelid'	=> 'generalfeatures-parkingtime',
				    'key'	=> false,
				    'bbf'	=> 'fm_generalfeatures_parkingtime-opt',
				    'bbfopt'	=> array('argmode'	=> 'paramvalue',
							 'time'		=> array(
									'from'		=> 'second',
									'format'	=> '%M%s')),
				    'selected'	=> $this->get_var('generalfeatures','parkingtime','var_val'),
				    'default'	=> $element['generalfeatures']['parkingtime']['default']),
			      $element['generalfeatures']['parkingtime']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_generalfeatures_parkpos'),
				  'name'	=> 'generalfeatures[parkpos]',
				  'labelid'	=> 'generalfeatures-parkpos',
				  'size'	=> 15,
				  'value'	=> $this->get_var('generalfeatures','parkpos','var_val'),
				  'default'	=> $element['generalfeatures']['parkpos']['default'],
				  'invalid'	=> isset($invalid['generalfeatures']['parkpos']))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_generalfeatures_parkfindnext'),
				      'name'	=> 'generalfeatures[parkfindnext]',
				      'labelid'	=> 'generalfeatures-parkfindnext',
				      'checked'	=> $this->get_var('generalfeatures','parkfindnext','var_val'),
				      'default'	=> $element['generalfeatures']['parkfindnext']['default'])),

		$form->checkbox(array('desc'	=> $this->bbf('fm_generalfeatures_adsipark'),
				      'name'	=> 'generalfeatures[adsipark]',
				      'labelid'	=> 'generalfeatures-adsipark',
				      'checked'	=> $this->get_var('generalfeatures','adsipark','var_val'),
				      'default'	=> $element['generalfeatures']['adsipark']['default']));
?>
	</div>

	<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-phoneprogfunckey'),
				      'name'	=> 'extenfeatures[phoneprogfunckey][enable]',
				      'labelid'	=> 'extenfeatures-enable-phoneprogfunckey',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','phoneprogfunckey','commented') === false)));
?>
		<div class="fm-paragraph">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[phoneprogfunckey][exten]',
				  'paragraph'	=> false,
				  'labelid'	=> 'extenfeatures-phoneprogfunckey',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','phoneprogfunckey','exten'),
				  'default'	=> $element['extenfeatures']['phoneprogfunckey']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['phoneprogfunckey']))),

		$form->select(array('paragraph'	=> false,
				    'name'	=> 'extenfeatures[list-phoneprogfunckey]',
				    'labelid'	=> 'extenfeatures-list-phoneprogfunckey',
				    'empty'	=> true,
				    'key'	=> false),
			      array('*',range(3,11)));
?>
		</div>
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_extenfeatures_enable-guestprov'),
				      'name'	=> 'extenfeatures[guestprov][enable]',
				      'labelid'	=> 'extenfeatures-enable-guestprov',
				      'checked'	=> ((bool) $this->get_var('extenfeatures','guestprov','commented') === false))),

		$form->text(array('desc'	=> $this->bbf('fm_extenfeatures-extension'),
				  'name'	=> 'extenfeatures[guestprov][exten]',
				  'labelid'	=> 'extenfeatures-guestprov',
				  'size'	=> 15,
				  'value'	=> $this->get_var('extenfeatures','guestprov','exten'),
				  'default'	=> $element['extenfeatures']['guestprov']['default'],
				  'invalid'	=> isset($invalid['extenfeatures']['guestprov']))),

		$form->select(array('desc'	=> $this->bbf('fm_generalfeatures_featuredigittimeout'),
				    'name'	=> 'generalfeatures[featuredigittimeout]',
				    'labelid'	=> 'generalfeatures-featuredigittimeout',
				    'key'	=> false,
				    'bbf'	=> 'fm_generalfeatures_featuredigittimeout-opt',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'selected'	=> $this->get_var('generalfeatures','featuredigittimeout','var_val'),
				    'default'	=> $element['generalfeatures']['featuredigittimeout']['default']),
			      $element['generalfeatures']['featuredigittimeout']['value']),

		$form->select(array('desc'	=> $this->bbf('fm_generalfeatures_courtesytone'),
				    'name'	=> 'generalfeatures[courtesytone]',
				    'labelid'	=> 'generalfeatures-courtesytone',
				    'empty'	=> $this->bbf('fm_generalfeatures_courtesytone-opt','default'),
				    'default'	=> $element['generalfeatures']['courtesytone']['default'],
				    'selected'	=> $this->get_var('generalfeatures','courtesytone','var_val')),
			      $sound_list);
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
