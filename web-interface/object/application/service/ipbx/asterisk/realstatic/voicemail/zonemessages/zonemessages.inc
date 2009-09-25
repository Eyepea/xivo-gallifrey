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

class xivo_application_service_asterisk_realstatic_voicemail_zonemessages extends xivo_application_service_asterisk_realstatic_abstract
{
	var $_name		= 'zonemessages';
	var $_modulename	= 'staticvoicemail';
	var $_category		= 'zonemessages';
	var $_cat_metric	= 1;
	var $_filename		= 'voicemail.conf';
	var $_timezones		= array();

	function xivo_application_service_asterisk_realstatic_voicemail_zonemessages(&$service)
	{
		return($this->_init(&$service));
	}

	function _load_config()
	{
		return(parent::_load_config(dirname(__FILE__)));
	}

	function get_all_name()
	{
		$this->_info = false;
		$this->_origin = false;

		if(($this->_info = $this->_realstatic->get_all_name()) === false)
			return(false);

		$this->_origin = $this->_realstatic->get_origin_list();

		return($this->_info);
	}

	function get_timezones()
	{
		if(empty($this->_timezones) === true
		&& ($timezones = dwho_i18n::get_timezone_list()) !== false)
			$this->_timezones = $timezones;

		return($this->_timezones);
	}

	function set($list)
	{
		$this->_reset();

		if(is_array($list) === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === 0)
			return(null);

		$info = $tmp = array();

		$this->get_timezones();

		for($i = $j = 0;$i < $nb;$i++)
		{
			if(($rs = $this->chk_values($list[$i])) === false)
			{
				$this->_set_result($j,$this->get_filter_result());
				$this->_set_error($j++,$this->get_filter_error());
			}
			else if(isset($tmp[$rs['name']]) === true
			|| $this->chk_msg_format($rs['msg_format']) === false
			|| isset($this->_timezones[$rs['timezone']]) === false)
			{
				$this->_set_result($j,$rs);
				$this->_set_error($j++,'wrong zonemessages');
			}
			else
			{
				$tmp[$rs['name']] = 1;
				$this->_set_result($j++,$rs);
			}
		}

		if($this->get_errnb() > 0)
			return(false);

		return(true);
	}

	function add()
	{
		$this->_status = 'add';

		if(($rs = $this->get_result()) === null || $rs === false)
			return($rs);

		$this->_return = $info = array();

		$info['category'] = $this->_category;
		$info['cat_metric'] = $this->_cat_metric;

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$info['var_name'] = $rs[$i]['name'];
			$info['var_val'] = $rs[$i]['timezone'].'|'.$rs[$i]['msg_format'];

			if(isset($rs[$i]['commented']) === true)
				$info['commented'] = $rs[$i]['commented'];
			else
				unset($info['commented']);

			if(($this->_return[] = $this->_realstatic->add($info)) !== false)
				continue;
			else if($this->_status === 'add')
				$this->_reverse_add();
			else if($this->_status === 'edit' || $this->_status === 'save')
				$this->_reverse_edit();

			return(false);
		}

		if(isset($this->_return[0]) === false)
			$this->_return = false;

		return(true);
	}

	function edit()
	{
		$this->_status = 'edit';

		return($this->_edit_save());
	}

	function save()
	{
		$this->_status = 'save';

		return($this->_edit_save());
	}

	function _edit_save()
	{
		if(empty($this->_info) === true)
			return($this->add());

		$this->_return = $this->delete_by_category();

		if(($rs = $this->get_result()) === null)
			return($this->_return);
		else if($rs === false)
			return(false);

		$this->_return = $info = array();

		$info['category'] = $this->_category;
		$info['cat_metric'] = $this->_cat_metric;

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$info['var_name'] = $rs[$i]['name'];
			$info['var_val'] = $rs[$i]['timezone'].'|'.$rs[$i]['msg_format'];

			if(isset($rs[$i]['commented']) === true)
				$info['commented'] = $rs[$i]['commented'];
			else
				unset($info['commented']);

			if(isset($this->_origin[$i]) === true)
				$zid = $this->_origin[$i]['id'];
			else
				$zid = 0;

			if(($this->_return[] = $this->_realstatic->add($info,$zid)) === false)
				return(false);
		}

		if(isset($this->_return[0]) === false)
			$this->_return = false;

		return(true);
	}

	function delete()
	{
		$r = true;

		$this->_status = 'delete';
		$this->_return = array();

		if(empty($this->_info) === true
		|| $this->delete_by_category() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';

		return($r);
	}

	function _reverse_add()
	{
		if(is_array($this->_return) === false || ($nb = count($this->_return)) === 0)
			return(null);

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_return[$i] !== false)
				$this->delete($this->_return[$i]);
		}

		return(true);
	}

	function _reverse_edit()
	{
		$this->delete_by_category();

		return($this->add_origin_list());
	}

	function _reverse_delete()
	{
		return($this->add_origin_list());
	}

	function chk_msg_format($str)
	{
		$str = (string) $str;

		if(($len = strlen($str)) === 0)
			return(false);
		else if(($nb = preg_match_all('/\'/',$str,$match,PREG_OFFSET_CAPTURE)) === 0)
			return($this->_chk_letter_format($str));
		else if($nb % 2 === 1)
			return(false);

		$match = $match[0];

		if($match[0][1] > 0)
		{
			$msg = substr($str,0,$match[0][1]);

			if($this->_chk_letter_format($msg) === false)
				return(false);
		}

		for($i = 0;$i < $nb;$i += 2)
		{
			$offset1 = $match[$i][1]+1;
			$offset2 = $match[$i+1][1];

			if($offset1 === $offset2)
				return(false);

			$msg = substr($str,$offset1,$offset2-$offset1);

			$regex = '[\/'.XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN.']'.
				  XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_LENGTH;

			if(preg_match('/^'.$regex.'$/',$msg) === 0)
				return(false);
			else if(isset($match[$i+2]) === false)
				break;

			$offset3 = $match[$i+2][1];

			if($offset3 <= $offset2+1)
				continue;

			$msg = substr($str,$offset2+1,$offset3-($offset2+1));

			if($this->_chk_letter_format($msg) === false)
				return(false);
		}

		if($len <= $offset2+1)
			return(true);

		$msg = substr($str,$offset2+1);

		return($this->_chk_letter_format($msg));
	}

	function _chk_letter_format($str)
	{
		$str = (string) $str;

		if(preg_match('/^[ AaBbcdehHIklmMNpPqQRSTxXY]+$/',$str) === 1)
			return(true);

		return(false);
	}
}

?>
