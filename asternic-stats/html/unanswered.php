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

$graphcolor =  "&bgcolor=0xF0ffff&bgcolorchart=0xdfedf3&fade1=ff6600&fade2=ff6600&colorbase=0xfff3b3&reverse=1";
$graphcolor2 = "&bgcolor=0xF0ffff&bgcolorchart=0xdfedf3&fade1=ff6600&colorbase=fff3b3&reverse=1&fade2=0x528252";

// ABANDONED CALLS

$query = "SELECT qs.datetime AS datetime, q.queue AS qname, ag.agent AS qagent, ac.event AS qevent, ";
$query.= "qs.info1 AS info1, qs.info2 AS info2,  qs.info3 AS info3 FROM queue_stats AS qs, qname AS q, ";
$query.= "qagent AS ag, qevent AS ac WHERE qs.qname = q.qname_id AND qs.qagent = ag.agent_id AND ";
$query.= "qs.qevent = ac.event_id AND qs.datetime >= '$start' AND qs.datetime <= '$end' ";
$query.= "AND q.queue IN ($queue) AND ac.event IN ('ABANDON', 'EXITWITHTIMEOUT') ORDER BY qs.datetime";
$res = consulta_db($query,$DB_DEBUG,$DB_MUERE);

$abandon_calls_queue = Array();
$abandon=0;
$timeout=0;

if(db_num_rows($res)>0) {

while($row=db_fetch_row($res)) {

	if($row[3]=="ABANDON") {
 		$abandoned++;
		$abandon_end_pos+=$row[4];
		$abandon_start_pos+=$row[5];
		$total_hold_abandon+=$row[6];
	}
	if($row[3]=="EXITWITHTIMEOUT") {
 		$timeout++;
	}
	$abandon_calls_queue["$row[1]"]++;
}
mysql_free_result($res);

if($abandoned > 0) {
	$abandon_average_hold = $total_hold_abandon / $abandoned;
} else {
	$abandon_average_hold = 0;
}
$abandon_average_hold = seconds2minutes($abandon_average_hold);

if($abandoned > 0) {
	$abandon_average_start = round($abandon_start_pos / $abandoned);
} else {
	$abandon_average_start = 0;
}
$abandon_average_start = number_format($abandon_average_start,0);

if($abandoned > 0) {
	$abandon_average_end = floor($abandon_end_pos / $abandoned);
} else {
	$abandon_average_end = 0;
}
$abandon_average_end = number_format($abandon_average_end,0);

$total_abandon = $abandoned + $timeout;

} else {
 	// No rows returned
	$abandoned = 0;
	$timeout = 0;
	$abandon_average_hold  = 0;
	$abandon_average_start = 0;
	$abandon_average_end   = 0;
	$total_abandon         = 0;
}


$start_parts = split(" ", $start);
$end_parts   = split(" ", $end);

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
				<CAPTION><?=$lang["$language"]['unanswered_calls']?></CAPTION>
				<TBODY>
		        <TR>
                  <TD><?=$lang["$language"]['number_unanswered']?>:</TD>
		          <TD><?=$total_abandon?> <?=$lang["$language"]['calls']?></TD>
	            </TR>
                <TR>
                  <TD><?=$lang["$language"]['avg_wait_before_dis']?>:</TD>
                  <TD><?=$abandon_average_hold?> <?=$lang["$language"]['minutes']?></TD>
                </TR>
		        <TR>
                  <TD><?=$lang["$language"]['avg_queue_pos_at_dis']?>:</TD>
		          <TD><?=$abandon_average_end?></TD>
	            </TR>
                <TR>
                  <TD><?=$lang["$language"]['avg_queue_start']?>:</TD>
                  <TD><?=$abandon_average_start?></TD>
                </TR>
				</TBODY>
	          </TABLE>

			</TD>
		</TR>
		</THEAD>
		</TABLE>
		<BR>

		<a name='1'></a>
		<TABLE width='99%' cellpadding=3 cellspacing=3 border=0>
		<CAPTION>
		<a href='#0'><img src='images/go-up.png' border=0 width=16 height=16 class='icon'
		<?
		tooltip($lang["$language"]['gotop'],200);
		?>
		></a>&nbsp;&nbsp;
		<?=$lang["$language"]['disconnect_cause']?>
		</CAPTION>
			<THEAD>
			<TR>
			<TD valign=top width='50%' bgcolor='#fffdf3'>
				<TABLE width='99%' cellpadding=1 cellspacing=1 border=0>
				<THEAD>
				<TR>
					<TH><?=$lang["$language"]['cause']?></TH>
					<TH><?=$lang["$language"]['count']?></TH>
					<TH><?=$lang["$language"]['percent']?></TH>
				</TR>
				</THEAD>
				<TBODY>
                <TR>
                  <TD><?=$lang["$language"]['user_abandon']?></TD>
			      <TD><?=$abandoned?> <?=$lang["$language"]['calls']?></TD>
			      <TD>
					  <?
						if($total_abandon > 0 ) {
							$percent=$abandoned*100/$total_abandon;
						} else {
							$percent=0;
						}
						$percent=number_format($percent,2);
						echo $percent;
                      ?>
                   <?=$lang["$language"]['percent']?></TD>
		        </TR>
			    <TR>
                  <TD><?=$lang["$language"]['timeout']?></TD>
			      <TD><?=$timeout?> <?=$lang["$language"]['calls']?></TD>
			      <TD>
					  <?
						if($total_abandon > 0 ) {
							$percent=$timeout*100/$total_abandon;
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
			<TD align=center bgcolor='#fffdf3'>
				<?
				$query2 = "var1=".$lang["$language"]['abandon']."&val1=".$abandoned."&";
				$query2 .= "var2=".$lang["$language"]['timeout']."&val2=".$timeout;
				$query2.="&title=".$lang["$language"]['disconnect_cause']."$graphcolor2";
				swf_bar($query2,350,211,"chart1",0);
				?>
			</TD>
			</TR>
			</THEAD>
			</TABLE>


		<?
		if(count($abandon_calls_queue)<=0) {
			$abandon_calls_queue[""]=0;
		}
		?>
			<a name='2'></a>
			<TABLE width='99%' cellpadding=3 cellspacing=3 border=0>
			<CAPTION>
			<a href='#0'><img src='images/go-up.png' border=0 width=16 height=16 class='icon'
			<?
			tooltip($lang["$language"]['gotop'],200);
			?>
			></a>&nbsp;&nbsp;
			<?=$lang["$language"]['unanswered_calls_qu']?>
			</CAPTION>
			<THEAD>
			<TR>
			<TD valign=top width='50%' bgcolor='#fffdf3'>
				<TABLE width='99%' cellpadding=1 cellspacing=1 border=0>
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
				asort($abandon_calls_queue);
				foreach($abandon_calls_queue as $key=>$val) {
					$cual = $countrow%2;
					if($cual>0) { $odd = " class='odd' "; } else { $odd = ""; }
					if($total_abandon > 0 ) {
						$percent = $val * 100 / $total_abandon;
					} else {
						$percent = 0;
					}
					$percent =number_format($percent,2);
					echo "<TR $odd><TD>$key</TD><TD>$val calls</TD><TD>$percent ".$lang["$language"]['percent']."</TD></TR>\n";
					$countrow++;
					$query2.="var$countrow=$key&val$countrow=$val&";
				}
				$query2.="title=".$lang["$language"]['unanswered_calls_qu']."$graphcolor";
				?>
			  </TBODY>
			  </TABLE>
			</TD>
			<TD valign=top width="50%" align=center bgcolor='#fffdf3'>
				<?
				//if ($countrow>1) {
			    	swf_bar($query2,350,211,"chart2",0);
			   	//}
               	?>
			</TD>
			</TR>
			</THEAD>
			</TABLE>
			<BR>
			<BR>
</div>
</div>
<script type="text/javascript" src="js/wz_tooltip.js"></script>
</body>
</html>
