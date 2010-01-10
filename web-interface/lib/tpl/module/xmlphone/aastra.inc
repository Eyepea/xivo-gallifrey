<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

require_once(dwho_file::joinpath(DWHO_PATH_ROOT,'tpl','module','xmlphone','abstract','abstract.inc'));

class dwho_tpl_xmlphone_aastra extends dwho_tpl_xmlphone_abstract
{
	var $_vendor		= 'aastra';
	var $_model		= null;
	var $_argseparator	= '&amp;';
	var $_cdata		= true;
	var $_escape		= false;
	var $_utf8		= false;
	var $_tags		= array('directory'	=> 'AastraIPPhoneDirectory',
					'menu'		=> 'AastraIPPhoneTextMenu',
					'input'		=> 'AastraIPPhoneInputScreen');
	var $_model_standardize	= array('51i'		=> '6751i',
					'53i'		=> '6753i',
					'55i'		=> '6755i',
					'57i'		=> '6757i',
					'57i CT'	=> '6757i CT');

	function dwho_tpl_xmlphone_aastra()
	{
		$this->_model = $this->get_model();
	}

	function get_model()
	{
		if(isset($_SERVER['HTTP_USER_AGENT']) === false
		|| preg_match('/^Aastra((?:(?:67)?5[1357]|673[01])i(?: CT)?) /',
			      $_SERVER['HTTP_USER_AGENT'],
			      $match) !== 1)
			return(null);
		else if(isset($this->_model_standardize[$match[1]]) === true)
			return($this->_model_standardize[$match[1]]);

		return($match[1]);
	}

	function has_softkeys()
	{
		switch($this->_model)
		{
			case '6755i':
			case '6757i':
			case '6757i CT':
				return(true);
		}

		return(false);
	}
}

?>
