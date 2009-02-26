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
else
	$agent = array();

$data = array();

$nb = count($agent);
$agentlist = '';

for($i = 0;$i < $nb;$i++) {
        if ($agent[$i]['loginclient'] !== '')
               $agentlist .= ',\''.$agent[$i]['loginclient'].'\'';
}

if($agentlist !== '')
        $agentlist = substr($agentlist,1);
else
       die('Error no agent found !');

###################################################################################################
############################# SQLITE #########
if ($cti_db_config == 'sqlite') {

	$db = sqlite_open('/var/lib/pf-xivo-cti-server/sqlite/xivo.db', 0666, $sqliteerror) or die ('Error DB : ' . $sqliteerror);

	$query = sqlite_query($db,'SELECT * FROM ctilog WHERE action IN(\'cti_login\',\'cti_logout\',\'cticommand:availstate\') '.
                      'AND eventdate >= \''.$start.'\' AND eventdate <= \''.$end.'\' '.
                      'AND loginclient IN('.$agentlist.') '.
                      'ORDER BY loginclient ASC, eventdate ASC');
	$res_login_logout_time = sqlite_fetch_all($query);


	$query = sqlite_query($db,'SELECT * FROM ctilog WHERE action = \'cticommand:actionfiche\' '.
                      'AND eventdate >= \''.$start.'\' AND eventdate <= \''.$end.'\' '. # AND arguments != \'answer\' '.
                      'AND loginclient IN('.$agentlist.') '.
                      'ORDER BY loginclient ASC, eventdate ASC');
	$res_event_stats = sqlite_fetch_all($query);

} elseif ($cti_db_config == 'mysql') {

	$mysqlconnect = mysql_connect("localhost", "xivo", "proformatique") or die("connect impossible : " . mysql_error());

	$db = mysql_select_db('xivo', $mysqlconnect);
	if (!$db) {
   		die ('connect bdd impossible : ' . mysql_error());
	}

	$query = mysql_query('SELECT * FROM ctilog WHERE action IN(\'cti_login\',\'cti_logout\',\'cticommand:availstate\') '.
			  'AND eventdate >= \''.$start.'\' AND eventdate <= \''.$end.'\' '.
			  'AND loginclient IN('.$agentlist.') '.
			  'ORDER BY loginclient ASC, eventdate ASC');

	$tmp = array();

	while ($row = mysql_fetch_assoc($query)) {
		array_push($tmp, $row);
	}

	$res_login_logout_time = $tmp;

	mysql_free_result($query);

	$query = mysql_query('SELECT * FROM ctilog WHERE action = \'cticommand:actionfiche\' '.
			  'AND eventdate >= \''.$start.'\' AND eventdate <= \''.$end.'\' '. # AND arguments != \'answer\' '.
			  'AND loginclient IN('.$agentlist.') '.
			  'ORDER BY loginclient ASC, eventdate ASC');

	$tmp = array();

	while ($row = mysql_fetch_assoc($query)) {
        	array_push($tmp, $row);
	}

	$res_event_stats = $tmp;

	mysql_free_result($query);

} else {
	die('Aucun type de DB choisi.');
}

$loginclient = false;
$tmp = array();

$nb = count($res_login_logout_time);
$nbminus1 = $nb-1;

$datenow = mktime();

$agent_fullname = array();

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

	if(isset($agent_fullname[$ref['loginclient']]) === false)
	{
		foreach($agent as $value)
		{
			if($ref['loginclient'] === $value['loginclient'])
			{
				$agent_fullname[$ref['loginclient']] = $value['fullname'];
				break;
			}
		}
	}

	$loginclient = $ref['loginclient'];
}

mysql_close($mysqlconnect);


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

<TABLE width='99%' cellpadding=1 cellspacing=1 border=0 class='sortable' id='table2'>
<CAPTION>
	Stats pour XiVO client
</CAPTION>

<THEAD>
<TR>
<TH>Utilisateurs</TH>

<?

$header = array();
$translation = array(
			'postcall'	=>	'Post Appel',
			'onlineincoming'	=>	'En Ligne appel entrant',
			'available'	=>	'Disponible',	
			'away'		=>	'Absent',
			'backoffice'	=>	'Back Office',
			'pause'		=>	'Pause',
			'fastpickup'	=>	'Décroche rapide',			
			'berightback'	=>	'Bientôt de retour',
			'donotdisturb'	=>	'Ne pas déranger',
			'outtolunch'	=>	'Parti manger',
			'onlineoutgoing'	=>	'En ligne appel sortant',	
);

$header_pdf=array('Utilisateurs');

foreach($data as $k => $v)
{

	foreach($v as $q => $z)
	{
		if(is_array($z) === false)
			continue;

		foreach($z as $m => $n)
		{
			if($m === 'xivo_unknown' || in_array($m, $header))
				continue;

			#echo "<TH colspan=2>$m</TH>";
			$header[] = $m;
			if (array_key_exists($m, $translation)) {
				$translation_header_pdf[] = $translation[$m];
				echo "<TH colspan=2>".$translation[$m]."</TH>";
			} else {
				$translation_header_pdf[] = $m;	
				echo "<TH colspan=2>$m</TH>";
			}
			array_push($header_pdf, $translation[$m]);
			array_push($header_pdf, '');
		}
	}
}

?>

<TH colspan=2>Total</TH>
</TR>
</THEAD>

<?
array_push($header_pdf, 'Total');
array_push($header_pdf, '');
$width_pdf=array();
$title_pdf='Stats pour XiVO client';

$count_header = count($header);
foreach($data as $k => $v)
{
	$linea_pdf = array($agent_fullname[$k]);
	echo "<TR>";
	echo "<TD>$agent_fullname[$k]</TD>";
	$c = 0;
	foreach($v as $q => $z)
	{

		if(is_array($z) === false)
			continue;
		
		foreach($z as $m => $n)
		{
			if($m !== 'xivo_unknown')
			{
				$n = $z[$header[$c]];		
				$c++;
				echo "<TD style='text-align: right'>" . print_human_hour($n['total']) . "</TD>\n";
				echo "<TD style='text-align: right'>" . $n['cnt'] . "</TD>\n";
				$total_cnt[$k] += $n['cnt'];
				$total_h[$k] += $n['total'];
				array_push($linea_pdf, print_human_hour($n['total']));
				array_push($linea_pdf, $n['cnt']);
			}
		}
	}
	$header_diff = $count_header-$c;
	for($h=0;$h<$header_diff;$h++)
	{
		echo "<TD></TD><TD></TD>";
		array_push($linea_pdf, '');
	}
	echo "<TD style='text-align: right'>" . print_human_hour($total_h[$k]) . "</TD>";
	echo "<TD style='text-align: right'>" . $total_cnt[$k] . "</TD>";
	echo "</TR>";
	array_push($linea_pdf, print_human_hour($total_h[$k]));
	array_push($linea_pdf, $total_cnt[$k]);
	$data_pdf[]=$linea_pdf;
}
?>

</TABLE>

<br>

<? print_exports($header_pdf,$data_pdf,$width_pdf,$title_pdf,$cover_pdf); ?>
<? unset($header_pdf); unset($data_pdf); ?>
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

$lscalltype = array(	'XIVO_CALL_STATUS-1' => 'Partenaire', 
			'XIVO_CALL_STATUS-2' => 'Client', 
			'XIVO_CALL_STATUS-3' => 'Prospect Infos', 
			'XIVO_CALL_STATUS-4' => 'Prospect Accueil', 
			'XIVO_CALL_STATUS-5' => 'Hors Cible'
		);
$countallcalltype = array();
foreach($lscalltype as $calltype => $name)
{ 
	$countallcalltype[$calltype] = 0;
}

$callinfos = array();
$userlist = array();
$totalcallduration = array();
$countAllCallTypeUser = array();

foreach ($res_event_stats as $stats)
{

	if ($stats['arguments'] == 'answer' 
	or $stats['arguments'] == 'hangup'): continue; endif;
	if (!isset($userlist[$stats['loginclient']])):	
	foreach($lscalltype as $calltype => $name)
	{
	        $userlist[$stats['loginclient']][$calltype] = array(
								'countcalltype' => 0,
								'callduration' => 0
								);
	}
	endif;
	$userlist[$stats['loginclient']][$stats['arguments']]['countcalltype']++;
	$userlist[$stats['loginclient']][$stats['arguments']]['callduration'] = $stats['callduration'];
	$countallcalltype[$stats['arguments']]++;
	$countAllCallTypeUser[$stats['loginclient']]++;		
	ksort($userlist[$stats['loginclient']]);

	if(isset($agent_fullname[$stats['loginclient']]) === true)
		continue;
	else
	{
		foreach($agent as $value)
		{
			if($stats['loginclient'] === $value['loginclient'])
			{
				$agent_fullname[$stats['loginclient']] = $value['fullname'];
				break;
			}
		}
	}
}

?>

<TABLE width='99%' cellpadding=1 cellspacing=1 border=0 class='sortable' id='table2'>
<CAPTION>
	Données par type d'appel
</CAPTION>

<THEAD>
<TR>
<TH>Utilisateurs</TH>
<?php

foreach($lscalltype as $calltype => $name)
{
	echo "<TH colspan=3>$name</TH>\n";
}

?>
<TH>Total</TH>
</TR>
</THEAD>

<TBODY>
<?php

$header_pdf=array('Utilisateurs');
$header_pdf = array_merge($header_pdf, $lscalltype); 
array_push($header_pdf, 'Total');
$width_pdf=array();
$title_pdf='Données par type d\'appel';

foreach($userlist as $user => $value)
{
	$linea_pdf = array($user);
	echo "<TR $odd>\n";
	echo "<TD>". $agent_fullname[$user] ."</TD>\n";
	
	$userProcess = $user;

	foreach($value as $type => $data)
	{		
		
		if ($user == $userProcess && $data['countcalltype'] > 0) {
			$actifUser[$type]++; 
		}

		$prc = round(($data['countcalltype']/$countAllCallTypeUser[$user])*100, 2);
		echo "
		<td>".$data['countcalltype']."</td> 
		<td>".print_human_hour($data['callduration']/count($data['countcalltype']))."</td>
		<td>".$prc."%</td>
		\n";
		$totalcallduration[$user] += $data['callduration'];
		$totalcallduration[$type] += $data['callduration'];
		
		array_push($linea_pdf, $data['countcalltype']." - ".print_human_hour($data['callduration']/count($data['countcalltype'])) . '  (' . $prc.'%)');
	}


	echo "<TD style='font-weight:bold'><span style='float:left'>".$countAllCallTypeUser[$user]."</TD>\n";
	echo "</TR>\n";
	$totalallcallduration += $totalcallduration[$user];
	array_push($linea_pdf, $countAllCallTypeUser[$user] . '   (' . print_human_hour($totalcallduration[$user]/count($lscalltype)).')');
	$data_pdf[]=$linea_pdf;
}
?>
<TR style="font-weight:bold">
<?php
        $linea_pdf = array("Total appels:");
        echo "<TD>Total appels:</TD>\n";
	foreach($lscalltype as $calltype => $name)
	{
	echo "<TD colspan=3>".$countallcalltype[$calltype]."</TD>";
        array_push($linea_pdf, $countallcalltype[$calltype]);
	}
        echo "<TD>".array_sum($countallcalltype)."</TD>\n";
        array_push($linea_pdf, array_sum($countallcalltype));
        $data_pdf[]=$linea_pdf;
?>
</TR>

<TR style="font-weight:bold">
<?php

        $linea_pdf = array("Moy. temps d'appels:");
	echo "<TD>Moy. temps d'appels:</TD>\n";
	foreach($lscalltype as $calltype => $name)
	{
	        echo "<TD colspan=3>".print_human_hour(round($totalcallduration[$calltype]/$actifUser[$calltype]))."</TD>";
	        array_push($linea_pdf, print_human_hour(round($totalcallduration[$calltype]/$actifUser[$calltype])));
        }
        //echo "<TD>".print_human_hour(round($totalallcallduration/count($lscalltype)/count($userlist)))."</TD>\n";
	echo "<TD></TD>";
        array_push($linea_pdf, print_human_hour(round($totalallcallduration/count($lscalltype)/count($userlist))));
        $data_pdf[]=$linea_pdf;
?>
</TR>

<TR style="font-weight:bold">
<?php
	$linea_pdf = array('Pourcentage d\'appels:');
	echo "<TD>Pourcentage d'appels: </TD>\n";
	foreach($lscalltype as $calltype => $name)
	{
	$prc = round(($countallcalltype[$calltype]/array_sum($countallcalltype))*100, 2);
	echo "<TD colspan=3>".$prc."%</TD>\n";
	array_push($linea_pdf, $prc.'%');
	}
	echo "<TD>100%</TD>\n";
	array_push($linea_pdf, '100%');
	$data_pdf[]=$linea_pdf;

?>
</TR>
</TBODY>
</TABLE>

<br>
<? print_exports($header_pdf,$data_pdf,$width_pdf,$title_pdf,$cover_pdf); ?>

<br><br>
<TABLE width='99%' cellpadding=1 cellspacing=1 border=0>
<CAPTION>
	Graphiques par type d'appel
</CAPTION>

<TR align=center>
<TD>
<?
$tmp=array();
$i=1;
foreach($lscalltype as $calltype => $value)
{
	array_push($tmp, "var$i=".$value."&amp;val$i=$countallcalltype[$calltype]");
	$i++;
}
?>
	<div id="chart1">
	<embed type="application/x-shockwave-flash" src="bar.swf" id="barchart" name="barchart" bgcolor="#336699" quality="high" wmode="transparent" flashvars="<? echo implode ('&amp;', $tmp); ?>&amp;title=Nombre d'appels répondus par status&amp;bgcolor=0xF0ffff&amp;bgcolorchart=0xdfedf3&amp;fade1=ff6600&amp;fade2=ff6314&amp;colorbase=0xfff3b3&amp;reverse=1" width="359" height="217">
	</div>
</TD>
<TD>
<?
$tmp=array();
$i=1;
foreach($userlist as $user => $value)
{
	array_push($tmp, "var$i=".$agent_fullname[$user]."&amp;val$i=$countAllCallTypeUser[$user]");
	$i++;
}
?>
	<div id="chart2">
	<embed type="application/x-shockwave-flash" src="bar.swf" id="barchart" name="barchart" bgcolor="#336699" quality="high" wmode="transparent" flashvars="<? echo implode ('&amp;', $tmp); ?>&amp;title=Nombre d'appels répondus par agent&amp;bgcolor=0xF0ffff&amp;bgcolorchart=0xdfedf3&amp;fade1=ff6600&amp;fade2=ff6314&amp;colorbase=0xfff3b3&amp;reverse=1" width="359" height="217">
	</div>
</TD>
</TR>
<TR align=center>
<TD>
<?
$tmp=array();
$i=1;
foreach($lscalltype as $calltype => $value)
{
	array_push($tmp, "var$i=".$value."&amp;val$i=".$totalcallduration[$calltype]/count($userlist));
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
	array_push($tmp, "var$i=".$agent_fullname[$user]."&amp;val$i=".$totalcallduration[$user]/count($lscalltype));
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
