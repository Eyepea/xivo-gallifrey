#! /usr/bin/php4 -q
<?php
	if(($opt = getopt('h:p:s:c:x')) === false
	|| isset($opt['h'],$opt['p'],$opt['s'],$opt['c']) === false)
		help();

	if(isset($opt['x']) === true)
		$sockhost = 'ssl://'.$opt['h'];
	else
		$sockhost = $opt['h'];
		
	if(strtolower(substr($opt['c'],-4)) === '.csv')
		$csv = $opt['c'];
	else
		$csv = $opt['c'].'.csv';

	$out = 'POST /service/ipbx/index.php/pbx_settings/users/?act=add&_eid='.$opt['s'].' HTTP/1.1'."\r\n";
	$out .= 'Host: '.$opt['h']."\r\n";
	$out .= 'Keep-Alive: 300'."\r\n";
	$out .= 'Connection: keep-alive'."\r\n";
	$out .= 'Cookie: _eid='.$opt['s']."\r\n";
	$out .= 'Cache-Control: max-age=0'."\r\n";
	$out .= 'Content-Type: application/x-www-form-urlencoded'."\r\n";

	$array = array();
	$array['ufeatures'] = array();

	$array['ufeatures']['ringseconds'] = 30;
	$array['ufeatures']['simultcalls'] = 5;
	$array['ufeatures']['musiconhold'] = '';
	$array['ufeatures']['comment'] = '';

	$array['protocol'] = array();
	$array['protocol']['host-dynamic'] = 'dynamic';
	$array['protocol']['dtmfmode'] = 'info';
	$array['protocol']['context'] = '';
	$array['protocol']['amaflags'] = 'documentation';
	$array['protocol']['accountcode'] = '';

	$array['voicemail'] = array();
	$array['voicemail']['attach'] = 1;

	$array['autoprov'] = array();
	$array['autoprov']['vendormodel'] = '';
	$array['autoprov']['macaddr'] = '';

	$row = 1;

	$total = $ok = $err = 0;

	$handle = fopen($csv,'r');

	while(($data = fgetcsv($handle,1000,',')) !== false)
	{
		$num = count($data);

		$lastname = trim($data[0]);
		$firstname = trim($data[1]);
		$email = strtolower(trim($data[2]));
		$number = trim($data[3]);
		
		if(isset($data[4],$data[4]{0}) === true)
			$password = trim($data[4]);
		else
			$password = '0000';

		if(isset($data[5],$data[5]{0}) === true)
			$secret = trim($data[5]);
		else
			$secret = '';

		if(isset($data[6],$data[6]{0}) === true)
			$outnumber = trim($data[6]);
		else
			$outnumber = '';

		$fullname = trim($firstname.' '.$lastname);

		$array['ufeatures']['firstname'] = $firstname;
		$array['ufeatures']['lastname'] = $lastname;
		$array['ufeatures']['number'] = $number;
		$array['ufeatures']['outnumber'] = $outnumber;

		$array['protocol']['name'] = $number;
		$array['protocol']['protocol'] = 'sip';
		$array['protocol']['secret'] = $secret;
		$array['protocol']['callerid'] = $fullname;

		$array['voicemail']['fullname'] = $fullname;
		$array['voicemail']['email'] = $email;
		$array['voicemail']['password'] = $password;

		$r = 'act=add&fm_send=1';
		$r .= '&'.mk_query_str($array['ufeatures'],'','ufeatures');
		$r .= '&'.mk_query_str($array['protocol'],'','protocol');

		if($email !== '')
			$r .= '&'.mk_query_str($array['voicemail'],'','voicemail');

		$r .= '&'.mk_query_str($array['autoprov'],'','autoprov');

		$len = strlen($r);

		$errno = 0;
		$errstr = '';

		$fp = fsockopen($sockhost, $opt['p'], $errno, $errstr, 60);

		$write = $out;

		if($fp === false)
		{
			$err++;
			echo	$errstr,' (',$errno,')',"\n",
				$row,': ERR - ',$number,"\n";
		}
		else
		{
			$write .= 'Content-Length: '.$len."\r\n";
			$write .= 'Connection: close'."\r\n\r\n";
			$write .= $r;

    			fwrite($fp, $write);
			$recv = trim(fgets($fp,25));

			if(preg_match('/^HTTP\/1\.(x|1|0) 302 Found/',$recv) === 1)
			{
				$ok++;
				echo $row,': OK - ',$number,"\n";
			}
			else
			{
				$err++;
				echo $row,': ERR - ',$number,"\n";
			}

			fclose($fp);
		}

		$total++;

		sleep(1);
		$row++;
	}

	fclose($handle);

	echo	'----- USER RESULTS -----',"\n",
		'- OK: ',$ok,"\n",
		'- ERR: ',$err,"\n",
		'- TOTAL: ',$total,"\n",
		'----- USER RESULTS -----',"\n";

	die();

	function help()
	{
		echo	'-h',"\t\t",'hostname',"\n",
			'-p',"\t\t",'port',"\n",
			'-c',"\t\t",'csv file',"\n",
			'-s',"\t\t",'session id',"\n",
			'-x',"\t\t",'enable ssl',"\n";

		die();
	}

	function mk_query_str($a,$pre='',$key='',$sep='')
	{
		$r = '';
		$a = (array) $a;
		$sep = (string) $sep;
		$key = (string) $key;
		$sep = (string) $sep;

		if($sep === '')
			$sep = '&';

		if(($arr = xivo_get_aks($a)) === false)
			return($r);

		$r = array();
	
		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = &$arr['keys'][$i];
			$v = &$a[$k];

			$tmp = $pre !== '' && is_int($k) === true ? $pre.$k : $k;

			if($key !== '')
				$tmp = $key.'%5B'.$tmp.'%5D';

			if(is_array($v) === true || is_object($v) === true)
				$r[] = mk_query_str($v,'','',$sep);
			else
				$r[] = $tmp.'='.urlencode($v);
		}

		return(implode($sep,$r));
	}

	function xivo_get_aks(&$a,$empty=false)
	{
		$r = false;

		if(is_array($a) === false)
			return($r);

		$ak = array_keys($a);

		if(($cnt = count($ak)) > 0 || $empty === true)
		{
			$r = array(
				'keys' => $ak,
				'cnt' => $cnt);	
		}

		return($r);
	}
?>
