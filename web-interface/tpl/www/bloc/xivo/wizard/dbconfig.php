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

echo	$form->select(array('desc'	=> $this->bbf('fm_dbconfig_backend'),
			    'name'	=> 'dbconfig[backend]',
			    'labelid'	=> 'dbconfig-backend',
			    'key'	=> 'label',
			    'help'	=> $this->bbf('hlp_fm_dbconfig_backend'),
			    'default'	=> $element['backend']['default'],
			    'selected'	=> $this->get_var('info','backend'),
			    'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('dbconfig','backend'))),
		      $this->get_var('dbbackend'));

if(($error = (string) $this->get_var('error','dbconfig','sqlite','xivo')) !== ''):
	echo	'<div id="error-dbconfig-sqlite-xivo" class="dwho-txt-error">','
			<span class="dwho-msg-error-icon">&nbsp;</span>',
			$this->bbf_args('error_dbconfig_xivo',$error),
		'</div>';
endif;

echo	$form->text(array('desc'	=> $this->bbf('fm_dbconfig_sqlite-xivodb'),
			  'name'	=> 'dbconfig[sqlite][xivodb]',
			  'labelid'	=> 'dbconfig-sqlite-xivodb',
			  'size'	=> 20,
			  'help'	=> $this->bbf('hlp_fm_dbconfig_sqlite-xivodb'),
			  'comment'	=> $this->bbf('cmt_fm_dbconfig_sqlite-xivodb'),
			  'default'	=> $element['sqlite']['xivodb']['default'],
			  'value'	=> $this->get_var('info','sqlite','xivodb'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','dbconfig','sqlite','xivodb'))));

if(($error = (string) $this->get_var('error','dbconfig','sqlite','ipbx')) !== ''):
	echo	'<div id="error-dbconfig-sqlite-ipbx" class="dwho-txt-error">','
			<span class="dwho-msg-error-icon">&nbsp;</span>',
			$this->bbf_args('error_dbconfig_ipbx',$error),
		'</div>';
endif;

echo	$form->text(array('desc'	=> $this->bbf('fm_dbconfig_sqlite-ipbxdb'),
			  'name'	=> 'dbconfig[sqlite][ipbxdb]',
			  'labelid'	=> 'dbconfig-sqlite-ipbxdb',
			  'size'	=> 20,
			  'help'	=> $this->bbf('hlp_fm_dbconfig_sqlite-ipbxdb'),
			  'comment'	=> $this->bbf('cmt_fm_dbconfig_sqlite-ipbxdb'),
			  'default'	=> $element['sqlite']['ipbxdb']['default'],
			  'value'	=> $this->get_var('info','sqlite','ipbxdb'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','dbconfig','sqlite','ipbxdb')))),

	$form->text(array('desc'	=> $this->bbf('fm_dbconfig_mysql-host'),
			  'name'	=> 'dbconfig[mysql][host]',
			  'labelid'	=> 'dbconfig-mysql-host',
			  'size'	=> 20,
			  'help'	=> $this->bbf('hlp_fm_dbconfig_mysql-host'),
			  'comment'	=> $this->bbf('cmt_fm_dbconfig_mysql-host'),
			  'default'	=> $element['mysql']['host']['default'],
			  'value'	=> $this->get_var('info','mysql','host'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','dbconfig','mysql','host')))),

	$form->text(array('desc'	=> $this->bbf('fm_dbconfig_mysql-port'),
			  'name'	=> 'dbconfig[mysql][port]',
			  'labelid'	=> 'dbconfig-mysql-port',
			  'size'	=> 20,
			  'help'	=> $this->bbf('hlp_fm_dbconfig_mysql-port'),
			  'comment'	=> $this->bbf('cmt_fm_dbconfig_mysql-port'),
			  'default'	=> $element['mysql']['port']['default'],
			  'value'	=> $this->get_var('info','mysql','port'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','dbconfig','mysql','port'))));

if(($error = (string) $this->get_var('error','dbconfig','mysql','xivo')) !== ''):
	echo	'<div id="error-dbconfig-mysql-xivo" class="dwho-txt-error">','
			<span class="dwho-msg-error-icon">&nbsp;</span>',
			$this->bbf_args('error_dbconfig_xivo',$error),
		'</div>';
endif;

echo	$form->text(array('desc'	=> $this->bbf('fm_dbconfig_mysql-xivodbname'),
			  'name'	=> 'dbconfig[mysql][xivodbname]',
			  'labelid'	=> 'dbconfig-mysql-xivodbname',
			  'size'	=> 20,
			  'help'	=> $this->bbf('hlp_fm_dbconfig_mysql-xivodbname'),
			  'comment'	=> $this->bbf('cmt_fm_dbconfig_mysql-xivodbname'),
			  'default'	=> $element['mysql']['xivodbname']['default'],
			  'value'	=> $this->get_var('info','mysql','xivodbname'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','dbconfig','mysql','xivodbname')))),

	$form->text(array('desc'	=> $this->bbf('fm_dbconfig_mysql-xivouser'),
			  'name'	=> 'dbconfig[mysql][xivouser]',
			  'labelid'	=> 'dbconfig-mysql-xivouser',
			  'size'	=> 20,
			  'help'	=> $this->bbf('hlp_fm_dbconfig_mysql-xivouser'),
			  'comment'	=> $this->bbf('cmt_fm_dbconfig_mysql-xivouser'),
			  'default'	=> $element['mysql']['xivouser']['default'],
			  'value'	=> $this->get_var('info','mysql','xivouser'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','dbconfig','mysql','xivouser')))),

	$form->text(array('desc'	=> $this->bbf('fm_dbconfig_mysql-xivopass'),
			  'name'	=> 'dbconfig[mysql][xivopass]',
			  'labelid'	=> 'dbconfig-mysql-xivopass',
			  'size'	=> 20,
			  'help'	=> $this->bbf('hlp_fm_dbconfig_mysql-xivopass'),
			  'comment'	=> $this->bbf('cmt_fm_dbconfig_mysql-xivopass'),
			  'default'	=> $element['mysql']['xivopass']['default'],
			  'value'	=> $this->get_var('info','mysql','xivopass'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','dbconfig','mysql','xivopass'))));

if(($error = (string) $this->get_var('error','dbconfig','mysql','ipbx')) !== ''):
	echo	'<div id="error-dbconfig-mysql-ipbx" class="dwho-txt-error">',
			'<span class="dwho-msg-error-icon">&nbsp;</span>',
			$this->bbf_args('error_dbconfig_ipbx',$error),
		'</div>';
endif;

echo	$form->text(array('desc'	=> $this->bbf('fm_dbconfig_mysql-ipbxdbname'),
			  'name'	=> 'dbconfig[mysql][ipbxdbname]',
			  'labelid'	=> 'dbconfig-mysql-ipbxdbname',
			  'size'	=> 20,
			  'help'	=> $this->bbf('hlp_fm_dbconfig_mysql-ipbxdbname'),
			  'comment'	=> $this->bbf('cmt_fm_dbconfig_mysql-ipbxdbname'),
			  'default'	=> $element['mysql']['ipbxdbname']['default'],
			  'value'	=> $this->get_var('info','mysql','ipbxdbname'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','dbconfig','mysql','ipbxdbname')))),

	$form->text(array('desc'	=> $this->bbf('fm_dbconfig_mysql-ipbxuser'),
			  'name'	=> 'dbconfig[mysql][ipbxuser]',
			  'labelid'	=> 'dbconfig-mysql-ipbxuser',
			  'size'	=> 20,
			  'help'	=> $this->bbf('hlp_fm_dbconfig_mysql-ipbxuser'),
			  'comment'	=> $this->bbf('cmt_fm_dbconfig_mysql-ipbxuser'),
			  'default'	=> $element['mysql']['ipbxuser']['default'],
			  'value'	=> $this->get_var('info','mysql','ipbxuser'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','dbconfig','mysql','ipbxuser')))),

	$form->text(array('desc'	=> $this->bbf('fm_dbconfig_mysql-ipbxpass'),
			  'name'	=> 'dbconfig[mysql][ipbxpass]',
			  'labelid'	=> 'dbconfig-mysql-ipbxpass',
			  'size'	=> 20,
			  'help'	=> $this->bbf('hlp_fm_dbconfig_mysql-ipbxpass'),
			  'comment'	=> $this->bbf('cmt_fm_dbconfig_mysql-ipbxpass'),
			  'default'	=> $element['mysql']['ipbxpass']['default'],
			  'value'	=> $this->get_var('info','mysql','ipbxpass'),
			  'error'	=> $this->bbf_args('error_generic',
							   $this->get_var('error','dbconfig','mysql','ipbxpass'))));

?>
