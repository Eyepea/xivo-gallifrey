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

dwho::load_class('dwho_http');
$http = new dwho_http();

if($this->get_var('act') === 'view')
	$data = dwho_json::encode($this->get_var('info'));
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
		$ref = &$list[$i];

		$arr['id'] = $ref['agentfeatures']['id'];
		$arr['firstname'] = $ref['agentfeatures']['firstname'];
		$arr['lastname'] = $ref['agentfeatures']['lastname'];
		$arr['number'] = $ref['agentfeatures']['number'];
		$arr['passwd'] = $ref['agentfeatures']['passwd'];
		$arr['context'] = $ref['agentfeatures']['context'];
		$arr['language'] = $ref['agentfeatures']['language'];
		$arr['silent'] = $ref['agentfeatures']['silent'];
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

		$data[] = $arr;
	}

	$data = dwho_json::encode($data);
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

header(dwho_json::get_header());
die($data);

?>
