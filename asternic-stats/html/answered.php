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
<?
/*
$tmp = explode(',', $agent);
$tmp2 = array();
foreach ($tmp as $res) {
        array_push($tmp2, '\''.$res.'\'');
}       

$agent = implode(',', $tmp2);
*/
$graphcolor2 = "&bgcolor=0xF0ffff&bgcolorchart=0xdfedf3&fade1=0xff6600&fade2=0x528252&colorbase=0xfff3b3&reverse=1";
$graphcolor  = "&bgcolor=0xF0ffff&bgcolorchart=0xdfedf3&fade1=0xff6600&fade2=0xff6600&colorbase=0xfff3b3&reverse=1";
// This query shows the hangup cause, how many calls an
// agent hanged up, and a caller hanged up.
$query = "SELECT count(ev.event) AS num, ev.event AS action ";
$query.= "FROM queue_stats AS qs, qname AS q, qevent AS ev WHERE ";
$query.= "qs.qname = q.qname_id and qs.qevent = ev.event_id and qs.datetime >= '$start' and ";
$query.= "qs.datetime <= '$end' and q.queue IN ($queue) AND ";
$query.= "ev.event IN ('COMPLETECALLER', 'COMPLETEAGENT') ";
$query.= "GROUP BY ev.event ORDER BY ev.event";

$hangup_cause["COMPLETECALLER"]=0;
$hangup_cause["COMPLETEAGENT"]=0;
$res = consulta_db($query,$DB_DEBUG,$DB_MUERE);
while($row=db_fetch_row($res)) {
  $hangup_cause["$row[1]"]=$row[0];
  $total_hangup+=$row[0];
}

$query = "SELECT qs.datetime AS datetime, q.queue AS qname, ag.agent AS qagent, "; 
$query.= "ac.event AS qevent, qs.info1 AS info1, qs.info2 AS info2,  qs.info3 AS info3 ";
$query.= "FROM queue_stats AS qs, qname AS q, qagent AS ag, qevent AS ac WHERE ";
$query.= "qs.qname = q.qname_id AND qs.qagent = ag.agent_id AND qs.qevent = ac.event_id AND ";
$query.= "qs.datetime >= '$start' AND qs.datetime <= '$end' AND ";
$query.= "q.queue IN ($queue) AND ag.agent in ($agent) AND ac.event IN ('COMPLETECALLER', 'COMPLETEAGENT','TRANSFER','CONNECT') ORDER BY qs.datetime";

$answer["15"]=0;
$answer["30"]=0;
$answer["45"]=0;
$answer["60"]=0;
$answer["75"]=0;
$answer["90"]=0;
$answer["91+"]=0;

$abandoned         = 0;
$transferidas      = 0;
$totaltransfers    = 0;
$total_hangup      = 0;
$total_calls       = 0;
$total_calls2      = Array();
$total_duration    = 0;
$total_calls_queue = Array();

$res = consulta_db($query,$DB_DEBUG,$DB_MUERE);
if($res) {
    while($row=db_fetch_row($res)) {
        if($row[3] <> "TRANSFER" && $row[3]<>"CONNECT") {
            $total_hold     += $row[4];
            $total_duration += $row[5];
            $total_calls++;
            $total_calls_queue["$row[1]"]++;
        } elseif($row[3]=="TRANSFER") {
            $transferidas++;
        }
        if($row[3]=="CONNECT") {

            if ($row[4] >=0 && $row[4] <= 15) {
                $answer["15"]++;
            }

            if ($row[4] >=16 && $row[4] <= 30) {
                $answer["30"]++;
            }

            if ($row[4] >=31 && $row[4] <= 45) {
              $answer["45"]++;
            }

            if ($row[4] >=46 && $row[4] <= 60) {
              $answer["60"]++;
            }

            if ($row[4] >=61 && $row[4] <= 75) {
              $answer["75"]++;
            }

            if ($row[4] >=76 && $row[4] <= 90) {
              $answer["90"]++;
            }

            if ($row[4] >=91) {
              $answer["91+"]++;
            }
        }
    }
} 

if($total_calls > 0) {
    ksort($answer);
    $average_hold     = $total_hold     / $total_calls;
    $average_duration = $total_duration / $total_calls;
    $average_hold     = print_human_hour($average_hold);
    $average_duration = print_human_hour($average_duration);
} else {
    // There were no calls
    $average_hold = 0;
    $average_duration = 0;
}

$total_duration_print = print_human_hour($total_duration);
// TRANSFERS
$query = "SELECT ag.agent AS agent, qs.info1 AS info1,  qs.info2 AS info2 ";
$query.= "FROM  queue_stats AS qs, qevent AS ac, qagent as ag, qname As q WHERE qs.qevent = ac.event_id ";
$query.= "AND qs.qname = q.qname_id AND ag.agent_id = qs.qagent AND qs.datetime >= '$start' ";
$query.= "AND qs.datetime <= '$end' AND  q.queue IN ($queue)  AND ag.agent in ($agent) AND  ac.event = 'TRANSFER'";


$res = consulta_db($query,$DB_DEBUG,$DB_MUERE);
if($res) {
    while($row=db_fetch_row($res)) {
        $keytra = "$row[0]^$row[1]@$row[2]";
        $transfers["$keytra"]++;
        $totaltransfers++;
    }
} else {
   $totaltransfers=0;
}

// ABANDONED CALLS
$query = "SELECT  ac.event AS action,  qs.info1 AS info1,  qs.info2 AS info2,  qs.info3 AS info3 ";
$query.= "FROM  queue_stats AS qs, qevent AS ac, qname As q, qagent as ag WHERE ";
$query.= "qs.qevent = ac.event_id AND qs.qname = q.qname_id AND qs.datetime >= '$start' AND ";
$query.= "qs.datetime <= '$end' AND  q.queue IN ($queue)  AND ag.agent in ($agent) AND  ac.event IN ('ABANDON', 'EXITWITHTIMEOUT', 'TRANSFER') ";
$query.= "ORDER BY  ac.event,  qs.info3";

$res = consulta_db($query,$DB_DEBUG,$DB_MUERE);

while($row=db_fetch_row($res)) {

    if($row[0]=="ABANDON") {
         $abandoned++;
        $abandon_end_pos+=$row[1];
        $abandon_start_pos+=$row[2];
        $total_hold_abandon+=$row[3];
    }
    if($row[0]=="EXITWITHTIMEOUT") {
         $timeout++;
        $timeout_end_pos+=$row[1];
        $timeout_start_pos+=$row[2];
        $total_hold_timeout+=$row[3];
    }
}

if($abandoned > 0) {
    $abandon_average_hold = $total_hold_abandon / $abandoned;
    $abandon_average_hold = number_format($abandon_average_hold,2);

    $abandon_average_start = floor($abandon_start_pos / $abandoned);
    $abandon_average_start = number_format($abandon_average_start,2);

    $abandon_average_end = floor($abandon_end_pos / $abandoned);
    $abandon_average_end = number_format($abandon_average_end,2);
} else {
    $abandoned = 0;
    $abandon_average_hold  = 0;
    $abandon_average_start = 0;
    $abandon_average_end   = 0;
}

// This query shows every call for agents, we collect into a named array the values of holdtime and calltime
$query = "SELECT qs.datetime AS datetime, q.queue AS qname, ag.agent AS qagent, ac.event AS qevent, ";
$query.= "qs.info1 AS info1, qs.info2 AS info2, qs.info3 AS info3  FROM queue_stats AS qs, qname AS q, ";
$query.= "qagent AS ag, qevent AS ac WHERE qs.qname = q.qname_id AND qs.qagent = ag.agent_id AND ";
$query.= "qs.qevent = ac.event_id AND qs.datetime >= '$start' AND qs.datetime <= '$end' AND ";
$query.= "q.queue IN ($queue) AND ag.agent in ($agent) AND ac.event IN ('COMPLETECALLER', 'COMPLETEAGENT') ORDER BY ag.agent";

$res = consulta_db($query,$DB_DEBUG,$DB_MUERE);
while($row=db_fetch_row($res)) {
    $total_calls2["$row[2]"]++;
    $record["$row[2]"][]=$row[0]."|".$row[1]."|".$row[3]."|".$row[4];
    $total_hold2["$row[2]"]+=$row[4];
    $total_time2["$row[2]"]+=$row[5];
    $grandtotal_hold+=$row[4];
    $grandtotal_time+=$row[5];
    $grandtotal_calls++;
}

$start_parts = split(" ", $start);
$end_parts   = split(" ", $end);

$cover_pdf = $lang["$language"]['queue'].": ".$queue."\n";
$cover_pdf.= $lang["$language"]['start'].": ".$start_parts[0]."\n";
$cover_pdf.= $lang["$language"]['end'].": ".$end_parts[0]."\n";
$cover_pdf.= $lang["$language"]['period'].": ".$period." ".$lang["$language"]['days']."\n\n";
$cover_pdf.= $lang["$language"]['answered_calls'].": ".$total_calls." ".$lang["$language"]['calls']."\n";
$cover_pdf.= $lang["$language"]['avg_calltime'].": ".$average_duration."\n";
$cover_pdf.= $lang["$language"]['total'].": ".$total_duration_print."\n";
$cover_pdf.= $lang["$language"]['avg_holdtime'].": ".$average_hold."\n";

?>
<body>
<? include("menu.php"); ?>
<div id="main">
    <div id="contents">
        <TABLE width='99%' cellpadding=3 cellspacing=3 border=0>
        <THEAD>
        <TR>
            <TD valign=top width='50%'>
                <TABLE width='100%' border=0 cellpadding=0 cellspacing=0>
                <CAPTION><?=$lang["$language"]['report_info']?></CAPTION>
                <TBODY>
                <TR>
                    <TD><?=$lang["$language"]['queue']?>:</TD>
                    <TD><?=$queue?></TD>
                </TR>
                </TR>
                       <TD><?=$lang["$language"]['start']?>:</TD>
                       <TD><?=$start_parts[0]?></TD>
                </TR>
                </TR>
                <TR>
                       <TD><?=$lang["$language"]['end']?>:</TD>
                       <TD><?=$end_parts[0]?></TD>
                </TR>
                <TR>
                       <TD><?=$lang["$language"]['period']?>:</TD>
                       <TD><?=$period?> <?=$lang["$language"]['days']?></TD>
                </TR>
                </TBODY>
                </TABLE>

            </TD>
            <TD valign=top width='50%'>

                <TABLE width='100%' border=0 cellpadding=0 cellspacing=0>
                <CAPTION><?=$lang["$language"]['answered_calls']?></CAPTION>
                <TBODY>
                <TR> 
                  <TD><?=$lang["$language"]['answered_calls']?></TD>
                  <TD><?=$total_calls?> <?=$lang["$language"]['calls']?></TD>
                </TR>
                <TR> 
                  <TD><?=$lang["$language"]['transferred_calls']?></TD>
                  <TD><?=$transferidas?> <?=$lang["$language"]['calls']?></TD>
                </TR>
                <TR>
                  <TD><?=$lang["$language"]['avg_calltime']?>:</TD>
                  <TD><?=$average_duration?></TD>
                </TR>
                <TR>
                  <TD><?=$lang["$language"]['total']?> <?=$lang["$language"]['calltime']?>:</TD>
                  <TD><?=$total_duration_print?></TD>
                </TR>
                <TR>
                  <TD><?=$lang["$language"]['avg_holdtime']?>:</TD>
                  <TD><?=$average_hold?></TD>
                </TR>
                </TBODY>
              </TABLE>

            </TD>
        </TR>
        </THEAD>
        </TABLE>
        <BR>
        <a name='1'></a>
        <TABLE width='99%' cellpadding=3 cellspacing=3 border=0 class='sortable' id='table1' >
        <CAPTION>
        <a href='#0'><img src='images/go-up.png' border=0 class='icon' width=16 height=16 
	<? 
        tooltip($lang["$language"]['gotop'],200);
        ?> ></a>&nbsp;&nbsp;
        <?=$lang["$language"]['answered_calls_by_agent']?>
        </CAPTION>
            <THEAD>
            <TR>
                  <TH><?=$lang["$language"]['agent']?></TH>
                  <TH><?=$lang["$language"]['Calls']?></TH>
                  <TH><?=$lang["$language"]['percent']?> <?=$lang["$language"]['Calls']?></TH>
                  <TH><?=$lang["$language"]['calltime']?></TH>
                  <TH><?=$lang["$language"]['percent']?> <?=$lang["$language"]['calltime']?></TH>
                  <TH><?=$lang["$language"]['avg']?> <?=$lang["$language"]['calltime']?></TH>
                  <TH><?=$lang["$language"]['holdtime']?></TH>
                  <TH><?=$lang["$language"]['avg']?> <?=$lang["$language"]['holdtime']?></TH>
                  <TH>Nb appels traités/h loggé</TH>
            </TR>
            </THEAD>
            <TBODY>
                <?
                $header_pdf=array($lang["$language"]['agent'],$lang["$language"]['Calls'],$lang["$language"]['percent'],$lang["$language"]['calltime'],$lang["$language"]['percent'],$lang["$language"]['avg'],$lang["$language"]['holdtime'],$lang["$language"]['avg'], 'Nb appels traites/h');
                $width_pdf=array(25,23,23,23,23,25,25,20,20);
                $title_pdf=$lang["$language"]['answered_calls_by_agent'];

                $contador=0;
                $query1 = "";
                $query2 = "";
                if($total_calls2>0) {
                foreach($total_calls2 as $agent=>$val) {
		    		$aa = populate_agents(array($agent)); 
                    $contavar = $contador +1;
                    $cual = $contador % 2;
                    if($cual>0) { $odd = " class='odd' "; } else { $odd = ""; }
                    $query1 .= "val$contavar=".$total_time2["$agent"]."&var$contavar=".$aa[0]['fullname']."&";
                    $query2 .= "val$contavar=".$val."&var$contavar=".$aa[0]['fullname']."&";

                    $time_print = print_human_hour($total_time2["$agent"]);
                    $avg_time = $total_time2["$agent"] / $val;
                    $avg_time = number_format($avg_time,2);
                    $avg_print = print_human_hour($avg_time);

                    echo "<TR $odd>\n";
                    echo "<TD>".$aa[0]['fullname']."</TD>\n";
                    echo "<TD>$val</TD>\n";

                    if($grandtotal_calls > 0) {
                       $percentage_calls = $val * 100 / $grandtotal_calls;
                    } else {
                       $percentage_calls = 0;
                    }
                    $percentage_calls = number_format($percentage_calls,2);
                    echo "<TD>$percentage_calls ".$lang["$language"]['percent']."</TD>\n";
//                    echo "<TD>".$total_time2["$agent"]." ".$lang["$language"]['secs']."</TD>\n";
                    echo "<TD>$time_print </TD>\n";
                    if($grandtotal_time > 0) {
                       $percentage_time = $total_time2["$agent"] * 100 / $grandtotal_time;
                    } else {
                       $percentage_time = 0;
                    }
                    $percentage_time = number_format($percentage_time,2);
                    echo "<TD>$percentage_time ".$lang["$language"]['percent']."</TD>\n";
                    //echo "<TD>$avg_time ".$lang["$language"]['secs']."</TD>\n";
                    echo "<TD>$avg_print</TD>\n";
                    echo "<TD>".print_human_hour($total_hold2["$agent"])."</TD>\n";
                    $avg_hold = $total_hold2["$agent"] / $val;
                    $avg_hold = number_format($avg_hold,2);
                    echo "<TD>".print_human_hour($avg_hold)."</TD>\n";
################################################################################


	$mysqlconnect = mysql_connect($dbhost, $dbuser, $dbpass) or die("connect impossible : " . mysql_error());

	$db = mysql_select_db($dbname, $mysqlconnect);
	if (!$db) {
   		die ('connect bdd impossible : ' . mysql_error());
	}

$query = "SELECT * FROM ctilog WHERE action IN('cti_login','cti_logout','cticommand:availstate') ";
$query.= "AND eventdate >= '$start' AND eventdate <= '$end' ";
$query.= "AND loginclient = '".$aa[0]['loginclient']."' ";
$query.= "ORDER BY loginclient ASC, eventdate ASC";

$res_cti = mysql_query($query);

$data = array();
$tmp = array();
$llt=0;
$nbminus1 = $nb-1;

$datenow = mktime();
    
while($row=mysql_fetch_assoc($res_cti)) {

	$ref = $row;

	if(isset($data[$ref['loginclient']]) === false)
		$data[$ref['loginclient']] = array('total'	=> 0,
						   'cnt'	=> 0,
						   'cti_login'	=> 0,
						   'cti_logout'	=> 0);

	if(isset($tmp[$ref['loginclient']]) === false)
		$tmp[$ref['loginclient']] = array();

	$reftmp = &$tmp[$ref['loginclient']];

	if(isset($reftmp['laststate']) === true
	&& $ref['status'] !== $reftmp['laststate'])
	{
		$calc = calc_duration(strtotime($reftmp['lastdate']),strtotime($ref['eventdate']));

		if($reftmp['laststate'] !== 'xivo_unknown')
			$data[$ref['loginclient']]['total'] += $calc['diff'];
	}

	if($ref['action'] === 'cti_login' || $ref['action'] === 'cti_logout')
		$data[$ref['loginclient']][$ref['action']]++;

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

		if($tmp[$loginclient]['laststate'] !== 'xivo_unknown')
			$data[$loginclient]['total'] += $calc['diff'];
	}

	$loginclient = $ref['loginclient'];
	
	$llt++;
}

echo "<TD>".round($val/($data[$aa[0]['loginclient']]['total']/60/60),2)." appels traités/h</TD>\n";

################################################################################
                    echo "</TR>\n";

                    $linea_pdf = array($aa[0]['fullname'],$val,"$percentage_calls ".$lang["$language"]['percent'],$total_time2["$agent"],"$percentage_time ".$lang["$language"]['percent'],"$avg_time ".$lang["$language"]['secs'],$total_hold2["$agent"]." ".$lang["$language"]['secs'], "$avg_hold ".$lang["$language"]['secs'], round($val/($data[$aa[0]['loginclient']]['total']/60/60),2)." appels traites");
                       $data_pdf[]=$linea_pdf;
                    $contador++;
                }
                
                $query1.="title=".addslashes($lang["$language"]['total_time_agent'])."$graphcolor";
                $query2.="title=".addslashes($lang["$language"]['no_calls_agent'])."$graphcolor";
                }
                ?>
            </TBODY>
        </TABLE>
            <? if($total_calls2>0) {
                print_exports($header_pdf,$data_pdf,$width_pdf,$title_pdf,$cover_pdf);
                }
            ?>
        <BR>    
            <?
            if($total_calls2>0) {



                echo "<TABLE width='99%' cellpadding=3 cellspacing=3 border=0>\n";
                echo "<THEAD>\n";
                echo "<TR><TD align=center bgcolor='#fffdf3' width='100%'>\n";
                #draw_bar($query1,364,220,"chart1",0);
                swf_bar($query1,364,220,"chart1",0);
                echo "</TD><TD align=center bgcolor='#fffdf3' width='100%'>\n";
                //draw_bar($query2,364,220,"chart2",0);
                swf_bar($query2,364,220,"chart2",0);
                echo "</TD></TR>\n";
                echo "</THEAD>\n";
                echo "</TABLE><BR>\n";
            }
            ?>

            <a name='2'></a>
            <TABLE width='99%' cellpadding=3 cellspacing=3 border=0 >
            <CAPTION>
            <a href='#0'><img src='images/go-up.png' border=0 class='icon' width=16 height=16 
            <? 
            tooltip($lang["$language"]['gotop'],200);
            ?>
            ></a>&nbsp;&nbsp;
            <?=$lang["$language"]['call_response']?></CAPTION>
            <THEAD>
            <TR>
            <TD valign=top width='50%' bgcolor='#fffdf3'>
                <TABLE width='99%' cellpadding=1 cellspacing=1 border=0 class='sortable' id='table2'>
                <THEAD>
                <TR> 
                  <TH><?=$lang["$language"]['answer']?></TH>
                  <TH><?=$lang["$language"]['count']?></TH>
                  <TH><?=$lang["$language"]['delta']?></TH>
                  <TH><?=$lang["$language"]['percent']?></TH>
                </TR>
                </THEAD>
                <TBODY>
                <?
                $countrow=0;
                $partial_total = 0;
                $query2="";
                $total_y_transfer = $total_hangup + $transferidas;
                foreach($answer as $key=>$val)
                {
                    $newcont = $countrow+1;
                    $query2.="val$newcont=$val&var$newcont=$key%20".$lang["$language"]['secs']."&";
                    $cual = ($countrow%2);
                    if($cual>0) { $odd = " class='odd' "; } else { $odd = ""; }
                    echo "<TR $odd>\n";
                    echo "<TD>".$lang["$language"]['within']."$key ".$lang["$language"]['secs']."</TD>\n";
                    $delta = $val;                   
		    if($delta > 0) { $delta = "+".$delta;}
                    $partial_total += $val;
                    if($total_y_transfer > 0) {
                    	$percent=$partial_total*100/count($answer);
                    } else {
                    	$percent = 0;
                    }
                    $percent=$partial_total*100/array_sum($answer);
                    $percent=number_format($percent,2);
                    //if($countrow==0) { $delta = ""; }
                    echo "<TD>$partial_total ".$lang["$language"]['calls']."</TD>\n";
                    echo "<TD>$delta</TD>\n";
                    echo "<TD>$percent ".$lang["$language"]['percent']."</TD>\n";
                    echo "</TR>\n";
                    $countrow++;
                }
                $query2.="title=".$lang["$language"]['call_response']."$graphcolor";
                ?>
                </TBODY>
              </TABLE>
              </TD>
                <TD valign=top width="50%" align=center  bgcolor='#fffdf3'>
                <?
                //draw_bar($query2,364,220,"chart3",0);
                swf_bar($query2,364,220,"chart3",0);
                ?>
                </TD>
            </TR>
            </THEAD>
          </TABLE>
          <BR>
            <a name='3'></a>
            <TABLE width='99%' cellpadding=3 cellspacing=3 border=0>
            <CAPTION>
            <a href='#0'><img src='images/go-up.png' border=0 class='icon' width=16 height=16 

            <? 
            tooltip($lang["$language"]['gotop'],200);
            ?>
            ></a>&nbsp;&nbsp;
            <?=$lang["$language"]['answered_calls_by_queue']?></CAPTION>
            <THEAD>
            <TR>
            <TD valign=top width='50%'  bgcolor='#fffdf3'  >
                <TABLE width='99%' cellpadding=1 cellspacing=1 border=0 class='sortable' id='table3' >
                <THEAD>
                <TR> 
                       <TH><?=$lang["$language"]['queue']?></TH>
                    <TH><?=$lang["$language"]['count']?></TH>
                    <TH><?=$lang["$language"]['percent']?></TH>
                </TR>
                </THEAD>
                <TBODY>
                <?
                $countrow=0;
                $query2="";
                if(count($total_calls_queue)==0) {
                        $total_calls_queue[""]=0;
                }
                asort($total_calls_queue);
                foreach($total_calls_queue as $key=>$val) {
                    $cual = $countrow%2;
                    if($cual>0) { $odd = " class='odd' "; } else { $odd = ""; }
                    if($total_calls>0) {
                    $percent = $val * 100 / $total_calls;
                    } else {
                    $percent=0;
                    }
                    $percent =number_format($percent,2);
                    echo "<TR $odd><TD>$key</TD><TD>$val ".$lang["$language"]['calls']."</TD><TD>$percent %</TD></TR>\n";
                    $countrow++;
                    $query2.="var$countrow=$key&val$countrow=$val&";
                }
                $query2.="title=".$lang["$language"]['answered_calls_by_queue']."$graphcolor";
                ?>
              </TBODY>
              </TABLE>
            </TD>
            <TD valign=top width="50%" align=center  bgcolor='#fffdf3'>
                <? 
                if ($countrow>1) {
                    //draw_bar($query2,364,220,"chart4",0);
                    swf_bar($query2,364,220,"chart4",0);
                   } 
                   ?>
            </TD>
            </TR>
            </THEAD>
            </TABLE>
            <BR>

            <a name='4'></a>
            <TABLE width='99%' cellpadding=3 cellspacing=3 border=0>
            <CAPTION>
            <a href='#0'><img src='images/go-up.png' border=0 class='icon' width=16 height=16 

            <? 
            tooltip($lang["$language"]['gotop'],200);
            ?>
            ></a>&nbsp;&nbsp;
            <?=$lang["$language"]['disconnect_cause']?></CAPTION>
            <THEAD>
            <TR>
            <TD valign=top width='50%'  bgcolor='#fffdf3' >
                <TABLE width='99%' cellpadding=1 cellspacing=1 border=0 class='sortable' id='table4'>
                <THEAD>
                <TR>
                    <TH><?=$lang["$language"]['cause']?></TH>
                    <TH><?=$lang["$language"]['count']?></TH>
                    <TH><?=$lang["$language"]['count']?></TH>
                </TR>
                </THEAD>
                <TBODY>
                <TR> 
                  <TD><?=$lang["$language"]['agent_hungup']?>:</TD>
                  <TD><?=$hangup_cause["COMPLETEAGENT"]?> <?=$lang["$language"]['calls']?></TD>
                  <TD>
                      <?
                        if($total_hangup > 0 ) {
                            $percent=$hangup_cause["COMPLETEAGENT"]*100/$total_hangup;
                        } else {
                            $percent=0;
                        }
                        $percent=number_format($percent,2);
                        echo $percent;
                      ?> 
                   <?=$lang["$language"]['percent']?></TD>
                </TR>
                <TR> 
                  <TD><?=$lang["$language"]['caller_hungup']?>:</TD>
                  <TD><?=$hangup_cause['COMPLETECALLER']?> <?=$lang["$language"]['calls']?></TD>
                  <TD>
                      <?
                        if($total_hangup > 0 ) {
                            $percent=$hangup_cause["COMPLETECALLER"]*100/$total_hangup;
                        } else {
                            $percent=0;
                        }
                        $percent=number_format($percent,2);
                        echo $percent;
                      ?> 
                    <?=$lang["$language"]['percent']?></TD>
                </TR>
                </TBODY>
              </TABLE>
            </TD>
            <TD align=center  bgcolor='#fffdf3'>
                <?
                $query2 = "var1=".$lang["$language"]['agent']."&val1=".$hangup_cause["COMPLETEAGENT"]."&";
                $query2 .= "var2=".$lang["$language"]['caller']."&val2=".$hangup_cause["COMPLETECALLER"];
                $query2.="&title=".$lang["$language"]['disconnect_cause']."$graphcolor2";
                //draw_bar($query2,364,220,"chart5",0);
                swf_bar($query2,364,220,"chart5",0);
                ?>
            </TD>
            </TR>
            </THEAD>
            </TABLE>

            <BR>

            <?
            if($totaltransfers>0) {
            ?>
            <TABLE width='99%' cellpadding=1 cellspacing=1 border=0 class='sortable' id='table5'>
            <CAPTION><?=$lang["$language"]['transfers']?></CAPTION>
            <THEAD>
            <TR>
                 <TH><?=$lang["$language"]['agent']?></TH>
                 <TH><?=$lang["$language"]['to']?></TH>
                 <TH><?=$lang["$language"]['count']?></TH>
            </TR>
            </THEAD>
            <TBODY>
            <?
            foreach($transfers as $key=>$val) {
                $partes = split("\^",$key);
                $agent = $partes[0];
                $extension = $partes[1];
                echo "<TR>\n";
                echo "<TD>$agent</TD>\n";
                echo "<TD>$extension</TD>\n";
                echo "<TD>$val</TD>\n";
                echo "</TR>";
            }
            ?>
            </TBODY>
            </TABLE>
        <? } ?>
</div>
</div>
</div>
<script type="text/javascript" src="js/wz_tooltip.js"></script>
</body>
</html>
