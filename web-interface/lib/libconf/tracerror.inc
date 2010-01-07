<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

define('DWHO_TE_ERR_TRAC',0);
define('DWHO_TE_ERR_ERROR',(E_ERROR | E_CORE_ERROR | E_COMPILE_ERROR | E_USER_ERROR));
define('DWHO_TE_ERR_WARNING',(E_WARNING | E_CORE_WARNING | E_COMPILE_WARNING | E_USER_WARNING));
define('DWHO_TE_ERR_NOTICE',(E_NOTICE | E_USER_NOTICE));
define('DWHO_TE_ERR_PARSE',E_PARSE);
define('DWHO_TE_ERR_ALL',(DWHO_TE_ERR_ERROR | DWHO_TE_ERR_WARNING | DWHO_TE_ERR_NOTICE | DWHO_TE_ERR_PARSE));

define('DWHO_TE_RTYPE_NONE',0);
define('DWHO_TE_RTYPE_LOG',1);
define('DWHO_TE_RTYPE_SCREEN',2);
define('DWHO_TE_RTYPE_MAIL',4);
define('DWHO_TE_RTYPE_ALL',(DWHO_TE_RTYPE_LOG | DWHO_TE_RTYPE_SCREEN | DWHO_TE_RTYPE_MAIL));

define('DWHO_TE_RMODE_REALTIME',1);
define('DWHO_TE_RMODE_DELAYED',2);

define('DWHO_TE_RFUNC_PRINTR',1);
define('DWHO_TE_RFUNC_VARDUMP',2);

define('DWHO_TE_CONTEXT','Global');

if(defined('DWHO_LABEL_CUSTOM') === true)
	define('DWHO_TE_LABEL',DWHO_LABEL_CUSTOM);
else
	define('DWHO_TE_LABEL',DWHO_LABEL);

define('DWHO_TE_TRAC_LABEL',DWHO_TE_LABEL.' Trace');
define('DWHO_TE_ERROR_LABEL',DWHO_TE_LABEL.' Error');
define('DWHO_TE_WARNING_LABEL',DWHO_TE_LABEL.' Warning');
define('DWHO_TE_NOTICE_LABEL',DWHO_TE_LABEL.' Notice');
define('DWHO_TE_PARSE_LABEL',DWHO_TE_LABEL.' Parse');

$array = array();

$array['level'] = array(
		DWHO_TE_ERR_TRAC	=> array('label' => DWHO_TE_TRAC_LABEL,
						 'color' => 'black'),
		DWHO_TE_ERR_ERROR	=> array('label' => DWHO_TE_ERROR_LABEL,
						 'color' => 'red'),
		DWHO_TE_ERR_WARNING	=> array('label' => DWHO_TE_WARNING_LABEL,
						 'color' => 'purple'),
		DWHO_TE_ERR_NOTICE	=> array('label' => DWHO_TE_NOTICE_LABEL,
						 'color' => 'blue'),
		DWHO_TE_ERR_PARSE	=> array('label' => DWHO_TE_PARSE_LABEL,
						 'color' => 'grey'));

$array['rtype'] = array(
		DWHO_TE_RTYPE_NONE	=> 'DWHO_TE_RTYPE_NONE',
		DWHO_TE_RTYPE_LOG	=> 'DWHO_TE_RTYPE_LOG',
		DWHO_TE_RTYPE_SCREEN	=> 'DWHO_TE_RTYPE_SCREEN',
		DWHO_TE_RTYPE_MAIL	=> 'DWHO_TE_RTYPE_MAIL');

$array['rmode'] = array(
		DWHO_TE_RMODE_REALTIME	=> 'DWHO_TE_RMODE_REALTIME',
		DWHO_TE_RMODE_DELAYED	=> 'DWHO_TE_RMODE_DELAYED');

$array['rfunc'] = array(
		DWHO_TE_RFUNC_PRINTR	=> 'DWHO_TE_RFUNC_PRINTR',
		DWHO_TE_RFUNC_VARDUMP	=> 'DWHO_TE_RFUNC_VARDUMP');

$array['e_context'] = array(
		E_ERROR			=> 'E_ERROR',
		E_CORE_ERROR		=> 'E_CORE_ERROR',
		E_COMPILE_ERROR		=> 'E_COMPILE_ERROR',
		E_USER_ERROR		=> 'E_USER_ERROR',
		E_WARNING		=> 'E_WARNING',
		E_CORE_WARNING		=> 'E_CORE_WARNING',
		E_COMPILE_WARNING	=> 'E_COMPILE_ERROR',
		E_USER_WARNING		=> 'E_USER_WARNING',
		E_NOTICE		=> 'E_NOTICE',
		E_USER_NOTICE		=> 'E_USER_NOTICE',
		E_PARSE			=> 'E_PARSE');

?>
