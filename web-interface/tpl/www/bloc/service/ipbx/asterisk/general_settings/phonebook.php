<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

$url = &$this->get_module('url');
$form = &$this->get_module('form');
$dhtml = &$this->get_module('dhtml');

$info = $this->get_var('info');
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
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_access');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-2"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-xivoserver');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_xivoservers');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-3"
		    class="dwsm-blur-last"
		    onclick="dwho_submenu.select(this,'sb-part-last',1);"
		    onmouseout="dwho_submenu.blur(this,1);"
		    onmouseover="dwho_submenu.focus(this,1);">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_ldapfilters');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

<div class="sb-content">
	<form action="#" method="post" accept-charset="utf-8" onsubmit="dwho.form.select('it-access');
									dwho.form.select('it-xivoserver');
									dwho.form.select('it-ldapfilter');">

<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),

		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1));
?>

<div id="sb-part-first">
	<div id="accesslist" class="fm-paragraph fm-multilist">
		<div class="slt-list">
<?php
		echo	$form->select(array('name'	=> 'accessfeatures[]',
					    'label'	=> false,
					    'id'	=> 'it-access',
					    'key'	=> true,
					    'altkey'	=> 'host',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false),
				      $info['accessfeatures']);
?>
		<div class="bt-adddelete">
			<a href="#"
			   onclick="xivo_form_select_add_host_ipv4_subnet('it-access',
									  prompt('<?=$dhtml->escape($this->bbf('accessfeatures_add'));?>'));
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_addaccess');?>">
				<?=$url->img_html('img/site/button/mini/blue/add.gif',
						  $this->bbf('bt_addaccess'),
						  'class="bt-addlist" id="bt-addaccess" border="0"');?></a><br />
			<a href="#"
			   onclick="dwho.form.select_delete_entry('it-access');
				    return(dwho.dom.free_focus());"
			   title="<?=$this->bbf('bt_deleteaccess');?>">
				<?=$url->img_html('img/site/button/mini/orange/delete.gif',
						  $this->bbf('bt_deleteaccess'),
						  'class="bt-deletelist" id="bt-deleteaccess" border="0"');?></a>
		</div>
	</div>
</div>
<div class="clearboth"></div>

</div>

<div id="sb-part-xivoserver" class="b-nodisplay">
<?php
	if($info['xivoserver']['list'] !== false):
?>
		<div id="xivoserverlist" class="fm-paragraph fm-multilist">
			<div class="slt-outlist">
<?php
		echo	$form->select(array('name'	=> 'xivoserverlist',
					    'label'	=> false,
					    'id'	=> 'it-xivoserverlist',
					    'key'	=> 'identity',
					    'altkey'	=> 'id',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false),
				      $info['xivoserver']['list']);
?>
			</div>
			<div class="inout-list">
				<a href="#"
				   onclick="dwho.form.move_selected('it-xivoserverlist','it-xivoserver');
					    return(dwho.dom.free_focus());"
				   title="<?=$this->bbf('bt_inxivoserver');?>">
					<?=$url->img_html('img/site/button/arrow-left.gif',
							  $this->bbf('bt_inxivoserver'),
							  'class="bt-inlist" id="bt-inxivoserver" border="0"');?></a><br />

				<a href="#"
				   onclick="dwho.form.move_selected('it-xivoserver','it-xivoserverlist');
					    return(dwho.dom.free_focus());"
				   title="<?=$this->bbf('bt_outxivoserver');?>">
					<?=$url->img_html('img/site/button/arrow-right.gif',
							  $this->bbf('bt_outxivoserver'),
							  'class="bt-outlist" id="bt-outxivoserver" border="0"');?></a>
			</div>
			<div class="slt-inlist">
<?php
		echo	$form->select(array('name'	=> 'xivoserver[]',
					    'label'	=> false,
					    'id'	=> 'it-xivoserver',
					    'key'	=> 'identity',
					    'altkey'	=> 'id',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false),
				      $info['xivoserver']['slt']);
?>
			</div>
		</div>
		<div class="clearboth"></div>
<?php
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_xivoserver'),
					'xivo/configuration/manage/server',
					'act=add'),
			'</div>';
	endif;
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	if($info['ldapfilter']['list'] !== false):
?>
		<div id="ldapfilterlist" class="fm-paragraph fm-multilist">
			<div class="slt-outlist">
<?php
		echo	$form->select(array('name'	=> 'ldapfilterlist',
					    'label'	=> false,
					    'id'	=> 'it-ldapfilterlist',
					    'key'	=> 'identity',
					    'altkey'	=> 'id',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false),
				      $info['ldapfilter']['list']);
?>
			</div>
			<div class="inout-list">
				<a href="#"
				   onclick="dwho.form.move_selected('it-ldapfilterlist','it-ldapfilter');
					    return(dwho.dom.free_focus());"
				   title="<?=$this->bbf('bt_inldapfilter');?>">
					<?=$url->img_html('img/site/button/arrow-left.gif',
							  $this->bbf('bt_inldapfilter'),
							  'class="bt-inlist" id="bt-inldapfilter" border="0"');?></a><br />
				<a href="#"
				   onclick="dwho.form.move_selected('it-ldapfilter','it-ldapfilterlist');
					    return(dwho.dom.free_focus());"
				   title="<?=$this->bbf('bt_outldapfilter');?>">
					<?=$url->img_html('img/site/button/arrow-right.gif',
							  $this->bbf('bt_outldapfilter'),
							  'class="bt-outlist" id="bt-outldapfilter" border="0"');?></a>
			</div>
			<div class="slt-inlist">
<?php
		echo	$form->select(array('name'	=> 'ldapfilter[]',
					    'label'	=> false,
					    'id'	=> 'it-ldapfilter',
					    'key'	=> 'identity',
					    'altkey'	=> 'id',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'paragraph'	=> false),
				      $info['ldapfilter']['slt']);
?>
			</div>
		</div>
		<div class="clearboth"></div>
<?php
	else:
		echo	'<div class="txt-center">',
			$url->href_html($this->bbf('create_ldapfilter'),
					'service/ipbx/system_management/ldapfilter',
					'act=add'),
			'</div>';
	endif;
?>
</div>
	<?=$form->submit(array('name'	=> 'submit',
			       'id'	=> 'it-submit',
			       'value'	=> $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>
