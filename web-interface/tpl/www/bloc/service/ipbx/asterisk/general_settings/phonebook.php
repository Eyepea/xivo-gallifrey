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
		<li id="smenu-tab-1"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-first');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_access');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-xivoserver');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_xivoservers');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3"
		    class="moo-last"
		    onclick="xivo_smenu_click(this,'moc','sb-part-last',1);"
		    onmouseout="xivo_smenu_out(this,'moo',1);"
		    onmouseover="xivo_smenu_over(this,'mov',1);">
			<div class="tab">
				<span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_ldapfilters');?></a></span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

<div class="sb-content">
	<form action="#" method="post" accept-charset="utf-8" onsubmit="xivo_fm_select('it-access');
									xivo_fm_select('it-xivoserver');
									xivo_fm_select('it-ldapfilter');">

<?php
	echo	$form->hidden(array('name'	=> XIVO_SESS_NAME,
				    'value'	=> XIVO_SESS_ID)),

		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1));
?>

<div id="sb-part-first">
	<div id="accesslist" class="fm-field fm-multilist">
		<div class="slt-list">
<?php
		echo	$form->select(array('name'	=> 'accessfeatures[]',
					    'label'	=> false,
					    'id'	=> 'it-access',
					    'key'	=> true,
					    'altkey'	=> 'host',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'field'	=> false),
				      $info['accessfeatures']);
?>
		<div class="bt-adddelete">
			<a href="#"
			   onclick="xivo_fm_select_add_host_ipv4_subnet('it-access',
									prompt('<?=$dhtml->escape($this->bbf('accessfeatures_add'));?>'));
				    return(xivo_free_focus());"
			   title="<?=$this->bbf('bt_addaccess');?>">
				<?=$url->img_html('img/site/button/mini/blue/add.gif',
						  $this->bbf('bt_addaccess'),
						  'class="bt-addlist" id="bt-addaccess" border="0"');?></a><br />
			<a href="#"
			   onclick="xivo_fm_select_delete_entry('it-access');
				    return(xivo_free_focus());"
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
		<div id="xivoserverlist" class="fm-field fm-multilist">
			<div class="slt-outlist">
<?php
		echo	$form->select(array('name'	=> 'xivoserverlist',
					    'label'	=> false,
					    'id'	=> 'it-xivoserverlist',
					    'key'	=> 'identity',
					    'altkey'	=> 'id',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'field'	=> false),
				      $info['xivoserver']['list']);
?>
			</div>
			<div class="inout-list">
				<a href="#"
				   onclick="xivo_fm_move_selected('it-xivoserverlist','it-xivoserver');
					    return(xivo_free_focus());"
				   title="<?=$this->bbf('bt_inxivoserver');?>">
					<?=$url->img_html('img/site/button/row-left.gif',
							  $this->bbf('bt_inxivoserver'),
							  'class="bt-inlist" id="bt-inxivoserver" border="0"');?></a><br />

				<a href="#"
				   onclick="xivo_fm_move_selected('it-xivoserver','it-xivoserverlist');
					    return(xivo_free_focus());"
				   title="<?=$this->bbf('bt_outxivoserver');?>">
					<?=$url->img_html('img/site/button/row-right.gif',
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
					    'field'	=> false),
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
		<div id="ldapfilterlist" class="fm-field fm-multilist">
			<div class="slt-outlist">
<?php
		echo	$form->select(array('name'	=> 'ldapfilterlist',
					    'label'	=> false,
					    'id'	=> 'it-ldapfilterlist',
					    'key'	=> 'identity',
					    'altkey'	=> 'id',
					    'multiple'	=> true,
					    'size'	=> 5,
					    'field'	=> false),
				      $info['ldapfilter']['list']);
?>
			</div>
			<div class="inout-list">
				<a href="#"
				   onclick="xivo_fm_move_selected('it-ldapfilterlist','it-ldapfilter');
					    return(xivo_free_focus());"
				   title="<?=$this->bbf('bt_inldapfilter');?>">
					<?=$url->img_html('img/site/button/row-left.gif',
							  $this->bbf('bt_inldapfilter'),
							  'class="bt-inlist" id="bt-inldapfilter" border="0"');?></a><br />
				<a href="#"
				   onclick="xivo_fm_move_selected('it-ldapfilter','it-ldapfilterlist');
					    return(xivo_free_focus());"
				   title="<?=$this->bbf('bt_outldapfilter');?>">
					<?=$url->img_html('img/site/button/row-right.gif',
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
					    'field'	=> false),
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
