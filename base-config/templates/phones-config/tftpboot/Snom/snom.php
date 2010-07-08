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


if(isset($_SERVER["HTTP_USER_AGENT"]) === true
&& preg_match("/(snom[0-9]{3})-/",$_SERVER["HTTP_USER_AGENT"],$match) === 1){
    $model = $match[1];
    $firmware_status = "http://#XIVO_NET4_IP#:8667/Snom/".$model."-firmware.xml";
    
    echo("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
    echo("<settings>\n");
    echo("<phone-settings>\n");
    echo("<pnp_config perm=\"R\">off</pnp_config>\n");
    echo("<update_policy perm=\"R\">auto_update</update_policy>\n");
    echo("<firmware_status perm=\"R\">".$firmware_status."</firmware_status>\n");
    echo("</phone-settings>\n");
    echo("</settings>\n");
}
else
{
    if(is_file("snom.htm") === true)
        include("snom.htm");
}

die();

?>
