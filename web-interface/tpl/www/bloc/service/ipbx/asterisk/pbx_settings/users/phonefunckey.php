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

$fkelem = $this->get_varra('element','phonefunckey');
$fkerror = $this->get_varra('error','phonefunckey');
$fkinfo = $this->get_var('fkidentity_list');

if(empty($fkinfo) === false):
	$nb = count($fkinfo);
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('dwho.dom.set_table_list(\'phonefunckey\','.$nb.');');
else:
	$nb = 0;
endif;

?>
<div class="sb-list">
<table cellspacing="0" cellpadding="0" border="0">
	<thead>
	<tr class="sb-top">
		<th class="th-left"><?=$this->bbf('col_phonefunckey-fknum');?></th>
		<th class="th-center"><?=$this->bbf('col_phonefunckey-type');?></th>
		<th class="th-center"><?=$this->bbf('col_phonefunckey-typeval');?></th>
		<th class="th-center"><?=$this->bbf('col_phonefunckey-label');?></th>
		<th class="th-center"><?=$this->bbf('col_phonefunckey-supervision');?></th>
		<th class="th-right"><?=$url->href_html($url->img_html('img/site/button/mini/orange/bo-add.gif',
								       $this->bbf('col_phonefunckey-add'),
								       'border="0"'),
							'#',
							null,
							'onclick="xivo_phonefunckey_add(this);
								  return(dwho.dom.free_focus());"',
							$this->bbf('col_phonefunckey-add'));?></th>
	</tr>
	</thead>
	<tbody id="phonefunckey">
<?php

$fknumelem = array();
$fknumelem['options'] = $fkelem['fknum']['value'];
$fknumelem['default'] = $fkelem['fknum']['default'];

$fkdata = array();

$labelem = array();
$labelem['default'] = $fkelem['label']['default'];

$supelem = array();
$supelem['options'] = $fkelem['supervision']['value'];
$supelem['default'] = $fkelem['supervision']['default'];

if($nb > 0):
	for($i = 0;$i < $nb;$i++):
		$ref = &$fkinfo[$i]['phonefunckey'];

		$fknumelem['value'] = $ref['fknum'];

		$this->set_var('fknumelem',$fknumelem);

		$fkdata['ex'] = false;
		$fkdata['type'] = $ref['type'];
		$fkdata['typeval'] = $ref['typeval'];
		$fkdata['extension'] = $ref['extension'];
		$fkdata['result'] = &$fkinfo[$i]['result'];

		if(array_key_exists($ref['type'],$ref) === true):
			$fkdata[$ref['type']] = $ref[$ref['type']];
		endif;

		$fkdata['incr'] = $i;

		$this->set_var('fkdata',$fkdata);

		$labelem['value'] = $ref['label'];

		$supelem['value'] = $ref['supervision'];

		$this->set_var('labelem',$labelem);
		$this->set_var('supelem',$supelem);

		if(isset($fkerror[$i]) === true):
			$errdisplay = ' l-infos-error';
		else:
			$errdisplay = '';
		endif;

		echo	'<tr class="fm-field',$errdisplay,'">',
			'<td class="td-left txt-center">';

		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/fknum');

		echo	'</td><td>';

		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/type');

		echo	'</td><td>';

		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/user');
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/group');
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/queue');
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/meetme');
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension',
				    array('fktype'	=> 'extension',
					  'fktypeval'	=> ''));

		if($fkdata['extension'] === true):
			$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension',
					array('fktype'		=> $fkdata['type'],
					      'fktypeval'	=> $fkdata['typeval']));
		endif;

		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-agent',
				    array('agenttype'	=> 'extenfeatures-agentstaticlogtoggle'));
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-agent',
				    array('agenttype'	=> 'extenfeatures-agentstaticlogin'));
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-agent',
				    array('agenttype'	=> 'extenfeatures-agentstaticlogoff'));
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-agent',
				    array('agenttype'	=> 'extenfeatures-agentdynamiclogin'));
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-groups',
				    array('grouptype'	=> 'extenfeatures-grouptogglemember'));
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-groups',
				    array('grouptype'	=> 'extenfeatures-groupaddmember'));
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-groups',
				    array('grouptype'	=> 'extenfeatures-groupremovemember'));
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-groups',
				    array('grouptype'	=> 'extenfeatures-queuetogglemember'));
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-groups',
				    array('grouptype'	=> 'extenfeatures-queueaddmember'));
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-groups',
				    array('grouptype'	=> 'extenfeatures-queueremovemember'));
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/bosssecretary');
		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/custom');

		echo	'</td><td>';

		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/label');

		echo	'</td><td>';

		$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/supervision');

		echo	'</td><td class="td-right">',
			$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
						       $this->bbf('opt_phonefunckey-delete'),
						       'border="0"'),
					'#',
					null,
					'onclick="dwho.dom.make_table_list(\'phonefunckey\',this,1); return(dwho.dom.free_focus());"',
					$this->bbf('opt_phonefunckey-delete')),
			'</td></tr>';
	endfor;
endif;
?>
	</tbody>
	<tfoot>
	<tr id="no-phonefunckey"<?=($nb > 0 ? ' class="b-nodisplay"' : '')?>>
		<td colspan="6" class="td-single"><?=$this->bbf('no_phonefunckey');?></td>
	</tr>
	</tfoot>
</table>
<table class="b-nodisplay" cellspacing="0" cellpadding="0" border="0">
	<tbody id="ex-phonefunckey">
	<tr class="fm-field">
<?php
	$this->set_var('fknumelem',$fknumelem);
	$this->set_var('fkdata',array('ex' => true));
	$this->set_var('labelem',$labelem);
	$this->set_var('supelem',$supelem);

	echo	'<td class="td-left txt-center">';

	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/fknum');

	echo	'</td><td>';

	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/type');

	echo	'</td><td>';

	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/user');
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/group');
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/queue');
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/meetme');
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension');
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-agent',
			    array('agenttype'	=> 'extenfeatures-agentstaticlogtoggle'));
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-agent',
			    array('agenttype'	=> 'extenfeatures-agentstaticlogin'));
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-agent',
			    array('agenttype'	=> 'extenfeatures-agentstaticlogoff'));
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-agent',
			    array('agenttype'	=> 'extenfeatures-agentdynamiclogin'));
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-groups',
			    array('grouptype'	=> 'extenfeatures-grouptogglemember'));
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-groups',
			    array('grouptype'	=> 'extenfeatures-groupaddmember'));
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-groups',
			    array('grouptype'	=> 'extenfeatures-groupremovemember'));
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-groups',
			    array('grouptype'	=> 'extenfeatures-queuetogglemember'));
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-groups',
			    array('grouptype'	=> 'extenfeatures-queueaddmember'));
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/extension-groups',
			    array('grouptype'	=> 'extenfeatures-queueremovemember'));
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/bosssecretary');
	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/custom');

	echo	'</td><td>';

	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/label');

	echo	'</td><td>';

	$this->file_include('bloc/service/ipbx/asterisk/pbx_settings/users/phonefunckey/supervision');

	echo	'</td><td class="td-right">',
		$url->href_html($url->img_html('img/site/button/mini/blue/delete.gif',
					       $this->bbf('opt_phonefunckey-delete'),
					       'border="0"'),
				'#',
				null,
				'onclick="dwho.dom.make_table_list(\'phonefunckey\',this,1);
					  return(dwho.dom.free_focus());"',
				$this->bbf('opt_phonefunckey-delete'));
?>
		</td>
	</tr>
	</tbody>
</table>
</div>
