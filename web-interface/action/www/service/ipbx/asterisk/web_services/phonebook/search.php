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

include(dwho_file::joinpath(dirname(__FILE__),'_common.php'));

$xmlphone = &$_TPL->get_module('xmlphone');

$pos = isset($_QR['pos']) === true ? dwho_uint($_QR['pos']) : 0;
$node = isset($_QR['node']) === true ? dwho_uint($_QR['node']) : 0;
$directory = isset($_QR['directory']) === true && $node === 1 ? true : false;
$vendor = isset($_QR['vendor']) === true ? $phonebook->chk_vendor($_QR['vendor']) : false;

if($vendor === false && ($vendor = $phonebook->get_vendor_from_useragent()) === false)
{
	$http_response->set_status_line(400);
	$http_response->send(false);
	dwho_die('Error/Invalid Vendor and User-Agent');
}

$xmlvendor = $xmlphone->factory($vendor);
$is_xmlphonebook = is_object($xmlvendor);

$path = '/bloc/service/ipbx/'.$ipbx->get_name().'/web_services/phonebook';

if($is_xmlphonebook === true)
	$displaypath = $path.'/phone';
else
	$displaypath = '/struct/service/ipbx/'.$ipbx->get_name().'/web_services/phonebook/'.$vendor;

$_TPL->set_var('path',$path);
$_TPL->set_var('vendor',$vendor);

if(isset($_QR['name']) === false || dwho_has_len($_QR['name']) === false)
{
	if($is_xmlphonebook === true && $xmlvendor->has_tag_input() === true)
		$_TPL->set_var('act','input');
	else if($vendor === 'polycom')
		$_TPL->set_var('act','input');
	else
		$_TPL->set_var('act','directory');

	$_TPL->display($displaypath);
	die();
}

$rs = array();

if(($rsp = $phonebook->get_phonebook_search($_QR['name'])) !== false)
	$rs = $rsp;

if(($rsu = $phonebook->get_user_search($_QR['name'],false)) !== false)
	$rs = array_merge($rs,$rsu);

$uri = array();
$uri['path'] = $_TPL->url('service/ipbx/json/phonebook/local');
$uri['query'] = array(array('name', $_QR['name']),
		      array('vendor', $vendor));

if(($rsx = $phonebook->get_phonebook_search_from_xivoserver($uri,false)) !== false)
	$rs = array_merge($rs,$rsx);

if(($rsl = $phonebook->get_phonebook_search_from_ldapfilter($_QR['name'],false)) !== false)
	$rs = array_merge($rs,$rsl);

if($is_xmlphonebook === true)
{
	$has_tag_menu = $xmlvendor->has_tag_menu();
	$nbbypage = $xmlvendor->nb_by_page();
}
else
{
	$has_tag_menu = true;
	$nbbypage = 16;
}

if(($nb = count($rs)) === 0 || $nb <= $nbbypage || $has_tag_menu === false)
{
	$_TPL->set_var('list',$rs);
	$_TPL->set_var('prevpos',0);
	$_TPL->set_var('act','directory');
	$_TPL->display($displaypath);
	die();
}

$maxnode = (int) floor(log($nb,$nbbypage));

if($node === 0 || $node > $maxnode)
{
	$node = $maxnode;
	$beg = $prevpos = 0;
	$end = $nb;
	$directory = false;
}
else
{
	$prevpos = pow($nbbypage,floor($node + 1));

	if($directory === true)
		$end = $nbbypage;
	else
		$end = $pos + $prevpos;

	if($pos > $nb || ($pos % $nbbypage) !== 0)
	{
		$beg = 0;
		$end = $directory === true ? $nbbypage : $prevpos;
	}
	else
		$beg = $pos;
}

$cnt = pow($nbbypage,floor($node));

dwho::load_class('dwho_sort');
$sort = new dwho_sort(array('key' => 'identity'));
usort($rs,array(&$sort,'str_usort'));

if($directory === true)
{
	$_TPL->set_var('list',array_slice($rs,$beg,$nbbypage));
	$_TPL->set_var('act','directory');
}
else
{
	$res = array();

	for($i = $beg,$j = $beg + $cnt - 1;
	    $i < $end && $i < $nb;
	    $i += $cnt,$j = $i + $cnt - 1)
	{
		if(isset($rs[$j]) === true)
			$res[] = array($rs[$i],$rs[$j],$i);
		else
		{
			$res[] = array($rs[$i],end($rs),$i);
			break;
		}
	}

	$_TPL->set_var('list',$res);
	$_TPL->set_var('act','menu');
}

$_TPL->set_var('node',$node);
$_TPL->set_var('maxnode',$maxnode);
$_TPL->set_var('pos',$beg);
$_TPL->set_var('prevpos',$prevpos);
$_TPL->set_var('name',$_QR['name']);
$_TPL->display($displaypath);
die();

?>
