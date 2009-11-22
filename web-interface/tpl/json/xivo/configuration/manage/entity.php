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

dwho::load_class('dwho_http');
$http_response = dwho_http::factory('response');

if($this->get_var('act') === 'view')
	$data = dwho_json::encode($this->get_var('info'));
else
{
	$list = $this->get_var('list');

	if(is_array($list) === false)
	{
		$http_response->set_status_line(500);
		$http_response->send(true);
	}
	else if(($nb = count($list)) === 0)
	{
		$http_response->set_status_line(204);
		$http_response->send(true);
	}

	$data = array();

	for($i = 0;$i < $nb;$i++)
		$data[] = $list[$i]['entity'];

	$data = dwho_json::encode($data);
}

if($data === false)
{
	$http_response->set_status_line(500);
	$http_response->send(true);
}

$sum = $this->get_var('sum');

if(isset($sum{0}) === true && $sum === md5($data))
{
	$http_response->set_status_line(304);
	$http_response->send(true);
}

header(dwho_json::get_header());
die($data);

?>
