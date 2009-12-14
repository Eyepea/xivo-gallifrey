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

$graphcolor = "&bgcolor=0xF0ffff&bgcolorchart=0xdfedf3&fade1=ff6600&fade2=ff6314&colorbase=0xfff3b3&reverse=1";
$graphcolorstack = "&bgcolor=0xF0ffff&bgcolorchart=0xdfedf3&fade1=ff6600&colorbase=fff3b3&reverse=1&fade2=0x528252";

// ABANDONED CALLS

$query = "SELECT qs.datetime AS datetime, q.queue AS qname, ag.agent AS qagent, ac.event AS qevent, ";
$query.= "qs.info1 AS info1, qs.info2 AS info2,  qs.info3 AS info3 FROM queue_stats AS qs, qname AS q, ";
$query.= "qagent AS ag, qevent AS ac WHERE qs.qname = q.qname_id AND qs.qagent = ag.agent_id AND ";
$query.= "qs.qevent = ac.event_id AND qs.datetime >= '$start' AND qs.datetime <= '$end' ";
$query.= "AND q.queue IN ($queue,'NONE') AND ac.event IN ('ABANDON', 'EXITWITHTIMEOUT','COMPLETECALLER','COMPLETEAGENT','AGENTLOGIN','AGENTLOGOFF','AGENTCALLBACKLOGIN','AGENTCALLBACKLOGOFF') ";
$query.= "ORDER BY qs.datetime";

$query_comb     = "";
$login          = 0;
$logoff         = 0;
$dias           = Array();
$logout_by_day  = Array();
$logout_by_hour = Array();
$logout_by_dw   = Array();
$login_by_day   = Array();
$login_by_hour  = Array();
$login_by_dw    = Array();

$res = consulta_db($query,$DB_DEBUG,$DB_MUERE);

$answered = 0;
$unanswered = 0;

if(db_num_rows($res)>0) {

	while($row=db_fetch_row($res)) {
		$partes_fecha = split(" ",$row[0]);
		$partes_hora  = split(":",$partes_fecha[1]);

		$timestamp = return_timestamp($row[0]);
		$day_of_week = date('w',$timestamp);
			
		$dias[] = $partes_fecha[0];
		$horas[] = $partes_hora[0];
		
		$login_by_day["$partes_fecha[0]"] = 0;
		$login_by_hour["$partes_hora[0]"] = 0;
		$login_by_dw["$day_of_week"] = 0;
		
		$logout_by_day["$partes_fecha[0]"] = 0;
		$logout_by_hour["$partes_hora[0]"] = 0;
		$logout_by_dw["$day_of_week"] = 0;

		if($row[3]=="ABANDON" || $row[3]=="EXITWITHTIMEOUT") {
 			$unanswered++;
			$unans_by_day["$partes_fecha[0]"]++;
			$unans_by_hour["$partes_hora[0]"]++;
			$unans_by_dw["$day_of_week"]++;
		}
		if($row[3]=="COMPLETECALLER" || $row[3]=="COMPLETEAGENT") {
 			$answered++;
			$ans_by_day["$partes_fecha[0]"]++;
			$ans_by_hour["$partes_hora[0]"]++;
			$ans_by_dw["$day_of_week"]++;

			$total_time_by_day["$partes_fecha[0]"]+=$row[5];
			$total_hold_by_day["$partes_fecha[0]"]+=$row[4];

			$total_time_by_dw["$day_of_week"]+=$row[5];
			$total_hold_by_dw["$day_of_week"]+=$row[4];
		
			$total_time_by_hour["$partes_hora[0]"]+=$row[5];
			$total_hold_by_hour["$partes_hora[0]"]+=$row[4];
		}
		if($row[3]=="AGENTLOGIN" || $row[3]=="AGENTCALLBACKLOGIN") {
 			$login++;
			$login_by_day["$partes_fecha[0]"]++;
			$login_by_hour["$partes_hora[0]"]++;
			$login_by_dw["$day_of_week"]++;
		}
		if($row[3]=="AGENTLOGOFF" || $row[3]=="AGENTCALLBACKLOGOFF") {
 			$logoff++;
			$logout_by_day["$partes_fecha[0]"]++;
			$logout_by_hour["$partes_hora[0]"]++;
			$logout_by_dw["$day_of_week"]++;
		}
	}
	$total_calls = $answered + $unanswered;
	$dias  = array_unique($dias);
	$horas = array_unique($horas);
    	asort($dias);
	asort($horas);
}

$start_parts = split(" ", $start);
$end_parts   = split(" ", $end);

$cover_pdf = $lang["$language"]['queue'].": ".$queue."\n";
$cover_pdf.= $lang["$language"]['start'].": ".$start_parts[0]."\n";
$cover_pdf.= $lang["$language"]['end'].": ".$end_parts[0]."\n";
$cover_pdf.= $lang["$language"]['period'].": ".$period." ".$lang["$language"]['days']."\n\n";
$cover_pdf.= $lang["$language"]['number_answered'].": ".$answered." ".$lang["$language"]['calls']."\n";
$cover_pdf.= $lang["$language"]['number_unanswered'].": ".$unanswered." ".$lang["$language"]['calls']."\n";
$cover_pdf.= $lang["$language"]['agent_login'].": ".$login."\n";
$cover_pdf.= $lang["$language"]['agent_logoff'].": ".$logoff."\n";
?>
<body>
<? include("menu.php"); ?>
<div id="main">
	<div id="contents">
		<TABLE width=99% cellpadding=3 cellspacing=3 border=0>
		<THEAD>
		<TR>
			<TD valign=top width=50%>
				<TABLE width=100% border=0 cellpadding=0 cellspacing=0>
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
			<TD valign=top width=50%>

				<TABLE width=100% border=0 cellpadding=0 cellspacing=0>
				<CAPTION><?=$lang["$language"]['totals']?></CAPTION>
				<TBODY>
		        <TR> 
                  <TD><?=$lang["$language"]['number_answered']?>:</TD>
		          <TD><?=$answered?> <?=$lang["$language"]['calls']?></TD>
	            </TR>
                <TR>
                  <TD><?=$lang["$language"]['number_unanswered']?>:</TD>
                  <TD><?=$unanswered?> <?=$lang["$language"]['calls']?></TD>
                </TR>
                <TR>
                  <TD><?=$lang["$language"]['number_totals']?>:</TD>
                  <TD><?=$total_calls?> <?=$lang["$language"]['calls']?></TD>
                </TR>
                <TR>
                  <TD>Taux de décroche :</TD>
                  <TD><?php echo round($answered/$total_calls*100, 2); ?> %</TD>
                </TR>
		        <!--TR>
                  <TD><?=$lang["$language"]['agent_login']?>:</TD>
		          <TD><?=$login?></TD>
	            </TR>
                <TR>
                  <TD><?=$lang["$language"]['agent_logoff']?>:</TD>
                  <TD><?=$logoff?></TD>
                </TR-->
				</TBODY>
	          </TABLE>

			</TD>
		</TR>
		</THEAD>
		</TABLE>
		<BR>	
			<?
				if(count($dias)<=0) {
					$dias['']=0;
				}
			?>
			<a name='1'></a>
			<TABLE width=99% cellpadding=1 cellspacing=1 border=0 class='sortable' id='table1'>
			<CAPTION>
			<a href='#0'><img src='images/go-up.png' border=0 width=16 height=16 class='icon' 
			<? 
			tooltip($lang["$language"]['gotop'],200);
			?>
			></a>&nbsp;&nbsp;
			<?=$lang["$language"]['call_distrib_day']?>
			</CAPTION>
				<THEAD>
				<TR>
					<TH><?=$lang["$language"]['date']?></TH>
					<TH>Taux de décroche</TH>
					<TH>Présenté</TH>
					<TH><?=$lang["$language"]['answered']?></TH>
					<TH><?=$lang["$language"]['percent_answered']?></TH>
					<TH><?=$lang["$language"]['unanswered']?></TH>
					<TH><?=$lang["$language"]['percent_unanswered']?></TH>
					<TH><?=$lang["$language"]['avg_calltime']?></TH>
					<TH><?=$lang["$language"]['avg_holdtime']?></TH>
					<TH><?=$lang["$language"]['agent_login']?></TH>
					<TH><?=$lang["$language"]['agent_logoff']?></TH>
				</TR>
				</THEAD>
				<TBODY>
				<?
				$header_pdf=array($lang["$language"]['date'], 'Taux de décroche', 'Présenté', $lang["$language"]['answered'],$lang["$language"]['percent_answered'],$lang["$language"]['unanswered'],$lang["$language"]['percent_unanswered'],$lang["$language"]['avg_calltime'],$lang["$language"]['avg_holdtime'],$lang["$language"]['agent_login'],$lang["$language"]['agent_logoff']);
				$width_pdf=array(25,25,25,23,23,23,23,25,25,20,20);
				$title_pdf=$lang["$language"]['call_distrib_day'];

				$count=1;
				foreach($dias as $key) {
					$cual = $count%2;
					if($cual>0) { $odd = " class='odd' "; } else { $odd = ""; }
					if(!isset($ans_by_day["$key"])) {
						$ans_by_day["$key"]=0;
					}
					if(!isset($unans_by_day["$key"])) {
						$unans_by_day["$key"]=0;
					}
					if($answered > 0) {
						$percent_ans   = $ans_by_day["$key"]   * 100 / $answered;
					} else {
						$percent_ans = 0;
					}
					if($ans_by_day["$key"] >0) {
						$average_call_duration = $total_time_by_day["$key"] / $ans_by_day["$key"];
						$average_hold_duration = $total_hold_by_day["$key"] / $ans_by_day["$key"];
					} else {
						$average_call_duration = 0;
						$average_hold_duration = 0;
					}
					if($unanswered > 0) {
						$percent_unans = $unans_by_day["$key"] * 100 / $unanswered;
					} else {
						$percent_unans = 0;
					}
					$percent_ans   = number_format($percent_ans,  2);
					$percent_unans = number_format($percent_unans,2);
					$average_call_duration_print = print_human_hour($average_call_duration);
					if($key<>"") {

					$total = $ans_by_day["$key"]+$unans_by_day["$key"];
					$taux_decroche = round($ans_by_day["$key"]/$total*100);

					$linea_pdf = array($key, $taux_decroche.'%', $total,$ans_by_day["$key"],"$percent_ans ".$lang["$language"]['percent'],$unans_by_day["$key"],"$percent_unans ".$lang["$language"]['percent'],$average_call_duration_print,number_format($average_hold_duration,0),$login_by_day["$key"],$logout_by_day["$key"]);

					echo "<TR $odd>\n";
					echo "<TD>$key</TD>\n";
					echo "<TD>$taux_decroche %</TD>\n";
					echo "<TD>$total</TD>\n";
					echo "<TD>".$ans_by_day["$key"]."</TD>\n";
					echo "<TD>$percent_ans ".$lang["$language"]['percent']."</TD>\n";
					echo "<TD>".$unans_by_day["$key"]."</TD>\n";
					echo "<TD>$percent_unans".$lang["$language"]['percent']."</TD>\n";
					echo "<TD>".$average_call_duration_print."</TD>\n";
					echo "<TD>".print_human_hour($average_hold_duration)."</TD>\n";
					echo "<TD>".$login_by_day["$key"]."</TD>\n";
					echo "<TD>".$logout_by_day["$key"]."</TD>\n";
					echo "</TR>\n";
					$count++;
					$data_pdf[]=$linea_pdf;
					}
				}
				?>
			</TBODY>
			</TABLE>
			
			<?
				if($count>1) {
					print_exports($header_pdf,$data_pdf,$width_pdf,$title_pdf,$cover_pdf); 
				}
			?>
			<BR>
			
			<a name='2'></a>
			<TABLE width='99%' cellpadding=1 cellspacing=1 border=0 class='sortable' id='table2'>
			<CAPTION>
			<a href='#0'><img src='images/go-up.png' border=0 width=16 height=16 class='icon' 
			<? 
			tooltip($lang["$language"]['gotop'],200);
			?>
			></a>&nbsp;&nbsp;
			<?=$lang["$language"]['call_distrib_hour']?>
			</CAPTION>
				<THEAD>
				<TR>
                    <TH><?=$lang["$language"]['hour']?></TH>
		    		<TH>Taux de décroche</TH>
                    <TH>Présenté</TH>
                    <TH><?=$lang["$language"]['answered']?></TH>
                    <TH><?=$lang["$language"]['percent_answered']?></TH>
                    <TH><?=$lang["$language"]['unanswered']?></TH>
                    <TH><?=$lang["$language"]['percent_unanswered']?></TH>
                    <TH><?=$lang["$language"]['avg_calltime']?></TH>
                    <TH><?=$lang["$language"]['avg_holdtime']?></TH>
                    <TH><?=$lang["$language"]['agent_login']?></TH>
                    <TH><?=$lang["$language"]['agent_logoff']?></TH>
				</TR>
				</THEAD>
				<TBODY>
				<?

				$header_pdf=array($lang["$language"]['hour'], 'Taux de décroche', 'Présent', $lang["$language"]['answered'],$lang["$language"]['percent_answered'],$lang["$language"]['unanswered'],$lang["$language"]['percent_unanswered'],$lang["$language"]['avg_calltime'],$lang["$language"]['avg_holdtime'],$lang["$language"]['agent_login'],$lang["$language"]['agent_logoff']);
				$width_pdf=array(25,25,25,23,23,23,23,25,25,20,20);
				$title_pdf=$lang["$language"]['call_distrib_hour'];
				$data_pdf = array();

				$query_ans = "";
				$query_unans = "";
				$query_time="";
				$query_hold="";
				$MoyCntPerHour = array();
				for($key=0;$key<24;$key++) {
					$cual = ($key+1)%2;
					if($cual>0) { $odd = " class='odd' "; } else { $odd = ""; }
					if(strlen($key)==1) { $key = "0".$key; }
					if(!isset($ans_by_hour["$key"])) {
						$ans_by_hour["$key"]=0;
						$average_call_duration = 0;
						$average_hold_duration = 0;
					} else {
						$average_call_duration = $total_time_by_hour["$key"] / $ans_by_hour["$key"];
						$average_hold_duration = $total_hold_by_hour["$key"] / $ans_by_hour["$key"];
					}
					if(!isset($unans_by_hour["$key"])) {
						$unans_by_hour["$key"]=0;
					}
					if($answered > 0) {
						$percent_ans   = $ans_by_hour["$key"] * 100 / $answered;
					} else {
						$percent_ans = 0;
					}
					if($unanswered > 0) {
						$percent_unans = $unans_by_hour["$key"] * 100 / $unanswered;
					} else {
						$percent_unans = 0;
					}
					$percent_ans   = number_format($percent_ans,  2);
					$percent_unans = number_format($percent_unans,2);

					if(!isset($login_by_hour["$key"])) {
					    $login_by_hour["$key"] = 0;
                    }
					if(!isset($logout_by_hour["$key"])) {
					    $logout_by_hour["$key"] = 0;
                    }

					$total = $ans_by_hour["$key"] + $unans_by_hour["$key"];
					
					if($total > 0) {
						$taux_decroche = round($ans_by_hour["$key"]/$total*100);
					} else {
						$taux_decroche = 0;
					}
					
					echo "<TR $odd>\n";
					echo "<TD>$key</TD>\n";
					echo "<TD>$taux_decroche %</TD>\n";
					echo "<TD>$total</TD>\n";
					echo "<TD>".$ans_by_hour["$key"]."</TD>\n";
					echo "<TD>$percent_ans".$lang["$language"]['percent']."</TD>\n";
					echo "<TD>".$unans_by_hour["$key"]."</TD>\n";
					echo "<TD>$percent_unans".$lang["$language"]['percent']."</TD>\n";
					echo "<TD>".seconds2minutes($average_call_duration)." ".$lang["$language"]['minutes']."</TD>\n";
					echo "<TD>".seconds2minutes($average_hold_duration)." ".$lang["$language"]['minutes']."</TD>\n";
					echo "<TD>".$login_by_hour["$key"]."</TD>\n";
					echo "<TD>".$logout_by_hour["$key"]."</TD>\n";
					echo "</TR>\n";
					$gkey = $key+1;
					$query_ans  .="var$gkey=$key&val$gkey=".$ans_by_hour["$key"]."&";
					$query_unans.="var$gkey=$key&val$gkey=".$unans_by_hour["$key"]."&";
					$query_comb.= "var$gkey=$key%20".$lang["$language"]['hours']."&valA$gkey=".$ans_by_hour["$key"]."&valB$gkey=".$unans_by_hour["$key"]."&";
					$query_time.="var$gkey=$key&val$gkey=".intval($average_call_duration)."&";
					$query_hold.="var$gkey=$key&val$gkey=".intval($average_hold_duration)."&";
					
					$linea_pdf = array($key, $taux_decroche.'%', $total, $ans_by_hour["$key"],"$percent_ans ".$lang["$language"]['percent'],$unans_by_hour["$key"],"$percent_unans ".$lang["$language"]['percent'],number_format($average_call_duration,0),number_format($average_hold_duration,0),$login_by_hour["$key"],$logout_by_hour["$key"]);
					$data_pdf[]=$linea_pdf;
					
					if($taux_decroche > 0){ $MoyCntPerHour['taux_decroche']++; }
					if($total > 0){ $MoyCntPerHour['presente']++; }
					if($ans_by_hour["$key"] > 0){ $MoyCntPerHour['ans_by_hour']++; }
					if($percent_ans > 0){ $MoyCntPerHour['percent_ans']++; }
					if($unans_by_hour["$key"] > 0){ $MoyCntPerHour['unans_by_hour']++; }
					if($percent_unans > 0){ $MoyCntPerHour['percent_unans']++; }
					if($average_call_duration > 0){ $MoyCntPerHour['average_call_duration']++; }
					if($average_hold_duration > 0){ $MoyCntPerHour['average_hold_duration']++; }
					if($login_by_hour["$key"] > 0){ $MoyCntPerHour['login_by_hour']++; }
					if($logout_by_hour["$key"] > 0){ $MoyCntPerHour['logout_by_hour']++; }
					
					$total_taux_decroche += $taux_decroche;
					$total_presente += $total;
					$total_ans_by_hour += $ans_by_hour["$key"];
					$total_percent_ans += $percent_ans;
					$total_unans_by_hour += $unans_by_hour["$key"];
					$total_percent_unans += $percent_unans;
					$total_average_call_duration += $average_call_duration;
					$total_average_hold_duration += $average_hold_duration;
					$total_login += $login_by_hour["$key"];
					$total_logout += $logout_by_hour["$key"];

				}
				$query_ans.="title=".$lang["$language"]['answ_by_hour']."$graphcolor";
				$query_unans.="title=".$lang["$language"]['unansw_by_hour']."$graphcolor";
				$query_time.="title=".addslashes($lang["$language"]['avg_call_time_by_hr'])."$graphcolor";
				$query_hold.="title=".addslashes($lang["$language"]['avg_hold_time_by_hr'])."$graphcolor";
				$query_comb.="title=".addslashes($lang["$language"]['anws_unanws_by_hour'])."$graphcolorstack&tagA=".$lang["$language"]['answered_calls']."&tagB=".$lang["$language"]['unanswered_calls'];

					echo "<TR style='font-weight:bold'>\n";
					echo "<TD>Moy. </TD>\n";
					echo "<TD>".round($total_taux_decroche/$MoyCntPerHour['taux_decroche'], 2)." %</TD>\n";
					echo "<TD>".round($total_presente/$MoyCntPerHour['presente'], 2)."</TD>\n";
                    echo "<TD>".round($total_ans_by_hour/$MoyCntPerHour['ans_by_hour'], 2)."</TD>\n";
                    echo "<TD>".round($total_percent_ans/$MoyCntPerHour['percent_ans'], 2)." %</TD>\n";
                    echo "<TD>".round($total_unans_by_hour/$MoyCntPerHour['unans_by_hour'], 2)."</TD>\n";
                    echo "<TD>".round($total_percent_unans/$MoyCntPerHour['percent_unans'], 2)." %</TD>\n";
                    echo "<TD>".print_human_hour($total_average_call_duration/$MoyCntPerHour['average_call_duration'])."</TD>\n";
                    echo "<TD>".print_human_hour($total_average_hold_duration/$MoyCntPerHour['average_hold_duration'])."</TD>\n";
                    echo "<TD>".round($total_login/$MoyCntPerHour['login_by_hour'], 2)."</TD>\n";
                    echo "<TD>".round($total_logout/$MoyCntPerHour['logout_by_hour'], 2)."</TD>\n";
                    echo "</TR>\n";

					echo "<TR style='font-weight:bold'>\n";
					echo "<TD>Total</TD>\n";
					echo "<TD>100%</TD>\n";
					echo "<TD>$total_presente</TD>\n";
					echo "<TD>$total_ans_by_hour</TD>\n";
					echo "<TD>100%</TD>\n";
					echo "<TD>$total_unans_by_hour</TD>\n";
					echo "<TD>100%</TD>\n";
					echo "<TD>".print_human_hour($total_average_call_duration)."</TD>\n";
					echo "<TD>".print_human_hour($total_average_hold_duration)."</TD>\n";
					echo "<TD>$total_login</TD>\n";
					echo "<TD>$total_logout</TD>\n";
					echo "</TR>\n";
					$linea_pdf_moy = array('Moyenne',
									round($total_taux_decroche/$MoyCntPerHour['taux_decroche'], 2),
									round($total_presente/$MoyCntPerHour['presente'], 2),
									round($total_ans_by_hour/$MoyCntPerHour['ans_by_hour'], 2),
									round($total_percent_ans/$MoyCntPerHour['percent_ans'], 2)." %",
									round($total_unans_by_hour/$MoyCntPerHour['unans_by_hour'], 2),
									round($total_percent_unans/$MoyCntPerHour['percent_unans'], 2)." %",
									print_human_hour($total_average_call_duration/$MoyCntPerHour['average_call_duration']),
									print_human_hour($total_average_hold_duration/$MoyCntPerHour['average_hold_duration']),
									round($total_login/$MoyCntPerHour['login_by_hour'], 2),
									round($total_logout/$MoyCntPerHour['logout_by_hour'], 2));
					$data_pdf[]=$linea_pdf_moy;
					$linea_pdf_tot = array('Total', 
									'100%',
									$total_presente, 
									$total_ans_by_hour,
									'100%',
									$total_unans_by_hour,
									'100%',
									print_human_hour($total_average_call_duration),
									print_human_hour($total_average_hold_duration),
									$total_login,
									$total_logout);
					$data_pdf[]=$linea_pdf_tot;
				?>
			</TBODY>
			</TABLE>
			<?
				print_exports($header_pdf,$data_pdf,$width_pdf,$title_pdf,$cover_pdf); 
			?>

			<BR>

			<TABLE width='99%' cellpadding=1 cellspacing=1 border=0>
			<THEAD>
			<TR>
				<TD align=center bgcolor='#fffdf3'>
				<hr>
				</TD>
			</TR>
			<TR>
				<TD align=center bgcolor='#fffdf3'>
					<?
					swf_bar($query_comb,'718','433',"chart1",1);
					?>
				</TD>
			</TR>
			<TR>
				<TD align=center bgcolor='#fffdf3'>
					<?
					swf_bar($query_time,'718','433',"chart3",0);
					?>
				</TD>
			</TR>
			<TR>
				<TD align=center bgcolor='#fffdf3'>
					<?
					swf_bar($query_hold,'718','433',"chart4",0);
					?>
				</TD>
			</TR>
			</THEAD>
			</TABLE>

			<BR>

			<a name='3'></a>
			<TABLE width='99%' cellpadding=1 cellspacing=1 border=0 class='sortable' id='table3'>
			<CAPTION>
			<a href='#0'><img src='images/go-up.png' border=0 width=16 height=16 class='icon' 
			<? 
			tooltip($lang["$language"]['gotop'],200);
			?>
			></a>&nbsp;&nbsp;

			<?=$lang["$language"]['call_distrib_week']?>
			</CAPTION>
				<THEAD>
				<TR>
                    <TH><?=$lang["$language"]['day']?></TH>
			    	<TH>Taux de décroche</TH>
			    	<TH>Présenté</TH>
                    <TH><?=$lang["$language"]['answered']?></TH>
                    <TH><?=$lang["$language"]['percent_answered']?></TH>
                    <TH><?=$lang["$language"]['unanswered']?></TH>
                    <TH><?=$lang["$language"]['percent_unanswered']?></TH>
                    <TH><?=$lang["$language"]['avg_calltime']?></TH>
                    <TH><?=$lang["$language"]['avg_holdtime']?></TH>
                    <TH><?=$lang["$language"]['agent_login']?></TH>
                    <TH><?=$lang["$language"]['agent_logoff']?></TH>
				</TR>
				</THEAD>
				<TBODY>
				<?

				$header_pdf=array($lang["$language"]['day'],'Taux de décroche','Présenté',$lang["$language"]['answered'],$lang["$language"]['percent_answered'],$lang["$language"]['unanswered'],$lang["$language"]['percent_unanswered'],$lang["$language"]['avg_calltime'],$lang["$language"]['avg_holdtime'],$lang["$language"]['agent_login'],$lang["$language"]['agent_logoff']);
				$width_pdf=array(25,23,23,23,23,25,25,20,20);
				$title_pdf=$lang["$language"]['call_distrib_week'];
				$data_pdf = array();


				$query_ans="";
				$query_unans="";
				$query_time="";
				$query_hold="";
				
				$MoyCount = array();
					$total_taux_decroche = 0;
					$total_presente = 0;
					$total_answered = 0;
					$total_percent_ans = 0;
					$total_unanswered = 0;
					$total_percent_unans = 0;
					$total_average_call_duration = 0;
					$total_average_hold_duration = 0;
					$total_login = 0;
					$total_logout = 0;
					
				for($key=0;$key<7;$key++) {
					$cual = ($key+1)%2;
					if($cual>0) { $odd = " class='odd' "; } else { $odd = ""; }
					if(!isset($total_time_by_dw["$key"])) {
						$total_time_by_dw["$key"]=0;
					}
					if(!isset($total_hold_by_dw["$key"])) {
						$total_hold_by_dw["$key"]=0;
					}
					if(!isset($ans_by_dw["$key"])) {
						$ans_by_dw["$key"]=0;
						$average_call_duration = 0;
						$average_hold_duration = 0;
					} else {
						$average_call_duration = $total_time_by_dw["$key"] / $ans_by_dw["$key"];
						$average_hold_duration = $total_hold_by_dw["$key"] / $ans_by_dw["$key"];
					}

					if(!isset($unans_by_dw["$key"])) {
						$unans_by_dw["$key"]=0;
					}
					if($answered > 0) {
						$percent_ans   = $ans_by_dw["$key"]   * 100 / $answered;
					} else {
						$percent_ans = 0;
					}
					if($unanswered > 0) {
						$percent_unans = $unans_by_dw["$key"] * 100 / $unanswered;
					} else {
						$percent_unans = 0;
					}
					$percent_ans   = number_format($percent_ans,  2);
					$percent_unans = number_format($percent_unans,2);

					if(!isset($login_by_dw["$key"])) {
					    $login_by_dw["$key"]=0;
                    }
					if(!isset($logout_by_dw["$key"])) {
					    $logout_by_dw["$key"]=0;
                    }

					$total = $ans_by_dw["$key"]+$unans_by_dw["$key"];
					$taux_decroche = round($ans_by_dw["$key"]/$total*100);
					$tt += $taux_decroche;
					
					echo "<TR $odd>\n";
					echo "<TD>".$dayp["$key"]."</TD>\n";
					echo "<TD>$taux_decroche %</TD>\n";
					echo "<TD>$total</TD>";
					echo "<TD>".$ans_by_dw["$key"]."</TD>\n";
					echo "<TD>$percent_ans".$lang["$language"]['percent']."</TD>\n";
					echo "<TD>".$unans_by_dw["$key"]."</TD>\n";
					echo "<TD>$percent_unans".$lang["$language"]['percent']."</TD>\n";
					echo "<TD>".seconds2minutes($average_call_duration)." min</TD>\n";
					echo "<TD>".seconds2minutes($average_hold_duration)." min</TD>\n";
					echo "<TD>".$login_by_dw["$key"]."</TD>\n";
					echo "<TD>".$logout_by_dw["$key"]."</TD>\n";
					echo "</TR>\n";
					
					$gkey = $key + 1;
					$query_ans  .="var$gkey=".$dayp["$key"]."&val$gkey=".intval($ans_by_dw["$key"])."&";
					$query_unans.="var$gkey=".$dayp["$key"]."&val$gkey=".intval($unans_by_dw["$key"])."&";
					$query_time.="var$gkey=".$dayp["$key"]."&val$gkey=".intval($average_call_duration)."&";
					$query_hold.="var$gkey=".$dayp["$key"]."&val$gkey=".intval($average_hold_duration)."&";

					$linea_pdf = array($dayp["$key"], 
									$taux_decroche .'%', 
									$total, 
									$ans_by_dw["$key"],
									"$percent_ans ".$lang["$language"]['percent'],
									$unans_by_dw["$key"],
									"$percent_unans ".$lang["$language"]['percent'],
									number_format($average_call_duration,0),
									number_format($average_hold_duration,0),
									$login_by_dw["$key"],
									$logout_by_dw["$key"]);
									
					$data_pdf[]=$linea_pdf;
					
					if($taux_decroche > 0){ $MoyCount['taux_decroche']++; }
					if($total > 0){ $MoyCount['presente']++; }
					if($ans_by_dw["$key"] > 0){ $MoyCount['ans_by_dw']++; }
					if($percent_ans > 0){ $MoyCount['percent_ans']++; }
					if($unans_by_dw["$key"] > 0){ $MoyCount['unans_by_dw']++; }
					if($percent_unans > 0){ $MoyCount['percent_unans']++; }
					if($average_call_duration > 0){ $MoyCount['average_call_duration']++; }
					if($average_hold_duration > 0){ $MoyCount['average_hold_duration']++; }
					if($login_by_dw["$key"] > 0){ $MoyCount['login_by_dw']++; }
					if($logout_by_dw["$key"] > 0){ $MoyCount['logout_by_dw']++; }
					
					$total_taux_decroche += $taux_decroche;
					$total_presente += $total;
					$total_answered += $ans_by_dw[$key];
					$total_percent_ans += $percent_ans;
					$total_unanswered += $unans_by_dw[$key];
					$total_percent_unans += $percent_unans;
					$total_average_call_duration += $average_call_duration;
					$total_average_hold_duration += $average_hold_duration;
					$total_login += $login_by_dw[$key];
					$total_logout += $logout_by_dw[$key];
				}
				
				$query_ans.="title=".addslashes($lang["$language"]['answ_by_day'])."$graphcolor";
				$query_unans.="title=".addslashes($lang["$language"]['unansw_by_day'])."$graphcolor";
				$query_time.="title=".addslashes($lang["$language"]['avg_call_time_by_day'])."$graphcolor";
				$query_hold.="title=".addslashes($lang["$language"]['avg_hold_time_by_day'])."$graphcolor";
				?>
				
				<?
							
					$moy_taux_decroche = round($total_taux_decroche/$MoyCount['taux_decroche'], 2);
					$moy_presente = round($total_presente/$MoyCount['presente'], 2);
					$moy_answered = round($total_answered/$MoyCount['ans_by_dw'], 2);
					$moy_percent_ans = round($total_percent_ans/$MoyCount['percent_ans'], 2);
					$moy_unanswered = round($total_unanswered/$MoyCount['unans_by_dw'], 2);
					$moy_percent_unans = round($total_percent_unans/$MoyCount['percent_unans'], 2);
					$moy_average_call_duration = round($total_average_call_duration/$MoyCount['average_call_duration'], 2);
					$moy_average_hold_duration = round($total_average_hold_duration/$MoyCount['average_hold_duration'], 2);
					$moy_login = round($total_login/$MoyCount['login_by_dw'], 2);
					$moy_logout = round($total_logout/$MoyCount['logout_by_dw'], 2);

					echo "<TR style='font-weight:bold'>\n";
					echo "<TD>Moy.</TD>\n";
					echo "<TD>$moy_taux_decroche %</TD>\n";
					echo "<TD>$moy_presente</TD>";
					echo "<TD>$moy_answered</TD>\n";
					echo "<TD>$moy_percent_ans %</TD>\n";
					echo "<TD>$moy_unanswered</TD>\n";
					echo "<TD>$moy_percent_unans %</TD>\n";
					echo "<TD>".print_human_hour($moy_average_call_duration)."</TD>\n";
					echo "<TD>".print_human_hour($moy_average_hold_duration)."</TD>\n";
					echo "<TD>$moy_login</TD>\n";
					echo "<TD>$moy_logout</TD>\n";
					echo "</TR>\n";
					

					echo "<TR style='font-weight:bold'>\n";
					echo "<TD>Total</TD>\n";
					echo "<TD>100%</TD>\n";
					echo "<TD>$total_presente</TD>";
					echo "<TD>$total_answered</TD>\n";
					echo "<TD>100%</TD>\n";
					echo "<TD>$total_unanswered</TD>\n";
					echo "<TD>100%</TD>\n";
					echo "<TD>".print_human_hour($total_average_call_duration)."</TD>\n";
					echo "<TD>".print_human_hour($total_average_hold_duration)."</TD>\n";
					echo "<TD>$total_login</TD>\n";
					echo "<TD>$total_logout</TD>\n";
					echo "</TR>\n";
					
					$linea_pdf_moy = array('Moy.',
									$moy_taux_decroche . "%",
									$moy_presente,
									$moy_answered,
									$moy_percent_ans . "%",
									$moy_unanswered,
									$moy_percent_unans . "%",
									print_human_hour($moy_average_call_duration),
									print_human_hour($moy_average_hold_duration),
									$moy_login,
									$moy_logout);
					$data_pdf[]=$linea_pdf_moy;
					$linea_pdf_tot = array('Total',
									'100%',
									$total_presente,
									$total_answered,
									'100%',
									$total_unanswered,
									'100%',
									print_human_hour($total_average_call_duration),
									print_human_hour($total_average_hold_duration),
									$total_login,
									$total_logout);
					$data_pdf[]=$linea_pdf_tot;
				?>
			</TBODY>
			</TABLE>
			<?
				print_exports($header_pdf,$data_pdf,$width_pdf,$title_pdf,$cover_pdf); 
			?>
			<BR>

			<TABLE width=99% cellpadding=1 cellspacing=1 border=0>
			<THEAD>
			<TR>
				<TD align=center bgcolor='#fffdf3'>
					<?
					swf_bar($query_ans,359,217,"chart5",0);
					?>
				</TD>
				<TD align=center bgcolor='#fffdf3'>
					<?
					swf_bar($query_unans,359,217,"chart6",0);
					?>
				</TD>
			</TR>
			<TR>
				<TD align=center bgcolor='#fffdf3'>
					<?
					swf_bar($query_time,359,217,"chart7",0);
					?>
				</TD>
				<TD align=center bgcolor='#fffdf3'>
					<?
					swf_bar($query_hold,359,217,"chart8",0);
					?>
				</TD>
			</TR>
			</THEAD>
			</TABLE>

</div>
</div>
<script type="text/javascript" src="js/wz_tooltip.js"></script>
</body>
</html>
