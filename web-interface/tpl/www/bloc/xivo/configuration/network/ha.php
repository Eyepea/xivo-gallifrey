<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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

$form    = &$this->get_module('form');
$dhtml   = &$this->get_module('dhtml');

$info     = $this->get_var('info');
$commodes = $this->get_var('commodes');

if(($fm_save = $this->get_var('fm_save')) === true):
	$dhtml->write_js('xivo_form_result(true,\'' .$dhtml->escape($this->bbf('fm_success-save')).'\');');
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
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_services');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-2"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-network');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_network');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-3"
		    class="dwsm-blur-last"
		    onclick="dwho_submenu.select(this,'sb-part-last',1);"
		    onmouseout="dwho_submenu.blur(this,1);"
		    onmouseover="dwho_submenu.focus(this,1);">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_params');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<div id="sb-part-first">
<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),

		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1)),

		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_apache2'),
				      'name'		=> 'pf-ha-apache2',
				      'labelid'		=> 'ha_apache2',
				      'checked'		=> $info['pf.ha.apache2'],
				      'help'        => $this->bbf('fm_help-ha_apache2'))),
		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_asterisk'),
				      'name'		=> 'pf-ha-asterisk',
				      'labelid'		=> 'ha_asterisk',
				      'checked'		=> $info['pf.ha.asterisk'],
				      'help'        => $this->bbf('fm_help-ha_asterisk'))),
		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_dhcp'),
				      'name'		=> 'pf-ha-dhcp',
				      'labelid'		=> 'ha_dhcp',
				      'checked'		=> $info['pf.ha.dhcp'],
				      'help'        => $this->bbf('fm_help-ha_dhcp'))),
		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_monit'),
				      'name'		=> 'pf-ha-monit',
				      'labelid'		=> 'ha_monit',
				      'checked'		=> $info['pf.ha.monit'],
				      'help'        => $this->bbf('fm_help-ha_monit'))),
		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_mysql'),
				      'name'		=> 'pf-ha-mysql',
				      'labelid'		=> 'ha_mysql',
				      'checked'		=> $info['pf.ha.mysql'],
				      'help'        => $this->bbf('fm_help-ha_mysql'))),
		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_ntp'),
				      'name'		=> 'pf-ha-ntp',
				      'labelid'		=> 'ha_ntp',
				      'checked'		=> $info['pf.ha.ntp'],
				      'help'        => $this->bbf('fm_help-ha_ntp'))),
		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_rsync'),
				      'name'		=> 'pf-ha-rsync',
				      'labelid'		=> 'ha_rsync',
				      'checked'		=> $info['pf.ha.rsync'],
				      'help'        => $this->bbf('fm_help-ha_rsync'))),
		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_smokeping'),
				      'name'		=> 'pf-ha-smokeping',
				      'labelid'		=> 'ha_smokeping',
				      'checked'		=> $info['pf.ha.smokeping'],
				      'help'        => $this->bbf('fm_help-ha_smokeping'))),
		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_mailto'),
				      'name'		=> 'pf-ha-mailto',
				      'labelid'		=> 'ha_mailto',
				      'checked'		=> $info['pf.ha.mailto'],
				      'help'        => $this->bbf('fm_help-ha_mailto')));

?>
</div>

<div id="sb-part-network" class="b-nodisplay">
    <div style="margin-bottom:15px">
    <?php
        $this->file_include('bloc/xivo/configuration/network/ha/uname');
    ?>
    </div>
  
    <div style="margin-bottom:15px">
    <?php
        $this->file_include('bloc/xivo/configuration/network/ha/ping');
    ?>
    </div>

    <div style="margin-bottom:15px">
    <?php
        $this->file_include('bloc/xivo/configuration/network/ha/virtnet');
    ?>
    </div>

    <div>
    <?php
	    $this->file_include('bloc/xivo/configuration/network/ha/nodes');
    ?>
    </div>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_ha_alert_emails'),
				  'name'	=> 'pf-ha-alert_emails',
				  'labelid'	=> 'alert_emails',
				  'size'	=> 15,
			      'help'    => $this->bbf('fm_help-ha_alert_emails'),
				  'value'	=> $this->get_var('info', 'pf.ha.alert_emails'))),

	$form->text(array('desc'	=> $this->bbf('fm_ha_serial'),
				  'name'	=> 'pf-ha-serial',
				  'labelid'	=> 'serial',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info', 'pf.ha.serial'))),
				  
	$form->text(array('desc'	=> $this->bbf('fm_ha_authkeys'),
				  'name'	=> 'pf-ha-authkeys',
				  'labelid'	=> 'authkeys',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info', 'pf.ha.authkeys'))),

    // bcast, mcast, ucast
	$form->select(array(
	        'desc'      => $this->bbf('fm_ha_com_mode'),
			'name'		=> 'pf-ha-com_mode',
			'id'		=> "it-pf-ha-com_mode",
			'empty'		=> false,
			'key'		=> false,
			'selected'	=> $this->get_var('info', 'pf.ha.com_mode'),
    		'error'    	=> $this->bbf_args	('error_pf_ha_com_mode', 
		    $this->get_var('error', 'pf_ha_com_mode'))),
		$commodes);
?>
<br/>
<p><?= $this->bbf('fm_ha_user_title') ?></p>
<div style="border: 1px solid grey; margin: 10px; padding-left: 5px;">
<?php
    echo $form->text(array('desc'	=> $this->bbf('fm_ha_user'),
				  'name'	=> 'pf-ha-user',
				  'labelid'	=> 'user',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info', 'pf.ha.user'))),

	$form->text(array('desc'	=> $this->bbf('fm_ha_password'),
				  'name'	=> 'pf-ha-password',
				  'labelid'	=> 'password',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info', 'pf.ha.password')));
?>
</div>

<br/>
<p><?= $this->bbf('fm_ha_dest_user_title') ?></p>
<div style="border: 1px solid grey; margin: 10px; padding-left: 5px;">
<?php
	echo $form->text(array('desc'	=> $this->bbf('fm_ha_dest_user'),
				  'name'	=> 'pf-ha-dest_user',
				  'labelid'	=> 'dest_user',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info', 'pf.ha.dest_user'))),
				  
	$form->text(array('desc'	=> $this->bbf('fm_ha_dest_password'),
				  'name'	=> 'pf-ha-dest_password',
				  'labelid'	=> 'dest_password',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info', 'pf.ha.dest_password')));
?>
</div>
</div>
<?php

echo	$form->submit(array('name'	=> 'submit',
			    'id'	=> 'it-submit',
			    'value'	=> $this->bbf('fm_bt-save')));

?>
</form>
	</div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>
