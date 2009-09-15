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
$error = $this->get_var('error');
$element = $this->get_var('element');
$ipbxapplications = $this->get_var('ipbxapplications');
$context_list = $this->get_var('context_list');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>
<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_voicemenu_name'),
				  'name'	=> 'voicemenu[name]',
				  'labelid'	=> 'voicemenu-name',
				  'size'	=> 15,
				  'default'	=> $element['voicemenu']['name']['default'],
				  'value'	=> $info['voicemenu']['name'])),

		$form->text(array('desc'	=> $this->bbf('fm_voicemenu_number'),
				  'name'	=> 'voicemenu[number]',
				  'labelid'	=> 'voicemenu-number',
				  'size'	=> 10,
				  'default'	=> $element['voicemenu']['number']['default'],
				  'value'	=> $info['voicemenu']['number']));

	if($context_list !== false):
		echo $form->select(array('desc'		=> $this->bbf('fm_voicemenu_context'),
					 'name'		=> 'voicemenu[context]',
					 'labelid'	=> 'voicemenu-context',
					 'key'		=> 'identity',
					 'altkey'	=> 'name',
					 'default'	=> $element['voicemenu']['context']['default'],
					 'value'	=> $info['voicemenu']['context']),
				   $context_list);
	else:
		echo	'<div id="fd-voicemenu-context" class="txt-center">',
			$url->href_html($this->bbf('create_context'),
		     			'service/ipbx/system_management/context',
					'act=add'),
			'</div>';
	endif;
?>
	<div class="fm-field fm-description">
		<p>
			<label id="lb-voicemenu-description" for="it-voicemenu-description"><?=$this->bbf('fm_voicemenu_description');?></label>
		</p>
		<?=$form->textarea(array('field'	=> false,
					 'label'	=> false,
					 'name'		=> 'voicemenu[description]',
					 'id'		=> 'it-voicemenu-description',
					 'cols'		=> 60,
					 'rows'		=> 5,
					 'default'	=> $element['voicemenu']['description']['default']),
				   $info['voicemenu']['description']);?>
	</div>
</div>
<div id="sb-part-voicemenuflow" class="b-nodisplay">
<?php
	$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/voicemenuflow-action',
			    array('event'	=> 'voicemenuflow'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/endcall',
			    array('event'	=> 'voicemenuflow'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/user',
			    array('event'	=> 'voicemenuflow'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/group',
			    array('event'	=> 'voicemenuflow'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/queue',
			    array('event'	=> 'voicemenuflow'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/meetme',
			    array('event'	=> 'voicemenuflow'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/voicemail',
			    array('event'	=> 'voicemenuflow'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/schedule',
			    array('event'	=> 'voicemenuflow'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/voicemenu',
			    array('event'	=> 'voicemenuflow'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/extension',
			    array('event'	=> 'voicemenuflow'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/application',
			    array('event'	=> 'voicemenuflow'));

	echo	'<div id="fd-dialaction-voicemenuflow-ipbxapplication-actiontype" class="b-nodisplay">',

		$form->select(array('desc'	=> $this->bbf('fm_dialaction_ipbxapplication-action'),
				    'name'	=> 'ipbxapplications',
				    'labelid'	=> 'dialaction-voicemenuflow-ipbxapplication-action',
				    'optgroup'	=> array('key'		=> 'category',
				 			 'unique'	=> true,
							 'bbf'		=> 'fm_ipbxapplications-optgroup-'),
				    'empty'	=> true,
				    'key'	=> true,
				    'bbf'	=> 'fm_ipbxapplications-opt-'),
			      $ipbxapplications,
			      'onchange="xivo_ast_chg_ipbxapplication(this.value);"');

	if(isset($ipbxapplications['answer']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/answer',
				    array('apparg_answer'		=> $ipbxapplications['answer']['arg']));
	endif;

	if(isset($ipbxapplications['authenticate']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/authenticate',
				    array('apparg_authenticate'		=> $ipbxapplications['authenticate']['arg']));
	endif;

	if(isset($ipbxapplications['vmauthenticate']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/vmauthenticate',
				    array('apparg_vmauthenticate'	=> $ipbxapplications['vmauthenticate']['arg']));
	endif;

	if(isset($ipbxapplications['macro']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/macro',
				    array('apparg_macro'		=> $ipbxapplications['macro']['arg']));
	endif;

	if(isset($ipbxapplications['agi']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/agi',
				    array('apparg_agi'			=> $ipbxapplications['agi']['arg']));
	endif;

	if(isset($ipbxapplications['goto']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/goto',
				    array('apparg_goto'			=> $ipbxapplications['goto']['arg']));
	endif;

	if(isset($ipbxapplications['gotoif']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/gotoif',
				    array('apparg_gotoif'		=> $ipbxapplications['gotoif']['arg']));
	endif;

	if(isset($ipbxapplications['mixmonitor']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/mixmonitor',
				    array('apparg_mixmonitor'		=> $ipbxapplications['mixmonitor']['arg']));
	endif;

	if(isset($ipbxapplications['monitor']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/monitor',
				    array('apparg_monitor'		=> $ipbxapplications['monitor']['arg']));
	endif;

	if(isset($ipbxapplications['record']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/record',
				    array('apparg_record'		=> $ipbxapplications['record']['arg']));
	endif;

	if(isset($ipbxapplications['stopmonitor']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/stopmonitor',
				    array('apparg_stopmonitor'		=> $ipbxapplications['stopmonitor']['arg']));
	endif;

	if(isset($ipbxapplications['background']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/background',
				    array('apparg_background'		=> $ipbxapplications['background']['arg']));
	endif;

	if(isset($ipbxapplications['playback']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/playback',
				    array('apparg_playback'		=> $ipbxapplications['playback']['arg']));
	endif;

	if(isset($ipbxapplications['absolutetimeout']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/absolutetimeout',
				    array('apparg_absolutetimeout'	=> $ipbxapplications['absolutetimeout']['arg']));
	endif;

	if(isset($ipbxapplications['digittimeout']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/digittimeout',
				    array('apparg_digittimeout'		=> $ipbxapplications['digittimeout']['arg']));
	endif;

	if(isset($ipbxapplications['responsetimeout']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/responsetimeout',
				    array('apparg_responsetimeout'	=> $ipbxapplications['responsetimeout']['arg']));
	endif;

	if(isset($ipbxapplications['read']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/read',
				    array('apparg_read'			=> $ipbxapplications['read']['arg']));
	endif;

	if(isset($ipbxapplications['set']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/set',
				    array('apparg_set'			=> $ipbxapplications['set']['arg']));
	endif;

	if(isset($ipbxapplications['setcallerid']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/setcallerid',
				    array('apparg_setcallerid'		=> $ipbxapplications['setcallerid']['arg']));
	endif;

	if(isset($ipbxapplications['setcidname']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/setcidname',
				    array('apparg_setcidname'		=> $ipbxapplications['setcidname']['arg']));
	endif;

	if(isset($ipbxapplications['setcidnum']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/setcidnum',
				    array('apparg_setcidnum'		=> $ipbxapplications['setcidnum']['arg']));
	endif;

	if(isset($ipbxapplications['setlanguage']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/setlanguage',
				    array('apparg_setlanguage'		=> $ipbxapplications['setlanguage']['arg']));
	endif;

	if(isset($ipbxapplications['noop']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/noop',
				    array('apparg_noop'			=> $ipbxapplications['noop']['arg']));
	endif;

	if(isset($ipbxapplications['wait']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/wait',
				    array('apparg_wait'			=> $ipbxapplications['wait']['arg']));
	endif;

	if(isset($ipbxapplications['waitexten']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/waitexten',
				    array('apparg_waitexten'		=> $ipbxapplications['waitexten']['arg']));
	endif;

	if(isset($ipbxapplications['waitforring']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/waitforring',
				    array('apparg_waitforring'		=> $ipbxapplications['waitforring']['arg']));
	endif;

	if(isset($ipbxapplications['waitmusiconhold']) === true):
		$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/ipbxapplication/waitmusiconhold',
				    array('apparg_waitfmusiconhold'	=> $ipbxapplications['waitmusiconhold']['arg']));
	endif;

	echo	'</div>';
?>
<div id="voicemenu-flow" class="fm-field fm-multilist">

	<div class="slt-list">
		<div class="bt-updown">
			<a href="#"
			   onclick="xivo_fm_order_selected('it-voicemenu-flow',1,true); return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_up_voicemenu-application');?>">
				<?=$url->img_html('img/site/button/row-up.gif',
			   			  $this->bbf('bt_up_voicemenu-application'),
						  'class="bt-uplist" id="bt-voicemenu-application-up" border="0"');?></a><br />
			<a href="#"
			   onclick="xivo_fm_order_selected('it-voicemenu-flow',-1,true); return(xivo.dom.free_focus());"
			   title="<?=$this->bbf('bt_down_voicemenu-application');?>">
				<?=$url->img_html('img/site/button/row-down.gif',
						  $this->bbf('bt_down_voicemenu-application'),
						  'class="bt-downlist" id="bt-voicemenu-application-down" border="0"');?></a>
		</div>

		<?=$form->select(array('name'		=> 'voicemenuflow[]',
				       'label'		=> false,
				       'id'		=> 'it-voicemenu-flow',
				       'key'		=> true,
				       'multiple'	=> true,
				       'size'		=> 5,
				       'field'		=> false),
				 null);?>
	</div>

	<div class="bt-delete">
		<a href="#"
		   onclick="xivo_fm_select_delete_entry('it-voicemenu-flow',true); return(xivo.dom.free_focus());"
		   title="<?=$this->bbf('bt_delete_voicemenu-application');?>">
			<?=$url->img_html('img/site/button/mini/orange/delete.gif',
					  $this->bbf('bt_delete_voicemenu-application'),
					  'class="bt-deletelist" id="bt-voicemenu-application-delete" border="0"');?></a>
	</div>
</div>
<div class="clearboth"></div>
<?php
	echo	$form->select(array('name'	=> 'voicemenuflowhidden[]',
				    'label'	=> false,
				    'id'	=> 'it-voicemenu-flow-hidden',
				    'multiple'	=> true,
				    'key'	=> false,
				    'altkey'	=> '',
				    'cols'	=> 0,
				    'size'	=> 0,
				    'field'	=> false),
			      $info['voicemenuflow-data'],
			      'class="b-nodisplay"');
?>
</div>
<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_voicemenuevent-type'),
				    'name'	=> 'voicemenuevent-type',
				    'labelid'	=> 'voicemenuevent-type',
				    'key'	=> false),
			      $element['voicemenuevent']['event']['value']),

		$form->hidden(array('name'	=> 'voicemenuevent[0]',
				    'id'	=> 'it-voicemenuevent-0',
				    'value'	=> $this->get_varra('voicemenuevent',0))),
		$form->hidden(array('name'	=> 'voicemenuevent[1]',
				    'id'	=> 'it-voicemenuevent-1',
				    'value'	=> $this->get_varra('voicemenuevent',1))),
		$form->hidden(array('name'	=> 'voicemenuevent[2]',
				    'id'	=> 'it-voicemenuevent-2',
				    'value'	=> $this->get_varra('voicemenuevent',2))),
		$form->hidden(array('name'	=> 'voicemenuevent[3]',
				    'id'	=> 'it-voicemenuevent-3',
				    'value'	=> $this->get_varra('voicemenuevent',3))),
		$form->hidden(array('name'	=> 'voicemenuevent[4]',
				    'id'	=> 'it-voicemenuevent-4',
				    'value'	=> $this->get_varra('voicemenuevent',4))),
		$form->hidden(array('name'	=> 'voicemenuevent[5]',
				    'id'	=> 'it-voicemenuevent-5',
				    'value'	=> $this->get_varra('voicemenuevent',5))),
		$form->hidden(array('name'	=> 'voicemenuevent[6]',
				    'id'	=> 'it-voicemenuevent-6',
				    'value'	=> $this->get_varra('voicemenuevent',6))),
		$form->hidden(array('name'	=> 'voicemenuevent[7]',
				    'id'	=> 'it-voicemenuevent-7',
				    'value'	=> $this->get_varra('voicemenuevent',7))),
		$form->hidden(array('name'	=> 'voicemenuevent[8]',
				    'id'	=> 'it-voicemenuevent-8',
				    'value'	=> $this->get_varra('voicemenuevent',8))),
		$form->hidden(array('name'	=> 'voicemenuevent[9]',
				    'id'	=> 'it-voicemenuevent-9',
				    'value'	=> $this->get_varra('voicemenuevent',9))),
		$form->hidden(array('name'	=> 'voicemenuevent[*]',
				    'id'	=> 'it-voicemenuevent-star',
				    'value'	=> $this->get_varra('voicemenuevent','*'))),
		$form->hidden(array('name'	=> 'voicemenuevent[#]',
				    'id'	=> 'it-voicemenuevent-sharp',
				    'value'	=> $this->get_varra('voicemenuevent','#'))),
		$form->hidden(array('name'	=> 'voicemenuevent[a]',
				    'id'	=> 'it-voicemenuevent-a',
				    'value'	=> $this->get_varra('voicemenuevent','a'))),
		$form->hidden(array('name'	=> 'voicemenuevent[o]',
				    'id'	=> 'it-voicemenuevent-o',
				    'value'	=> $this->get_varra('voicemenuevent','o'))),
		$form->hidden(array('name'	=> 'voicemenuevent[t]',
				    'id'	=> 'it-voicemenuevent-t',
				    'value'	=> $this->get_varra('voicemenuevent','t'))),
		$form->hidden(array('name'	=> 'voicemenuevent[T]',
				    'id'	=> 'it-voicemenuevent-tt',
				    'value'	=> $this->get_varra('voicemenuevent','T'))),
		$form->hidden(array('name'	=> 'voicemenuevent[i]',
				    'id'	=> 'it-voicemenuevent-i',
				    'value'	=> $this->get_varra('voicemenuevent','i'))),
		$form->hidden(array('name'	=> 'voicemenuevent[h]',
				    'id'	=> 'it-voicemenuevent-h',
				    'value'	=> $this->get_varra('voicemenuevent','h')));

	$this->file_include('bloc/service/ipbx/asterisk/call_management/voicemenu/voicemenuevent-action',
			    array('event'	=> 'voicemenuevent'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/none',
			    array('event'	=> 'voicemenuevent'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/endcall',
			    array('event'	=> 'voicemenuevent'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/user',
			    array('event'	=> 'voicemenuevent'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/group',
			    array('event'	=> 'voicemenuevent'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/queue',
			    array('event'	=> 'voicemenuevent'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/meetme',
			    array('event'	=> 'voicemenuevent'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/voicemail',
			    array('event'	=> 'voicemenuevent'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/schedule',
			    array('event'	=> 'voicemenuevent'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/voicemenu',
			    array('event'	=> 'voicemenuevent'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/extension',
			    array('event'	=> 'voicemenuevent'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/application',
			    array('event'	=> 'voicemenuevent'));
	$this->file_include('bloc/service/ipbx/asterisk/dialaction/sound',
			    array('event'	=> 'voicemenuevent'));
?>
	<div id="voicemenu-event" class="sb-list">
	<table cellspacing="0" cellpadding="0" border="0">
		<thead>
		<tr class="sb-top">
			<th class="th-left voicemenu-event-desc"><?=$this->bbf('col_voicemenu-event');?></th>
			<th class="th-right"><?=$this->bbf('col_voicemenu-action');?></th>
		</tr>
		</thead>
		<tbody>
		<tr id="voicemenuevent-info-0"
		    class="l-infos-<?=(isset($error['voicemenuevent'][0]) === false ? '1on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-0');?></td>
			<td id="voicemenuevent-0" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-1"
		    class="l-infos-<?=(isset($error['voicemenuevent'][1]) === false ? '2on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-1');?></td>
			<td id="voicemenuevent-1" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-2"
		    class="l-infos-<?=(isset($error['voicemenuevent'][2]) === false ? '1on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-2');?></td>
			<td id="voicemenuevent-2" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-3"
		    class="l-infos-<?=(isset($error['voicemenuevent'][3]) === false ? '2on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-3');?></td>
			<td id="voicemenuevent-3" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-4"
		    class="l-infos-<?=(isset($error['voicemenuevent'][4]) === false ? '1on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-4');?></td>
			<td id="voicemenuevent-4" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-5"
		    class="l-infos-<?=(isset($error['voicemenuevent'][5]) === false ? '2on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-5');?></td>
			<td id="voicemenuevent-5" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-6"
		    class="l-infos-<?=(isset($error['voicemenuevent'][6]) === false ? '1on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-6');?></td>
			<td id="voicemenuevent-6" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-7"
		    class="l-infos-<?=(isset($error['voicemenuevent'][7]) === false ? '2on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-7');?></td>
			<td id="voicemenuevent-7" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-8"
		    class="l-infos-<?=(isset($error['voicemenuevent'][8]) === false ? '1on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-8');?></td>
			<td id="voicemenuevent-8" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-9"
		    class="l-infos-<?=(isset($error['voicemenuevent'][9]) === false ? '2on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-9');?></td>
			<td id="voicemenuevent-9" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-star"
		    class="l-infos-<?=(isset($error['voicemenuevent']['*']) === false ? '1on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-star');?></td>
			<td id="voicemenuevent-star" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-sharp"
		    class="l-infos-<?=(isset($error['voicemenuevent']['#']) === false ? '2on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-sharp');?></td>
			<td id="voicemenuevent-sharp" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-a"
		    class="l-infos-<?=(isset($error['voicemenuevent']['a']) === false ? '1on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-a');?></td>
			<td id="voicemenuevent-a" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-o"
		    class="l-infos-<?=(isset($error['voicemenuevent']['o']) === false ? '2on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-o');?></td>
			<td id="voicemenuevent-o" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-t"
		    class="l-infos-<?=(isset($error['voicemenuevent']['t']) === false ? '1on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-t');?></td>
			<td id="voicemenuevent-t" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-tt"
		    class="l-infos-<?=(isset($error['voicemenuevent']['T']) === false ? '2on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-T');?></td>
			<td id="voicemenuevent-tt" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-i"
		    class="l-infos-<?=(isset($error['voicemenuevent']['i']) === false ? '1on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-i');?></td>
			<td id="voicemenuevent-i" class="td-right txt-center">-</td>
		</tr>
		<tr id="voicemenuevent-info-h"
		    class="l-infos-<?=(isset($error['voicemenuevent']['h']) === false ? '2on2' : 'error')?>">
			<td class="td-left txt-left voicemenu-event-desc"><?=$this->bbf('voicemenu_event-h');?></td>
			<td id="voicemenuevent-h" class="td-right txt-center">-</td>
		</tr>
		</tbody>
	</table>
	</div>
</div>
