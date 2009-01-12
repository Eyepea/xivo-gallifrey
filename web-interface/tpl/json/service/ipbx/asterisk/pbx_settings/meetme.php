<?php

#
# XiVO Web-Interface
# Copyright (C) 2006, 2007, 2008  Proformatique <technique@proformatique.com>
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

xivo::load_class('xivo_http');
$http = new xivo_http();

$meetme = $this->get_var('meetme');

if(is_array($meetme) === false)
{
	$http->set_status(500);
	$http->send(true);
}
else if(($nb = count($meetme)) === 0)
{
	$http->set_status(204);
	$http->send(true);
}

$data = $list = array();

for($i = 0;$i < $nb;$i++)
{
	$ref = &$meetme[$i];

	$data['id'] = $ref['mfeatures']['id'];
	$data['name'] = $ref['mfeatures']['name'];
	$data['number'] = $ref['mfeatures']['number'];
	$data['pin'] = $ref['meetmeroom']['pin'];
	$data['admin-pin'] = $ref['meetmeroom']['admin-pin'];
	$data['musiconhold'] = $ref['mfeatures']['musiconhold'];
	$data['context'] = $ref['mfeatures']['context'];
	$data['commented'] = $ref['meetmeroom']['commented'];

	$list[] = $data;
}

if(($list = xivo_json::encode($list)) === false)
{
	$http->set_status(500);
	$http->send(true);
}
else if($this->get_var('sum') === md5($list))
{
	$http->set_status(304);
	$http->send(true);
}

header(xivo_json::get_header());
die($list);

?>
