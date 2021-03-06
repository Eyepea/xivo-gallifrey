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
$data     = $info['global'];
$status   = $this->get_var('status');
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
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_status');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-2"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-services');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_services');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-3"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-network');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_network');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-4"
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
	echo	$form->text(array('desc'	=> $this->bbf('fm_ha_status'),
	              'class'   => 'readonly',
				  'size'	=> 20,
				  'readonly'=> true,
			      'help'    => $this->bbf('fm_help-ha_status'),
				  'value'	=> $this->bbf_args('ha_status', $status)));

?></div>

<div id="sb-part-services" class="b-nodisplay">
<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),

		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1)),

		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_apache2'),
				      'name'		=> 'apache2',
				      'labelid'		=> 'ha_apache2',
				      'checked'		=> $data['apache2'],
#				      'help'        => $this->bbf('fm_help-ha_apache2')
				      )),
		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_asterisk'),
				      'name'		=> 'asterisk',
				      'labelid'		=> 'ha_asterisk',
				      'checked'		=> $data['asterisk'],
#				      'help'        => $this->bbf('fm_help-ha_asterisk')
				      )),
		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_dhcp'),
				      'name'		=> 'dhcp',
				      'labelid'		=> 'ha_dhcp',
				      'checked'		=> $data['dhcp'],
#				      'help'        => $this->bbf('fm_help-ha_dhcp')
				      )),
		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_monit'),
				      'name'		=> 'monit',
				      'labelid'		=> 'ha_monit',
				      'checked'		=> $data['monit'],
#				      'help'        => $this->bbf('fm_help-ha_monit')
				      )),
		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_mysql'),
				      'name'		=> 'mysql',
				      'labelid'		=> 'ha_mysql',
				      'checked'		=> $data['mysql'],
#				      'help'        => $this->bbf('fm_help-ha_mysql')
				      )),
		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_ntp'),
				      'name'		=> 'ntp',
				      'labelid'		=> 'ha_ntp',
				      'checked'		=> $data['ntp'],
#				      'help'        => $this->bbf('fm_help-ha_ntp')
				      )),
		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_rsync'),
				      'name'		=> 'rsync',
				      'labelid'		=> 'ha_rsync',
				      'checked'		=> $data['rsync'],
#				      'help'        => $this->bbf('fm_help-ha_rsync')
				      )),
		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_smokeping'),
				      'name'		=> 'smokeping',
				      'labelid'		=> 'ha_smokeping',
				      'checked'		=> $data['smokeping'],
#				      'help'        => $this->bbf('fm_help-ha_smokeping')
				      )),
		$form->checkbox(array('desc'		=> $this->bbf('fm_ha_mailto'),
				      'name'		=> 'mailto',
				      'labelid'		=> 'ha_mailto',
				      'checked'		=> $data['mailto'],
#				      'help'        => $this->bbf('fm_help-ha_mailto')
				      ));

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
	    $this->file_include('bloc/xivo/configuration/network/ha/peer');
    ?>
    </div>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_ha_alert_emails'),
				  'name'	=> 'alert_emails',
				  'labelid'	=> 'alert_emails',
				  'size'	=> 15,
			      'help'    => $this->bbf('fm_help-ha_alert_emails'),
				  'value'	=> $this->get_var('info', 'global', 'alert_emails'))),

	$form->text(array('desc'	=> $this->bbf('fm_ha_serial'),
				  'name'	=> 'serial',
				  'labelid'	=> 'serial',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info', 'global', 'serial'))),
				  
	$form->text(array('desc'	=> $this->bbf('fm_ha_authkeys'),
				  'name'	=> 'authkeys',
				  'labelid'	=> 'authkeys',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info', 'global', 'authkeys'))),

    // bcast, mcast, ucast
	$form->select(array(
	        'desc'      => $this->bbf('fm_ha_com_mode'),
			'name'		=> 'com_mode',
			'id'		=> "it-pf-ha-com_mode",
			'empty'		=> false,
			'key'		=> false,
			'selected'	=> $this->get_var('info', 'global', 'com_mode'),
    		'error'    	=> $this->bbf_args	('error_pf_ha_com_mode', 
		    $this->get_var('error', 'pf_ha_com_mode'))),
		$commodes);
?>
<br/>

<fieldset id="fld-group">
	<legend><?=$this->bbf('fm_ha_user_title');?></legend>
<div>
<?php
    echo $form->text(array('desc'	=> $this->bbf('fm_ha_user'),
				  'name'	=> 'user',
				  'labelid'	=> 'user',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info', 'global', 'user'))),

	$form->text(array('desc'	=> $this->bbf('fm_ha_password'),
				  'name'	=> 'password',
				  'labelid'	=> 'password',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info', 'global', 'password')));
?>
</div>
</fieldset>

<fieldset id="fld-group">
	<legend><?= $this->bbf('fm_ha_dest_user_title') ?></legend>
<div>
<?php
	echo $form->text(array('desc'	=> $this->bbf('fm_ha_dest_user'),
				  'name'	=> 'dest_user',
				  'labelid'	=> 'dest_user',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info', 'global', 'dest_user'))),
				  
	$form->text(array('desc'	=> $this->bbf('fm_ha_dest_password'),
				  'name'	=> 'dest_password',
				  'labelid'	=> 'dest_password',
				  'size'	=> 15,
				  'value'	=> $this->get_var('info', 'global', 'dest_password')));
?>
</div>
</fieldset>

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
