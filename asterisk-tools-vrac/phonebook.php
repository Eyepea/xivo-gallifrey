<?php

header('Content-Type: text/xml; charset=utf-8');

$site = array();
$site[] = 'http://localhost/service/ipbx/sso.php';

define('PHONEBOOK_URL','http://192.168.0.254/phonebook/search.php');

define('XLDAP_ENABLE',false);
define('XLDAP_HOST','');
define('XLDAP_USER','');
define('XLDAP_PASS','');
define('XLDAP_DB','');

$xmlphone = array();

$xmlphone['snom'] = array();
$xmlphone['snom']['begtag'] = '<SnomIPPhoneDirectory>';
$xmlphone['snom']['endtag'] = '</SnomIPPhoneDirectory>';

$xmlphone['snom']['entry'] =		"\t".'<DirectoryEntry>'."\n".
					"\t\t".'<Name>{NAME}</Name>'."\n".
					"\t\t".'<Telephone>{PHONE}</Telephone>'."\n".
					"\t".'</DirectoryEntry>';

$xmlphone['snom']['noentry'] =		$xmlphone['snom']['begtag']."\n".
						strtr($xmlphone['snom']['entry'],array(
							'{NAME}' => 'Aucune entree',
							'{PHONE}' => ''))."\n".
					$xmlphone['snom']['endtag'];

$xmlphone['thomson'] = array();

$xmlphone['thomson']['begtag'] = '<ThomsonPhoneBook>';
$xmlphone['thomson']['endtag'] = '</ThomsonPhoneBook>';
$xmlphone['thomson']['entry'] =	$xmlphone['snom']['entry'];

$xmlphone['thomson']['noentry'] =	$xmlphone['thomson']['begtag']."\n".
						strtr($xmlphone['thomson']['entry'],array(
							'{NAME}' => 'Aucune entree',
							'{PHONE}' => ''))."\n".
					$xmlphone['thomson']['endtag'];

$useragent = isset($_SERVER['HTTP_USER_AGENT']) === true ? $_SERVER['HTTP_USER_AGENT'] : false;
$vendor = isset($_GET['vendor']) === true ? strval($_GET['vendor']) : false;

$xldap_enable = defined('XLDAP_ENABLE') === true ? (bool) XLDAP_ENABLE : false;

if($vendor === false && $useragent === false)
	die();

if($vendor === false)
{
	$uagent = array();
	$uagent['THOMSON ST2022'] = 'thomson';
	$uagent['THOMSON ST2030'] = 'thomson';
	$uagent['Mozilla/4.0 (compatible; snom300-SIP'] = 'snom';
	$uagent['Mozilla/4.0 (compatible; snom320-SIP'] = 'snom';
	$uagent['Mozilla/4.0 (compatible; snom360-SIP'] = 'snom';
	$uagent['Mozilla/4.0 (compatible; snom370-SIP'] = 'snom';

	$uakeys = array_keys($uagent);

	$nb = count($uakeys);

	for($i = 0;$i < $nb;$i++)
	{
		$key = &$uakeys[$i];
		$klen = strlen($key);

		if(strncasecmp($key,$useragent,$klen) !== 0)
			continue;

		$vendor = $uagent[$key];
		break;
	}
}

if(isset($xmlphone[$vendor]) === false)
	die();

$begtag = $xmlphone[$vendor]['begtag'];
$endtag = $xmlphone[$vendor]['endtag'];
$entry = $xmlphone[$vendor]['entry'];
$noentry = $xmlphone[$vendor]['noentry'];

if(isset($_GET['NAME']) === false && $vendor === 'snom')
{
	echo	'<SnomIPPhoneInput>',"\n",
		'<Title>Menu</Title>',"\n",
		'<Prompt>Prompt</Prompt>',"\n",
		'<URL>',PHONEBOOK_URL,'</URL>',"\n",
		'<InputItem>',"\n",
		'<DisplayName>Rechercher</DisplayName>',"\n",
		'<QueryStringParam>NAME</QueryStringParam>',"\n",
		'<DefaultValue />',"\n",
		'<InputFlags>a</InputFlags>',"\n",
		'</InputItem>',"\n",
		'</SnomIPPhoneInput>',"\n";

	die();
}

if(isset($_GET['NAME']) === false || strlen($_GET['NAME']) === 0)
	die($noentry);

$name = $_GET['NAME'];

/* XIVO */

$nb = count($site);

$result = array();

for($i = 0;$i < $nb;$i++)
{
	if(($search = sso_search_name($name,$site[$i])) === false)
		continue;

	$result = array_merge($search,$result);
}

/* LDAP */

if($xldap_enable === true
&& ($search = ldap_search_name($name)) !== false)
	$result = array_merge($search,$result);

/* RESULTS */

if(($nb = count($result)) === 0)
	die($noentry);

usort($result,'compare');

$arr = array();

echo $begtag,"\n";

for($i = 0;$i < $nb;$i++)
{
	$ref = &$result[$i];

	if(isset($arr[$ref['sign']]) === true)
		continue;

	$arr[$ref['sign']] = 1;

	$arr_entry = array('{NAME}' => $ref['name'],'{PHONE}' => $ref['phone']);

	echo strtr($entry,$arr_entry),"\n";
}

echo $endtag,"\n";

die();

function compare($a,$b)
{
	$a = strtolower($a['name']);
	$b = strtolower($b['name']);

	return(strcmp($a,$b));
}

function sso_search_name($gname,$url)
{
	$r = false;

	$len = strlen($gname);

	if(($handle = fopen($url,'r')) === false)
		return($r);

	$r = $tmp = array();
	$nb = 0;

	while(($tmp = fgetcsv($handle,1000,'|')) !== false && count($tmp) > 8)
	{
		if($tmp[1] === 'guest' || $tmp[1] === 'xivosb')
			continue;

		if(isset($tmp[9]) === false)
			$tmp[9] = '';

		if(strncasecmp($gname,$tmp[8],$len) !== 0 && strncasecmp($gname,$tmp[9],$len) !== 0)
			continue;

		$name = trim(trim($tmp[8]).' '.trim($tmp[9]));
		$phone = trim($tmp[1]);

		if($name === '' || $phone === '')
			continue;

		$sign = sha1('name:'.strtolower($name).';phone:'.strtolower($phone));

		$r[] = array('name' => $name,'phone' => $phone,'sign' => $sign);
		$nb++;
	}

	fclose($handle);

	if(isset($r[0]) === false)
		$r = false;

	return($r);
}

function ldap_search_name($gname)
{
	$gname = ldap_quote($gname);

	if(($ldapconn = ldap_connect(XLDAP_HOST)) === false)
		return(false);

	ldap_set_option($ldapconn,LDAP_OPT_PROTOCOL_VERSION,3);

	if(($ldapbind = ldap_bind($ldapconn,XLDAP_USER,XLDAP_PASS)) === false)
		return(false);

	$filter = '';

	if(ctype_digit($gname) === true)
	{
		$len = strlen($gname);

		for($i = 0;$i < $len;$filter .= $gname{$i}.(($i % 2) === 1 ? '*' : ''),$i++);

		$filter = '(telephoneNumber='.$filter.')'.
			  '(mobile='.$filter.')';
	}

	$filter = '(|'.
	          '(sn='.$gname.'*)'.
		  $filter.
		  '(givenName='.$gname.'*)'.
		  '(displayName='.$gname.'*)'.
		  '(name='.$gname.'*))';

	$field = array();
	$field[] = 'sn';
	$field[] = 'givenName';
	$field[] = 'displayName';
	$field[] = 'name';
	$field[] = 'telephoneNumber';
	$field[] = 'mobile';

	if(($ldapsearch = ldap_search($ldapconn,XLDAP_DB,$filter,$field)) === false)
		return(false);

	if(($entries = ldap_get_entries($ldapconn,$ldapsearch)) === false || $entries['count'] === 0)
		return(false);

	$r = array();

	$nb = $entries['count'];

	for($i = 0;$i < $nb;$i++)
	{
		$lastname = $firstname = '';

		$ref = &$entries[$i];

		if(isset($ref['displayname']) === true)
			$name = trim($ref['displayname'][0]);
		else if(isset($ref['name']) === true)
			$name = trim($ref['name'][0]);

		if(isset($ref['sn']) === true)
			$lastname = trim($ref['sn'][0]);

		if(isset($ref['givename']) === true)
			$firstname = trim($ref['givename'][0]);

		$fullname = trim($lastname.' '.$firstname);

		if($name === '')
			$name = $fullname;

		if($name === '')
			continue;

		if(isset($ref['telephonenumber']) === true
		&& ($phone = format_phone($ref['telephonenumber'][0])) !== '')
		{
			$sign = sha1('name:'.strtolower($name).';phone:'.strtolower($phone));

			$r[] = array('name' => $name,'phone' => $phone,'sign' => $sign);
		}

		if(isset($ref['mobile']) === true
		&& ($mobile = format_phone($ref['mobile'][0])) !== '')
		{
			$sign = sha1('name:'.strtolower($name).';phone:'.strtolower($mobile));

			$r[] = array('name' => $name.' - GSM','phone' => $mobile,'sign' => $sign);
		}
	}

	if(isset($r[0]) === false)
		$r = false;

	return($r);
}

function format_phone($phone)
{
	$phone = trim((string) $phone);

	if(isset($phone{0}) === false)
		return($phone);

	if($phone{0} === '+')
		$phone = '00'.substr($phone,1);

	$phone = str_replace(array(' ','+'),'',$phone);

	if(preg_match('@^(0033|33)(\(\d+\))?(\d+)$@',$phone,$match) !== 1)
	{
		if(isset($phone{9}) === true)
			$phone = '0'.$phone;

		return($phone);
	}

	$phone = '';

	if(isset($match[2],$match[2]{0}) === true)
		$phone = $match[2];

	$phone .= $match[3];

	$phone = str_replace(array('(',')'),'',$phone);

	if(isset($phone{8}) === true && isset($phone{9}) === false)
		$phone = '0'.$phone;

	if(isset($phone{9}) === true)
		$phone = '0'.$phone;

	return($phone);
}

function ldap_quote($str)
{
	$pat = array('\\',' ','*','(',')');
	$rep = array('\\5c','\\20','\\2a','\\28','\\29');

	return(str_replace($pat,$rep,$str));
}

?>
