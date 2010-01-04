<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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

$array['element'] = array();

$array['element']['meetmeid'] = array();
$array['element']['meetmeid']['default'] = '';

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['admin_typefrom'] = array();
$array['element']['admin_typefrom']['value'] = array('none','internal','external','undefined');
$array['element']['admin_typefrom']['default'] = 'none';

$array['element']['admin_internalid'] = array();
$array['element']['admin_internalid']['default'] = '';
$array['element']['admin_internalid']['null'] = true;

$array['element']['admin_externalid'] = array();
$array['element']['admin_externalid']['default'] = '';
$array['element']['admin_externalid']['null'] = true;

$array['element']['admin_identification'] = array();
$array['element']['admin_identification']['value'] = array('calleridnum','pin','all');
$array['element']['admin_identification']['default'] = 'pin';
$array['element']['admin_identification']['setdef'] = true;

$array['element']['admin_mode'] = array();
$array['element']['admin_mode']['value'] = array('listen','talk','all');
$array['element']['admin_mode']['default'] = 'all';
$array['element']['admin_mode']['setdef'] = true;

$array['element']['admin_announceusercount'] = array();
$array['element']['admin_announceusercount']['value'] = array(0,1);
$array['element']['admin_announceusercount']['default'] = 0;
$array['element']['admin_announceusercount']['set'] = true;

$array['element']['admin_announcejoinleave'] = array();
$array['element']['admin_announcejoinleave']['value'] = array('no','yes','noreview');
$array['element']['admin_announcejoinleave']['default'] = 'no';
$array['element']['admin_announcejoinleave']['setdef'] = true;

$array['element']['admin_moderationmode'] = array();
$array['element']['admin_moderationmode']['value'] = array(0,1);
$array['element']['admin_moderationmode']['default'] = 0;
$array['element']['admin_moderationmode']['set'] = true;

$array['element']['admin_initiallymuted'] = array();
$array['element']['admin_initiallymuted']['value'] = array(0,1);
$array['element']['admin_initiallymuted']['default'] = 0;
$array['element']['admin_initiallymuted']['set'] = true;

$array['element']['admin_musiconhold'] = array();
$array['element']['admin_musiconhold']['default'] = XIVO_SRE_IPBX_AST_MOH_DEFAULT;
$array['element']['admin_musiconhold']['null'] = true;

$array['element']['admin_poundexit'] = array();
$array['element']['admin_poundexit']['value'] = array(0,1);
$array['element']['admin_poundexit']['default'] = 0;
$array['element']['admin_poundexit']['set'] = true;

$array['element']['admin_quiet'] = array();
$array['element']['admin_quiet']['value'] = array(0,1);
$array['element']['admin_quiet']['default'] = 0;
$array['element']['admin_quiet']['set'] = true;

$array['element']['admin_starmenu'] = array();
$array['element']['admin_starmenu']['value'] = array(0,1);
$array['element']['admin_starmenu']['default'] = 0;
$array['element']['admin_starmenu']['set'] = true;

$array['element']['admin_closeconflastmarkedexit'] = array();
$array['element']['admin_closeconflastmarkedexit']['value'] = array(0,1);
$array['element']['admin_closeconflastmarkedexit']['default'] = 0;
$array['element']['admin_closeconflastmarkedexit']['set'] = true;

$array['element']['admin_enableexitcontext'] = array();
$array['element']['admin_enableexitcontext']['value'] = array(0,1);
$array['element']['admin_enableexitcontext']['default'] = 0;
$array['element']['admin_enableexitcontext']['set'] = true;

$array['element']['admin_exitcontext'] = array();
$array['element']['admin_exitcontext']['default'] = '';
$array['element']['admin_exitcontext']['null'] = true;

$array['element']['user_mode'] = array();
$array['element']['user_mode']['value'] = array('listen','talk','all');
$array['element']['user_mode']['default'] = 'all';
$array['element']['user_mode']['setdef'] = true;

$array['element']['user_announceusercount'] = array();
$array['element']['user_announceusercount']['value'] = array(0,1);
$array['element']['user_announceusercount']['default'] = 0;
$array['element']['user_announceusercount']['set'] = true;

$array['element']['user_hiddencalls'] = array();
$array['element']['user_hiddencalls']['value'] = array(0,1);
$array['element']['user_hiddencalls']['default'] = 0;
$array['element']['user_hiddencalls']['set'] = true;

$array['element']['user_announcejoinleave'] = array();
$array['element']['user_announcejoinleave']['value'] = array('no','yes','noreview');
$array['element']['user_announcejoinleave']['default'] = 'no';
$array['element']['user_announcejoinleave']['setdef'] = true;

$array['element']['user_initiallymuted'] = array();
$array['element']['user_initiallymuted']['value'] = array(0,1);
$array['element']['user_initiallymuted']['default'] = 0;
$array['element']['user_initiallymuted']['set'] = true;

$array['element']['user_musiconhold'] = array();
$array['element']['user_musiconhold']['default'] = XIVO_SRE_IPBX_AST_MOH_DEFAULT;
$array['element']['user_musiconhold']['null'] = true;

$array['element']['user_poundexit'] = array();
$array['element']['user_poundexit']['value'] = array(0,1);
$array['element']['user_poundexit']['default'] = 0;
$array['element']['user_poundexit']['set'] = true;

$array['element']['user_quiet'] = array();
$array['element']['user_quiet']['value'] = array(0,1);
$array['element']['user_quiet']['default'] = 0;
$array['element']['user_quiet']['set'] = true;

$array['element']['user_starmenu'] = array();
$array['element']['user_starmenu']['value'] = array(0,1);
$array['element']['user_starmenu']['default'] = 0;
$array['element']['user_starmenu']['set'] = true;

$array['element']['user_enableexitcontext'] = array();
$array['element']['user_enableexitcontext']['value'] = array(0,1);
$array['element']['user_enableexitcontext']['default'] = 0;
$array['element']['user_enableexitcontext']['set'] = true;

$array['element']['user_exitcontext'] = array();
$array['element']['user_exitcontext']['default'] = '';
$array['element']['user_exitcontext']['null'] = true;

$array['element']['talkeroptimization'] = array();
$array['element']['talkeroptimization']['value'] = array(0,1);
$array['element']['talkeroptimization']['default'] = 1;
$array['element']['talkeroptimization']['set'] = true;
$array['element']['talkeroptimization']['forcedef'] = true;

$array['element']['record'] = array();
$array['element']['record']['value'] = array(0,1);
$array['element']['record']['default'] = 0;
$array['element']['record']['set'] = true;

$array['element']['talkerdetection'] = array();
$array['element']['talkerdetection']['value'] = array(0,1);
$array['element']['talkerdetection']['default'] = 1;
$array['element']['talkerdetection']['set'] = true;
$array['element']['talkerdetection']['forcedef'] = true;

$array['element']['noplaymsgfirstenter'] = array();
$array['element']['noplaymsgfirstenter']['value'] = array(0,1);
$array['element']['noplaymsgfirstenter']['default'] = 0;
$array['element']['noplaymsgfirstenter']['set'] = true;

$array['element']['durationm'] = array();
$array['element']['durationm']['default'] = '';
$array['element']['durationm']['null'] = true;

$array['element']['closeconfdurationexceeded'] = array();
$array['element']['closeconfdurationexceeded']['value'] = array(0,1);
$array['element']['closeconfdurationexceeded']['default'] = 0;
$array['element']['closeconfdurationexceeded']['set'] = true;

$array['element']['maxuser'] = array();
$array['element']['maxuser']['default'] = '';
$array['element']['maxuser']['null'] = true;

$array['element']['startdate'] = array();
$array['element']['startdate']['default'] = '';
$array['element']['startdate']['null'] = true;

$array['element']['startdatemday'] = array();
$array['element']['startdatemday']['value'] = range(1,31);
$array['element']['startdatemday']['default'] = '';
$array['element']['startdatemday']['unset'] = true;

$array['element']['startdatemon'] = array();
$array['element']['startdatemon']['value'] = array(1	=> 'jan',
						   2	=> 'feb',
						   3	=> 'mar',
						   4	=> 'apr',
						   5	=> 'may',
						   6	=> 'jun',
						   7	=> 'jul',
						   8	=> 'aug',
						   9	=> 'sep',
						   10	=> 'oct',
						   11	=> 'nov',
						   12	=> 'dec');
$array['element']['startdatemon']['default'] = '';
$array['element']['startdatemon']['unset'] = true;

$array['element']['startdateyear'] = array();
$array['element']['startdateyear']['value'] = array(strftime('%Y',time() + 3600),
						    strftime('%Y',mktime(date('H'),
						    			 date('i'),
									 date('s'),
									 date('n'),
									 date('j'),
									 date('Y') + 1)),
						    strftime('%Y',mktime(date('H'),
									 date('i'),
									 date('s'),
									 date('n'),
									 date('j'),
									 date('Y') + 2)));
$array['element']['startdateyear']['default'] = '';
$array['element']['startdateyear']['unset'] = true;

$array['element']['startdatehours'] = array();
$array['element']['startdatehours']['value'] = range(0,23);
$array['element']['startdatehours']['default'] = '';
$array['element']['startdatehours']['unset'] = true;

$array['element']['startdateminutes'] = array();
$array['element']['startdateminutes']['value'] = range(0,59);
$array['element']['startdateminutes']['default'] = '';
$array['element']['startdateminutes']['unset'] = true;

$array['element']['emailfrom'] = array();
$array['element']['emailfrom']['default'] = 'no-reply+meetme@xivo.fr';
$array['element']['emailfrom']['setdef'] = true;

$array['element']['emailfromname'] = array();
$array['element']['emailfromname']['default'] = 'XIVO PBX';
$array['element']['emailfromname']['null'] = true;

$array['element']['emailsubject'] = array();
$array['element']['emailsubject']['default'] = '';
$array['element']['emailsubject']['null'] = true;

$array['element']['emailbody'] = array();
$array['element']['emailbody']['default'] = '';

$array['element']['preprocess_subroutine'] = array();
$array['element']['preprocess_subroutine']['default'] = '';
$array['element']['preprocess_subroutine']['null'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';


dwho::load_class('dwho_network');

$array['filter'] = array();
$array['filter']['meetmeid'] = array('cast' => 'ulongint');
$array['filter']['name'] = array('regexp' => '/^[a-zA-Z0-9_\.-]+$/','minlen' => 1,'maxlen' => 80);
$array['filter']['number'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['admin_typefrom'] = array('key' => array('none','internal','external','undefined'));
$array['filter']['admin_internalid'] = array('set' => false,'chk' => 2,'cast' => 'ulongint');
$array['filter']['admin_externalid'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['admin_identification'] = array('set' => false,'chk' => 2,'key' => array('calleridnum','pin','all'));
$array['filter']['admin_mode'] = array('set' => false,'chk' => 2,'key' => array('listen','talk','all'));
$array['filter']['admin_announceusercount'] = array('bool' => true);
$array['filter']['admin_announcejoinleave'] = array('set' => false,'chk' => 2,'key' => array('no','yes','noreview'));
$array['filter']['admin_moderationmode'] = array('bool' => true);
$array['filter']['admin_initiallymuted'] = array('bool' => true);
$array['filter']['admin_musiconhold'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 20);
$array['filter']['admin_poundexit'] = array('bool' => true);
$array['filter']['admin_quiet'] = array('bool' => true);
$array['filter']['admin_starmenu'] = array('bool' => true);
$array['filter']['admin_closeconflastmarkedexit'] = array('bool' => true);
$array['filter']['admin_enableexitcontext'] = array('bool' => true);
$array['filter']['admin_exitcontext'] = array('set' => false,'chk' => 2,'callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['user_mode'] = array('set' => false,'chk' => 2,'key' => array('listen','talk','all'));
$array['filter']['user_announceusercount'] = array('bool' => true);
$array['filter']['user_hiddencalls'] = array('bool' => true);
$array['filter']['user_announcejoinleave'] = array('set' => false,'chk' => 2,'key' => array('no','yes','noreview'));
$array['filter']['user_initiallymuted'] = array('bool' => true);
$array['filter']['user_musiconhold'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 20);
$array['filter']['user_poundexit'] = array('bool' => true);
$array['filter']['user_quiet'] = array('bool' => true);
$array['filter']['user_starmenu'] = array('bool' => true);
$array['filter']['user_enableexitcontext'] = array('bool' => true);
$array['filter']['user_exitcontext'] = array('set' => false,'chk' => 2,'callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['talkeroptimization'] = array('bool' => true);
$array['filter']['record'] = array('bool' => true);
$array['filter']['talkerdetection'] = array('bool' => true);
$array['filter']['noplaymsgfirstenter'] = array('bool' => true);
$array['filter']['durationm'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,999));
$array['filter']['closeconfdurationexceeded'] = array('bool' => true);
$array['filter']['maxuser'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,99));
$array['filter']['startdate'] = array('set' => false,'chk' => 2,'callback' => 'dwho_filter::chk_datetime','callbackarg' => array(true));
$array['filter']['emailfrom'] = array('set' => false,'chk' => 2,'maxlen' => 255,'callback' => 'dwho_network::valid_email');
$array['filter']['emailfromname'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 255);
$array['filter']['emailsubject'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 255);
$array['filter']['emailbody'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);
$array['filter']['preprocess_subroutine'] = array('set' => false,'chk' => 2,'callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>
