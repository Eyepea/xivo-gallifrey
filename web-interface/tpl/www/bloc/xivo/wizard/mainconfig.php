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

$element = $this->get_var('element');

?>
<fieldset id="fld-mainconfig-hostname">
	<legend><?=$this->bbf('fld-mainconfig-hostname');?></legend>
<?php

echo	$form->text(array('desc'	=> $this->bbf('fm_mainconfig_hostname'),
			  'name'	=> 'mainconfig[hostname]',
			  'labelid'	=> 'mainconfig-hostname',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_mainconfig_hostname'),
			  'comment'	=> $this->bbf('cmt_fm_mainconfig_hostname'),
			  'default'	=> $element['mainconfig']['hostname'],
			  'value'	=> $this->get_var('info','mainconfig','hostname'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','mainconfig','hostname'))));

?>
</fieldset>

<fieldset id="fld-mainconfig-adminpasswd">
	<legend><?=$this->bbf('fld-mainconfig-adminpasswd');?></legend>
<?php

echo	$form->password(array('desc'	=> $this->bbf('fm_mainconfig_adminpasswd'),
			      'name'	=> 'mainconfig[adminpasswd]',
			      'labelid'	=> 'mainconfig-adminpasswd',
			      'size'	=> 15,
			      'help'	=> $this->bbf('hlp_fm_mainconfig_adminpasswd'),
			      'comment'	=> $this->bbf('cmt_fm_mainconfig_adminpasswd'),
			      'default'	=> $element['mainconfig']['adminpasswd'],
			      'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','mainconfig','adminpasswd')))),

	$form->password(array('desc'	=> $this->bbf('fm_mainconfig_confirmadminpasswd'),
			      'name'	=> 'mainconfig[confirmadminpasswd]',
			      'labelid'	=> 'mainconfig-confirmadminpasswd',
			      'size'	=> 15,
			      'help'	=> $this->bbf('hlp_fm_mainconfig_confirmadminpasswd'),
			      'comment'	=> $this->bbf('cmt_fm_mainconfig_confirmadminpasswd'),
			      'default'	=> $element['mainconfig']['confirmadminpasswd'],
			      'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','mainconfig','confirmadminpasswd'))));

?>
</fieldset>

<fieldset id="fld-mainconfig-netiface">
	<legend><?=$this->bbf('fld-mainconfig-netiface');?></legend>
<?php

echo	$form->text(array('desc'	=> $this->bbf('fm_netiface_address'),
			  'name'	=> 'netiface[address]',
			  'labelid'	=> 'netiface-address',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_netiface_address'),
			  'comment'	=> $this->bbf('cmt_fm_netiface_address'),
			  'default'	=> $element['netiface']['address'],
			  'value'	=> $this->get_var('info','netiface','address'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','netiface','address')))),

	$form->text(array('desc'	=> $this->bbf('fm_netiface_netmask'),
			  'name'	=> 'netiface[netmask]',
			  'labelid'	=> 'netiface-netmask',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_netiface_netmask'),
			  'comment'	=> $this->bbf('cmt_fm_netiface_netmask'),
			  'default'	=> $element['netiface']['netmask'],
			  'value'	=> $this->get_var('info','netiface','netmask'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','netiface','netmask')))),

	$form->text(array('desc'	=> $this->bbf('fm_netiface_gateway'),
			  'name'	=> 'netiface[gateway]',
			  'labelid'	=> 'netiface-gateway',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_netiface_gateway'),
			  'comment'	=> $this->bbf('cmt_fm_netiface_gateway'),
			  'default'	=> $element['netiface']['gateway'],
			  'value'	=> $this->get_var('info','netiface','gateway'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','netiface','gateway'))));

?>
</fieldset>

<fieldset id="fld-mainconfig-resolvconf">
	<legend><?=$this->bbf('fld-mainconfig-resolvconf');?></legend>
<?php

echo	$form->text(array('desc'	=> $this->bbf('fm_resolvconf_nameserver1'),
			  'name'	=> 'resolvconf[nameserver1]',
			  'labelid'	=> 'resolvconf-nameserver1',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_resolvconf_nameserver1'),
			  'comment'	=> $this->bbf('cmt_fm_resolvconf_nameserver1'),
			  'default'	=> $element['resolvconf']['nameserver1'],
			  'value'	=> $this->get_var('info','resolvconf','nameserver1'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','resolvconf','nameserver1')))),

	$form->text(array('desc'	=> $this->bbf('fm_resolvconf_nameserver2'),
			  'name'	=> 'resolvconf[nameserver2]',
			  'labelid'	=> 'resolvconf-nameserver2',
			  'size'	=> 15,
			  'help'	=> $this->bbf('hlp_fm_resolvconf_nameserver2'),
			  'comment'	=> $this->bbf('cmt_fm_resolvconf_nameserver2'),
			  'default'	=> $element['resolvconf']['nameserver2'],
			  'value'	=> $this->get_var('info','resolvconf','nameserver2'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','resolvconf','nameserver2'))));

?>
</fieldset>
