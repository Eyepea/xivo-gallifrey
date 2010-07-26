<?
/*
   This file is part of Asternic call center stats.

    Asternic call center stats is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Asternic call center stats is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Asternic call center stats.  If not, see <http://www.gnu.org/licenses/>.
*/

function return_timestamp($date_string)
{
  list ($year,$month,$day,$hour,$min,$sec) = preg_split("/-|:| /",$date_string,6);
  $u_timestamp = mktime($hour,$min,$sec,$month,$day,$year);
  return $u_timestamp;
}

function debug($array) {
	print '<pre>';
	print_r($array);
	print '</pre>';
}

function Sdump($var, $title = null) {

	echo '<pre style="border: 1px solid gray; padding: 5px; text-align: left;">';
	
	if (!is_null($title)):
	
		echo '<h3>', htmlspecialchars($title), '</h3>';
	
	endif;
	
	ob_start();
	
	var_dump($var);
	
	echo htmlspecialchars(preg_replace("/\]\=\>\n(\s+)/m", '] => ', ob_get_clean()));
	
	echo '</pre>';

}

function print_human_hour($sec) {
	$sec = calc_duration(false,false,$sec);

	$res = '';
	if($sec['d'] != 0)
		$res .= $sec['d'] . 'j ';
	if($sec['h'] != 0)
		$res .= $sec['h'] . 'h ';
	if($sec['m'] != 0)
		$res .= $sec['m'] . 'm ';
	if($sec['s'] != 0)
		$res .= round($sec['s'], 0) . 's';
		
		
	if($sec['s'] == 0)
		$res = 0;
	
	return $res;
}

function draw_bar($values,$width,$height,$divid,$stack) {
	print "<img src=\"pie.php?$values&width=$width&height=$height\" alt=\"Graph\">";
}

function get_xivo_json_info($xivo_url) {
	$buffer = '';
	$datafile = fopen($xivo_url , "r");
	  if ($datafile) {
	    while (!feof($datafile)) {
	       $buffer .= fgets($datafile, 4096);
	    } 
	    fclose($datafile);
	  } else {
	    die( "fopen failed!" ) ;
	  }

	return(json_decode($buffer));
}

function populate_agents($agents_array) {
	// Get informations from XiVO in JSON
	$xivo_user = get_xivo_json_info("https://127.0.0.1/service/ipbx/json.php/private/pbx_settings/users");
	$xivo_agent = get_xivo_json_info("https://127.0.0.1/service/ipbx/json.php/private/call_center/agents");

	$agent = array();

	$c = count($agents_array);

	for ($a=0;$a<$c;$a++) {
		$agents_array[$a] = str_replace('\'', '', $agents_array[$a]);
		list($type, $num) = split('/', $agents_array[$a]);

		if ($type === 'Agent'
		&& ctype_digit($num)
		&& ($r = array_search_recursive($num, 'number', $xivo_agent))) {
			$t = array_search_recursive($r->id, 'agentid', $xivo_user);
			$agent[] = array('interface'	=> $agents_array[$a],
					 'fullname'	=> $r->firstname . ' ' . $r->lastname,
					 'agentid'	=> $r->id,
					 'loginclient'	=> $t->loginclient);
		} else {
			$agent[] = array('interface'	=> $agents_array[$a],
					 'fullname'	=> $agents_array[$a],
					 'loginclient'	=> '',
					 'agentid'	=> 0);
		}
	}

	return $agent;
}

function array_search_recursive($text, $find, $array) {
	for ($a=0;$a<count($array);$a++) {
		if ($array[$a]->$find == $text)
			return $array[$a];
	}
}

function calc_duration($beg,$end,$diff=false,$unset=false)
{
	$r = array();

	if(is_numeric($diff) === true)
		$r['diff'] = $diff;
	else if(is_numeric($beg) === false
	|| is_numeric($end) === false)
		return(false);
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

	ksort($r);

	if((bool) $unset === false)
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


function swf_bar($values,$width,$height,$divid,$stack) {

	if($stack==1) {
		$chart = "barstack.swf";
	} else {
		$chart = "bar.swf";
	}

?>
<div id="<?=$divid?>">
<?=$values?>
</div>

<script type="text/javascript">
   var fo = new FlashObject("<?=$chart?>", "barchart", "<?=$width?>", "<?=$height?>", "7", "#336699");
   fo.addParam("wmode", "transparent");
//   fo.addParam("salign", "t");
	<?
		$variables = split("&",$values);
		foreach ($variables as $deauna) {
			echo "//$deauna\n";
			$pedazos = split("=",$deauna);
			echo "fo.addVariable('".$pedazos[0]."','".$pedazos[1]."');\n";
		}
	?>
   fo.write("<?=$divid?>");
</script>

<?
}

function tooltip($texto,$width) {
 echo " onmouseover=\"this.T_WIDTH=$width;this.T_PADDING=5;this.T_STICKY = false; return escape('$texto')\" ";
}


function print_exports($header_pdf,$data_pdf,$width_pdf,$title_pdf,$cover_pdf) {
		global $lang;
		global $language;
		$head_serial = serialize($header_pdf);
		$data_serial = serialize($data_pdf);
		$width_serial = serialize($width_pdf);
		$title_serial = serialize($title_pdf);
		$cover_serial = serialize($cover_pdf);
		$head_serial = rawurlencode($head_serial);
		$data_serial = rawurlencode($data_serial);
		$width_serial = rawurlencode($width_serial);
		$title_serial = rawurlencode($title_serial);
		$cover_serial = rawurlencode($cover_serial);
		echo "<BR><form method=post action='export.php'>\n";
		echo $lang["$language"]['export'];
		echo "<input type='hidden' name='head' value='".$head_serial."' />\n";
		echo "<input type='hidden' name='rawdata' value='".$data_serial."' />\n";
		echo "<input type='hidden' name='width' value='".$width_serial."' />\n";
		echo "<input type='hidden' name='title' value='".$title_serial."' />\n";
		echo "<input type='hidden' name='cover' value='".$cover_serial."' />\n";
		#echo "<input type=image name='pdf' src='images/pdf.gif' ";
		#tooltip($lang["$language"]['pdfhelp'],200);
		#echo ">\n";
		echo "<input type=image name='csv' src='images/excel.gif' "; 
		tooltip($lang["$language"]['csvhelp'],200);
		echo ">\n";
		echo "</form>";
}

function seconds2minutes($segundos) {
    $minutos = intval($segundos / 60);
    $segundos = $segundos % 60;
    if(strlen($segundos)==1) {
		$segundos = "0".$segundos;
	}
    return "$minutos:$segundos";
}
?>
