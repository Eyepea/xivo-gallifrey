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
require_once("sesvars.php");

$start_today = date('Y-m-d 00:00:00');
$end_today = date('Y-m-d 23:59:59');

$start_today_ts = return_timestamp($start_today);

$day = date('w',$start_today_ts);
$diff_to_monday = $start_today_ts - (($day - 1) * 86400);

// Start and End date for last week (it counts from the first monday back
// till the next sunday
$begin_week_monday = date('Y-m-d 00:00:00',$diff_to_monday);
$end_week_sunday   = date('Y-m-d 23:59:59',($diff_to_monday + (6 * 86400)));

$end_year = date('Y');

$begin_month = date('Y-m-01 00:00:00');
$begin_month_ts = return_timestamp($begin_month);
$end_month_ts = $begin_month_ts + (86400 * 32);


$end_past_month_ts = $begin_month_ts - 1;
$end_past_month =  date('Y-m-d 23:59:59',$end_past_month_ts);
$begin_past_month = date('Y-m-01 00:00:00',$end_past_month_ts);

$begin_past_month_ts = return_timestamp($begin_past_month);
$end_past2_month_ts = $begin_past_month_ts - 1;
$end_past2_month =  date('Y-m-d 23:59:59',$end_past2_month_ts);
$begin_past2_month = date('Y-m-01 00:00:00',$end_past2_month_ts);

for ($a=4; $a>0; $a--) {
   $day_number = date('d',$end_month_ts);
   if($day_number == 1) {
      $a==0;
   } else {
      $end_month_ts -= 86400;
   }
}
$end_month_ts -= 86400;

$end_month = date('Y-m-d',$end_month_ts);

// Get informations from XiVO in JSON
$xivo_user = get_xivo_json_info("https://127.0.0.1/service/ipbx/json.php/private/pbx_settings/users");
$xivo_agent = get_xivo_json_info("https://127.0.0.1/service/ipbx/json.php/private/call_center/agents");

$query = "SELECT queue FROM qname ORDER BY queue";
$res = consulta_db($query,0,0);
while ($row = db_fetch_row($res)) {
  $colas[] = $row[0];
}

if ($agent != "''") {
	$agents = explode(',', $agent);
	unset($agent);
	$agent = populate_agents($agents);
}

foreach ($xivo_agent as $agents)
{
	$agentes[] = array('interface'		=> 'Agent/'.$agents->number,
			   'fullname'		=> $agents->firstname.' '.$agents->lastname);
}

?>
<!-- http://devnull.tagsoup.com/quirksmode -->
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
	<title>Asternic Call Center Stats</title>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<style type="text/css" media="screen">@import "css/basic.css";</style>
	<style type="text/css" media="screen">@import "css/tab.css";</style>
	<style type="text/css" media="screen">@import "css/fixed-all.css";</style>
	<script src="js/validmonth.js" type="text/javascript" language="javascript1.2"></script>
	<script language="JavaScript">

	function List_move_around(direction, all, box) {

	    if (direction=="right") {
			if(box=="queues") {
        		box1 = "List_Queue_available";
	        	box2 = "List_Queue[]";
			} else {
    	    	box1 = "List_Agent_available";
	    	    box2 = "List_Agent[]";
			}
    	} else {
			if(box=="queues") {
    	    	box1 = "List_Queue[]";
	    	    box2 = "List_Queue_available" + "";
			} else {
    	    	box1 = "List_Agent[]";
	    	    box2 = "List_Agent_available" + "";
			}
    	}

	    for (var i=0;i<document.forms[0].elements[box1].length;i++) {
    	  	if ((document.forms[0].elements[box1][i].selected || all)) {
        	    document.forms[0].elements[box2].options[document.forms[0].elements[box2].length] =    new Option(document.forms[0].elements[box1].options[i].text, document.forms[0].elements[box1][i].value);
	            document.forms[0].elements[box1][i] = null;
    	        i--;
	        }
    	}
	return false;
	}

	function List_Queue_check_submit() {
       box = "List_Queue[]";
       if (document.forms[0].elements[box]) {
         for (var i=0;i<document.forms[0].elements[box].length;i++) {
            document.forms[0].elements[box][i].selected = true;
         }
       }
       box = "List_Agent[]";
       if (document.forms[0].elements[box]) {
         for (var i=0;i<document.forms[0].elements[box].length;i++) {
            document.forms[0].elements[box][i].selected = true;
         }
       }
      return true;
    }

	function envia() {

		List_Queue_check_submit();

 		box = "List_Queue[]";
       	if (document.forms[0].elements[box].length == 0) {
			alert("Please select at least one queue");
			return false;
		}
 		box = "List_Agent[]";
       	if (document.forms[0].elements[box].length == 0) {
			alert("Please select at least one Agent");
			return false;
		}

		month_start = parseInt(document.forms[0].month1.value) + 1;
		month_end   = parseInt(document.forms[0].month2.value) + 1;

		fecha_s  = document.forms[0].year1.value  + '-';
		if(String(month_start).length == 1) {
			fecha_s += "0";
		} 
        fecha_s += month_start + '-';
		if(String(document.forms[0].day1.value).length == 1) {
			fecha_s += "0";
		}
        fecha_s += document.forms[0].day1.value   + ' ';
		fecha_s += '00:00:00';

		fecha_check_s = document.forms[0].year1.value;
		if(String(month_start).length == 1) {
			fecha_check_s += "0";
		} 
		fecha_check_s += month_start;
		if(String(document.forms[0].day1.value).length == 1) {
			fecha_check_s += "0";
		}
		fecha_check_s += document.forms[0].day1.value;

		fecha_check_e = document.forms[0].year2.value;
		if(String(month_end).length == 1) {
			fecha_check_e += "0";
		} 
		fecha_check_e += month_end;
		if(String(document.forms[0].day2.value).length == 1) {
			fecha_check_e += "0";
		}
		fecha_check_e += document.forms[0].day2.value;

		fecha_e  = document.forms[0].year2.value  + '-';
		if(String(month_end).length == 1) {
			fecha_e += "0";
		} 
        fecha_e += month_end + '-';
		if(String(document.forms[0].day2.value).length == 1) {
			fecha_e += "0";
		}
        fecha_e += document.forms[0].day2.value   + ' ';
		fecha_e += '23:59:59';

		document.forms[0].start.value = fecha_s;
		document.forms[0].end.value   = fecha_e;

		if(fecha_check_e < fecha_check_s) {
			alert("<?=$lang["$language"]['invaliddate']?>");
		} else { 
		  document.forms[0].submit();
		}
		return false;
	}

	function setdates(start,end) {
		var start_year  = start.substr(0,4);
		var start_month = start.substr(5,2);
		var start_day   = start.substr(8,2);
	
		var end_year  = end.substr(0,4);
		var end_month = end.substr(5,2);
		var end_day   = end.substr(8,2);

		dstart = MWJ_findSelect( "day1" ), mstart = MWJ_findSelect( "month1" ), ystart = MWJ_findSelect( "year1" );
		dend   = MWJ_findSelect( "day2" ), mend   = MWJ_findSelect( "month2" ), yend   = MWJ_findSelect( "year2" );

		while( dstart.options.length ) { dstart.options[0] = null; }
		while( dend.options.length   ) { dend.options[0]   = null; }

		for( var x = 0; x < 31; x++  ) { dstart.options[x] = new Option( x + 1, x + 1 ); }
		for( var x = 0; x < 31; x++  ) { dend.options[x]   = new Option( x + 1, x + 1 ); }

		x = start_day - 1;
		y = end_day - 1;
	    dstart.options[x].selected = true;
	    dend.options[y].selected = true;
		
		x = start_month - 1;
		y = end_month - 1;
		mstart.options[x].selected = true;
		mend.options[y].selected   = true;

		for( var x = 0; x < ystart.options.length; x++ ) { 
			if( ystart.options[x].value == '' + start_year + '' ) { 
				ystart.options[x].selected = true; 
				if( window.opera && document.importNode ) { 
					window.setTimeout('MWJ_findSelect( \''+ystart.name+'\' ).options['+x+'].selected = true;',0); 
				} 
			} 
		}
		for( var x = 0; x < yend.options.length; x++ ) { 
			if( yend.options[x].value == '' + end_year + '' ) { 
				yend.options[x].selected = true; 
				if( window.opera && document.importNode ) { 
					window.setTimeout('MWJ_findSelect( \''+yend.name+'\' ).options['+x+'].selected = true;',0); 
				} 
			} 
		}

	}
	</script>
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
<div id='wrapper'>
<?include("menu.php");?>
<div id="main">
	<div id="contents">

<form method=POST action='answered.php'>
<input type="hidden" name="start">
<input type="hidden" name="end">


<div id='left'>
<h2>
<?=$lang["$language"]['select_queue']?>
</h2>
<BR>
<? 
	function remove_quotes($argument)
	{
		return substr($argument,1,-1);
	}
	$items_cola = explode(",",$queue);
	$items_cola = array_map("remove_quotes",$items_cola);

	$items_agente = $agent;

?>

<table border="0" cellspacing="0" cellpadding="8">
<tr>
   <td>
	<?=$lang["$language"]['available']?><BR>
    <select name="List_Queue_available" multiple="multiple" id="myform_List_Queue_from" size=10 style="height: 100px;width: 125px;" onDblClick="List_move_around('right',false,'queues');" >
		<?	

	foreach($colas as $queueel) {
		if($queueel <> "NONE" && !in_array($queueel,$items_cola)) {
   			echo "<option value=\"'$queueel'\">$queueel</option>\n";
		}
	}
	?>
    </select>
</td>
<td align="left">
		<a href='#' onclick="List_move_around('right',false,'queues'); return false;"><img src='images/go-next.png' width=16 height=16 border=0></a>
		<a href='#' onclick="List_move_around('left', false,'queues'); return false;"><img src='images/go-previous.png' width=16 height=16 border=0></a>
		<br>
		<br>
		<a href='#' onclick="List_move_around('right', true,'queues'); return false;"><img src='images/go-last.png' width=16 height=16 border=0></a>
		<a href='#' onclick="List_move_around('left', true,'queues'); return false;"><img src='images/go-first.png' width=16 height=16 border=0></a>
</td>
<td>
	<?=$lang["$language"]['selected']?><BR>
    <select size=10 name="List_Queue[]" multiple="multiple" style="height: 100px;width: 125px;" id="myform_List_Queue_to" onDblClick="List_move_around('left',false,'queues');" >
		<?
		foreach($items_cola as $queueel) {
			if($queueel <> "NONE") {
   				echo "<option value=\"'$queueel'\">$queueel</option>\n";
			}
		}
		?>
    </select>
   </td>
</tr> 
</table>

</div>
<div id='right'>
<h2>
<?=$lang["$language"]['select_agent']?>

</h2>
<BR>

<?
	foreach($agentes as $agentkey => $agentel) 
	{
		foreach($agent as $g)
		{
			if($agentel['interface'] === $g['interface'])
				unset($agentes[$agentkey]);
		}
	}
?>

<table border="0" cellspacing="0" cellpadding="8">
<tr>
   <td>
	<?=$lang["$language"]['available']?><BR>
    <select size=10 name="List_Agent_available" multiple="multiple" id="myform_List_Agent_from" style="height: 100px;width: 125px;" onDblClick="List_move_around('right',false,'agents');" >
		<?	

	foreach($agentes as $agentel) {
   		echo '<option value="',$agentel['interface'],'">',$agentel['fullname'],'</option>',"\n";
	}
	?>
    </select>
</td>
<td align="left">
		<a href='#' onclick="List_move_around('right',false,'agents'); return false;"><img src='images/go-next.png' width=16 height=16 border=0></a>
		<a href='#' onclick="List_move_around('left', false,'agents'); return false;"><img src='images/go-previous.png' width=16 height=16 border=0></a>
		<br>
		<br>
		<a href='#' onclick="List_move_around('right', true,'agents'); return false;"><img src='images/go-last.png' width=16 height=16 border=0></a>
		<a href='#' onclick="List_move_around('left', true,'agents'); return false;"><img src='images/go-first.png' width=16 height=16 border=0></a>
</td>
<td>
	<?=$lang["$language"]['selected']?><BR>
    <select size=10 name="List_Agent[]" multiple="multiple" style="height: 100px;width: 125px;" id="myform_List_Agent_to" onDblClick="List_move_around('left',false,'agents');" >
		<?
		if(is_array($agent)) {
			foreach($items_agente as $agentel) {
				echo '<option value="',$agentel['interface'],'">',$agentel['fullname'],'</option>',"\n";
			}
		}
		?>
    </select>
   </td>
</tr> 
</table>



</div>
<div style="clear: both;">&nbsp;</div>
<div id='rest'>
<h2><?=$lang["$language"]['select_timeframe']?></h2>
<h3><?=$lang["$language"]['shortcuts']?></h3>
<?
echo "<a href=\"javascript:setdates('$start_today', '$end_today')\">".$lang["$language"]['today']."</a> | ";
echo "<a href=\"javascript:setdates('$begin_week_monday', '$end_week_sunday')\">".$lang["$language"]['this_week']."</a> | ";
echo "<a href=\"javascript:setdates('$begin_month', '$end_month')\">".$lang["$language"]['this_month']."</a> | ";
echo "<a href=\"javascript:setdates('$begin_past2_month', '$end_month')\">".$lang["$language"]['last_three_months']."</a><BR>";
?>
<BR>
<TABLE>
<TR>
<TD><?=$lang["$language"]['start']?></TD>
<TD>
        <select name="day1" size="1">
		<? 
		for($a=1;$a<32;$a++) {
			echo "<option value='$a' ";
			if($fstart_day == $a) { echo " selected "; }
			echo ">$a</option>\n";
		}
		?>
        </select>

        <select name="month1" size="1" onchange="dateChange('day1','month1','year1');">
		<?
		for($a=0;$a<12;$a++)
		{
		$amonth = $a+1;
        echo "<option value='$a' ";
		if ($fstart_month == $amonth) { echo "selected "; }
		echo ">$yearp[$a]</option>\n";
		}
		?>
        </select>
	
		<?
		$start_year = $end_year - 5;
		$super_start_year = $start_year - 50;
		$super_end_year   = $end_year + 5;
        echo "<select name='year1' size='1' onchange=\"checkMore( this, $start_year, $end_year, $super_start_year, $super_end_year );dateChange('day1','month1','year1');\">\n";
		echo "<option value=\"MWJ_DOWN\">".$lang["$language"]['lower']."</option>\n";
		for($a=$start_year;$a<=$end_year;$a++)
		{
        	echo "<option value='$a' ";
			if ($fstart_year == $a) { echo "selected "; }
			echo ">$a</option>\n";
		}
		echo "<option value=\"MWJ_UP\">".$lang["$language"]['higher']."</option>\n";
		?>
        </select>
</TD></TR>
<TR>
<TD><?=$lang["$language"]['end']?></TD>
<TD>
        <select name="day2" size="1">
		<? 
		for($a=1;$a<32;$a++) {
			echo "<option value='$a' ";
			if($fend_day == $a) { echo " selected "; }
			echo ">$a</option>\n";
		}
		?>
        </select>

        <select name="month2" size="1" onchange="dateChange('day2','month2','year2');">
		<?
		for($a=0;$a<12;$a++)
		{
		$amonth = $a+1;
        echo "<option value='$a' ";
		if ($fend_month == $amonth) { echo "selected "; }
		echo ">$yearp[$a]</option>\n";
		}
		?>
        </select>
	
		<?
		$start_year = $end_year - 5;
		$super_start_year = $start_year - 50;
		$super_end_year   = $end_year + 5;
        echo "<select name='year2' size='1' onchange=\"checkMore( this, $start_year, $end_year, $super_start_year, $super_end_year );dateChange('day2','month2','year2');\">\n";
		echo "<option value=\"MWJ_DOWN\">".$lang["$language"]['lower']."</option>\n";
		for($a=$start_year;$a<=$end_year;$a++)
		{
        	echo "<option value='$a' ";
			if ($fend_year == $a) { echo "selected "; }
			echo ">$a</option>\n";
		}
		echo "<option value=\"MWJ_UP\">".$lang["$language"]['higher']."</option>\n";
		?>
        </select>
</TD></TR>
<TR>
<TD colspan=2>
</TD>
</TR>
</TABLE>
<BR>
<INPUT TYPE=submit VALUE='<?=$lang["$language"]['display_report']?>' onClick='return envia();'>
</form>
		</div>
	</div>
</div>
</div>
</div>
<div id='footer'>Asternic Asterisk Tools</a> Licensed under <a href='http://www.opensource.org/licenses/gpl-3.0.html'>GPL3</a></div>
</body>
</html>
