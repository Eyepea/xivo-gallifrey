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

echo "<h2>" . $lang["$language"]['realtime_agentstatus'] . "</h2><br/>";

$color['unavailable']="#dadada";
$color['unknown']="#dadada";
$color['busy']="#d0303f";
$color['dialout']="#d0303f";
$color['not in use']="#00ff00";
$color['paused']="#000000";

foreach($queue as $qn) {
    if($filter=="" || stristr($qn,$filter)) {
        $contador=1;
        if(!isset($queues[$qn]['members'])) continue;
        foreach($queues[$qn]['members'] as $key=>$val) {
            $stat="";
            $last="";
            $dur="";
            $clid="";
            $akey = $queues[$qn]['members'][$key]['agent'];
            $aval = $queues[$qn]['members'][$key]['type'];
            if(array_key_exists($key,$inuse)) {
                if($aval=="not in use") {
                   $aval = "dialout";
                }
                if($channels[$inuse[$key]]['duration']=='') {
                   $newkey = $channels[$inuse[$key]]['bridgedto'];
                   $dur = $channels[$newkey]['duration_str'];
                   $clid = $channels[$newkey]['callerid'];
                } else {
                   $newkey = $channels[$inuse[$key]]['bridgedto'];
                   $clid = $channels[$newkey]['callerid'];
                   $dur = $channels[$inuse[$key]]['duration_str'];
                }
            }
            $stat = $queues[$qn]['members'][$key]['status'];
            $last = $queues[$qn]['members'][$key]['lastcall'];
            
            if(($aval == "unavailable" || $aval == "unknown") && $ocultar=="true") {
                 // Skip
            } else {
                if($contador==1) {
                   echo "<table width='700' cellpadding=3 cellspacing=3 border=0>\n";
                   echo "<thead>";
                   echo "<tr>";
                   echo "<th>" . $lang["$language"]['realtime_agentstatus_queue'] . "</th>";
                   echo "<th>" . $lang["$language"]['realtime_agentstatus_agent'] . "</th>";
                   echo "<th>" . $lang["$language"]['realtime_agentstatus_state'] . "</th>";
                   echo "<th>" . $lang["$language"]['realtime_agentstatus_duration'] . "</th>";
                   echo "<th>" . $lang["$language"]['realtime_agentstatus_clid'] . "</th>";
                   echo "<th>" . $lang["$language"]['realtime_agentstatus_lastincall'] . "</th>";
                   echo "</tr>\n";
                   echo "</thead><tbody>\n";
                }
                if($contador%2) { $odd="class='odd'"; } else { $odd=""; }
                if($last<>"") { $last=$last."  min. ago"; } else { $last = "no info available"; }
                $agent_name = agent_name($akey);
                echo "<tr $odd>";
                echo "<td width=200>$qn</td>";
                echo "<td width=200>$agent_name</td>";

                if($stat<>"") $aval="paused";

                if(!array_key_exists($key,$inuse)) {
                    if($aval=="busy") $aval="not in use";
                }
                echo "<td><div style='float: left; background: ".$color[$aval]."; width: 1em;'>&nbsp;</div>&nbsp; $aval ";
                echo "</td>";
                echo "<td>$dur</td>";
                echo "<td>$clid</td>";
                echo "<td>$last</td>";
                echo "</tr>";
                $contador++;
            }
        }
        if($contador>1) {
            echo "</tbody>";
            echo "</table><br/>\n";
        }
    } 
}
?>
