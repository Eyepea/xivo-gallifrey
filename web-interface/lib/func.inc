<?php

function xivo_get_memory($value,$name=true)
{
	$get_name = strtolower((string) $value);
	$name = (bool) $name;

	if($name === true &&
	($get_name === 'memory_limit' || $get_name === 'post_max_size' || $get_name === 'upload_max_filesize') === true)
		$mem = ini_get($get_name);
	else
		$mem = strtoupper($value);

	$arr = array('K' => 1024, 'M' => 1048576);

	$type = substr($mem,-1,1);

	if(xivo_digit($mem) !== false)
		$r = $mem;
	else if(isset($arr[$type]) === true)
		$r = substr($mem,0,strlen($mem)-1) * $arr[$type];
	else
		$r = false;

	return($r);
}

function xivo_bool($bool)
{
	$bool = strtolower($bool);

	switch($bool)
	{
		case 'y':
		case 't':
		case 'on':
		case 'yes':
		case 'true':
			$r = true;
			break;
		case 'n':
		case 'f':
		case 'off':
		case 'no':
		case 'false':
			$r = false;
			break;
		default:
			$r = (bool) $bool;
	}

	return($r);
}

function xivo_empty($val,$str=true)
{
	if(empty($val) === false || $val === '0' || $val === 0
	|| ($str !== true && $val !== $str) === true)
		return(false);

	return(true);
}

function xivo_digit($str=0,$def=null,$t=false)
{
	$r = false;

	$t = (bool) $t;

	if(ctype_digit($str) === true)
		return($str);
	
	if($def === null)
		return($r);

	if($t === true)
		$r = ctype_digit($def) === true ? $def : false;
	else
		$r = $def;

	return($r);	
}

function xivo_xdigit($str=0,$def=null,$t=false)
{
	$r = false;

	$t = (bool) $t;

	if(ctype_xdigit($str) === true)
		return($str);
	
	if($def === null)
		return($r);

	if($t === true)
		$r = ctype_xdigit($def) === true ? $def : false;
	else
		$r = $def;

	return($r);	
}

function xivo_uint($str=0,$def=0,$t=false)
{
	$str = (int) $str;
	$def = (int) $def;
	$t = (bool) $t;

	if($str < 0)
		$str = (int) substr($str,1);

	if($def !== 0 && $str === 0)
		$str = $t === true ? xivo_uint($def) : $def;

	return($str);
}

function xivo_cprint($str='',$def=null,$t=false)
{
	$r = false;

	$t = (bool) $t;

	if(xivo_cast_print($str,'print') === true)
		return($str);
	
	if($def === null)
		return($r);

	if($t === true)
		$r = xivo_cast_print($def,'print') === true ? $def : false;
	else
		$r = $def;

	return($r);
}

function xivo_cast_print($str,$type='')
{
	$type = strtolower((string) $type);

	$reg = '\x00-\x08\x0B\x0C\x0E-\x1F\7F';

	switch($type)
	{
		case 'crlf':
			$reg .= '\x09';
			break;
		case 'tab':
			$reg .= '\x0A\x0D';
			break;
		case 'print':
			$reg .= '\x09\x0A\x0D';
			break;
		case 'strict':
			return(ctype_print($str));
	}

	if(preg_match('/['.$reg.']/',$str) === 0)
		return(true);

	return(false);
}

function xivo_cast_print_crlf($str)
{
	return(xivo_cast_print($str,'crlf'));
}

function xivo_cast_print_tab($str)
{
	return(xivo_cast_print($str,'tab'));
}

function xivo_cast_print_strict($str)
{
	return(xivo_cast_print($str,'strict'));
}

function xivo_cast_specials($str,$type='')
{
	$str = (string) $str;
	$type = strtolower((string) $type);

	$reg = '\x20-\x2F\x3A-\x40\x5B-\x60\x7B-\x7E';

	switch($type)
	{
		case 'num':
			$reg .= '0-9';
			break;
		case 'alpha':
			$reg .= 'a-zA-Z';
			break;
		case 'alphanum':
			$reg .= 'a-zA-Z0-9';
			break;
	}

	if(preg_match('/^['.$reg.']+$/',$str) === 1)
		return(true);

	return(false);
}

function xivo_cast_numspecials($str)
{
	return(xivo_cast_specials($str,'num'));
}

function xivo_cast_alphaspecials($str)
{
	return(xivo_cast_specials($str,'alpha'));
}

function xivo_cast_alphanumspecials($str)
{
	return(xivo_cast_specials($str,'alphanum'));
}

function xivo_cast_except($val,$except,$type='')
{
	$type = (string) $type;

	if(is_numeric($except) === true)
		$texcept = 'numeric';
	else if(is_scalar($except) === true)
		$texcept = 'scalar';
	else
		$texcept = strtolower(gettype($except));

	switch($texcept.'-'.strtolower(gettype($val)))
	{
		case 'object-object':
		case 'array-array':
		case 'resource-resource':
		case 'null-null':
		case 'scalar-integer':
		case 'scalar-double':
		case 'scalar-boolean':
		case 'scalar-string':
		case 'numeric-integer':
		case 'numeric-double':
		case 'numeric-string':
			return($val);
	}

	$except = (array) $except;

	if(in_array($val,$except,true) === true)
		return($val);

	switch($type)
	{
		case 'xbool':
		case 'xboolean':
			$val = xivo_bool($val);
			break;
		case 'uint':
		case 'uinteger':
			$val = xivo_uint($val);
			break;
		case 'str':
		case 'string':
			$val = strval($val);
			break;
		case 'real':
		case 'float':
		case 'double':
			$type = 'double';
		case 'bool':
		case 'boolean':
		case 'int':
		case 'integer':
		case 'array':
		case 'object':
		case 'null':
			$val = settype($val,$type);
			break;
	}

	return($val);
}

function xivo_trunc(&$str,$nb=0,$chr=' ',$end='')
{
	$r = (string) $str;
	$nb = (int) $nb;
	$chr = (string) $chr;
	$end = (string) $end;
	
	$len = strlen($r);

	if($nb < 1)
		$nb = strlen($r);

	if($len > $nb && ($sub = substr($r,0,$nb)) !== '')
	{
		$r = $sub;
		if(($spos = strrpos($sub,$chr)) !== false)
			$r = substr($r,0,$spos);
		if($end)
	       		$r .= $end;
	}

	return($r);
}

function xivo_ak($k,&$a,$t=false,$f=false)
{
	$r = false;
	$t = (bool) $t;
	$f = (bool) $f;

	if(is_scalar($k) === false || is_array($a) === false)
		return($r);

	if($f === false)
	{
		if(isset($a[$k]) === true)
			$r = $t === true ? $a[$k] : true;
		else
			$r = false;
	}
	elseif(array_key_exists($k,$a) === true)
		$r = $t === true ? $a[$k] : true;
	else
		$r = false;

	return($r);
}

function xivo_pk($k,&$a,$t=false,$f=false)
{
	$r = false;
	$t = (bool) $t;
	$f = (bool) $f;

	if(is_scalar($k) === false || is_object($a) === false)
		return($r);

	if($f === false)
	{
		if(isset($a->{$k}) === true)
			$r = $t === true ? $a->{$k} : true;
		else
			$r = false;
	}
	elseif(array_key_exists($k,get_object_vars($a)) === true)
		$r = $t === true ? $a->{$k} : true;
	else
		$r = false;

	return($r);
}

function xivo_issa($k,&$a,$t=false)
{
	$r = false;
	$t = (bool) $t;

	if(is_scalar($k) === false || is_array($a) === false)
		return($r);

	if(isset($a[$k]) === true && is_array($a[$k]) === true)
		$r = $t === true ? $a[$k] : true;

	return($r);
}

function xivo_issa_val($k,&$a,$e=false)
{
	$r = false;
	$e = (bool) $e;

	if(xivo_issa($k,$a) === false)
		return($r);

	$r = array_values($a[$k]);

	if($e === false && array_key_exists(0,$r) === false)
		$r = false;
		
	return($r);
}

function xivo_isso($k,&$o,$t=false)
{
	$r = false;
	$t = (bool) $t;

	if(is_scalar($k) === false || is_object($o) === false)
		return($r);

	if(isset($o->{$k}) === true && is_object($o->{$k}) === true)
		$r = $t === true ? $o->{$k} : true;

	return($r);
}

function xivo_unserialize(&$str)
{
	$r = array();

	if(is_string($str) === true && empty($str) === false)
		$r = unserialize($str);

	return($r);
}

function xivo_print_r($a,$n='')
{
	$n = (string) $n;

	if(empty($n) === false || $n === '0')
		print '<b>'.$n.'</b> : ';
	print '<pre>'; print_r($a); print '</pre>';
}

function xivo_var_dump($a,$n='')
{
	$n = (string) $n;

	if(empty($n) === false || $n === '0')
		print '<b>'.$n.'</b> : ';
	print '<pre>'; var_dump($a); print '</pre>';
}

function xivo_sprint_r(&$a,$t=false)
{
	ob_start();
	print_r($a);
	$a = ob_get_contents();
	ob_end_clean();
	return(($t ? nl2br($a) : $a));
}

function xivo_svar_dump(&$a,$t=false)
{
	ob_start();
	var_dump($a);
	$a = ob_get_contents();
	ob_end_clean();
	return(($t ? nl2br($a) : $a));
}

function xivo_go($v='',$k='')
{
	$v = (string) $v;

	$sep = XIVO_SEP_ARG_OUT;

	if($k === true)
	{
		$query = &xivo_gct::get('xivo_query');
		$qstring = &$query->qstring_raw();
		$qry = &$query->qstring_raw_str();
	}
	else if(is_array($k) === true)
		$qry = xivo_mk_query($k);
	else if($k !== '' && $k !== false)
		$qry = (string) $k;
	else $qry = '';

	if(defined('XIVO_SESS_STR') === true)
	{
		if($k === true && $_SERVER['argc'] > 0)
			$q = xivo_ak(XIVO_SESS_NAME,$qstring,true) === XIVO_SESS_ID ? $qry : ($qry !== '' ? $qry.$sep : '').XIVO_SESS_STR;
		else if($k === false)
			$q = '';
		else if($k !== '')
			$q = ($qry !== '' ? $qry.$sep : '').XIVO_SESS_STR;
		else $q = XIVO_SESS_STR;
	}
	else $q = $qry;

	die(header('Location: '.$v.'?'.$q));
}

function xivo_chk_email($email,$strict=false)
{
	$nb = 0;
	$email = strtolower((string) $email);
	$strict = (bool) $strict;
	$len = strlen($email);

	if($len < 6 || $len > 255
	|| preg_match('/^[a-z0-9_-]+(\.[a-z0-9_-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*\.[a-z]{2,4}$/',$email) !== 1)
		return(false);

	if($strict === false || function_exists('getmxrr') === false)
		return($email);

	$a = explode('@',$email);
	$m = array();

	if(getmxrr($a[1],$m) === true && $nb === 0)
		return($email);

	return(false);
}

function xivo_mk_list(&$a,$f='')
{
	$r = false;
	$a = (array) $a;

	if(empty($a) === true)
		return($r);

	if($f === 'key')
		$a = array_keys($a);

	$r = '\''.implode('\',\'',array_map('addslashes', $a)).'\'';

	return($r);
}

function xivo_str_multicut($str,$arr)
{
	$r = false;

	$str = (string) $str;
	$arr = (array) $arr;

	if(($cnt = count($arr)) === 0 || ($len = strlen($str)) === 0)
		return($r);
	
	$r = array();

	if($len === 1)
		return(array($str));

	for($i = 0;$i < $cnt;$i++)
	{
		$cut = (int) $arr[$i];
		$r[] = substr($str,0,$cut);
		$str = substr($str,$cut);

		if(($len = strlen($str)) === 0)
			break;
	}

	if($len > 0)
		$r[] = $str;

	if(empty($r) === true)
		return(false);

        return($r);
}

function xivo_stript($str,$style=ENT_NOQUOTES)
{
	$r = preg_replace('/\s+/',' ',(string) $str);
	$r = str_replace(array('\'','"'),array('\\\'','\"'),$r);

	$r = xivo_htmlsc($r,$style);

	return($r);
}

function xivo_eol($str)
{
	$r = preg_replace('#\r\n|\n|\r#',XIVO_EOL,(string) $str);

	return($r);
}

function xivo_alttitle($str,$sc=true)
{
	$sc = (bool) $sc;

	$pat = array('/<.+?>/','/\t+/','/\s+/','/&nbsp;| +/');
	$rep = array('','',' ',' ');
	$r = preg_replace($pat,$rep,(string) $str);

	if($sc !== false)
		$r = xivo_htmlsc($r);

	return($r);
}

function xivo_notag($str)
{
	$pat = array('/<.+?>/','/\t+/','/&nbsp;| +/');
	$rep = array('','',' ');

	return(preg_replace($pat,$rep,(string) $str));
}

function xivo_notagscript($str)
{
	$r = preg_replace('@<script[^>]*?>.*?</script>@si','',(string) $str);

	return($r);
}

function xivo_tagreplace($str)
{
	$r = (string) $str;

	$pat = array('"','\'','<','>');
	$rep = array('&quot;','&#039;','&lt;','&gt;');

	$r = str_replace($pat,$rep,(string) $str);
	
	return($r);
}

function xivo_htmlsc($str,$style=ENT_QUOTES)
{
	return(htmlspecialchars((string) $str,$style,'UTF-8'));
}

function xivo_htmlen($str,$style=ENT_QUOTES)
{
	return(htmlentities((string) $str,$style,'UTF-8'));
}

function xivo_malert($subject='',$msg='',$to='')
{
	$subject = (string) $subject;
	$msg = (string) $msg;
	$mail = xivo_chk_email($to);

	$h  = 'MIME-Version: 1.0'."\n";
	$h .= 'Content-type: text/html; charset=utf-8'."\n";
	$h .= 'Content-Transfer-Encoding: 8bit'."\n";

	if($mail === false)
	{
		return(trigger_error('Invalid e-mail',E_USER_WARNING));
	}
	else if($mail === '' && defined('XIVO_ROOT_MAIL') && xivo_chk_email(XIVO_ROOT_MAIL))
	{
		$mail = 'TECK XIVO <'.XIVO_ROOT_MAIL.'>';
		$h .= 'To: '.$mail."\n";
	}
	else
		$h .= 'To: '.$mail.' <'.$mail.'>'."\n";

	$h .= 'From: Xivo <xivo@xivo>'."\n";
	$h .= 'X-Mailer: Xivo'."\n"; 
	$h .= 'X-Priority: 1 (Highest)'."\n";

	mb_send_mail($mail,$subject,$msg,$h);
}

function xivo_get_aks(&$a,$empty=false)
{
	$r = false;

	if(is_array($a) === false)
		return($r);

	$ak = array_keys($a);

	if(($cnt = count($ak)) > 0 || $empty === true)
		$r = array('keys' => $ak,'cnt' => $cnt);	

	return($r);
}

function xivo_last_key(&$a,$sort=SORT_NUMERIC)
{
	$r = false;

	if(is_array($a) === true)
	{
		$ak = array_keys($a);
		ksort($ak,$sort);
		$r = end($ak);
	}

	return($r);
}

function xivo_genpwd($str='')
{
	$r  = (string) $str;
	$r .= preg_replace('/[^a-z0-9]/i','',crypt(uniqid(rand(), true)));

	$len = strlen($r);

	if($len < 9)
		$r .= xivo_genpwd($r);

	return(substr($r,1,8));
}

function xivo_calc_page($page,$nb,$total)
{
	$r = array(
		'page'	=> 1,
		'nb'	=> 0,
		'pages'	=> 0,
		'total' => 0,
		'next'	=> 0,
		'prev'	=> 0,
		'beg'	=> 0,
		'end'	=> 0);

	$r['page'] = xivo_uint($page,1);
	$r['nb'] = xivo_uint($nb,10);
	$r['total'] = xivo_uint($total);

	if($r['total'] === 0)
		return($r);

	$r['pages'] = ceil($r['total']/$r['nb']);

	if($r['page'] > $r['pages'])
		$r['page'] = 1;

	$r['end'] = $r['page'] * $r['nb'];
	$r['beg'] = $r['end'] - $r['nb'];

	if($r['end'] < $r['total'])
		$r['next'] = true;

	if($r['page'] > 1 && $r['total'] > $r['nb'])
		$r['prev'] = true;

	return($r);
}

function xivo_mk_order($a,$def,$order='',$desc=true)
{
	$r = array();

	$a = (array) $a;
	$def = (string) $def;
	$order = (string) $order;
	$torder = (bool) $desc === true ? 'd' : 'a';

	if($order !== '')
		$korder = substr($order,1);

	if(isset($korder,$a[$korder]) === false)
	{
		$order = $def;
		$korder = substr($order,1);
	}

	if(($arr = xivo_get_aks($a)) !== false)
	{
		$r['tab'] = array();

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = (string) $arr['keys'][$i];

			$r['tab'][$k] = $torder.$k;
		}
	}

	if(isset($r['tab']) === false)
		return(false);
	else if(isset($korder,$r['tab'][$korder]) === true)
		$torder = $order{0} === 'a' ? 'a' : 'd';
	else
		$korder = key($r['tab']);

	$r['order'] = $torder.$korder;
	$r['sql'] = array('col' => $a[$korder]);

	if($torder === 'a')
	{
		$r['tab'][$korder]{0} = 'd';
		$r['sql']['ending'] = 'ASC';
	}
	else
	{
		$r['tab'][$korder]{0} = 'a';
		$r['sql']['ending'] = 'DESC';
	}

	return($r);
}

function xivo_mk_date($date)
{
	$r = false;

	$date = (string) $date;

	if(empty($date) === true)
		return($r);

	if(preg_match('@^(2[0-9]{3})-(0?[1-9]|1[0-2])-(0?[1-9]|[1-2][0-9]|3[0-1])$@',$date,$out) === 1 && checkdate($out[2],$out[3],$out[1]) === true)
	{
		$r = array('y' => $out[1],'m' => $out[2],'d' => $out[3],'cnt' => 3,'str' => $date,'int' => $out[1].$out[2].$out[3],'unix' => mktime(0,0,0,$out[2],$out[3],$out[1]));
	}
	elseif(preg_match('@^(2[0-9]{3})-(0?[1-9]|1[0-2])$@',$date,$out) === 1)
	{
		$r = array('y' => $out[1],'m' => $out[2],'cnt' => 2,'str' => $date,'int' => $out[1].$out[2],'unix' => mktime(0,0,0,$out[2],1,$out[1]));
	}
	elseif(preg_match('@^(2[0-9]{3})$@',$date,$out) === 1)
	{
		$r = array('y' => $out[1],'cnt' => 1,'str' => $date,'int' => $out[1],'unix' => mktime(0,0,0,1,1,$out[1]));
	}

	return($r);
}

function xivo_chk_date($dbeg='',$dend='',$strict=false)
{
	$r = array('cnt' => 0,'chk' => 2,'beg' => array(),'end' => array(),'sql' => array());

	$strict = (bool) $strict;

	if(($dbeg = &xivo_mk_date($dbeg)) === false)
	{
		if($strict === true)
			$r = false;
		else
			$r = &xivo_date_def();

		return($r);
	}

	$r['beg'] = $dbeg;

	$date = &xivo_date_def($dbeg['cnt']);
		
	if($date['beg']['int'] < $dbeg['int'])
	{
		$dbeg = $date['beg'];
		$dbeg['cnt'] = $date['cnt'];
		$r['beg'] = $dbeg;
	}

	if(empty($dend) === false && ($dend = &xivo_mk_date($dend)) !== false)
	{
		if($dbeg['cnt'] !== $dend['cnt'] || $dend['int'] < $dbeg['int'])
			$r['end'] = null;
		else if(($dmax = &xivo_date_interval($dbeg)) !== false && $dend['int'] > $dmax['int'])
		{
			if(($r['end'] = &xivo_mk_date($dmax['str'])) === false)
				$r['end'] = null;
		}
		else
			$r['end'] = $dend;

		if($r['end'] !== null)
		{
			if($date['beg']['int'] < $r['end']['int'])
			{
				$r['end'] = $date['beg'];
				$r['end']['cnt'] = $date['cnt'];
			}
		}
	}
	else $r['end'] = null;

	$r['chk'] = 1;
	$r['cnt'] = $r['beg']['cnt'];

	return($r);
}

function xivo_date_def($type=3)
{
	$type = (int) $type;

	$arr = array(
		1 => array('str' => '%Y','int' => '%Y'),
		2 => array('str' => '%Y-%m','int' => '%Y%m'),
		3 => array('str' => '%Y-%m-%d','int' => '%Y%m%d'));

	if(isset($arr[$type]) === false)
		$type = 3;

	$r = array('cnt' => 0,'chk' => 2,'beg' => array(),'end' => array(),'sql' => array());

	$r['chk'] = 1;
	$r['cnt'] = $type;
	$r['beg'] = array(
		'str'	=> strftime($arr[$type]['str']),
		'int'	=> strftime($arr[$type]['int']));

	switch($type)
	{
		case 3:
			$r['beg']['d'] = strftime('%d');
		case 2:
			$r['beg']['m'] = strftime('%m');
		case 1:
			$r['beg']['y'] = strftime('%Y');
	}

	$r['end'] = null;

	return($r);
}

function xivo_gmlcstrftime($format,$time)
{
	$r = false;

	$format = (string) $format;
	$time = (int) $time;

	$arr = explode('-',gmstrftime('%H-%M-%S-%m-%d-%Y',$time));

	if(isset($arr[5]) === false)
		return($r);

	$o = strftime('%z',$time);

	$h = (int) substr($o,0,-3);
	$m = (int) substr($o,-2);

	if($o{0} === '-')
	{
		$h = $arr[0]-$h;
		$m = $arr[1]-$m;
	}
	else
	{
		$h += $arr[0];
		$m += $arr[1];
	}

	$r = strftime($format,gmmktime($h,$m,$arr[2],$arr[3],$arr[4],$arr[5]));

	return($r);
}

function xivo_date_interval($dbeg)
{
	$r = false;

	if(xivo_ak('cnt',$dbeg) === true)
	{
		$date = array('y' => strftime('%Y'),'m' => 1,'d' => 1);

		switch($dbeg['cnt'])
		{
			case 3:
				$date['d'] = $dbeg['d'];
			case 2:
				$date['m'] = $dbeg['m'];
			case 1:
				$date['y'] = $dbeg['y'];
		}

		$arr = array(1 => '%Y',2 => '%Y-%m',3 => '%Y-%m-%d');

		$r = array();
		$r['int'] = '';
		$r['str'] = strftime($arr[$dbeg['cnt']],mktime(0,0,0,$date['m']+18,$date['d'],$date['y']));

		$dmax = explode('-',$r['str']);

		switch($dbeg['cnt'])
		{
			case 3:
				$r['int'] = $dmax[2];
				$r['d'] = $dmax[2];
			case 2:
				$r['int'] = $dmax[1].$r['int'];
				$r['m'] = $dmax[1];
			case 1:
				$r['int'] = $dmax[0].$r['int'];
				$r['y'] = $dmax[0];
		}
	}

	return($r);
}

function xivo_printf_strip($format,$argc=0)
{
	$format = (string) $format;
	$argc = xivo_uint($argc);

	if(preg_match_all('/%+/',$format,$match,PREG_OFFSET_CAPTURE) === 0)
		return($format);

	$j = $k = 0;

	$match = $match[0];
	$nb = count($match);

	for($i = 0;$i < $nb;$i++)
	{
		if(strlen($match[$i][0]) % 2 === 0)
			continue;

		if($argc <= $j)
		{
			$format = substr($format,0,$match[$i][1]+$k).'%'.substr($format,$match[$i][1]+$k);
			$k++;
		}

		$j++;
	}

	return($format);
}

function xivo_uncompress(&$data,$level=6)
{
	$r = (string) $data;
	$level = (int) $level;

	if($level < 0 || $level > 9)
		$level = 6;

	if(extension_loaded('zlib') === true)
		$r = gzcompress($r,$level);

	return($r);
}

function xivo_compress(&$data,$level=6)
{
	$r = (string) $data;
	$level = (int) $level;

	if($level < 0 || $level > 9)
		$level = 6;

	if(extension_loaded('zlib') === true)
		$r = gzcompress($r,$level);

	return($r);
}

?>
