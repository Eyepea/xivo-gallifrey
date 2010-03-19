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

$element = $this->get_var('element');
$netifaces = $this->get_var('netifaces');

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

<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<div id="sb-part-first">
<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),

		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1)),

		$form->checkbox(array('desc'		=> $this->bbf('fm_active'),
				      'name'		=> 'active',
				      'labelid'		=> 'active',
				      'checked'		=> $this->get_var('info', 'active'))),

		$form->text(array('desc'	=> $this->bbf('fm_pool_start'),
				  'name'	=> 'pool_start',
				  'labelid'	=> 'pool_start',
				  'size'	=> 15,
				  'default'	=> $element['dhcp']['pool_start']['default'],
				  'value'	=> $this->get_var('info','pool_start'),
				  'error'   => $this->bbf_args('ipaddr', 
                    $this->get_var('error', 'pool_start'))
               )),

		$form->text(array('desc'	=> $this->bbf('fm_pool_end'),
				  'name'	=> 'pool_end',
				  'labelid'	=> 'pool_end',
				  'size'	=> 15,
				  'default'	=> $element['dhcp']['pool_end']['default'],
				  'value'	=> $this->get_var('info','pool_end'),
				  'error'   => $this->bbf_args('ipaddr', 
                    $this->get_var('error', 'pool_end'))
				  )),

		$form->text(array('desc'	=> $this->bbf('fm_interfaces'),
				  'name'	=> 'extra_ifaces',
				  'labelid'	=> 'extra_ifaces',
				  'size'	=> 15,
				  'default'	=> $element['dhcp']['interfaces']['default'],
				  'value'	=> $this->get_var('info','extra_ifaces')));

?>
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
