#! /usr/bin/php -q
<?php
	/**
    Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
		along with this program.  If not, see <http://www.gnu.org/licenses/>.
	*/

	if(($opt = getopt('h:p:c:r')) === false
	|| isset($opt['h'],$opt['p'],$opt['c']) === false)
		help();

	$sockhost = $opt['h'];
		
	if(strtolower(substr($opt['c'],-4)) === '.csv')
		$csv = $opt['c'];
	else
		$csv = $opt['c'].'.csv';

	$out = 'POST /prov HTTP/1.1'."\r\n";
	$out .= 'Host: '.$opt['h']."\r\n";
	$out .= 'Keep-Alive: 300'."\r\n";
	$out .= 'Connection: keep-alive'."\r\n";
	$out .= 'Cache-Control: max-age=0'."\r\n";

	$row = 1;

	$total = $ok = $err = 0;

	$handle = fopen($csv,'r');

	$r = array();
	$r['mode']    = 'mode=notification';
	$r['from']    = 'from=webi';
	$r['actions'] = 'actions=' . (isset($opt['r'])?'yes':'no');
	$r['proto']   = 'proto=sip';

	while(($data = fgetcsv($handle,1000,',')) !== false)
	{
		$num = count($data);
		$error = false;

		if(isset($data[0]) === true
		&& ($iduserfeatures = trim($data[0])) !== ''
		&& ctype_digit($iduserfeatures) === true)
			$r['iduserfeatures'] = 'iduserfeatures='.$iduserfeatures;
		else
			$error = 'iduserfeatures';

		if(isset($data[1]) === true)
		{
			$macaddr = trim($data[1]);

			if(empty($macaddr) === true)
				$error = 'macaddr';
			else
				$r['macaddr'] = 'macaddr='.$macaddr;
		}
		else
			$error = 'macaddr';

		if($error !== false)
		{
			$err++;
			echo	$row,': ERR - ',$error,"\n";
			$row++;
			$total++;
			continue;
		}

		$str = implode("\r\n",$r)."\r\n";

		$len = strlen($str);

		$errno = 0;
		$errstr = '';

		$fp = fsockopen($sockhost, $opt['p'], $errno, $errstr, 60);

		$write = $out;

		if($fp === false)
		{
			$err++;
			echo	$errstr,' (',$errno,')',"\n",
				$row,': ERR - ',$iduserfeatures,"\n";
		}
		else
		{
			$write .= 'Content-Length: '.$len."\r\n";
			$write .= 'Connection: close'."\r\n\r\n";
			$write .= $str;

 			fwrite($fp, $write);
			$recv = trim(fgets($fp,32));

			if(preg_match('/^HTTP\/1\.(x|1|0) 200 OK/',$recv) === 1)
			{
				$ok++;
				echo $row,': OK - ',$iduserfeatures,"\n";
			}
			else
			{
				$err++;
				echo $row,': ERR - ',$iduserfeatures,"\n";
			}

			fclose($fp);
		}

		$total++;

		sleep(1);
		$row++;
	}

	fclose($handle);

	echo	'----- REAPPROV RESULTS -----',"\n",
		'- OK: ',$ok,"\n",
		'- ERR: ',$err,"\n",
		'- TOTAL: ',$total,"\n",
		'----- REAPPROV RESULTS -----',"\n";

	die();

	function help()
	{
		echo	'-h',"\t\t",'hostname',"\n",
			'-p',"\t\t",'port',"\n",
			'-c',"\t\t",'csv file (format: idusefeatures,macaddr)',"\n",
			'-r',"\t\t",'reboot phones',"\n";

		die();
	}

?>
