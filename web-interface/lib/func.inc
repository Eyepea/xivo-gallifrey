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
	$bool = strtolower(strval($bool));

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

function xivo_haslen($val,$key=null)
{
	if($key !== null &&
	is_array($val) === true
	&& isset($val[$key]) === true)
		$val = $val[$key];
	
	if(is_scalar($val) === false)
		return(false);

	$val = (string) $val;

	return(isset($val{0}));
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
	if(is_scalar($str) === false)
		return(false);

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
	if(is_scalar($str) === false)
		return(false);

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
			return(ctype_print($str));
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

function xivo_trunc(&$str,$nb=0,$end='',$chr=null)
{
	$r = strval($str);
	$nb = (int) $nb;
	$end = (string) $end;
	$chr = $chr === null ? ' ' : (string) $chr;
	
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

function xivo_group_array($ref,&$arr)
{
	if(xivo_issa($ref,$arr) === false
	|| ($nb = count($arr[$ref])) === 0
	|| ($list = xivo_get_aks($arr)) === false)
		return(false);

	$r = array();

	for($i = 0;$i < $list['cnt'];$i++)
	{
		$key = $list['keys'][$i];
		$val = &$arr[$key];

		if(is_array($val) === false)
			continue;

		for($j = 0;$j < $nb;$j++)
		{
			if(isset($r[$j]) === false)
				$r[$j] = array();

			$r[$j][$key] = isset($val[$j]) === false ? '' : $val[$j];
		}
	}

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
		$qry = xivo_query::mk_query_str($k);
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
	$email = strtolower(strval($email));
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
	$r = preg_replace('/\s+/',' ',strval($str));
	$r = str_replace(array('\'','"'),array('\\\'','\"'),$r);

	$r = xivo_htmlsc($r,$style);

	return($r);
}

function xivo_eol($str)
{
	$r = preg_replace('#\r\n|\n|\r#',XIVO_EOL,strval($str));

	return($r);
}

function xivo_alttitle($str,$sc=true)
{
	$sc = (bool) $sc;

	$pat = array('/<.+?>/','/\t+/','/\s+/','/&nbsp;| +/');
	$rep = array('','',' ',' ');
	$r = preg_replace($pat,$rep,strval($str));

	if($sc !== false)
		$r = xivo_htmlsc($r);

	return($r);
}

function xivo_notag($str)
{
	$pat = array('/<.+?>/','/\t+/','/&nbsp;| +/');
	$rep = array('','',' ');

	return(preg_replace($pat,$rep,strval($str)));
}

function xivo_notagscript($str)
{
	$r = preg_replace('@<script[^>]*?>.*?</script>@si','',strval($str));

	return($r);
}

function xivo_tagreplace($str)
{
	$pat = array('"','\'','<','>');
	$rep = array('&quot;','&#039;','&lt;','&gt;');

	$r = str_replace($pat,$rep,strval($str));
	
	return($r);
}

function xivo_htmlsc($str,$style=ENT_QUOTES)
{
	return(htmlspecialchars(strval($str),$style,'UTF-8'));
}

function xivo_htmlen($str,$style=ENT_QUOTES)
{
	return(htmlentities(strval($str),$style,'UTF-8'));
}

function xivo_malert($subject='',$msg='',$to='')
{
	$subject = (string) $subject;
	$msg = (string) $msg;
	$mail = $to !== '' ? xivo_chk_email($to) : '';

	$h  = 'MIME-Version: 1.0'."\n";
	$h .= 'Content-type: text/html; charset=utf-8'."\n";
	$h .= 'Content-Transfer-Encoding: 8bit'."\n";

	if($mail === false)
		return(trigger_error('Invalid e-mail',E_USER_WARNING));
	else if($mail === '' && defined('XIVO_ROOT_MAIL') === true && xivo_chk_email(XIVO_ROOT_MAIL) !== false)
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

	if(is_array($a) === false)
		return($r);

	$ak = array_keys($a);
	ksort($ak,$sort);
	$r = end($ak);

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

function xivo_calc_duration($beg,$end,$diff=false,$unset=false)
{
	$r = array();

	$unset = (bool) $unset;

	if(is_numeric($diff) === true)
		$r['diff'] = $diff;
	else
	{
		$r['beg'] = $beg;
		$r['end'] = $end;
		$r['diff'] = $end - $beg;
	}

	$r['s']  = $r['diff'];
	$r['d']  = floor($r['s'] / 86400);
	$r['s'] -= $r['d'] * 86400;
	$r['h']  = floor($r['s'] / 3600);
	$r['s'] -= $r['h'] * 3600;
	$r['m']  = floor($r['s'] / 60);
	$r['s'] -= $r['m'] * 60;

	if($unset === false)
		return($r);
	
	unset($r['beg'],$r['end'],$r['diff']);

	if($r['d'] === (float) 0)
		unset($r['d']);
	else
		return($r);

	if($r['h'] === (float) 0)
		unset($r['h']);
	else
		return($r);

	if($r['m'] === (float) 0)
		unset($r['m']);

	return($r);
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

?>
