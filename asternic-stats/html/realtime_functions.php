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

function get_queue_numbers($qn) {
    global $queues;
    $agents_logedof=0;
    $agents_busy=0;
    $agents_paused=0;
    $llamadas = 0;
    $llamadas = $queues[$qn]['ncalls'];
    if(isset($queues[$qn]['calls'])) {
        $maxwait = $queues[$qn]['calls'][0]['chaninfo']['duration_str'];
    } else {
        $maxwait = 0;
    }

    $staffed=0;
    $paused=0;
    $ready=0;
    if(isset($queues[$qn]['members'])) {
        foreach($queues[$qn]['members'] as $key=>$val) {
            foreach($val as $type => $valor) {
                if($type=='type') {
                    if($valor == "unknown") {
                        $agents_logedof++;
                    } 
                    if($valor == "unavailable") {
                        $agents_logedof++;
                    }
                    if($valor == "busy") {
                        $agents_busy++;
                        $staffed++;
                    } 
                    if($valor == "not in use") {
                        $ready++;
                        $staffed++;
                    }
                }
                if($type=='status' && $valor=="paused") {
                    $paused++;
                }
            }
        }
    }
    if(!isset($llamadas)) $llamadas = 0;
    $return["agents_logedof"]=$agents_logedof;
    $return["agents_busy"]=$agents_busy;
    $return["agents_ready"]=$staffed;
    $return["agents_paused"]=$paused;
    $return["settext"]=$llamadas;
    $return["maxwait"]=$maxwait;
    return $return;
}

function get_channels($am) {
    $res=$am->Command("show channels concise");
    $res=$res['data'];
    $responselines=split("\n",$res);
    $lines=array();

    foreach($responselines as $l) {
        if (preg_match("/^Response/",$l)) continue;
        if (preg_match("/^Privilege/",$l)) continue;
        if (preg_match("/^$/",$l)) break;

        $lines[]=$l;
    }

    $channels=array();
    foreach($lines as $l) {
        $chan=split("!",$l);
        if (count($chan)==1)
            $chan=split(":",$l);
        $ci=array();
        $ci['channel']=$chan[0];
        $ci['context']=$chan[1];
        $ci['exten']=$chan[2];
        $ci['priority']=$chan[3];
        $ci['state']=$chan[4];
        $ci['application']=$chan[5];
        $ci['applicationdata']=$chan[6];
        $ci['callerid']=$chan[7];
        $ci['accountcode']=$chan[8];
        $ci['amaflags']=$chan[9];
        $ci['duration']=$chan[10];
        $ci['bridgedto']=$chan[11];

        $dur=$ci['duration']+0;
        $durstr=sprintf("%d:%02d",$dur/60,$dur%60);
        $ci['duration_str']=$durstr;

        $channels[$chan[0]]=$ci;
    }

    return $channels;    
}


function get_queues($am,$channels) {
    $res=$am->Command("show queues");
    $res=$res['data']; 
    $lines=split("\n",$res);

    $queue=null;
    $data=null;
    $reading=null;
      
    foreach ($lines as $l) {
//        echo "line ($l)<BR>";
        if (is_null($queue) && preg_match("/^(\S+)\s+has\s(\d+)/",$l,$matches)) {
            $queue=$matches[1];
            $data=array();
            $data['ncalls']=$matches[2];
            continue;
        } 
        if (!is_null($queue) && $l=="") {
            //Grabamos esta cola
            $queuelist[$queue]=$data;
            $queue=null;
            $data=null;
            $reading=null;
            continue;
        } 

        if (is_null($reading) && preg_match("/Members:/",$l)) {
            $reading="members";
            continue;
        }

        if ($reading=="members" && preg_match("/^\s+(\S+)\s+[^\(]*\(/",$l,$matches)) {
            $member=$matches[1];
	        $member = convertlocal($member);
            $status="";
            $seconds="";
            if(preg_match("/\(Unavailable\)/",$l)) {
                $tipo="unavailable";
            } elseif(preg_match("/\(Not in use\)/",$l)) {
                $tipo="not in use";
            } elseif(preg_match("/\(Busy\)/",$l) || preg_match("/\(In use\)/",$l)) {
                $tipo="busy";
            } else {
                $tipo="unknown";
                $tipo="not in use";
            } 
            if(preg_match("/paused/",$l)) {
                $status="paused";
            }
            if(preg_match("/last was/",$l)) {
                $partes = split("last was",$l,2);
                $seconds = $partes[1];
                preg_match("/(\d+)/",$seconds,$matches);
                $seconds = seconds2minutes($matches[1]);
            }
            $agentenumber = ereg_replace("Agent/","",$member);
            $mem['id']=$member;
            $mem['agent']=$agentenumber;
            $mem['type']=$tipo;
            $mem['status']=$status;
            $mem['lastcall']=$seconds;
            $data['members'][$member]=$mem;
            continue;
	}

        if (preg_match("/Callers:/",$l)) {
            $reading="callers";
            continue;
        }
        if ($reading=="callers" && preg_match("/^\s+\d+\.\s+(\S+)\s+\(wait:\s*([\d:]+),\s*prio:\s*(\d+)/",$l,$matches)) {
            $callinfo=array();
            $callinfo['channel']=$matches[1];
            $callinfo['chaninfo']=$channels[$matches[1]];
            $callinfo['waittime']=$matches[2];
            $callinfo['prio']=$matches[3];
            $data['calls'][]=$callinfo;
            continue;
        } else if ($reading=="callers") {
            $reading=null;
        }
    }
    return $queuelist;
}

function seconds2minutes($segundos) {
    $minutos = intval($segundos/60);
    $segundos = $segundos % 60;
    if(strlen($segundos)==1) {
        $segundos = "0".$segundos;
    }
    return "$minutos:$segundos";
}

function convertlocal($agent) {
    $agent = preg_replace("/^Local/","SIP",$agent);
    $agent = preg_replace("/(.*)(@from.*)/","$1",$agent);
    return $agent;
}

function agent_name($channel) {
    list ($nada,$number) = split("/",$channel,2);
	if(is_file("agents.txt")) {
        $lineas = file("agents.txt");
        foreach ($lineas as $linea_num => $linea) {
            list ($num,$nombre) = split(",",$linea,2);
            if($num==$number) {
                return "$num - $nombre";
            }
        }
    }
    return $channel;
}
?>
