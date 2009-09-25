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

class dwho_tpl_image
{
	function make($url,$alt='',$opt='')
	{
		$r = '';

		$url = (string) $url;
		$alt = (string) $alt;
		$opt = (string) $opt;

		if(isset($url{0}) === false)
			return($r);

		$url = $url{0} !== '/' ? '/i/i18n/'.DWHO_I18N_BABELFISH_LOCALE.'/'.$url : $url;

		$r = '<img src="'.$url.'"'.(isset($opt{0}) === true ? ' '.$opt : '').' alt="'.dwho_alttitle($alt).'" />';

		return($r);
	}

	function get($url)
	{
		$r = '';

		$url = (string) $url;

		if(isset($url{0}) === false)
			return($r);

		$r = $url{0} !== '/' ? '/i/i18n/'.DWHO_I18N_BABELFISH_LOCALE.'/'.$url : $url;

		return($r);
	}
}

?>
