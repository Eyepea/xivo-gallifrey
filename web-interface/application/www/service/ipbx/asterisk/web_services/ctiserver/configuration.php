<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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

$access_category = 'ctiserver';
$access_subcategory = 'configuration';

include(dwho_file::joinpath(dirname(__FILE__),'..','_common.php'));

$act = $_QRY->get('act');

switch($act)
{
	case 'view':
	default:
		$act = 'view';

		$app = &$ipbx->get_application('serverfeatures', array('feature' => 'phonebook', 'type' => 'xivo'));
//		$contexts = &$ipbx->get_application('context');
		$ctimain = &$ipbx->get_module('ctimain');
		$db_type = $app->_serverfeatures->_dso->_dso->_type;
		$db_path =    $app->_serverfeatures->_dso->_dso->_param['db'];
		$db_timeout = $app->_serverfeatures->_dso->_dso->_param['timeout'];
		$load_inf = $ctimain->get_all();
		$list = $app->get_server_list();
//		dwho_print_r($load_inf, 'load_inf');
//		dwho_print_r($list, 'list');
//		$cc = $contexts->get_contexts_list();  PAS CA
//		dwho_print_r($cc, 'context');

		$out = array(
			'main' 			=> array(),
			'reverseid' 	=> array(),
			'contexts' 		=> array(),
			'directories' 	=> array(),
			'displays' 		=> array(),
			'sheets' 		=> array(),
			'xivocti' 		=> array(),
			'presences' 	=> array(),
			'phonehints' 	=> array()
		);
		$out['main']['commandset'] = $load_inf[0]['commandset'];
		$out['main']['incoming_tcp_fagi'] = array($load_inf[0]['fagi_ip'], $load_inf[0]['fagi_port']);
		$out['main']['incoming_tcp_cti'] = array($load_inf[0]['cti_ip'], $load_inf[0]['cti_port']);
		$out['main']['incoming_tcp_webi'] = array($load_inf[0]['webi_ip'], $load_inf[0]['webi_port']);
		$out['main']['incoming_tcp_info'] = array($load_inf[0]['info_ip'], $load_inf[0]['info_port']);
		$out['main']['incoming_udp_announce'] = array($load_inf[0]['announce_ip'], $load_inf[0]['announce_port']);
		$out['main']['sockettimeout'] = $load_inf[0]['socket_timeout'];
		$out['main']['updates_period'] = $load_inf[0]['updates_period'];
		$out['main']['logintimeout'] = $load_inf[0]['login_timeout'];
		$out['main']['asterisklist'] = array();
		$out['main']['contextlist'] = array();
		$out['main']['userlists'] = array();
		$out['main']['parting_astid_context'] = array();
		if($load_inf[0]['parting_astid_context'] != "")
			$out['main']['parting_astid_context'] = explode(",", $load_inf[0]['parting_astid_context']);

		# REMOVED $out['main']['ctilog'] = '';
		# REMOVED $out['main']['prefixfile'] = '';

		if(isset($load_inf[0]['asterisklist']) && dwho_has_len($load_inf[0]['asterisklist']))
		{
			$astlist = explode(',', $load_inf[0]['asterisklist']);
			foreach($astlist as $k => $v)
			{
				$hostname = $list[$v]['name'];
				$url_scheme = $list[$v]['url']['scheme'];
				$url_auth_host = $list[$v]['url']['authority']['host'];
				$json = $url_scheme . '://' . $url_auth_host . '/service/ipbx/json.php/private/';
				$out['main']['asterisklist'][] = $hostname;
				$out['main']['userlists'][] = $json . 'pbx_settings/users';
				$out[$hostname] = array(
					'localaddr' => $list[$v]['host'],
					'ipaddress' => $list[$v]['host'],
					'ipaddress_webi' => $list[$v]['webi'],
					'urllist_agents' => array($json . 'pbx_settings/agents'),
					'urllist_phones' => array($json . 'pbx_settings/users'),
					'urllist_queues' => array($json . 'pbx_settings/queues'),
					'urllist_groups' => array($json . 'pbx_settings/groups'),
					'urllist_meetme' => array($json . 'pbx_settings/meetme'),
					'urllist_voicemail' => array($json . 'pbx_settings/voicemail'),
					'urllist_incomingcalls' => array($json . 'call_management/incall'),
					'urllist_trunks' => array(
											$json . 'trunk_management/sip',
											$json . 'trunk_management/iax'
										),
					'urllist_phonebook' => array($json . 'pbx_services/phonebook'),
					'ami_port' => $list[$v]['ami_port'],
					'ami_login' => $list[$v]['ami_login'],
					'ami_pass' => $list[$v]['ami_pass'],
					'cdr_db_uri' => $db_type . ':' . $db_path . '?timeout_ms=' . $db_timeout,
					'userfeatures_db_uri' => $db_type . ':' . $db_path . '?timeout_ms=' . $db_timeout,
					'url_queuelog' => 'file:' . $app->_serverfeatures->_sre->_ini['logfiles']['path'] . '/queue_log'
				);
			}
			$out['main']['userlists'][] = "file:///etc/pf-xivo/ctiservers/guest_account.json";
		}


//		dwho_print_r($out, 'out');
		/*
		die('toto');
		$nocomponents = array('meetmemacro'		=> true,
				      'meetmeadmininternal'	=> true,
				      'extenumbers'		=> true,
				      'contextnummember'	=> true,
				      'contextmember'		=> true);
		
		if(($info = $appmeetme->get($_QRY->get('id'),
					    null,
					    $nocomponents)) === false)
		{
			$http_response->set_status_line(404);
			$http_response->send(true);
		}
		*/
		$_TPL->set_var('info',$out);
		break;
}

$_TPL->set_var('act',$act);
$_TPL->set_var('sum',$_QRY->get('sum'));
$_TPL->display('/service/ipbx/'.$ipbx->get_name().'/generic');

?>
