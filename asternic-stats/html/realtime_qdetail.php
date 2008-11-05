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


echo "<BR><h2>". $lang["$language"]['realtime_callwaiting_detail'] ."</h2><BR>";

foreach($queue as $qn) {
    $position=1;
    if(!isset($queues[$qn]['calls']))  continue;
    foreach($queues[$qn]['calls'] as $key=>$val) {
        if($position==1) {
            echo "<table width='700' cellpadding=3 cellspacing=3 border=0 class='sortable' id='table1' >\n";
            echo "<thead>";
            echo "<tr>";
            echo "<th>" . $lang["$language"]['realtime_callwaiting_queue'] . "</th>";
            echo "<th>" . $lang["$language"]['realtime_callwaiting_position'] . "</th>";
            echo "<th>" . $lang["$language"]['realtime_callwaiting_callerid'] . "</th>";
            echo "<th>" . $lang["$language"]['realtime_callwaiting_waitime'] . "</th>";
            echo "</tr>\n";
            echo "</thead>\n";
            echo "<tbody>\n";
        }
        echo "<tr><td>$qn</td><td>$position</td>";
        echo "<td>".$queues[$qn]['calls'][$key]['chaninfo']['callerid']."</td>";
        echo "<td>".$queues[$qn]['calls'][$key]['chaninfo']['duration_str']." min</td>";
        echo "</tr>";
        $position++;
    }
    if($position>1) {
          echo "</tbody>\n";
          echo "</table>\n";
    }
}
?>

