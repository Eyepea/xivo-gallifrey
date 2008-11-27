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


require_once("/etc/pf-asternic-stats/config.php");
include("sesvars.php");

if ($agent != "''") {
        $agents = explode(',', $agent);
        unset($agent);
        $agent = populate_agents($agents);
}

$data = array();

$db = sqlite_open('/var/lib/pf-xivo-cti-server/sqlite/xivo.db', 0666, $sqliteerror) or die ('Error DB : ' . $sqliteerror);



$nb = count($agent);
$agentlist = '';

if(is_array($agent))
{
  for($i = 0;$i < $nb;$i++) {
	if ($agent[$i][3] !== '')
		$agentlist .= ',\''.$agent[$i][3].'\'';
	}
}

if($agentlist !== '')
	$agentlist = substr($agentlist,1);
else
	die('Error no agent found !');

$query = sqlite_query($db,'SELECT * FROM ctilog WHERE action IN(\'cti_login\',\'cti_logout\',\'cticommand:availstate\') '.
			  'AND eventdate >= \''.$start.'\' AND eventdate <= \''.$end.'\' '.
			  'AND loginclient IN('.$agentlist.')'.
			  'ORDER BY loginclient ASC, eventdate ASC');
$res_login_logout_time = sqlite_fetch_all($query);

$query = sqlite_query($db,'SELECT * FROM ctilog WHERE action = \'cticommand:actionfiche\' '.
			  'AND eventdate >= \''.$start.'\' AND eventdate <= \''.$end.'\' '.
			  'ORDER BY loginclient ASC, eventdate ASC');
$res_event_stats = sqlite_fetch_all($query);

$loginclient = false;
$tmp = array();

$nb = count($res_login_logout_time);
$nbminus1 = $nb-1;

$datenow = mktime();

for ($llt=0;$llt<$nb;$llt++)
{
	$ref = &$res_login_logout_time[$llt];

	if(isset($data[$ref['loginclient']]) === false)
		$data[$ref['loginclient']] = array('connect'	=> array(),
						   'total'	=> 0,
						   'cnt'	=> 0,
						   'cti_login'	=> 0,
						   'cti_logout'	=> 0);

	if(isset($tmp[$ref['loginclient']]) === false)
		$tmp[$ref['loginclient']] = array();

	$refconnect = &$data[$ref['loginclient']]['connect'];
	$reftmp = &$tmp[$ref['loginclient']];

	if(isset($refconnect[$ref['status']]) === false)
		$refconnect[$ref['status']] = array('total'	=> 0,
						    'cnt'	=> 0);

	if(isset($reftmp['laststate']) === true
	&& $ref['status'] !== $reftmp['laststate'])
	{
		$calc = calc_duration(strtotime($reftmp['lastdate']),strtotime($ref['eventdate']));

		$refconnect[$reftmp['laststate']]['total'] += $calc['diff'];

		if($reftmp['laststate'] !== 'xivo_unknown')
			$data[$ref['loginclient']]['total'] += $calc['diff'];
	}

	if($ref['action'] === 'cti_login' || $ref['action'] === 'cti_logout')
		$data[$ref['loginclient']][$ref['action']]++;

	$refconnect[$ref['status']]['cnt']++;

	if($ref['status'] !== 'xivo_unknown')
		$data[$ref['loginclient']]['cnt']++;

	$tmp[$ref['loginclient']]['lastdate'] = $ref['eventdate'];
	$tmp[$ref['loginclient']]['laststate'] = $ref['status'];

	if($loginclient !== false && $loginclient !== $ref['loginclient'])
		$consolid = true;
	else if($llt === $nbminus1)
	{
		$consolid = true;
		$loginclient = $ref['loginclient'];
	}
	else
		$consolid = false;

	if($consolid === true)
	{
		$calc = calc_duration(strtotime($tmp[$loginclient]['lastdate']),$datenow);
		$data[$loginclient]['connect'][$tmp[$loginclient]['laststate']]['total'] += $calc['diff'];

		if($tmp[$loginclient]['laststate'] !== 'xivo_unknown')
			$data[$loginclient]['total'] += $calc['diff'];
	}

	$loginclient = $ref['loginclient'];
}

sqlite_close($db);


?>
<!-- http://devnull.tagsoup.com/quirksmode -->
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <title>Asternic Call Center Stats</title>
    <style type="text/css" media="screen">@import "css/basic.css";</style>
    <style type="text/css" media="screen">@import "css/tab.css";</style>
    <style type="text/css" media="screen">@import "css/table.css";</style>
    <style type="text/css" media="screen">@import "css/fixed-all.css";</style>
    <script type="text/javascript" src="js/flashobject.js"></script>
    <script type="text/javascript" src="js/sorttable.js"></script>

<!--[if gte IE 5.5000]>
<style type='text/css'> img { behavior:url(pngbehavior.htc) } </style>
<![endif]-->

<!--[if IE]>
<link 
 href="css/fixed-ie.css" 
 rel="stylesheet" 
 type="text/css" 
 media="screen"> 
<script type="text/javascript"> 
onload = function() { content.focus() } 
</script> 
<![endif]-->
</head>
<body>
<? include("menu.php"); ?>
<div id="main">
<div id="contents">

<TABLE width='99%' cellpadding=3 cellspacing=3 border=0>
<THEAD>
<CAPTION>Informations des clients XiVO</CAPTION>
<TBODY>
<TR>
	<TD>Nombre total de client loggué :</TD>
	<TD><?=$res_total_login?></TD>
</TR>
	<TD>Nombre total de client déloggué :</TD>
	<TD><?=$res_total_logout?></TD>
</TR>
</THEAD>
</TABLE>

<br><br>

<TABLE width='99%' cellpadding=1 cellspacing=1 border=0 class='sortable' id='table2'>
<CAPTION>
	Stats pour XiVO client
</CAPTION>

<THEAD>
<TR>
<TH>Utilisateurs</TH>


<?

$header = array();
foreach($data as $k => $v)
{

	foreach($v as $q => $z)
	{
		if(is_array($z) === false)
			continue;

		foreach($z as $m => $n)
		{
			if($m !== 'xivo_unknown')
			{
				if(!in_array($m, $header)) {
					echo "<TH colspan=2>$m</TH>";
					$header[] = $m;
				}
			}
		}
	}

}

?>

<TH colspan=2>Total</TH>
</TR>
</THEAD>

<?

$count_header = count($header);
foreach($data as $k => $v)
{
	echo "<TR>";
	echo "<TD>$k</TD>";
	$c = 0;
	foreach($v as $q => $z)
	{
		if(is_array($z) === false)
			continue;

		foreach($z as $m => $n)
		{
			if($m !== 'xivo_unknown')
			{
				echo "<TD style='text-align: right'>" . print_human_hour($n['total']) . "</TD>\n";
				echo "<TD style='text-align: right'>" . print_human_hour($n['cnt']) . "</TD>\n";
				$c = $c+1;
			}
		}
	}

	$header_diff = $count_header-$c;
	for($h=0;$h<$header_diff;$h++)
		echo "<TD></TD><TD></TD>";
	echo "<TD style='text-align: right'>" . print_human_hour($v['total']) . "</TD>";
	echo "<TD style='text-align: right'>" . print_human_hour($n['cnt']) . "</TD>";
	echo "</TR>";
}
?>

</TABLE>

<br>

<? print_exports($header_pdf,$data_pdf,$width_pdf,$title_pdf,$cover_pdf); ?>

<br><br>

<!--TABLE width='99%' cellpadding=1 cellspacing=1 border=0 class='sortable' id='table2'>
<CAPTION>
	Donnée post appel
</CAPTION>
<TBODY>

<?

foreach($res_event_stats as $stats)
{
	echo "<TR $odd>\n";
	echo "<TD>". $stats['eventdate'] ."</TD>\n";
	echo "<TD>". $stats['loginclient'] ."</TD>\n";
	echo "<TD>". $stats['status'] ."</TD>\n";
	echo "<TD>". $stats['arguments'] ."</TD>\n";
	echo "</TR>\n";
}

?>

</TBODY>
</TABLE>

<br-->

<?php
/***********************************************************************
 * ADD BY CEDRIC
*/

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


$lscalltype = array();
$countallcalltype = array();

for($nbStatus=1;$nbStatus<6;$nbStatus++)
{
	$lscalltype['XIVO_CALL_STATUS-'.$nbStatus] = 'XIVO_CALL_STATUS-'.$nbStatus; 
	$countallcalltype['XIVO_CALL_STATUS-'.$nbStatus] = 0;
}

$callinfos = array();
$userlist = array();
$totalcallduration = array();
$countAllCallTypeUser = array();

foreach ($res_event_stats as $stats)
{
	$stats = array_merge($stats, array('callduration' => rand(180, 1080)));
	$userlist[$stats['loginclient']][$lscalltype[$stats['arguments']]]['countcalltype']++;
	$userlist[$stats['loginclient']][$lscalltype[$stats['arguments']]]['callduration'] = $stats['callduration'];
	$countallcalltype[$lscalltype[$stats['arguments']]]++;
	$countAllCallTypeUser[$stats['loginclient']]++;		
	ksort($userlist[$stats['loginclient']]);
}

?>

<TABLE width='99%' cellpadding=1 cellspacing=1 border=0 class='sortable' id='table2'>
<CAPTION>
	Données
</CAPTION>

<THEAD>
<TR>
<TH>Utilisateurs</TH>
<?php

foreach($countallcalltype as $calltype => $value)
{
	echo "<TH>$calltype</TH>\n";
}

?>
<TH>Total</TH>
</TR>
</THEAD>

<TBODY>
<?php

foreach($userlist as $user => $value)
{
	echo "<TR $odd>\n";
	echo "<TD>". $user ."</TD>\n";

foreach($value as $type => $data)
{
	$prc = round(($data['countcalltype']/$countAllCallTypeUser[$user])*100, 2);
	echo "<TD>".$data['countcalltype']." - ".seconds2minutes($data['callduration']/count($data['countcalltype']))."min - ".$prc."%</TD>\n";
	$totalcallduration[$user] += $data['callduration'];
	$totalcallduration[$type] += $data['callduration'];
}

	echo "<TD style='font-weight:bold'>".$countAllCallTypeUser[$user]." - ".seconds2minutes($totalcallduration[$user]/count($lscalltype))."min</TD>\n";
	echo "</TR>\n";
	$totalallcallduration += $totalcallduration[$user];
}

?>

<TR style="font-weight:bold">
<?php

	echo "<TD>Nb Agent: ". count($userlist) ."</TD>\n";
foreach($countallcalltype as $calltype => $value)
{
	$prc = round(($value/array_sum($countallcalltype))*100, 2);
	echo "<TD>$value - ".seconds2minutes($totalcallduration[$calltype]/count($userlist))."min - ".$prc."%</TD>\n";
}
	echo "<TD>".array_sum($countallcalltype)." - ".seconds2minutes($totalallcallduration/count($lscalltype)/count($userlist)) ."min</TD>\n";

?>
</TR>
</TBODY>
</TABLE>
<?php
/***********************************************************************
*/
?>
<br>
<? print_exports($header_pdf,$data_pdf,$width_pdf,$title_pdf,$cover_pdf); ?>

<br><br>
<TABLE width='99%' cellpadding=1 cellspacing=1 border=0>
<CAPTION>
	Graphiques
</CAPTION>

<TR align=center>
<TD>
<?
$tmp=array();
$i=1;
foreach($countallcalltype as $calltype => $value)
{
	array_push($tmp, "var$i=".substr($calltype,-8 ,8)."&amp;val$i=$value");
	$i++;
}
?>
	<div id="chart1">
	<embed type="application/x-shockwave-flash" src="bar.swf" id="barchart" name="barchart" bgcolor="#336699" quality="high" wmode="transparent" flashvars="<? echo implode ('&amp;', $tmp); ?>&amp;title=Nombre d'appels répondu par status&amp;bgcolor=0xF0ffff&amp;bgcolorchart=0xdfedf3&amp;fade1=ff6600&amp;fade2=ff6314&amp;colorbase=0xfff3b3&amp;reverse=1" width="359" height="217">
	</div>
</TD>
<TD>
<?
$tmp=array();
$i=1;
foreach($userlist as $user => $value)
{
	array_push($tmp, "var$i=".$user."&amp;val$i=$countAllCallTypeUser[$user]");
	$i++;
}
?>
	<div id="chart2">
	<embed type="application/x-shockwave-flash" src="bar.swf" id="barchart" name="barchart" bgcolor="#336699" quality="high" wmode="transparent" flashvars="<? echo implode ('&amp;', $tmp); ?>&amp;title=Nombre d'appels répondu par agent&amp;bgcolor=0xF0ffff&amp;bgcolorchart=0xdfedf3&amp;fade1=ff6600&amp;fade2=ff6314&amp;colorbase=0xfff3b3&amp;reverse=1" width="359" height="217">
	</div>
</TD>
</TR>
<TR align=center>
<TD>
<?
$tmp=array();
$i=1;
foreach($countallcalltype as $calltype => $value)
{
	array_push($tmp, "var$i=".substr($calltype,-8 ,8)."&amp;val$i=".$totalcallduration[$calltype]/count($userlist));
	$i++;
}
?>
	<div id="chart3">
	<embed type="application/x-shockwave-flash" src="bar.swf" id="barchart" name="barchart" bgcolor="#336699" quality="high" wmode="transparent" flashvars="<? echo implode ('&amp;', $tmp); ?>&amp;title=Temps moyen par type&amp;bgcolor=0xF0ffff&amp;bgcolorchart=0xdfedf3&amp;fade1=ff6600&amp;fade2=ff6314&amp;colorbase=0xfff3b3&amp;reverse=1" width="359" height="217">
	</div>
</TD>
<TD>
<?
$tmp=array();
$i=1;
foreach($userlist as $user => $value)
{
	array_push($tmp, "var$i=".$user."&amp;val$i=".$totalcallduration[$user]/count($lscalltype));
	$i++;
}
?>
	<div id="chart4">
	<embed type="application/x-shockwave-flash" src="bar.swf" id="barchart" name="barchart" bgcolor="#336699" quality="high" wmode="transparent" flashvars="<? echo implode ('&amp;', $tmp); ?>&amp;title=Temps moyen par agent&amp;bgcolor=0xF0ffff&amp;bgcolorchart=0xdfedf3&amp;fade1=ff6600&amp;fade2=ff6314&amp;colorbase=0xfff3b3&amp;reverse=1" width="359" height="217">
	</div>
</TD>
</TR>
</TABLE>

</div>
</div>
</body>
</html>
