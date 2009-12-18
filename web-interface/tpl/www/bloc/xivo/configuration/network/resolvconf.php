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

<div class="sb-smenu">
	<ul>
		<li id="dwsm-tab-1"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-first');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_general');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-2"
		    class="dwsm-blur-last"
		    onclick="dwho_submenu.select(this,'sb-part-last',1);"
		    onmouseout="dwho_submenu.blur(this,1);"
		    onmouseover="dwho_submenu.focus(this,1);">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_search');?></a></span>
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

		$form->text(array('desc'	=> $this->bbf('fm_hostname'),
				  'name'	=> 'hostname',
				  'labelid'	=> 'hostname',
				  'size'	=> 15,
				  'default'	=> $element['resolvconf']['hostname']['default'],
				  'value'	=> $this->get_var('info','hostname'))),

		$form->text(array('desc'	=> $this->bbf('fm_nameserver1'),
				  'name'	=> 'nameserver1',
				  'labelid'	=> 'nameserver1',
				  'size'	=> 15,
				  'default'	=> $element['resolvconf']['nameserver1']['default'],
				  'value'	=> $this->get_var('info','nameserver1'))),

		$form->text(array('desc'	=> $this->bbf('fm_nameserver2'),
				  'name'	=> 'nameserver2',
				  'labelid'	=> 'nameserver2',
				  'size'	=> 15,
				  'default'	=> $element['resolvconf']['nameserver2']['default'],
				  'value'	=> $this->get_var('info','nameserver2'))),

		$form->text(array('desc'	=> $this->bbf('fm_nameserver3'),
				  'name'	=> 'nameserver3',
				  'labelid'	=> 'nameserver3',
				  'size'	=> 15,
				  'default'	=> $element['resolvconf']['nameserver3']['default'],
				  'value'	=> $this->get_var('info','nameserver3')));
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
					 'default'	=> $element['resolvconf']['description']['default']),
				   $this->get_var('info','description'));?>
	</div>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_search1'),
				  'name'	=> 'search[]',
				  'labelid'	=> 'search1',
				  'size'	=> 15,
				  'value'	=> $this->get_var('searches',0))),

		$form->text(array('desc'	=> $this->bbf('fm_search2'),
				  'name'	=> 'search[]',
				  'labelid'	=> 'search2',
				  'size'	=> 15,
				  'value'	=> $this->get_var('searches',1))),

		$form->text(array('desc'	=> $this->bbf('fm_search3'),
				  'name'	=> 'search[]',
				  'labelid'	=> 'search3',
				  'size'	=> 15,
				  'value'	=> $this->get_var('searches',2))),

		$form->text(array('desc'	=> $this->bbf('fm_search4'),
				  'name'	=> 'search[]',
				  'labelid'	=> 'search4',
				  'size'	=> 15,
				  'value'	=> $this->get_var('searches',3))),

		$form->text(array('desc'	=> $this->bbf('fm_search5'),
				  'name'	=> 'search[]',
				  'labelid'	=> 'search5',
				  'size'	=> 15,
				  'value'	=> $this->get_var('searches',4))),

		$form->text(array('desc'	=> $this->bbf('fm_search6'),
				  'name'	=> 'search[]',
				  'labelid'	=> 'search6',
				  'size'	=> 15,
				  'value'	=> $this->get_var('searches',5)));
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
