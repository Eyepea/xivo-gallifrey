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

$array['element'] = array();

$array['element']['dbeg'] = array();
$array['element']['dbeg']['default'] = '';
$array['element']['dbeg']['unset'] = true;

$array['element']['dend'] = array();
$array['element']['dend']['default'] = '';
$array['element']['dend']['unset'] = true;

$array['element']['channel'] = array();
$array['element']['channel']['value'] = array(
			XIVO_SRE_IPBX_AST_CHAN_SIP	=> XIVO_SRE_IPBX_AST_CHAN_SIP,
			XIVO_SRE_IPBX_AST_CHAN_IAX	=> XIVO_SRE_IPBX_AST_CHAN_IAX,
			XIVO_SRE_IPBX_AST_CHAN_LOCAL	=> XIVO_SRE_IPBX_AST_CHAN_LOCAL,
			XIVO_SRE_IPBX_AST_CHAN_AGENT	=> XIVO_SRE_IPBX_AST_CHAN_AGENT,
			XIVO_SRE_IPBX_AST_CHAN_ZAP	=> XIVO_SRE_IPBX_AST_CHAN_ZAP,
			XIVO_SRE_IPBX_AST_CHAN_DAHDI	=> XIVO_SRE_IPBX_AST_CHAN_DAHDI,
			XIVO_SRE_IPBX_AST_CHAN_CAPI	=> XIVO_SRE_IPBX_AST_CHAN_CAPI,
			XIVO_SRE_IPBX_AST_CHAN_MISDN	=> XIVO_SRE_IPBX_AST_CHAN_MISDN,
			XIVO_SRE_IPBX_AST_CHAN_MGCP	=> XIVO_SRE_IPBX_AST_CHAN_MGCP,
			XIVO_SRE_IPBX_AST_CHAN_SCCP	=> XIVO_SRE_IPBX_AST_CHAN_SCCP,
			XIVO_SRE_IPBX_AST_CHAN_H323	=> XIVO_SRE_IPBX_AST_CHAN_H323,
			XIVO_SRE_IPBX_AST_CHAN_UNKNOWN	=> 'unknown',
			'others'			=> 'others');
$array['element']['channel']['default'] = '';

$array['element']['disposition'] = array();
$array['element']['disposition']['value'] = array(
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_ANSWERED,
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_NOANSWER,
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_BUSY,
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_FAILED,
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_UNKNOWN);
$array['element']['disposition']['default'] = '';

$array['element']['amaflags'] = array();
$array['element']['amaflags']['value'] = xivo_service_asterisk::get_amaflags_list(false);
$array['element']['amaflags']['default'] = '';

$array['element']['src'] = array();
$array['element']['src']['default'] = '';

$array['element']['srcformat'] = array();
$array['element']['srcformat']['value'] = array('exact','begin','contain','end');
$array['element']['srcformat']['default'] = 'exact';
$array['element']['srcformat']['unset'] = true;

$array['element']['dst'] = array();
$array['element']['dst']['default'] = '';

$array['element']['dcontext'] = array();
$array['element']['dcontext']['default'] = '';

$array['element']['dcontext-custom'] = array();
$array['element']['dcontext-custom']['default'] = '';
$array['element']['dcontext-custom']['unset'] = true;

$array['element']['dstformat'] = array();
$array['element']['dstformat']['value'] = array('exact','begin','contain','end');
$array['element']['dstformat']['default'] = 'exact';
$array['element']['dstformat']['unset'] = true;

$array['element']['clid'] = array();
$array['element']['clid']['default'] = '';

$array['element']['clidformat'] = array();
$array['element']['clidformat']['value'] = array('exact','begin','contain','end');
$array['element']['clidformat']['default'] = 'exact';
$array['element']['clidformat']['unset'] = true;

$array['element']['accountcode'] = array();
$array['element']['accountcode']['default'] = '';

$array['element']['accountcodeformat'] = array();
$array['element']['accountcodeformat']['value'] = array('exact','begin','contain','end');
$array['element']['accountcodeformat']['default'] = 'exact';
$array['element']['accountcodeformat']['unset'] = true;

$array['element']['userfield'] = array();
$array['element']['userfield']['default'] = '';

$array['element']['userfieldformat'] = array();
$array['element']['userfieldformat']['value'] = array('exact','begin','contain','end');
$array['element']['userfieldformat']['default'] = 'exact';
$array['element']['userfieldformat']['unset'] = true;

$array['element']['dubeg'] = array();
$array['element']['dubeg']['default'] = '';

$array['element']['duend'] = array();
$array['element']['duend']['default'] = '';

$array['element']['dubeg'] = array();
$array['element']['dubeg']['default'] = '';
$array['element']['dubeg']['unset'] = true;

$array['element']['duend'] = array();
$array['element']['duend']['default'] = '';
$array['element']['duend']['unset'] = true;

$array['element']['dubegunit'] = array();
$array['element']['dubegunit']['default'] = 'seconds';
$array['element']['dubegunit']['value'] = array('seconds','minutes');
$array['element']['dubegunit']['unset'] = true;

$array['element']['duendunit'] = array();
$array['element']['duendunit']['default'] = 'seconds';
$array['element']['duendunit']['value'] = array('seconds','minutes');
$array['element']['duendunit']['unset'] = true;

$array['filter'] = array();
$array['filter']['dbeg'] = array('minlen' => 4,'maxlen' => 10,
				 'regexp' => '/^2[0-9]{3}(-(0?[1-9]|1[0-2])(-(0?[1-9]|1[0-9]|2[0-9]|3[0-1]))?)?$/');
$array['filter']['dend'] = array('set' => false,'chk' => 2,'minlen' => 4,'maxlen' => 10,
				 'regexp' => '/^2[0-9]{3}(-(0?[1-9]|1[0-2])(-(0?[1-9]|1[0-9]|2[0-9]|3[0-1]))?)?$/');
$array['filter']['channel'] = array('set' => false,'chk' => 2,'key' => array(
					XIVO_SRE_IPBX_AST_CHAN_SIP,
					XIVO_SRE_IPBX_AST_CHAN_IAX,
					XIVO_SRE_IPBX_AST_CHAN_LOCAL,
					XIVO_SRE_IPBX_AST_CHAN_AGENT,
					XIVO_SRE_IPBX_AST_CHAN_ZAP,
					XIVO_SRE_IPBX_AST_CHAN_DAHDI,
					XIVO_SRE_IPBX_AST_CHAN_CAPI,
					XIVO_SRE_IPBX_AST_CHAN_MISDN,
					XIVO_SRE_IPBX_AST_CHAN_MGCP,
					XIVO_SRE_IPBX_AST_CHAN_SCCP,
					XIVO_SRE_IPBX_AST_CHAN_H323,
					XIVO_SRE_IPBX_AST_CHAN_UNKNOWN,
					'others'));
$array['filter']['disposition'] = array('set' => false,'chk' => 2,'key' => array(
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_ANSWERED,
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_NOANSWER,
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_BUSY,
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_FAILED,
					XIVO_SRE_IPBX_AST_CDR_DISPOSITION_UNKNOWN));
$array['filter']['dcontext'] = array('set' => false,'chk' => 2,'callback' => 'xivo_service_asterisk::chk_context',
				     'callbackarg' => array(array(
					XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION,
					XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['amaflags'] = array('set' => false,'chk' => 2,'key' => xivo_service_asterisk::get_amaflags_list());
$array['filter']['src'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 80);
$array['filter']['srcformat'] = array('key' => array('exact','begin','contain','end'));
$array['filter']['dst'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 80);
$array['filter']['dstformat'] = array('key' => array('exact','begin','contain','end'));
$array['filter']['clid'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 80,'notag' => false);
$array['filter']['clidformat'] = array('key' => array('exact','begin','contain','end'));
$array['filter']['accountcode'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9_-]+$/i','maxlen' => 20);
$array['filter']['accountcodeformat'] = array('key' => array('exact','begin','contain','end'));
$array['filter']['userfield'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-zA-Z0-9-\._~\!\$&\'\[\]\*\+;=:%@]+$/','maxlen' => 255);
$array['filter']['userfieldformat'] = array('key' => array('exact','begin','contain','end'));
$array['filter']['dubeg'] = array('set' => false,'chk' => 2,'cast' => 'uint','maxlen' => 5);
$array['filter']['duend'] = array('set' => false,'chk' => 2,'cast' => 'uint','maxlen' => 5);
$array['filter']['dubegunit'] = array('key' => array('seconds','minutes'));
$array['filter']['duendunit'] = array('key' => array('seconds','minutes'));

?>
