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

$_CF['_libs']['i18n']['charset']['real'] = array(
	'ARMSCII-8'	=> array('support' => 2,'mime' => 'ARMSCII-8'),
	'BIG5'		=> array('support' => 2,'mime' => 'Big5'),
	'BIG5-HKSCS'	=> array('support' => 2,'mime' => 'BIG5-HKSCS'),
	'CP1251'	=> array('support' => 2,'mime' => 'CP1251'),
	'CP1255'	=> array('support' => 2,'mime' => 'CP1255'),
	'CP1256'	=> array('support' => 2,'mime' => 'CP1256'),
	'EUC-CN'	=> array('support' => 2,'mime' => 'EUC-CN'),
	'EUC-JP'	=> array('support' => 2,'mime' => 'EUC-JP'),
	'EUC-KR'	=> array('support' => 2,'mime' => 'EUC-KR'),
	'EUC-TW'	=> array('support' => 2,'mime' => 'EUC-TW'),
	'GB18030'	=> array('support' => 2,'mime' => 'GB18030'),
	'GBK'		=> array('support' => 2,'mime' => 'GBK'),
	'GEORGIAN-PS'	=> array('support' => 2,'mime' => 'GEORGIAN-PS'),
	'ISO-8859-1'	=> array('support' => 1,'mime' => 'ISO-8859-1'),
	'ISO-8859-2'	=> array('support' => 1,'mime' => 'ISO-8859-2'),
	'ISO-8859-3'	=> array('support' => 1,'mime' => 'ISO-8859-3'),
	'ISO-8859-6'	=> array('support' => 1,'mime' => 'ISO-8859-6'),
	'ISO-8859-7'	=> array('support' => 1,'mime' => 'ISO-8859-7'),
	'ISO-8859-8'	=> array('support' => 1,'mime' => 'ISO-8859-8'),
	'ISO-8859-9'	=> array('support' => 1,'mime' => 'ISO-8859-9'),
	'ISO-8859-14'	=> array('support' => 1,'mime' => 'ISO-8859-14'),
	'ISO-8859-15'	=> array('support' => 1,'mime' => 'ISO-8859-15'),
	'KOI8-R'	=> array('support' => 2,'mime' => 'KOI8-R'),
	'KOI8-T'	=> array('support' => 2,'mime' => 'KOI8-T'),
	'KOI8-U'	=> array('support' => 1,'mime' => 'KOI8-U'),
	'TCVN'		=> array('support' => 2,'mime' => 'TCVN'),
	'TIS-620'	=> array('support' => 2,'mime' => 'TIS-620'),
	'SHIFT_JIS'	=> array('support' => 2,'mime' => 'Shift_JIS'),
	'UTF-8'		=> array('support' => 2,'mime' => 'UTF-8'),
	'US-ASCII'	=> array('support' => 1,'mime' => 'US-ASCII'));

$_CF['_libs']['i18n']['charset']['alias'] = array(
	'ARMSCII8'		=> 'ARMSCII-8',

	'BIG-5'			=> 'BIG5',
	'BIG-FIVE'		=> 'BIG5',
	'BIGFIVE'		=> 'BIG5',
	'CN-BIG5'		=> 'BIG5',
	'csBig5'		=> 'BIG5',

	'BIG5HKSCS'		=> 'BIG5-HKSCS',

	'WINDOWS-1251'		=> 'CP1251',
	'MICROSOFTCP1251'	=> 'CP1251',
	'MICROSOFT-CP1251'	=> 'CP1251',
	'MS-CYRL'		=> 'CP1251',

	'WINDOWS-1255'		=> 'CP1255',
	'MS-HEBR'		=> 'CP1255',

	'WINDOWS-1256'		=> 'CP1256',
	'MS-ARAB'		=> 'CP1256',

	'EUCCN'			=> 'EUC-CN',
	'GB2312'		=> 'EUC-CN',
	'CN-GB'			=> 'EUC-CN',
	'CSGB2312'		=> 'EUC-CN',
	'EUC_CN'		=> 'EUC-CN',

	'CP936'			=> 'GBK',

	'EUCJP'			=> 'EUC-JP',
	'CSEUCPKDFMTJAPANESE'	=> 'EUC-JP',
	'EUC_JP'		=> 'EUC-JP',

	'EUCKR'			=> 'EUC-KR',
	'CSEUCKR'		=> 'EUC-KR',
	'EUC_KR'		=> 'EUC-KR',

	'EUCTW'			=> 'EUC-TW',
	'CSEUCTW'		=> 'EUC-TW',
	'EUC_TW'		=> 'EUC-TW',

	'ISO_8859-1'		=> 'ISO-8859-1',
	'ISO_8859-1:1987'	=> 'ISO-8859-1',
	'ISO-IR-100'		=> 'ISO-8859-1',
	'CP819'			=> 'ISO-8859-1',
	'IBM819'		=> 'ISO-8859-1',
	'LATIN1'		=> 'ISO-8859-1',
	'L1'			=> 'ISO-8859-1',
	'CSISOLATIN1'		=> 'ISO-8859-1',
	'ISO8859-1'		=> 'ISO-8859-1',
	'ISO8859_1'		=> 'ISO-8859-1',

	'ISO_8859-2'		=> 'ISO-8859-2',
	'ISO_8859-2:1987'	=> 'ISO-8859-2',
	'ISO-IR-101'		=> 'ISO-8859-2',
	'LATIN2'		=> 'ISO-8859-2',
	'L2'			=> 'ISO-8859-2',
	'CSISOLATIN2'		=> 'ISO-8859-2',
	'ISO8859-2'		=> 'ISO-8859-2',
	'ISO8859_2'		=> 'ISO-8859-2',

	'ISO_8859-3'		=> 'ISO-8859-3',
	'ISO_8859-3:1988'	=> 'ISO-8859-3',
	'ISO-IR-109'		=> 'ISO-8859-3',
	'LATIN3'		=> 'ISO-8859-3',
	'L3'			=> 'ISO-8859-3',
	'CSISOLATIN3'		=> 'ISO-8859-3',
	'ISO8859-3'		=> 'ISO-8859-3',
	'ISO8859_3'		=> 'ISO-8859-3',

	'ISO_8859-6'		=> 'ISO-8859-6',
	'ISO_8859-6:1987'	=> 'ISO-8859-6',
	'ISO-IR-127'		=> 'ISO-8859-6',
	'ECMA-114'		=> 'ISO-8859-6',
	'ASMO-708'		=> 'ISO-8859-6',
	'ARABIC'		=> 'ISO-8859-6',
	'CSISOLATINARABIC'	=> 'ISO-8859-6',
	'ISO8859-6'		=> 'ISO-8859-6',
	'ISO8859_6'		=> 'ISO-8859-6',

	'ISO_8859-7'		=> 'ISO-8859-7',
	'ISO_8859-7:1987'	=> 'ISO-8859-7',
	'ISO-IR-126'		=> 'ISO-8859-7',
	'ECMA-118'		=> 'ISO-8859-7',
	'ELOT_928GREEK8'	=> 'ISO-8859-7',
	'GREEK'			=> 'ISO-8859-7',
	'CSISOLATINGREEK'	=> 'ISO-8859-7',
	'ISO8859-7'		=> 'ISO-8859-7',
	'ISO8859_7'		=> 'ISO-8859-7',

	'ISO_8859-8'		=> 'ISO-8859-8',
	'ISO_8859-8:1988'	=> 'ISO-8859-8',
	'ISO-IR-138'		=> 'ISO-8859-8',
	'HEBREW'		=> 'ISO-8859-8',
	'CSISOLATINHEBREW'	=> 'ISO-8859-8',
	'ISO8859-8'		=> 'ISO-8859-8',
	'ISO8859_8'		=> 'ISO-8859-8',

	'ISO_8859-9'		=> 'ISO-8859-9',
	'ISO_8859-9:1989'	=> 'ISO-8859-9',
	'ISO-IR-148'		=> 'ISO-8859-9',
	'LATIN5'		=> 'ISO-8859-9',
	'L5'			=> 'ISO-8859-9',
	'CSISOLATIN5'		=> 'ISO-8859-9',
	'ISO8859-9'		=> 'ISO-8859-9',
	'ISO8859_9'		=> 'ISO-8859-9',

	'ISO_8859-14'		=> 'ISO-8859-14',
	'ISO_8859-14:1998'	=> 'ISO-8859-14',
	'ISO-IR-199'		=> 'ISO-8859-14',
	'LATIN8'		=> 'ISO-8859-14',
	'L8'			=> 'ISO-8859-14',
	'ISO-CELTIC'		=> 'ISO-8859-14',
	'ISO8859-14'		=> 'ISO-8859-14',

	'ISO_8859-15'		=> 'ISO-8859-15',
	'ISO_8859-15:1998'	=> 'ISO-8859-15',
	'ISO-IR-203'		=> 'ISO-8859-15',
	'ISO8859-15'		=> 'ISO-8859-15',

	'CSKOI8R'		=> 'KOI8-R',

	'SJIS'			=> 'SHIFT_JIS',
	'MS_KANJI'		=> 'SHIFT_JIS',
	'CSSHIFTJIS'		=> 'SHIFT_JIS',

	'TCVN-5712'		=> 'TCVN',
	'TCVN5712-1'		=> 'TCVN',
	'TCVN5712-1:1993'	=> 'TCVN',

	'TIS620'		=> 'TIS-620',
	'TIS620-0'		=> 'TIS-620',
	'TIS620.2529-1'		=> 'TIS-620',
	'TIS620.2533-0'		=> 'TIS-620',
	'TIS620.2533-1'		=> 'TIS-620',
	'ISO-IR-166'		=> 'TIS-620',

	'UTF8'			=> 'UTF-8',

	'ANSI_X3.4-1986'	=> 'US-ASCII',
	'ISO-IR-6'		=> 'US-ASCII',
	'ANSI_X3.4-1968'	=> 'US-ASCII',
	'ISO_646.IRV:1991'	=> 'US-ASCII',
	'ASCII'			=> 'US-ASCII',
	'ISO646-US'		=> 'US-ASCII',
	'IBM367'		=> 'US-ASCII',
	'CP367'			=> 'US-ASCII');

?>
