#!/usr/bin/php -q
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

require_once("/etc/pf-asternic-stats/parselog_config.php");

if($argv[1]=="purge") {
  echo "Purging tables...\n";
  $query = "DELETE FROM qname";
  $res = consulta_db($query,0,0);
  $query = "DELETE FROM qagent";
  $res = consulta_db($query,0,0);
  $query = "DELETE FROM queue_stats";
  $res = consulta_db($query,0,0);
  echo "Done...\n";
exit;
}

// Select the most recent event saved
$query = "SELECT datetime FROM queue_stats ORDER BY datetime DESC LIMIT 1";
$res = consulta_db($query,0,0);

if(db_num_rows($res)>0) {
	$row = db_fetch_row($res);
	mysql_free_result($res);
	$last_event_ts = return_timestamp($row[0]);
	$last_event_ts -= 10;
} else {
	$last_event_ts = 0;
}

//$last_event_ts = 0;

// Populates an array with the EVENTS ids
$query = "SELECT * FROM qevent ORDER BY event_id";
$res = consulta_db($query,0,0);
while($row = db_fetch_row($res)) {
	$event_array["$row[1]"] = $row[0];
}

$filename = "$queue_log_dir/$queue_log_file";
$dataFile = fopen( $filename, "r" );

if ( $dataFile ) {
	while (!feof($dataFile)) {
		$buffer = fgets($dataFile, 4096);
		procesa($buffer);
	}
	fclose($dataFile);
}
else {
	die( "fopen failed for $filename" ) ;
}

?>
