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

xivo::load_class('xivo_http');
$http = new xivo_http();

if($this->get_var('act') === 'get')
	$data = xivo_json::encode($this->get_var('info'));
else
{
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

	$arr = $data = array();

	for($i = 0;$i < $nb;$i++)
	{
		$ref = &$agents[$i];

		$arr['id'] = $ref['afeatures']['id'];
		$arr['firstname'] = $ref['afeatures']['firstname'];
		$arr['lastname'] = $ref['afeatures']['lastname'];
		$arr['number'] = $ref['afeatures']['number'];
		$arr['passwd'] = $ref['afeatures']['passwd'];
		$arr['context'] = $ref['afeatures']['context'];
		$arr['language'] = $ref['afeatures']['language'];
		$arr['silent'] = $ref['afeatures']['silent'];
		$arr['ackcall'] = $ref['agentoptions']['ackcall'];
		$arr['endcall'] = $ref['agentoptions']['endcall'];
		$arr['autologoff'] = $ref['agentoptions']['autologoff'];
		$arr['autologoffunavail'] = $ref['agentoptions']['autologoffunavail'];
		$arr['wrapuptime'] = $ref['agentoptions']['wrapuptime'];
		$arr['maxlogintries'] = $ref['agentoptions']['maxlogintries'];
		$arr['updatecdr'] = $ref['agentoptions']['updatecdr'];
		$arr['recordagentcalls'] = $ref['agentoptions']['recordagentcalls'];
		$arr['recordformat'] = $ref['agentoptions']['recordformat'];
		$arr['urlprefix'] = $ref['agentoptions']['urlprefix'];
		$arr['beep'] = $ref['agentoptions']['custom_beep'];
		$arr['goodbye'] = $ref['agentoptions']['goodbye'];
		$arr['commented'] = $ref['agent']['commented'];
	}

	$data = xivo_json::encode($data);
}

if($data === false)
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
