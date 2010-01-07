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

$xmlphone = &$this->get_module('xmlphone');
$xmlvendor = $xmlphone->factory($this->get_var('vendor'));
$taginput = $xmlvendor->tag_input();

?>
<<?=$taginput?>>
	<Title><?=$xmlvendor->escape($this->bbf('phone_search-title'));?></Title>
	<Prompt><?=$xmlvendor->escape($this->bbf('phone_search-prompt'));?></Prompt>
	<URL><?=$this->url('service/ipbx/web_services/phonebook/search',true);?></URL>
	<InputItem>
		<DisplayName><?=$xmlvendor->escape($this->bbf('phone_search-prompt'));?></DisplayName>
		<QueryStringParam>name</QueryStringParam>
		<DefaultValue />
		<InputFlags>a</InputFlags>
	</InputItem>
</<?=$taginput?>>
