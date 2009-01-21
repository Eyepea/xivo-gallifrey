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

define('XIVO_TE_ERR_TRAC',0);
define('XIVO_TE_ERR_ERROR',(E_ERROR | E_CORE_ERROR | E_COMPILE_ERROR | E_USER_ERROR));
define('XIVO_TE_ERR_WARNING',(E_WARNING | E_CORE_WARNING | E_COMPILE_WARNING | E_USER_WARNING));
define('XIVO_TE_ERR_NOTICE',(E_NOTICE | E_USER_NOTICE));
define('XIVO_TE_ERR_PARSE',E_PARSE);
define('XIVO_TE_ERR_ALL',(XIVO_TE_ERR_ERROR | XIVO_TE_ERR_WARNING | XIVO_TE_ERR_NOTICE | XIVO_TE_ERR_PARSE));

define('XIVO_TE_RTYPE_NONE',0);
define('XIVO_TE_RTYPE_LOG',1);
define('XIVO_TE_RTYPE_SCREEN',2);
define('XIVO_TE_RTYPE_MAIL',4);
define('XIVO_TE_RTYPE_ALL',7);

define('XIVO_TE_RMODE_REALTIME',1);
define('XIVO_TE_RMODE_DELAYED',2);

define('XIVO_TE_RFUNC_PRINTR',1);
define('XIVO_TE_RFUNC_VARDUMP',2);

define('XIVO_TE_CONTEXT','Global');

define('XIVO_TE_TRAC_LABEL',XIVO_LABEL.' Trace');
define('XIVO_TE_ERROR_LABEL',XIVO_LABEL.' Error');
define('XIVO_TE_WARNING_LABEL',XIVO_LABEL.' Warning');
define('XIVO_TE_NOTICE_LABEL',XIVO_LABEL.' Notice');
define('XIVO_TE_PARSE_LABEL',XIVO_LABEL.' Parse');

$array = array();

$array['level'] = array(
		XIVO_TE_ERR_TRAC	=> array('label' => XIVO_TE_TRAC_LABEL,
						 'color' => 'black'),
		XIVO_TE_ERR_ERROR	=> array('label' => XIVO_TE_ERROR_LABEL,
						 'color' => 'red'),
		XIVO_TE_ERR_WARNING	=> array('label' => XIVO_TE_WARNING_LABEL,
						 'color' => 'purple'),
		XIVO_TE_ERR_NOTICE	=> array('label' => XIVO_TE_NOTICE_LABEL,
						 'color' => 'blue'),
		XIVO_TE_ERR_PARSE	=> array('label' => XIVO_TE_PARSE_LABEL,
						 'color' => 'grey'));

$array['rtype'] = array(
		XIVO_TE_RTYPE_NONE	=> 'XIVO_TE_RTYPE_NONE',
		XIVO_TE_RTYPE_LOG	=> 'XIVO_TE_RTYPE_LOG',
		XIVO_TE_RTYPE_SCREEN	=> 'XIVO_TE_RTYPE_SCREEN',
		XIVO_TE_RTYPE_MAIL	=> 'XIVO_TE_RTYPE_MAIL');

$array['rmode'] = array(
		XIVO_TE_RMODE_REALTIME	=> 'XIVO_TE_RMODE_REALTIME',
		XIVO_TE_RMODE_DELAYED	=> 'XIVO_TE_RMODE_DELAYED');

$array['rfunc'] = array(
		XIVO_TE_RFUNC_PRINTR	=> 'XIVO_TE_RFUNC_PRINTR',
		XIVO_TE_RFUNC_VARDUMP	=> 'XIVO_TE_RFUNC_VARDUMP');

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
