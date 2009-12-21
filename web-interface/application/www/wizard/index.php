<?php
$all_steps = array('welcome', 
					'lang', 
					'license', 
					'ipbxengine', 
					'dbconfig', 
					'check', 
					'adminsetup', 
					'entctx',
					'ipbximportuser', 
					'validate',
					'send');

function prev_step($s)
{
	global $all_steps;
	$cs = array_search($s, $all_steps);
	if($cs > 0)
		$p = $cs - 1;
	else
		$p = 0;
	return $all_steps[$p];
}

function next_step($s)
{
	global $all_steps;
	$cs = array_search($s, $all_steps);
	if($cs < count($all_steps))
		$p = $cs + 1;
	else
		$p = count($all_steps);
	return $all_steps[$p];
}

/*
function request_check($host)
{
	$ck = "https://" . $host . "/hw.php";
	$chdl = curl_init($ck);
	curl_setopt($chdl, CURLOPT_SSL_VERIFYPEER, false);
	curl_exec($chdl);
	curl_close($chdl);
}
*/

$step = isset($_QR['step']) === true ? $_QR['step']  : 'welcome';	# current step, as-is
$submitted = isset($_QR['next']) === true ? $step : '';				# if submitted through 'next', set to just-submitted (current) step
if(isset($_QR['prev']))												# if submitted through 'prev', re-set step to previous
{
	$step = prev_step($step);
}

if(!isset($_SESSION['_wizard']))
	$_SESSION['_wizard'] = array();

/*
class XMLParser
{
    var $rawXML;
    var $valueArray = array();
    var $keyArray = array();
    var $parsed = array();
    var $index = 0;
    var $attribKey = 'attributes';
    var $valueKey = 'value';
    var $cdataKey = 'cdata';
    var $isError = false;
    var $error = '';

    function XMLParser($xml = NULL)
    {
        $this->rawXML = $xml;
    }

    function parse($xml = NULL)
    {
        if (!is_null($xml))
        {
            $this->rawXML = $xml;
        }

        $this->isError = false;
           
        if (!$this->parse_init())
        {
            return false;
        }

        $this->index = 0;
        $this->parsed = $this->parse_recurse();
        $this->status = 'parsing complete';
        return $this->parsed;
    }

    function parse_recurse()
    {       
        $found = array();
        $tagCount = array();

        while (isset($this->valueArray[$this->index]))
        {
            $tag = $this->valueArray[$this->index];
            $this->index++;

            if ($tag['type'] == 'close')
            {
                return $found;
            }

            if ($tag['type'] == 'cdata')
            {
                $tag['tag'] = $this->cdataKey;
                $tag['type'] = 'complete';
            }

            $tagName = isset($tag['attributes']['id']) ? $tag['attributes']['id'] : $tag['tag'];

            if (isset($tagCount[$tagName]))
            {       
                if ($tagCount[$tagName] == 1)
                {
                    $found[$tagName] = array($found[$tagName]);
                }
                   
                $tagRef =& $found[$tagName][$tagCount[$tagName]];
                $tagCount[$tagName]++;
            }
            else   
            {
                $tagCount[$tagName] = 1;
                $tagRef =& $found[$tagName];
            }

            switch ($tag['type'])
            {
                case 'open':
                    $tagRef = $this->parse_recurse();

                    if (isset($tag['attributes']))
                    {
                        $tagRef[$this->attribKey] = $tag['attributes'];
                    }
                       
                    if (isset($tag['value']))
                    {
                        if (isset($tagRef[$this->cdataKey]))   
                        {
                            $tagRef[$this->cdataKey] = (array)$tagRef[$this->cdataKey];   
                            array_unshift($tagRef[$this->cdataKey], $tag['value']);
                        }
                        else
                        {
                            $tagRef[$this->cdataKey] = $tag['value'];
                        }
                    }
                    break;

                case 'complete':
                    if (isset($tag['attributes']))
                    {
                        $tagRef[$this->attribKey] = $tag['attributes'];
                        $tagRef =& $tagRef[$this->valueKey];
                    }

                    if (isset($tag['value']))
                    {
                        $tagRef = $tag['value'];
                    }
                    break;
            }
        }
        return $found;
    }

    function parse_init()
    {
        $this->parser = xml_parser_create();

        $parser = $this->parser;
        xml_parser_set_option($parser, XML_OPTION_CASE_FOLDING, 0);    
        xml_parser_set_option($parser, XML_OPTION_SKIP_WHITE, 1);       
        if (!$res = (bool)xml_parse_into_struct($parser, $this->rawXML, $this->valueArray, $this->keyArray))
        {
            $this->isError = true;
            $this->error = 'error: '.xml_error_string(xml_get_error_code($parser)).' at line '.xml_get_current_line_number($parser);
        }
        xml_parser_free($parser);

        return $res;
    }
}
*/
function parse_obj($o)
{
	if(is_array($o) === true)
	{
		$a = array();
		foreach($o as $k => $v)
		{
			if(is_object($v) === true)
			{
				if(isset($v->id) === true)
					$attrname = $v->id;
				else if(isset($v->type) === true)
					$attrname = $v->type;
				else
				{
					$a[$k] = $v;
					continue;
				}

				if(isset($a[$attrname]) === true)
				{
					$hold = $a[$attrname];
					$a[$attrname] = array();
					$a[$attrname][] = $hold;
					$a[$attrname][] = parse_obj($v);
				}
				else if(isset($v->value) === true
				&& is_scalar($v->value) === true)
					$a[$attrname] = $v->value;
				else if(isset($v->cdata) === true
				&& is_scalar($v->cdata) === true)
					$a[$attrname] = $v->cdata;
				else if(count((array) $v) === 1)
					$a[$attrname] = '';
				else
					$a[$attrname] = parse_obj($v);
			}
			else
				$a[$k] = $v;
		}

		return($a);
	}
	else if(is_object($o) === true)
	{
		$a = array();

		foreach($o as $k => $v)
		{
			$a[$k] = parse_obj($v);
		}

		return($a);
	}
	else
		return($o);
}

function pkg_check($json_str)
{
	$deps = array();
	if(isset($json_str))
	{
		$dep_list = dwho_json::decode($json_str);
		foreach($dep_list->base->depends as $pkgname => $pkginfo)
			$deps[$pkgname] = $pkginfo->status;
	}
	return($deps);
}

#
# Process submitted form
#
$_TPL->load_i18n_file('bloc/wizard/index');
if(!isset($_QR['prev']))
{
	if($submitted === '')
	#
	# a form-specific submit-field, other than 'next', was hit
	#
	{
		switch($step)
		{
			case 'check':
				if(isset($_QR['db-cnx-check']))
				{
					# mysql db cnx check here and set result as db-cnx-check-res either 'success' or 'fail'
					$mycnx = @mysql_connect(
						$_SESSION['_wizard']['db']['mysql']['host'],
						$_SESSION['_wizard']['db']['mysql']['xivo']['db-user'],
						$_SESSION['_wizard']['db']['mysql']['xivo']['db-pwd']);
					if($mycnx === false)
						$_TPL->set_var('db-cnx-check-res', 'fail');
					else
					{
						$_TPL->set_var('db-cnx-check-res', 'success');
						mysql_close($mycnx);
					}
				}
				break;
			case 'ipbximportuser':
				$import_failed = array();
				if(isset($_FILES))
				{
					$imported = array();
					$import_lines = file($_FILES['import']['tmp_name']);
					foreach($import_lines as $line)
					{
						$line = rtrim($line);
						if(strlen($line) === 0)	
							continue;

						$line_data = explode("|", $line);
						if(count($line_data) === 5)
						{
							if(($line_data[3] >= $_SESSION['_wizard']['context']['internal']['numbeg'])
								&& ($line_data[3] <= $_SESSION['_wizard']['context']['internal']['numend']))
							{
								if($line_data[4] === '')				# if no pwd, set same as username
									$line_data[4] = $line_data[2];
								$imported[] = $line_data;
							}
							else
								$import_failed[] = array($line, $_TPL->bbf('wz-import-failed-number'));
						}
						else
							$import_failed[] = array($line, $_TPL->bbf('wz-import-failed-fields'));
					
						if(count($imported) > 0)
						{
							$msg = count($imported) . ' ' . $_TPL->bbf('wz-import-lines-imported');
							$_SESSION['_wizard']['import'] = $imported;
							if(count($import_failed) > 0)
							{
								$msg .= '<br>';
								foreach($import_failed as $failed)
								{
									$msg .= $failed[0] . ': ' . $failed[1] . '<br>';
								}
							}
							$_TPL->set_var('wz-message', $msg);
						}
						else
							$_TPL->set_var('wz-message', $_TPL->bbf('wz-import-no-valid-data'));
					}
				}
				break;
			default:
				break;
		}
	}
	else
	{
		#
		# 'next' hit, consider it regular form submission/processing
		#
		$next_disp = $step;
		switch($submitted)
		{
			case 'welcome':
				$next_disp = next_step($step);
				break;
			case 'lang':
				if(isset($_QR['language']))
					$_SESSION['_wizard']['lang'] = $_QR['language'];
				$next_disp = next_step($step);
				break;
			case 'license':
				if(isset($_QR['wz-license-agree']))
					$next_disp = next_step($step);
				else
					$_TPL->set_var('wz-message', $_TPL->bbf('wz-license-validation'));
				break;
			case 'check':
				if(!isset($_QR['reload']))
					$next_disp = next_step($step);
				break;
			case 'ipbxengine':
				if(isset($_QR['ipbx-engine']))
					$_SESSION['_wizard']['ipbx-engine'] = $_QR['ipbx-engine'];
				$next_disp = next_step($step);
				break;
			case 'dbconfig':
				#
				# TODO: Add check if option alternate from the one currently selected is defined 
				#		purge it then
				#
				$db_msg = '';
				$db_targets = array('xivo', 'ipbx');
				$db_uris 	= array('xivo' => '', 
									'ipbx' => '');
				if(isset($_QR['db-backend']))
				{
					$_SESSION['_wizard']['db']['backend'] = $_QR['db-backend'];
					switch($_QR['db-backend'])
					{
						case 'sqlite':
							foreach($db_targets as $dbtarget)
							{
								if(dwho_has_len($_QR, 'db-file-' . $dbtarget))
								{
									$_SESSION['_wizard']['db'][$_QR['db-backend']][$dbtarget] = $_QR['db-file-' . $dbtarget];
									$db_uris[$dbtarget] = 'sqlite:/' . $_QR['db-file-' . $dbtarget];
								}
								else
									$db_msg .= $_TPL->bbf('wz-db-sqlite-missing-' . $dbtarget);
							}
							if(dwho_has_len($db_uris, 'xivo') && dwho_has_len($db_uris, 'ipbx'))
								$next_disp = next_step($step);
							else
								$_TPL->set_var('wz-message', $db_msg);
							break;
						case 'mysql':
							if(dwho_has_len($_QR, 'db-host'))
							{
								$_SESSION['_wizard']['db'][$_QR['db-backend']]['host'] = $_QR['db-host'];
								$db_ok = array('xivo' => 0, 'ipbx' => 0);
								foreach($db_targets as $dbtarget)
								{
									foreach(array('db-dbname', 'db-user', 'db-pwd') as $fld_head)
									{
										if(dwho_has_len($_QR, $fld_head . '-' . $dbtarget))
										{
											$_SESSION['_wizard']['db'][$_QR['db-backend']][$dbtarget][$fld_head] = $_QR[$fld_head . '-' . $dbtarget];
											$db_ok[$dbtarget] += 1;
										}
									}
								}
								if(($db_ok['xivo'] !== 3) || ($db_ok['ipbx'] !== 3))
									$db_msg .= $_TPL->bbf('wz-db-mysql-missing-info');
								else
								{
									# compose uris
								}
								if(dwho_has_len($_QR, 'db-host-port'))
									$_SESSION['_wizard']['db'][$_QR['db-backend']]['host-port'] = $_QR['db-host-port'];
							}
							else
								$db_msg = $_TPL->bbf('wz-db-mysql-missing-host');
							
							if(dwho_has_len($db_msg))
								$_TPL->set_var('wz-message', $db_msg);
							else
								$next_disp = next_step($step);
							break;
						default:
							$_TPL->set_var('wz-message', $_TPL->bbf('wz-db-unknown-backend'));
							break;
					}
				}
				else {
					$_TPL->set_var('wz-message', $_TPL->bbf('wz-db-backend-not-set'));
				}
				break;
			case 'adminsetup':
				$pwd_ok = $sname_ok = false;
				if(isset($_QR['fm-admin-pwd']) 
				&& isset($_QR['fm-admin-pwd-retype']) 
				&& isset($_QR['fm-server-name']))
				{
					if(($_QR['fm-admin-pwd'] === $_QR['fm-admin-pwd-retype']) 
					&& (!($_QR['fm-admin-pwd'] === '')))
					{
						$_SESSION['_wizard']['server']['admin-pwd'] = $_QR['fm-admin-pwd'];
						$pwd_ok = true;	
					}
					else
						if($_QR['fm-admin-pwd'] === '')
							$_TPL->set_var('wz-message', $_TPL->bbf('wz-admin-password-blank'));
						else
							$_TPL->set_var('wz-message', $_TPL->bbf('wz-admin-password-match'));
					if($_QR['fm-server-name'] !== '')
					{
						$_SESSION['_wizard']['server']['name'] = $_QR['fm-server-name'];
						$sname_ok = true;
					}
					else
						$_TPL->set_var('wz-message', $_TPL->bbf('wz-admin-server-blank'));
				}
				if(dwho_has_len($_QR, 'fm-server-ip'))
					$_SESSION['_wizard']['server']['ip'] = $_QR['fm-server-ip'];
				if(dwho_has_len($_QR, 'fm-server-netmask'))
					$_SESSION['_wizard']['server']['netmask'] = $_QR['fm-server-netmask'];
				if(dwho_has_len($_QR, 'fm-server-gw'))
					$_SESSION['_wizard']['server']['gw'] = $_QR['fm-server-gw'];
				if(dwho_has_len($_QR, 'fm-server-dns1'))
					$_SESSION['_wizard']['server']['dns1'] = $_QR['fm-server-dns1'];
				if(dwho_has_len($_QR, 'fm-server-dns2'))
					$_SESSION['_wizard']['server']['dns2'] = $_QR['fm-server-dns2'];

				if($pwd_ok && $sname_ok)
					$next_disp = next_step($step);
				break;
			case 'entctx':
				if(isset($_QR['ent']))
				{
					$entity = $_QR['ent'];
					if((!dwho_has_len($entity, 'name')) || (!dwho_has_len($entity, 'dispname')))
						$_TPL->set_var('wz-message', $_TPL->bbf('wz-entctx-entity-missing'));
					else
					{
						$_SESSION['_wizard']['entity'] = $entity;
						$next_disp = next_step($step);
					}
				}
				if(isset($_QR['ctx']))
					$_SESSION['_wizard']['context'] = $_QR['ctx'];
				break;
			case 'ipbximportuser':
				$next_disp = next_step($step); 
				break;
			case 'validate':
				$next_disp = next_step($step);
				break;
			case 'send':
				break;
			default:
				$next_disp = 'welcome';
		}
		$step = $next_disp;
	}
}

#
# render form
#
switch($step)
{
	case 'welcome':
		$_SESSION['_wizard'] = array();
		break;
	case 'lang':
		$_LANG 		= &dwho_gat::load_get('language',XIVO_PATH_OBJECTCONF);
		$lang_list 	= dwho_array_intersect_key($_LANG,dwho_i18n::get_language_translated_list());
		$_TPL->set_var('lang-list', $lang_list);
		break;
	case 'license':
		break;
	case 'check':
		dwho::load_class('dwho_curl');
		dwho::load_class('dwho_json');
		$curl_base_url = 'http://192.168.0.18:8668';
		$curl = new dwho_curl();
		$curl->set_option('failonerror', true);
		#
		# hardware info
		#
		$curl_req = '/lshw?class%5B%5D=memory&class%5B%5D=network';
		$hw = $curl->load($curl_base_url . $curl_req);
		$curl->close();
		if($hw === false)
			$_TPL->set_var('wz-message', $_TPL->bbf('wz-check-error') . '<br>' . $curl->error());
		else
		{
			$hwa = dwho_json::decode($hw);

			if(isset($hwa->lshw) === true)
			{
				$a = parse_obj($hwa->lshw);
				$memsize 	= $a['node']['memory'][0]['size']['cdata'];
				$net_key 	= isset($a['node']['network']) ? 'network' : 'network:0';
		
				$net_vendor = isset($a['node'][$net_key]['vendor'], $a['node'][$net_key]['product'])
								? $a['node'][$net_key]['vendor'] . ' ' . $a['node'][$net_key]['product']
								: $_TPL->bbf('wz-not-available');
				$net_driver = isset($a['node'][$net_key]['configuration']['setting']['driver'],
									$a['node'][$net_key]['configuration']['setting']['driverversion'])
								? $a['node'][$net_key]['configuration']['setting']['driver']
									. ' ver. ' . $a['node'][$net_key]['configuration']['setting']['driverversion']
								: $_TPL->bbf('wz-not-available');
				$net_mac 	= isset($a['node'][$net_key]['serial'])
								? $a['node'][$net_key]['serial']
								: $_TPL->bbf('wz-not-available');
				if(isset($a['node'][$net_key]['logicalname']))
				{
					$net_iface = $a['node'][$net_key]['logicalname'];
					$_SESSION['_wizard']['network']['iface'] = $net_iface;
				}
				else
					$net_iface = $_TPL->bbf('wz-not-available');
				if(isset($a['node'][$net_key]['configuration']['setting']['ip']))
				{
					$net_ip = $a['node'][$net_key]['configuration']['setting']['ip'];
					$_SESSION['_wizard']['network']['ip'] = $net_ip;
				}
				else
					$net_ip = $_TPL->bbf('wz-not-available');
				$net_speed = isset($a['node'][$net_key]['configuration']['setting']['speed'],
									$a['node'][$net_key]['configuration']['setting']['duplex']) 
								? $a['node'][$net_key]['configuration']['setting']['speed']
									. ' duplex: ' . $a['node'][$net_key]['configuration']['setting']['duplex']
								: $_TPL->bbf('wz-not-available');
				$net_autoneg = isset($a['node'][$net_key]['configuration']['setting']['autonegotiation']) 
								? $a['node'][$net_key]['configuration']['setting']['autonegotiation']
								: $_TPL->bbf('wz-not-available');
	
				$hw_info = array(
							'memsize' => isset($memsize) ? round($memsize / (1024 * 1024)) . 'MB' : $_TPL->bbf('wz-not-available'),
							'net-vnd' => $net_vendor,
							'net-ifc' => $net_iface,
							'net-drv' => $net_driver,
							'net-mac' => $net_mac,
							'net-ipa' => $net_ip,
							'net-aut' => $net_autoneg,
							'net-spd' => $net_speed);
				$_TPL->set_var('hw-info',$hw_info);
			}
		}
		#
		# packages
		#
		$pkglist = array();
		$curl_req = '/aptcache_update';
		$pku = $curl->load($curl_base_url . $curl_req);
		$curl->close();
		if($pku === false)
			$_TPL->set_var('wz-message', $_TPL->bbf('wz-check-error'));
		else
		{
			#
			# base packages
			#
			$xd = $curl->load($curl_base_url . '/dependencies_xivo');
			$curl->close();
			$pkglist['required']['base'] = pkg_check($xd);
			#
			# ipbx packages
			#
			$xd = $curl->load($curl_base_url . '/dependencies_' . $_SESSION['_wizard']['ipbx-engine']);
			$curl->close();
			$pkglist['required']['ipbx'] = pkg_check($xd);
			#
			# db packages
			#
			$curl_req = $curl_base_url . '/dependencies_' 
							. $_SESSION['_wizard']['db']['backend']
							. '?ipbxengine=' . $_SESSION['_wizard']['ipbx-engine'];
			$xd = $curl->load($curl_req);
			$curl->close();
			$pkglist['required']['db'] = pkg_check($xd);
		}
		$_TPL->set_var('sw-required', $pkglist['required']);
		break;
	case 'ipbxengine':
		$_TPL->set_var('ipbx-engines', array('asterisk' => 'Asterisk'));
		break;
	case 'dbconfig':
		$_TPL->set_var('db-backends', array('sqlite' => 'SQLite',
											'mysql'  => 'MySQL'));
		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/xivo/wizard.js');
		break;
	case 'adminsetup':
		$_TPL->set_var('ip',
			isset($_SESSION['_wizard']['server']['ip']) 
				? $_SESSION['_wizard']['server']['ip']
				: $_SESSION['_wizard']['network']['ip']);
		$_TPL->set_var('mask', 
			isset($_SESSION['_wizard']['server']['netmask'])
				? $_SESSION['_wizard']['server']['netmask']
				: $_SESSION['_wizard']['network']['mask']);
		$_TPL->set_var('gw', 
			isset($_SESSION['_wizard']['server']['gw'])
				? $_SESSION['_wizard']['server']['gw']
				: $_SESSION['_wizard']['network']['gw']);
		$_TPL->set_var('dns1',
			isset($_SESSION['_wizard']['server']['dns1']) ? $_SESSION['_wizard']['server']['dns1'] : '');
		$_TPL->set_var('dns2',
			isset($_SESSION['_wizard']['server']['dns2']) ? $_SESSION['_wizard']['server']['dns2'] : '');
		break;
	case 'entctx':
		$_TPL->set_var('ent-name',
			isset($_SESSION['_wizard']['entity']['name']) ? $_SESSION['_wizard']['entity']['name'] : '');
		$_TPL->set_var('ent-dispname',
			isset($_SESSION['_wizard']['entity']['dispname']) ? $_SESSION['_wizard']['entity']['dispname'] : '');
		if(isset($_SESSION['_wizard']['context']))
			$_TPL->set_var('ctx', $_SESSION['_wizard']['context']);
		break;
	case 'ipbximportuser':
		break;
	case 'validate':
		#
		# display all info from session storage
		#
		break;
	case 'send':
		dwho::load_class('dwho_json');
		$conf_json = dwho_json::encode($_SESSION['_wizard']);
		#
		# send configuration and set confirmation
		#
		$_TPL->set_var('send-result', false);
default:
		break;
}
$_TPL->set_var('step', $step);


$menu = &$_TPL->get_module('menu');
$menu->set_left('left/wizard/index');
$_TPL->set_struct('wizard/index');
$_TPL->display('wizard');

?>
