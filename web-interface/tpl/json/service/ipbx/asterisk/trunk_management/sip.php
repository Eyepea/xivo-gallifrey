<?php

#
# XiVO Web-Interface
# Copyright (C) 2009  Proformatique <technique@proformatique.com>
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

$trunks = $this->get_var('trunks');

if(is_array($trunks) === false)
{
	$http->set_status(500);
	$http->send(true);
}
else if(($nb = count($trunks)) === 0)
{
	$http->set_status(204);
	$http->send(true);
}

$data = $list = array();

for($i = 0;$i < $nb;$i++)
{
	$ref = &$trunks[$i];

	$data = $ref['protocol'];
	$data['protocolid'] = $ref['protocol']['id'];
	$data['id'] = $ref['trunkfeatures']['id'];
	$data['description'] = $ref['trunkfeatures']['description'];

	if(xivo_issa('register',$ref) === false)
		$data['register'] = null;
	else
	{
		$data['register'] = array();
		$data['register']['id'] = $ref['register']['id'];
		$data['register']['username'] = $ref['register']['username'];
		$data['register']['password'] = $ref['register']['password'];
		$data['register']['authuser'] = $ref['register']['authuser'];
		$data['register']['host'] = $ref['register']['host'];
		$data['register']['port'] = $ref['register']['port'];
		$data['register']['contact'] = $ref['register']['contact'];
		$data['register']['commented'] = $ref['register']['commented'];
	}

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
