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

?>
<dl>
	<dt>
		<span class="span-left">&nbsp;</span>
		<span class="span-center"><?=$this->bbf('mn_left_name');?></span>
		<span class="span-right">&nbsp;</span>
	</dt>
	<dd>
		<dl>
			<dt><?=$this->bbf('mn_left_ti_manage');?></dt>
			<dd id="mn-manage--user">
				<?=$url->href_html($this->bbf('mn_left_manage-user'),
						   'xivo/configuration/manage/user',
						   'act=list');?>
			</dd>
			<dd id="mn-manage--entity">
				<?=$url->href_html($this->bbf('mn_left_manage-entity'),
							      'xivo/configuration/manage/entity',
							      'act=list');?>
			</dd>
			<dd id="mn-manage--server">
				<?=$url->href_html($this->bbf('mn_left_manage-server'),
						   'xivo/configuration/manage/server',
						   'act=list');?>
			</dd>
			<dd id="mn-manage--ldapserver">
				<?=$url->href_html($this->bbf('mn_left_manage-ldapserver'),
							      'xivo/configuration/manage/ldapserver',
							      'act=list');?>
			</dd>
			<dd id="mn-manage--accesswebservice">
				<?=$url->href_html($this->bbf('mn_left_manage-accesswebservice'),
						   'xivo/configuration/manage/accesswebservice',
						   'act=list');?>
			</dd>
		</dl>
		<dl>
			<dt><?=$this->bbf('mn_left_ti_network');?></dt>
			<dd id="mn-network--interface">
				<?=$url->href_html($this->bbf('mn_left_network-interface'),
						   'xivo/configuration/network/interface',
						   'act=list');?>
			</dd>
			<dd id="mn-network--iproute">
				<?=$url->href_html($this->bbf('mn_left_network-iproute'),
						   'xivo/configuration/network/iproute',
						   'act=list');?>
			</dd>
		</dl>
	</dd>
	<dd class="b-nosize">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</dd>
</dl>
