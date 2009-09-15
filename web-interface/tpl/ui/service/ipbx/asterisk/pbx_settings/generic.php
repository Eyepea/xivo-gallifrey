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

$list = $this->get_var('list');

if(is_array($list) === false)
{
	$http->set_status(500);
	$http->send(true);
}
else if(($nb = count($list)) === 0)
{
	$http->set_status(204);
	$http->send(true);
}

$except = $this->get_var('except');

$data = array();

for($i = 0;$i < $nb;$i++)
{
	$ref = &$list[$i];
	
	if($ref['id'] === $except)
		continue;
	else if(preg_match('/^(.+) \((.+)\)$/',$ref['identity'],$match) !== 1)
		$data[] = array('id'		=> $ref['id'],
				'identity'	=> $ref['identity'],
				'info'		=> '');
	else
		$data[] = array('id'		=> $ref['id'],
				'identity'	=> $match[1],
				'info'		=> $match[2]);
}

$data = xivo_json::encode($data);

if($data === false)
{
	$http->set_status(500);
	$http->send(true);
}

header(xivo_json::get_header());
die($data);

?>
