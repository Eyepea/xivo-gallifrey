<?php

#
# XiVO Web-Interface
# Copyright (C) 2009  Proformatique <technique@proformatique.com>
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

$array = array();

/* Standard interface flags. */
$array['up']		= 0x1;		/* interface is up				*/
$array['broadcast']	= 0x2;		/* broadcast address valid			*/
$array['debug']		= 0x4;		/* turn on debugging				*/
$array['loopback']	= 0x8;		/* is a loopback net				*/
$array['pointopoint']	= 0x10;		/* interface is has p-p link			*/
$array['notrailers']	= 0x20;		/* avoid use of trailers			*/
$array['running']	= 0x40;		/* interface RFC2863 OPER_UP			*/
$array['noarp']		= 0x80;		/* no ARP protocol				*/
$array['promisc']	= 0x100;	/* receive all packets				*/
$array['allmulti']	= 0x200;	/* receive all multicast packets		*/

$array['master']	= 0x400;	/* master of a load balancer			*/
$array['slave']		= 0x800;	/* slave of a load balancer			*/

$array['multicast']	= 0x1000;	/* Supports multicast				*/

$array['portsel']	= 0x2000;	/* can set media type				*/
$array['automedia']	= 0x4000;	/* auto media select active			*/
$array['dynamic']	= 0x8000;	/* dialup device with changing addresses	*/

$array['lower_up']	= 0x10000;	/* driver signals L1 up				*/
$array['dormant']	= 0x20000;	/* driver signals dormant			*/

$array['echo']		= 0x40000;	/* echo sent packets				*/

$array['volatile']	= ($array['loopback'] |
			   $array['pointopoint'] |
			   $array['broadcast'] |
			   $array['echo'] |
			   $array['master'] |
			   $array['slave'] |
			   $array['running'] |
			   $array['lower_up'] |
			   $array['dormant']);

?>
