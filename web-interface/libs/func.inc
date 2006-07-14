<?php

function get_memory_limit()
{
	$r = false;

	$arr = array('K' => 1024, 'M' => 1048576);
	$mem = ini_get('memory_limit');

	$len = strlen($mem);
	$mema = substr($mem,0,$len-1);
	$type = substr($mem,$len,1);

	if (is_numeric($memory_limit))
		$r = $mem;
	else if (isset($arr[$type]))
		$r = $mema * $arr[$type];

	return($r);
}

function xivo_uint($str=0)
{
	$str = (int) $str;
	if($str < 0)
		$str = (int) substr($str, 1);

	return($str);
}

function xivo_ak($k,$a,$f='',$t=false)
{
	$r = false;

	$k = (string) $k;

	if(!is_array($a))
		return($r);

	if($f != 'key')
	{
		if(isset($a[$k]))
			$r = $t !== false ? $a[$k] : true;
		else
			$r = false;
	}
	elseif(array_key_exists($k,$a))
		$r = $t !== false ? $a[$k] : true;
	else
		$r = false;

	return($r);
}

function xivo_issa($k,$a,$f='',$t=false)
{
	$r = false;

	$k = (string) $k;

	if(!is_array($a))
		return($r);

	if($f != 'key')
	{
		if(isset($a[$k]))
			$r = true;
		else
			return($r);
	}
	elseif(array_key_exists($k,$a))
		$r = true;
	else
		return($r);

	if(is_array($a[$k]))
		$r = $t !== false ? $a[$k] : true;

	return($r);
}

function xivo_unserialize($str)
{
	$r = false;

	if(is_string($str) && !empty($str))
		$r = unserialize($str);

	return($r);
}

function xivo_print_r($a,$n='')
{
	$n = (string) $n;

	if(!empty($n) || $n === '0')
		print '<b>'.$n.'</b> : ';
	print '<pre>'; print_r($a); print '</pre>';
}

function xivo_sprint_r($a,$t=false)
{
	ob_start();
	print_r($a);
	$a = ob_get_contents();
	ob_end_clean();
	return(($t ? nl2br($a) : $a));
}

function xivo_svar_dump($a,$t=false)
{
	ob_start();
	var_dump($a);
	$a = ob_get_contents();
	ob_end_clean();
	return(($t ? nl2br($a) : $a));
}

function xivo_die($v='',$k='')
{
	$v = (string) $v;

	if($k === true)
		$qry = $_SERVER['QUERY_STRING'];
	elseif($k !== '' && $k !== false)
		$qry = (string) $k;
	else $qry = '';

	if(defined('EID_SESS'))
	{
		if($k === true && $_SERVER['argc'] > 0)
		{
			parse_str($qry,$arr_qry);
			$q = xivo_ak(EID_NAME,$arr_qry,false,true) == EID_SEID ? $qry : $qry.'&'.EID_SESS;
		}
		elseif($k === false)
			$q = '';
		elseif($k !== '')
			$q = $qry.'&'.EID_SESS;
		else $q = EID_SESS;
	}
	else $q = $qry;

	die(header('Location: '.$v.'?'.$q));
}

function xivo_chk_email($e,$t=false)
{
	$r = false;
	$nb = 0;
	$e = (string) $e;
	$len = strlen($e);

	if($len >= 6 && $len < 255 && eregi('^([_a-z0-9-]+)(\.[_a-z0-9-]+)*@([a-z0-9-]+)(\.[a-z0-9-]+)*(\.[a-z]{2,4})$',$e))
	{
		if($t === true)
		{
			$a = explode('@',$e);
			$m = array();

			if(getmxrr($a[1], $m) && !$nb)
				$r = true;
		}
		else $r = true;
	}

	return($r);
}

function xivo_mk_list($a,$f='')
{
	$r = false;

	$func = 'mysql_real_escape_string';

	if(!is_array($a) || empty($a))
		return($r);

	if($f == 'key')
		$a = array_keys($a);

	if(!function_exists($func))
		$func = 'addslashes';

	$r = '\''.implode('\',\'',array_map($func, $a)).'\'';

	return($r);
}

function xivo_str_multicut($str,$arr)
{
	$r = false;

	$str = (string) $str;
	$arr = (array) $arr;

	if(($cnt = count($arr)) == 0 || ($len = strlen($str)) == 0)
		return($r);
	
	$r = array();

	if($len === 1)
		return(array($str));

	for($i = 0;$i < $cnt;$i++)
	{
		$cut = (int) $arr[$i];
		$r[] = substr($str,0,$cut);
		$str = substr($str,$cut);

		if(($len = strlen($str)) == 0)
			break;
	}

	if($len > 0)
		$r[] = $str;

	if(empty($r))
		return(false);

        return($r);
}

function xivo_alttitle($str)
{
	$str = (string) $str;

	$str = preg_replace('/<.+?>/', '', $str);
	$str = preg_replace('/&nbsp;|\s+/', ' ', $str);

	$str = str_replace('"', '\"', $str);

	return $str;
}

function xivo_notag($str)
{
	$str = (string) $str;

	$str = preg_replace('/<.+?>/', '', $str);
	$str = preg_replace('/\t+/', '', $str);
	$str = preg_replace('/&nbsp;| +/', ' ', $str);

	return $str;
}

function xivo_notagjs($str)
{
	$str = (string) $str;

	$str = preg_replace('@<script[^>]*?>.*?</script>@si','',$str);

	return($str);
}

function xivo_tagreplace($str)
{
	$str = (string) $str;

	$str = str_replace('"','&quot;',$str);
	$str = str_replace('\'','&#039;',$str);
	$str = str_replace('<','&lt;',$str);
	$str = str_replace('>','&gt;',$str);
	
	return($str);
}

function xivo_htmlsc($str)
{
	$str = (string) $str;

	return(htmlspecialchars($str,ENT_QUOTES));
}

function xivo_htmlen($str)
{
	$str = (string) $str;

	return(htmlentities($str,ENT_QUOTES));
}

function xivo_trunc($str,$nb=0,$chr=' ',$end='')
{
	$str = (string) $str;
	$nb = (int) $nb;
	$chr = (string) $chr;
	$end = (string) $end;
	
	$len = strlen($str);

	if($nb < 1)
		$nb = strlen($str);

	if($len > $nb && ($sub = substr($str,0,$nb)))
	{
		$str = $sub;
		if(($spos = strrpos($sub,$chr)))
			$str = substr($str,0,$spos);
		if($end)
	       		$str .= $end;
	}

	return($str);
}

function xivo_malert($subject='',$msg='',$to='')
{
	$subject = (string) $subject;
	$msg = (string) $msg;
	$mail = xivo_chk_email($to) === true ? (string) $to : false;

	$h  = 'MIME-Version: 1.0'."\n";
	$h .= 'Content-type: text/html; charset=utf-8'."\n";

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

	$h .= 'From: XIVO <xivo@xivo>'."\n";
	$h .= 'X-Mailer: XIVO'."\n"; 
	$h .= 'X-Priority: 1 (Highest)'."\n";

	mail($mail, $subject, $msg, $h);
}

?>
