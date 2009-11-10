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

/* ARP protocol HARDWARE identifiers.			*/
$array[0] = 'netrom';			/* from KA9Q: NET/ROM pseudo		*/
$array[1] = 'ether';			/* Ethernet 10Mbps			*/
$array[2] = 'eether';			/* Experimental Ethernet		*/
$array[3] = 'ax25';			/* AX.25 Level 2			*/
$array[4] = 'pronet';			/* PROnet token ring			*/
$array[5] = 'chaos';			/* Chaosnet				*/
$array[6] = 'ieee802';			/* IEEE 802.2 Ethernet/TR/TB		*/
$array[7] = 'arcnet';			/* ARCnet				*/
$array[8] = 'appletlk';			/* APPLEtalk				*/
$array[15] = 'dlci';			/* Frame Relay DLCI			*/
$array[19] = 'atm';			/* ATM 					*/
$array[23] = 'metricom';		/* Metricom STRIP (new IANA id)		*/
$array[24] = 'ieee1394';		/* IEEE 1394 IPv4 - RFC 2734		*/
$array[27] = 'eui64';			/* EUI-64				*/
$array[32] = 'infiniband';		/* InfiniBand				*/

/* Dummy types for non ARP hardware			*/
$array[256] = 'slip';
$array[257] = 'cslip';
$array[258] = 'slip6';
$array[259] = 'cslip6';
$array[260] = 'rsrvd';			/* Notional KISS type			*/
$array[264] = 'adapt';
$array[270] = 'rose';
$array[271] = 'x25';			/* CCITT X.25				*/
$array[272] = 'hwx25';			/* Boards with X.25 in firmware		*/
$array[280] = 'can';			/* Controller Area Network		*/
$array[512] = 'ppp';
$array[513] = 'cisco';			/* Cisco HDLC				*/
$array[513] = 'hdlc';
$array[516] = 'lapb';			/* LAPB					*/
$array[517] = 'ddcmp';			/* Digital's DDCMP protocol		*/
$array[518] = 'rawhdlc';		/* Raw HDLC				*/
$array[768] = 'tunnel';			/* IPIP tunnel				*/
$array[769] = 'tunnel6';		/* IP6IP6 tunnel			*/
$array[770] = 'frad';			/* Frame Relay Access Device		*/
$array[771] = 'skip';			/* SKIP vif				*/
$array[772] = 'loopback';		/* Loopback device			*/
$array[773] = 'localtlk';		/* Localtalk device			*/
$array[774] = 'fddi';			/* Fiber Distributed Data Interface	*/
$array[775] = 'bif';			/* AP1000 BIF				*/
$array[776] = 'sit';			/* sit0 device - IPv6-in-IPv4		*/
$array[777] = 'ipddp';			/* IP over DDP tunneller		*/
$array[778] = 'ipgre';			/* GRE over IP				*/
$array[779] = 'pimreg';			/* PIMSM register interface		*/
$array[780] = 'hippi';			/* High Performance Parallel Interface	*/
$array[781] = 'ash';			/* Nexus 64Mbps Ash			*/
$array[782] = 'econet';			/* Acorn Econet				*/
$array[783] = 'irda';			/* Linux-IrDA				*/

/* ARP works differently on different FC media .. so	*/
$array[784] = 'fcpp';			/* Point to point fibrechannel		*/
$array[785] = 'fcal';			/* Fibrechannel arbitrated loop		*/
$array[786] = 'fcpl';			/* Fibrechannel public loop		*/
$array[787] = 'fcfabric';		/* Fibrechannel fabric			*/

/* 787->799 reserved for fibrechannel media types	*/

$array[800] = 'ieee802_tr';		/* Magic type ident for TR		*/
$array[801] = 'ieee80211';		/* IEEE 802.11				*/
$array[802] = 'ieee80211_prism';	/* IEEE 802.11 + Prism2 header		*/
$array[803] = 'ieee80211_radiotap';	/* IEEE 802.11 + radiotap header	*/

$array[0xFFFF] = 'void';		/* Void type, nothing is known		*/
$array[0xFFFE] = 'none';		/* zero header length			*/

?>
