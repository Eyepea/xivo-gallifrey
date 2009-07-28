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

$context = $this->get_var('context');

if(is_array($context) === false)
{
	$http->set_status(500);
	$http->send(true);
}
else if(($nb = count($context)) === 0)
{
	$http->set_status(204);
	$http->send(true);
}

$data = array();

for($i = 0;$i < $nb;$i++)
{
	$ref = &$context[$i];

	$data[$i] = $ref;

	if(xivo_issa('entity',$ref) === true)
		$data[$i]['context']['entityid'] = $ref['entity']['id'];
	
	unset($data[$i]['entity']);
}

if(($data = xivo_json::encode($data)) === false)
{
	$http->set_status(500);
	$http->send(true);
}

$sum = $this->get_var('sum');

if(isset($sum{0}) === true && $sum === md5($data))
{
	$http->set_status(304);
	$http->send(true);
}

header(xivo_json::get_header());
die($data);

?>