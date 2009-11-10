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

$info = $this->get_var('info');
$element = $this->get_var('element');
$interfaces = $this->get_var('interfaces');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

echo	$form->text(array('desc'	=> $this->bbf('fm_name'),
			  'name'	=> 'name',
			  'labelid'	=> 'name',
			  'size'	=> 15,
			  'default'	=> $element['netiface']['name']['default'],
			  'value'	=> $this->get_var_default('info','name',$this->get_var('name')))),

	$form->select(array('desc'	=> $this->bbf('fm_networktype'),
			    'name'	=> 'networktype',
			    'labelid'	=> 'networktype',
			    'key'	=> false,
			    'bbf'	=> 'fm_networktype-opt',
			    'bbfopt'	=> array('argmode' => 'paramvalue'),
			    'default'	=> $element['netiface']['networktype']['default'],
			    'selected'	=> $info['networktype']),
		      $element['netiface']['networktype']['value']),

	$form->select(array('desc'	=> $this->bbf('fm_method'),
			    'name'	=> 'method',
			    'labelid'	=> 'method',
			    'key'	=> false,
			    'bbf'	=> 'fm_method-opt',
			    'bbfopt'	=> array('argmode' => 'paramvalue'),
			    'default'	=> $element['netiface']['method']['default'],
			    'selected'	=> $info['method']),
		      $element['netiface']['method']['value']),

	$form->text(array('desc'	=> $this->bbf('fm_address'),
			  'name'	=> 'address',
			  'labelid'	=> 'address',
			  'size'	=> 15,
			  'default'	=> $element['netiface']['address']['default'],
			  'value'	=> $info['address'])),

	$form->text(array('desc'	=> $this->bbf('fm_netmask'),
			  'name'	=> 'netmask',
			  'labelid'	=> 'netmask',
			  'size'	=> 15,
			  'default'	=> $element['netiface']['netmask']['default'],
			  'value'	=> $info['netmask'])),

	$form->text(array('desc'	=> $this->bbf('fm_broadcast'),
			  'name'	=> 'broadcast',
			  'labelid'	=> 'broadcast',
			  'size'	=> 15,
			  'default'	=> $element['netiface']['broadcast']['default'],
			  'value'	=> $info['broadcast'])),

	$form->text(array('desc'	=> $this->bbf('fm_gateway'),
			  'name'	=> 'gateway',
			  'labelid'	=> 'gateway',
			  'size'	=> 15,
			  'default'	=> $element['netiface']['gateway']['default'],
			  'value'	=> $info['gateway'])),

	$form->text(array('desc'	=> $this->bbf('fm_mtu'),
			  'name'	=> 'mtu',
			  'labelid'	=> 'mtu',
			  'size'	=> 10,
			  'default'	=> $element['netiface']['mtu']['default'],
			  'value'	=> $info['mtu']));

	if(($interfaces = $this->get_var('interfaces')) !== false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_vlan'),
					    'name'	=> 'vlan',
					    'labelid'	=> 'vlan',
					    'empty'	=> true,
					    'key'	=> 'name',
					    'altkey'	=> 'name',
					    'default'	=> $element['netiface']['vlan']['default'],
					    'selected'	=> $info['vlan']),
				      $interfaces);
	endif;
?>
<div class="fm-paragraph fm-description">
	<p>
		<label id="lb-description" for="it-description"><?=$this->bbf('fm_description');?></label>
	</p>
	<?=$form->textarea(array('paragraph'	=> false,
				 'label'	=> false,
				 'name'		=> 'description',
				 'id'		=> 'it-description',
				 'cols'		=> 60,
				 'rows'		=> 5,
				 'default'	=> $element['netiface']['description']['default']),
			   $info['description']);?>
</div>
