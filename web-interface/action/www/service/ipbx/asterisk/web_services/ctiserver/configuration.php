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
define("CLIENT_VERSION", '5956');

switch($act)
{
	case 'view':
	default:
		$act = 'view';

		$app = &$ipbx->get_application('serverfeatures', array('feature' => 'phonebook', 'type' => 'xivo'));
		$cticontexts = &$ipbx->get_module('cticontexts');
		$ctidirectories = &$ipbx->get_module('ctidirectories');
		$ctidisplays = &$ipbx->get_module('ctidisplays');
		$ctisheetactions = &$ipbx->get_module('ctisheetactions');
		$ctisheetevents = &$ipbx->get_module('ctisheetevents');
		$ctimain = &$ipbx->get_module('ctimain');
		$ctiprofiles = &$ipbx->get_module('ctiprofiles');
		$ctipresences = &$ipbx->get_module('ctipresences');
		$ctistatus = &$ipbx->get_module('ctistatus');
		$ctiphonehints = &$ipbx->get_module('ctiphonehints');
		$ctirdid = &$ipbx->get_module('ctireversedirectories');

		$db_type = $app->_serverfeatures->_dso->_dso->_type;
		$db_path =    $app->_serverfeatures->_dso->_dso->_param['db'];
		$db_timeout = $app->_serverfeatures->_dso->_dso->_param['timeout'];
		
		$load_contexts = $cticontexts->get_all();
		$load_directories = $ctidirectories->get_all();
		$load_displays = $ctidisplays->get_all();
		$load_sheetactions = $ctisheetactions->get_all();
		$load_sheetevents = $ctisheetevents->get_all();
		$load_inf = $ctimain->get_all();
		$load_profiles = $ctiprofiles->get_all();
		$load_presences = $ctipresences->get_all();
		$load_status = $ctistatus->get_all();
		$load_phonehints = $ctiphonehints->get_all();
		$load_rdid = $ctirdid->get_all();
		$list = $app->get_server_list();

		$ctxlist = array();

		$out = array(
			'main' 			=> array(),
			'reversedid' 	=> array(),
			'contexts' 		=> array(),
			'directories' 	=> array(),
			'displays' 		=> array(),
			'sheets' 		=> array(),
			'xivocti' 		=> array(),
			'presences' 	=> array(),
			'phonehints' 	=> array()
		);

		# CONTEXTS
		if(isset($load_contexts))
		{
			$ctxout = array();
			foreach($load_contexts as $context)
			{
				$ctxid = $context['name'];
				$ctxlist[] = "contexts.".$ctxid;
				$ctxdirs = explode(',', $context['directories']);
				$ctxout[$ctxid] = array();
				$arr = array();
				foreach($ctxdirs as $cd)
				{
					$arr[] = "directories.".$cd;
				}
				$ctxout[$ctxid]['directories'] = $arr;

				$ctxout[$ctxid]['display'] = $context['display'];
			}
			$out['contexts'] = $ctxout;
		}

		# DISPLAYS
		if(isset($load_displays))
		{
			$dspout = array();

			foreach($load_displays as $display)
			{
				$dspid = $display['name'];
				$dspout[$dspid] = dwho_json::decode($display['data'], true);

			}
			$out['displays'] = $dspout;
		}

		# DIRECTORIES
		if(isset($load_directories))
		{
			$dirout = array();

			foreach($load_directories as $dir)
			{
				$dirid = $dir['name'];
				$dirout[$dirid]['uri'] = $dir['uri'];
				$dirout[$dirid]['delimiter'] = $dir['delimiter'];
				$dirout[$dirid]['name'] = $dir['description'];
				$dirout[$dirid]['match_direct'] = dwho_json::decode($dir['match_direct'], true) == false ? array() : dwho_json::decode($dir['match_direct'], true);
				$dirout[$dirid]['match_reverse'] = dwho_json::decode($dir['match_reverse'], true) == false ? array() : dwho_json::decode($dir['match_reverse'], true);
				$dirout[$dirid]['field_phone'] = dwho_json::decode($dir['field_phone'], true) == false ? array() : dwho_json::decode($dir['field_phone'], true);
				$dirout[$dirid]['field_fullname'] = dwho_json::decode($dir['field_fullname'], true) == false ? array() : dwho_json::decode($dir['field_fullname'], true);
				$dirout[$dirid]['field_firstname'] = dwho_json::decode($dir['field_firstname'], true) == false ? array() : dwho_json::decode($dir['field_firstname'], true);
				$dirout[$dirid]['field_lastname'] = dwho_json::decode($dir['field_lastname'], true) == false ? array() : dwho_json::decode($dir['field_lastname'], true);
				$dirout[$dirid]['field_company'] = dwho_json::decode($dir['field_company'], true) == false ? array() : dwho_json::decode($dir['field_company'], true);
				$dirout[$dirid]['field_mail'] = dwho_json::decode($dir['field_mail'], true) == false ? array() : dwho_json::decode($dir['field_mail'], true);
				$dirout[$dirid]['display_reverse'] = dwho_json::decode($dir['display_reverse'], true) == false ? array() : dwho_json::decode($dir['display_reverse'], true);
			}
			$out['directories'] = $dirout;
		}

		# REVERSEDID
		if(isset($load_rdid))
		{
			$rdidout = array();
			
			foreach($load_rdid as $rdid)
			{
				$rdidid = $rdid['number'];
				$rdiddir = array();
				$rdiddir = dwho_json::decode($rdid['directories'], true);
				foreach($rdiddir as $k => $v)
				{
					$rdiddir[$k] = "directories.".$v;
				}
				$rdidout[$rdidid] = $rdiddir;
			}
			$out['reversedid'] = $rdidout;
		}

		# SHEETS
		$sheetsout = array();
		if(isset($load_sheetevents))
		{
			$evtout = array();
			foreach(array_keys($load_sheetevents[0]) as $k)
			{
				if($k == 'id')
					continue;
				$evtout[$k] = $load_sheetevents[0][$k];
			}
			$evtout['custom'] = dwho_json::decode($evtout['custom'], true) == false ? array() : dwho_json::decode($evtout['custom'], true);
			$out['sheets']['events'] = $evtout;
		}
		if(isset($load_sheetactions))
		{
			$actout = array();
			$dispout = array();
			foreach($load_sheetactions as $action)
			{
				$qtui = "null";
				$actid = $action['name'];
				$actout[$actid]['whom'] = $action['whom'];
				$actout[$actid]['focus'] = $action['focus'] == 1 ? "yes" : "no";
				$actout[$actid]['contexts'] = dwho_json::decode($action['context'], true) == false ? array() : dwho_json::decode($action['context'], true);
				$actout[$actid]['capaids'] = dwho_json::decode($action['capaids'], true) == false ? array() : dwho_json::decode($action['capaids'], true);
				$actout[$actid]['sheet_info'] = "sheets.displays.screens.".$actid;
				$actout[$actid]['systray_info'] = "sheets.displays.systrays.".$actid;
				$actout[$actid]['sheet_qtui'] = "sheets.displays.qtui.".$actid;
				$actout[$actid]['action_info'] = "sheets.displays.infos.".$actid;

				$arr = array();
				$arr = dwho_json::decode($action['sheet_info'], true);
				foreach($arr as $k=>$v)
				{
					$a1 = $arr[$k];
					if($a1[1] == 'form')
						$qtui = $a1[3];
				}
				$dispout['systrays'][$actid] = dwho_json::decode($action['systray_info'], true) == false ? array() : dwho_json::decode($action['systray_info'], true);
				$dispout['qtui'][$actid][$qtui] = $action['sheet_qtui'];
				$dispout['infos'][$actid] = dwho_json::decode($action['action_info'], true) == false ? array() : dwho_json::decode($action['action_info'], true);

			}
			$out['sheets']['actions'] = $actout;
			$out['sheets']['displays'] = $dispout;
		}
			

		# PRESENCES
		if(isset($load_presences))
		{
			$presout = array();
			foreach($load_presences as $pres)
			{
				$presid = $pres['name'];
				$id = $pres['id'];
				$where = array();
				$where['presence_id'] = $id;
				$load_status = $ctistatus->get_all_where($where);

				$statref = array();
				foreach($load_status as $stat)
				{
					$statref[$stat['id']] = $stat['name'];
				}

				foreach($load_status as $stat)
				{
					$name = $stat['name'];
					$presout[$presid][$name]['display'] = $stat['display_name'];
					$presout[$presid][$name]['color'] = $stat['color'];
					$accessids = $stat['access_status'];
				
					$accessstatus = array();
					foreach(explode(',', $accessids) as $i)
					{
						$accessstatus[] = $statref[$i];
					}
					$presout[$presid][$name]['status'] = $accessstatus;

					$actions = explode(',', $stat['actions']);
					$pattern = '/^(.*)\((.*)\)/';
					$actionsout = array();
					foreach($actions as $a)
					{
						$match = array();
						preg_match($pattern, $a, $match);
						$actionsout[$match[1]] = $match[2];
					}
					$presout[$presid][$name]['actions'] = $actionsout;
				}
			}
			$out['presences'] = $presout;
		}

		# MAIN
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
		$out['main']['contextlist'] = $ctxlist;
		$out['main']['userlists'] = array();
		$out['main']['parting_astid_context'] = array();
		if($load_inf[0]['parting_astid_context'] != "")
			$out['main']['parting_astid_context'] = explode(",", $load_inf[0]['parting_astid_context']);

		# PHONEHINTS
		if(isset($load_phonehints))
		{
			$hintsout = array();
			foreach($load_phonehints as $ph)
			{
				$phid = $ph['number'];
				$hintsout[$phid] = array($ph['name'], $ph['color']);
			}
			$out['phonehints'] = $hintsout;
		}

		# PROFILES
		if(isset($load_profiles))
		{
			foreach($load_profiles as $pf)
			{
				$pfid = $pf['name'];
				$prefs = array();
				$prefout = array();

				$prefs = explode(',', $pf['preferences']);
				$pattern = '/^(.*)\((.*)\)/';
				foreach($prefs as $p)
				{
					$match = array();
					preg_match($pattern, $p, $match);
					$prefout[$match[1]] = $match[2];
				}
				$out['xivocti']['required_client_version'] = CLIENT_VERSION;
				$out['xivocti']['profils'][$pfid] = array(
					'xlets' => dwho_json::decode($pf['xlets'], true),
					'funcs' => explode(',', $pf['funcs']),
					'maxgui' => $pf['maxgui'],
					'appliname' => $pf['appliname'],
					'presence' => "presences.".$pf['presence'],
					'services' => explode(',', $pf['services']),
					'preferences' => $prefout
				);
			}
		}

		# XiVO SERVERS
		if(isset($load_inf[0]['asterisklist']) && dwho_has_len($load_inf[0]['asterisklist']))
		{
			$astlist = explode(',', $load_inf[0]['asterisklist']);
			foreach($astlist as $k => $v)
			{
				$hostname = $list[$v]['name'];
				$url_scheme = $list[$v]['url']['scheme'];
				$url_auth_host = $list[$v]['url']['authority']['host'];
				if((preg_match('/^127\./', $url_auth_host) > 0) || (preg_match('/^localhost/', $url_auth_host) > 0))
				{
					$json = $url_scheme . '://' . $url_auth_host . '/service/ipbx/json.php/private/';
				}
				else
				{
					$json = $url_scheme . '://' . $url_auth_host . '/service/ipbx/json.php/restricted/';
				}

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

		$_TPL->set_var('info',$out);
		break;
}

$_TPL->set_var('act',$act);
$_TPL->set_var('sum',$_QRY->get('sum'));
$_TPL->display('/service/ipbx/'.$ipbx->get_name().'/generic');

?>
