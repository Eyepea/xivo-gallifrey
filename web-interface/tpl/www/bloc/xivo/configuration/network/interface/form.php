<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_name'),
				  'name'	=> 'name',
				  'labelid'	=> 'name',
				  'size'	=> 15,
				  'default'	=> $element['netiface']['name']['default'],
				  'value'	=> $info['name'],
		          'error'	=> $this->bbf_args('name',
					   $this->get_var('error', 'name')))),

		$form->text(array('desc'	=> $this->bbf('fm_ifname'),
				  'name'	=> 'ifname',
				  'labelid'	=> 'ifname',
				  'size'	=> 15,
				  'default'	=> $element['netiface']['ifname']['default'],
				  'value'	=> $this->get_var_default('info','ifname',$this->get_var('devinfo','name')),
		          'error'	=> $this->bbf_args('ifname',
					   $this->get_var('error', 'ifname')))),

		$form->checkbox(array('desc'		=> $this->bbf('fm_disable'),
				      'name'		=> 'disable',
				      'labelid'		=> 'disable',
				      'default'		=> $element['netiface']['disable']['default'],
				      'checked'		=> $this->get_var_default('info','disable',$this->get_var('devinfo','auto')),
				      'disabled'	=> $this->get_var('deletable') === false)),

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
				    'bbf'	=> 'network_method',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['netiface']['method']['default'],
				    'selected'	=> $this->get_var_default('info','method',$this->get_var('devinfo','method'))),
			      $element['netiface']['method']['value']),

		$form->text(array('desc'	=> $this->bbf('fm_address'),
				  'name'	=> 'address',
				  'labelid'	=> 'address',
				  'size'	=> 15,
				  'default'	=> $element['netiface']['address']['default'],
				  'value'	=> $this->get_var_default('info','address',$this->get_var('devinfo','address')),
		          'error'	=> $this->bbf_args('address',
					   $this->get_var('error', 'address')))),

		$form->text(array('desc'	=> $this->bbf('fm_netmask'),
				  'name'	=> 'netmask',
				  'labelid'	=> 'netmask',
				  'size'	=> 15,
				  'default'	=> $element['netiface']['netmask']['default'],
				  'value'	=> $this->get_var_default('info','netmask',$this->get_var('devinfo','netmask')),
		          'error'	=> $this->bbf_args('netmask',
					   $this->get_var('error', 'netmask')))),

		$form->text(array('desc'	=> $this->bbf('fm_broadcast'),
				  'name'	=> 'broadcast',
				  'labelid'	=> 'broadcast',
				  'size'	=> 15,
				  'default'	=> $element['netiface']['broadcast']['default'],
				  'value'	=> $this->get_var_default('info','broadcast',$this->get_var('devinfo','broadcast')),
		          'error'	=> $this->bbf_args('broadcast',
					   $this->get_var('error', 'broadcast')))),

		$form->text(array('desc'	=> $this->bbf('fm_gateway'),
				  'name'	=> 'gateway',
				  'labelid'	=> 'gateway',
				  'size'	=> 15,
				  'default'	=> $element['netiface']['gateway']['default'],
				  'value'	=> $this->get_var_default('info','gateway',$this->get_var('devinfo','gateway')),
		          'error'	=> $this->bbf_args('gateway',
					   $this->get_var('error', 'gateway')))),

		$form->text(array('desc'	=> $this->bbf('fm_mtu'),
				  'name'	=> 'mtu',
				  'labelid'	=> 'mtu',
				  'size'	=> 10,
				  'default'	=> $element['netiface']['mtu']['default'],
				  'value'	=> $this->get_var_default('info','mtu',$this->get_var('devinfo','mtu')),
		          'error'	=> $this->bbf_args('mtu',
					   $this->get_var('error', 'mtu'))));
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
					 'default'	=> $element['netiface']['description']['default'],
		          'error'	=> $this->bbf_args('description',
					   $this->get_var('error', 'description'))),
				   $info['description']);?>
	</div>
</div>

<div id="sb-part-vlan" class="b-nodisplay">
<?php
	if(($interfaces = $this->get_var('interfaces')) !== false):
		echo	$form->select(array('desc'	=> $this->bbf('fm_vlanrawdevice'),
					    'name'	=> 'vlanrawdevice',
					    'labelid'	=> 'vlanrawdevice',
					    'empty'	=> true,
					    'key'	=> 'identity',
					    'altkey'	=> 'ifname',
					    'default'	=> $element['netiface']['vlanrawdevice']['default'],
					    'selected'	=> $this->get_var_default('info',
										  'vlanrawdevice',
										  $this->get_var('devinfo','vlan-raw-device'))),
				      $interfaces),

			$form->text(array('desc'	=> $this->bbf('fm_vlanid'),
					  'name'	=> 'vlanid',
					  'labelid'	=> 'vlanid',
					  'size'	=> 10,
					  'default'	=> $element['netiface']['vlanid']['default'],
					  'value'	=> $this->get_var_default('info','vlanid',$this->get_var('devinfo','vlan-id')),
		          'error'	=> $this->bbf_args('vlanid',
					   $this->get_var('error', 'vlanid'))));
	else:
		echo	'<div class="txt-center">',
			$this->bbf('no_available_physical_interface'),
			'</div>';
	endif;
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
	<div class="fm-paragraph fm-description">
		<p>
			<label id="lb-options" for="it-options"><?=$this->bbf('fm_options');?></label>
		</p>
		<?=$form->textarea(array('paragraph'	=> false,
					 'label'	=> false,
					 'name'		=> 'options',
					 'id'		=> 'it-options',
					 'cols'		=> 72,
					 'rows'		=> 30,
					 'default'	=> $element['netiface']['options']['default'],
		          'error'	=> $this->bbf_args('options',
					   $this->get_var('error', 'options'))),
				   $this->get_var_default('info','options',$this->get_var('devinfo','options')));?>
	</div>
</div>
