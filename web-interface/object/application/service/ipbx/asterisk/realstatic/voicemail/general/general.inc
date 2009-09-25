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

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','realstatic','abstract.inc'),true);

class xivo_application_service_asterisk_realstatic_voicemail_general extends xivo_application_service_asterisk_realstatic_abstract
{
	var $_name		= 'generalvoicemail';
	var $_modulename	= 'staticvoicemail';
	var $_category		= 'general';
	var $_cat_metric	= 0;
	var $_filename		= 'voicemail.conf';
	var $_contextmember	= false;
	var $_contextlist	= array();

	function xivo_application_service_asterisk_realstatic_voicemail_general(&$service)
	{
		return($this->_init(&$service));
	}

	function _load_config()
	{
		if(($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'generalvoicemail'))) === false)
			return(false);

		return(parent::_load_config(dirname(__FILE__)));
	}

	function _chk_pre_save($rs)
	{
		if(is_array($rs) === false
		|| isset($rs['var_name']) === false
		|| array_key_exists('var_val',$rs) === false)
			return(null);

		switch($rs['var_name'])
		{
			case 'cidinternalcontexts':
				$this->_get_contextmember(null,'cidinternalcontexts');

				$res = array();
				$res['generalvoicemail-cidinternalcontexts'] = array();
				$res['generalvoicemail-cidinternalcontexts']['context'] = $rs['var_val'];
				$res['generalvoicemail-cidinternalcontexts']['varname'] = 'cidinternalcontexts';

				if($this->set_contextmember($res) === false
				|| $this->edit_contextmember() === false)
					return(false);
				break;
			case 'callback':
				$this->_get_contextmember(null,'callback');

				$res = array();
				$res['generalvoicemail-callback'] = array();
				$res['generalvoicemail-callback']['context'] = $rs['var_val'];
				$res['generalvoicemail-callback']['varname'] = 'callback';

				if($this->set_contextmember($res) === false
				|| $this->edit_contextmember() === false)
					return(false);
				break;
			case 'dialout':
				$this->_get_contextmember(null,'dialout');

				$res = array();
				$res['generalvoicemail-dialout'] = array();
				$res['generalvoicemail-dialout']['context'] = $rs['var_val'];
				$res['generalvoicemail-dialout']['varname'] = 'dialout';

				if($this->set_contextmember($res) === false
				|| $this->edit_contextmember() === false)
					return(false);
				break;
			case 'exitcontext':
				$this->_get_contextmember(null,'exitcontext');

				$res = array();
				$res['generalvoicemail-exitcontext'] = array();
				$res['generalvoicemail-exitcontext']['context'] = $rs['var_val'];
				$res['generalvoicemail-exitcontext']['varname'] = 'exitcontext';

				if($this->set_contextmember($res) === false
				|| $this->edit_contextmember() === false)
					return(false);
				break;
			case 'format':
				if(is_array($rs['var_val']) === true)
				{
					$rs['var_val'] = implode('|',$rs['var_val']);
					$this->_set_result($this->_name,$rs);
				}
				break;
		}

		return(true);
	}

	function set_save_all($arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['minmessage']) === false
		&& isset($arr['maxmessage']) === true)
			$arr['minmessage'] = $arr['maxmessage'];
		else if(isset($arr['minmessage'],$arr['maxmessage']) === true
		&& $arr['minmessage'] > $arr['maxmessage'])
			$arr['minmessage'] = $arr['maxmessage'];

		if(isset($arr['format'],$arr['attachformat']) === true
		&& is_array($arr['format']) === true
		&& dwho_has_len($arr['attachformat']) === true
		&& ($format = (int) array_search($arr['attachformat'],$arr['format'],true)) !== 0)
		{
			unset($arr['format'][$format]);
			array_unshift($arr['format'],$arr['attachformat']);
		}
		else
			unset($arr['attachformat']);

		return(parent::set_save_all($arr,$disable));
	}
}

?>
