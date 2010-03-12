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
			<dd id="mn-manage--directories">
				<?=$url->href_html($this->bbf('mn_left_manage-directories'),
							      'xivo/configuration/manage/directories',
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
			<dd id="mn-network--resolvconf">
				<?=$url->href_html($this->bbf('mn_left_network-resolvconf'),
						   'xivo/configuration/network/resolvconf');?>
			</dd>
			<dd id="mn-network--mail">
				<?=$url->href_html($this->bbf('mn_left_network-mail'),
						   'xivo/configuration/network/mail');?>
			</dd>
			<dd id="mn-network--dhcp">
				<?=$url->href_html($this->bbf('mn_left_network-dhcp'),
						   'xivo/configuration/network/dhcp');?>
			</dd>
			<dd id="mn-network--ha">
				<?=$url->href_html($this->bbf('mn_left_network-ha'),
						   'xivo/configuration/network/ha');?>
			</dd>

		</dl>
		<dl>
			<dt><?=$this->bbf('mn_left_ti_support');?></dt>
			<dd id="mn-support--xivo">
				<?=$url->href_html($this->bbf('mn_left_support-xivo'),
						   'xivo/configuration/support/xivo');?>
			</dd>
			<dd id="mn-support--alerts">
				<?=$url->href_html($this->bbf('mn_left_support-alerts'),
						   'xivo/configuration/support/alerts');?>
			</dd>
			<dd id="mn-support--limits">
				<?=$url->href_html($this->bbf('mn_left_support-limits'),
						   'xivo/configuration/support/limits');?>
			</dd>
		</dl>
	</dd>
	<dd class="b-nosize">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</dd>
</dl>
