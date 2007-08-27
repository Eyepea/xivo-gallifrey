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

	$arr_number = array();

	$handle = fopen($csv,'r');

	while(($data = fgetcsv($handle,1000,',')) !== false)
	{
		$number = trim($data[0]);
		$did = trim($data[1]);

		if($number === '' || $did === '')
			continue;

		$arr_number[$number] = $did;
	}

	fclose($handle);

	$arr_did = array();

	$array = array();
	$array['extenumbers'] = array();

	$array['dfeatures'] = array();
	$array['dfeatures']['type'] = 'user';

	$out = 'GET /service/ipbx/index.php/call_management/did/?act=add&_eid='.$opt['s'].' HTTP/1.1'."\r\n";
	$out .= 'Host: '.$opt['h']."\r\n";
	$out .= 'Connection: close'."\r\n\r\n";

	$fp = fsockopen($sockhost, $opt['p'], $errno, $errstr, 60);

	if($fp === false)
		echo $errstr,' (',$errno,')',"\n";
	else
	{
		$get = '';

		fwrite($fp, $out);

		while(!feof($fp))
			$get .= fgets($fp, 4096);

		if(preg_match('#<label id="lb-dfeatures-user-typeid" for="it-dfeatures-user-typeid">.*</label>#smU',$get,$get_option) === 1
		&& preg_match_all('/<option value="([0-9]+)">([0-9]+)(@[a-z-_]+)?<\/option>/',$get_option[0],$match) !== 0)
		{
			$nb = count($match[2]);

			for($i = 0;$i < $nb;$i++)
			{
				if(isset($arr_number[$match[2][$i]]) === false)
					continue;

				$arr_did[] = array('number' => $arr_number[$match[2][$i]],'typeid' => $match[1][$i]);
			}
		}

		fclose($fp);
	}

	if(($nb = count($arr_did)) === 0)
		die('No did found');

	$row = 1;

	$out = 'POST /service/ipbx/index.php/call_management/did/?act=add&_eid='.$opt['s'].' HTTP/1.1'."\r\n";
	$out .= 'Host: '.$opt['h']."\r\n";
	$out .= 'Keep-Alive: 300'."\r\n";
	$out .= 'Connection: keep-alive'."\r\n";
	$out .= 'Cookie: _eid='.$opt['s']."\r\n";
	$out .= 'Cache-Control: max-age=0'."\r\n";
	$out .= 'Content-Type: application/x-www-form-urlencoded'."\r\n";

	$total = $ok = $err = 0;

	for($i = 0;$i < $nb;$i++)
	{
		$array['extenumbers']['number'] = $arr_did[$i]['number'];
		$array['dfeatures']['typeid'] = $arr_did[$i]['typeid'];

		$r = 'act=add&fm_send=1';
		$r .= '&'.mk_query_str($array['extenumbers'],'','extenumbers');
		$r .= '&'.mk_query_str($array['dfeatures'],'','dfeatures');

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
				echo $row,': OK - ',$arr_did[$i]['number'],"\n";
			}
			else
			{
				$err++;
				echo $row,': ERR - ',$arr_did[$i]['number'],"\n";
			}

			fclose($fp);
		}

		$total++;

		sleep(1);
		$row++;
	}

	echo	'----- DID RESULTS -----',"\n",
		'- OK: ',$ok,"\n",
		'- ERR: ',$err,"\n",
		'- TOTAL: ',$nb,"\n",
		'----- DID RESULTS -----',"\n";

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
