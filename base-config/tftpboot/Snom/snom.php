<?php

/*
 * XIVO Configuration
 * Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

if(isset($_GET['mac']) === true
&& preg_match('/^[A-F0-9]{12}$/',strval($_GET['mac']),$match) === 1)
        $macaddr = $match[0];
else
        $macaddr = '';

if(isset($_SERVER['HTTP_USER_AGENT']) === true
&& preg_match('/(snom[0-9]{3})-/',$_SERVER['HTTP_USER_AGENT'],$match) === 1){
        $type = $match[1];
}
else
{
        if(is_file('snom.htm') === true)
                include('snom.htm');
        die();
}

if(isset($macaddr{0}) === true)
{
        print "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n <settings>\n";
        $filename = $type.'-'.$macaddr.'.xml';
        if(is_file($filename) === true)
                print file_get_contents($filename);
        else
        {
                $filename = $type.'.xml';
                if(is_file($filename) === true)
                        print file_get_contents($filename);
        }

        print "</settings>";
}

die();

?>
