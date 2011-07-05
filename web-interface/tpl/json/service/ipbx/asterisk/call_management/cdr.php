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

dwho::load_class('dwho_http');
$http_response = dwho_http::factory('response');

// NOTE: we limit response to 5K lines of results as json_encode() crash if more
// results are required
if(($data = dwho_json::encode(array_slice($this->get_var('result'),0,5000))) === false)
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
