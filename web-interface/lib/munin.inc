<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

require_once(DWHO_PATH_ROOT.DIRECTORY_SEPARATOR.'file.inc');


function dwho_munin_graphstree($urldir, $domain)
{
    $classifier = array(
        'apache_.*'       => 'apache',
        'asterisk_.*'     => 'asterisk',
        'df.*'            => 'disk', 
        'iostat'          => 'disk',
        'smart_.*'        => 'disk',
        'hddtemp_.*'      => 'disk',
        'if_.*'           => 'network',
        'netstat'         => 'network',
        'ip_.*'           => 'network',
        'vlan_.*'         => 'network',
        'postfix_.*'      => 'postfix',
        'forks'           => 'processes',
        'processes'       => 'processes',
        'vmstat'          => 'processes',
        'cpu'             => 'system', 
        'entropy'         => 'system', 
        'interrupts'      => 'system', 
        'irqstats'        => 'system', 
        'load'            => 'system', 
        'memory'          => 'system',
        'open_.*'         => 'system', 
        'open_.*'         => 'system', 
        'swap'            => 'system',
        'ntp_.*'          => 'time',
        'mysql_.*'        => 'mysql'
    );


    $basedir = dwho_file::joinpath(XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'www'.DIRECTORY_SEPARATOR.$urldir);
    $r = dwho_file::read_d($basedir,'file',FILE_R_OK,'/day.png$/');
    sort($r);
    
    $tree = array();
    foreach($r as $graph)
    {
        list($xivo, $domain, $type, $freq, $fmt)= split("[\.-]", $graph);
        
        $cat = null;
        foreach($classifier as $key => $value)
        {
            if(preg_match("/^$key$/", $type) > 0)
            { $cat = $value; break; }
        }

        if(is_null($cat))
            $cat = 'other';
            
        if(!array_key_exists($cat, $tree))
            $tree[$cat] = array();

        $tree[$cat][] = $type;
    }

#    var_dump($tree);
    return($tree);
}

?>
