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

require_once(dwho_file::joinpath(XIVO_PATH_OBJECT,'application','abstract','datastorage','sql.inc'));

class xivo_application_service_asterisk_trunk_sql extends xivo_application_abstract_datastorage_sql
{
	var $_table	= array(
				'staticsip'	=> '',
				'staticiax'	=> '',
				'trunkfeatures'	=> '',
				'trunksip'	=> '',
				'trunkiax'	=> '',
				'trunkcustom'	=> '');

	function get_all($protocols,$disable=null,$order=null,$limit=null,$value=null)
	{
		$this->_cnt = 0;

		$search = dwho_has_len($value);

		$arr = array();

		$this->_dso->new_select($this->_table['trunkfeatures']);

		if(is_array($protocols) === false)
			$use_sip = $use_iax = $use_custom = true;
		else
		{
			$use_sip = in_array(XIVO_SRE_IPBX_AST_PROTO_SIP,$protocols);
			$use_iax = in_array(XIVO_SRE_IPBX_AST_PROTO_IAX,$protocols);
			$use_custom = in_array(XIVO_SRE_IPBX_AST_PROTO_CUSTOM,$protocols);
			$this->_dso->wherein('protocol',$protocols,$this->_table['trunkfeatures']);
		}

		if($use_sip === false && $use_iax === false && $use_custom === false)
			return(false);

		$sip = $iax = $custom = array();
		$sip['category'] = $iax['category'] = $custom['category'] = 'trunk';

		if($disable !== null)
		{
			$disable = intval((bool) $disable);
			$arr['commented'] = $disable;
			$sip['commented'] = $disable;
			$iax['commented'] = $disable;
			$custom['commented'] = $disable;
		}

		$this->_dso->andwhere($arr,$this->_table['trunkfeatures']);

		$this->_dso->reset_order();

		if($use_sip === true)
		{
			$this->_dso->leftjoin($this->_table['trunksip'],
					      $this->_table['trunkfeatures'].'.protocolid = '.$this->_table['trunksip'].'.id',
					      null,
					      '*');

			$this->_dso->joinwhere($this->_table['trunksip'],$sip);

			$this->_dso->andjoinwhere($this->_table['trunksip'],
						  $this->_table['trunkfeatures'].'.protocol = '.$this->_table['trunksip'].'.protocol');

			$this->_dso->leftjoin($this->_table['staticsip'],
					      $this->_table['trunkfeatures'].'.registerid = '.$this->_table['staticsip'].'.id',
					      null,
					      array('register'	=> 'var_val'));

			$this->_dso->joinwhere($this->_table['staticsip'],
					       $this->_table['trunkfeatures'].'.protocol = %s','sip');

			$this->_dso->add_order($order,null,$this->_table['trunksip']);
		}

		if($use_iax === true)
		{
			$this->_dso->leftjoin($this->_table['trunkiax'],
					      $this->_table['trunkfeatures'].'.protocolid = '.$this->_table['trunkiax'].'.id',
					      null,
					      '*');

			$this->_dso->joinwhere($this->_table['trunkiax'],$iax);

			$this->_dso->andjoinwhere($this->_table['trunkiax'],
						  $this->_table['trunkfeatures'].'.protocol = '.$this->_table['trunkiax'].'.protocol');

			$this->_dso->leftjoin($this->_table['staticiax'],
					      $this->_table['trunkfeatures'].'.registerid = '.$this->_table['staticiax'].'.id',
					      null,
					      array('register'	=> 'var_val'));

			$this->_dso->joinwhere($this->_table['staticiax'],
					       $this->_table['trunkfeatures'].'.protocol = %s','iax');

			$this->_dso->add_order($order,null,$this->_table['trunkiax']);
		}

		if($use_custom === true)
		{
			$this->_dso->leftjoin($this->_table['trunkcustom'],
					      $this->_table['trunkfeatures'].'.protocolid = '.$this->_table['trunkcustom'].'.id',
					      null,
					      '*');

			$this->_dso->joinwhere($this->_table['trunkcustom'],$custom);

			$this->_dso->andjoinwhere($this->_table['trunkcustom'],
						  $this->_table['trunkfeatures'].'.protocol = '.$this->_table['trunkcustom'].'.protocol');

			$this->_dso->add_order($order,null,$this->_table['trunkcustom']);
		}

		$this->_dso->aggregate_where();

		if($search === true)
		{
			if($use_sip === true)
			{
				$this->_dso->orwherecontain('name',$value,true,$this->_table['trunksip']);
				$this->_dso->orwherecontain('username',$value,true,$this->_table['trunksip']);
			}

			if($use_iax === true)
			{
				$this->_dso->orwherecontain('name',$value,true,$this->_table['trunkiax']);
				$this->_dso->orwherecontain('username',$value,true,$this->_table['trunkiax']);
			}

			if($use_custom === true)
			{
				$this->_dso->orwherecontain('name',$value,true,$this->_table['trunkcustom']);
				$this->_dso->orwherecontain('interface',$value,true,$this->_table['trunkcustom']);
			}
		}

		$this->_dso->aggregate_where(false,'AND');

		if($use_sip === true)
			$this->_dso->orwhere($this->_table['trunksip'].'.id IS NOT NULL');

		if($use_iax === true)
			$this->_dso->orwhere($this->_table['trunkiax'].'.id IS NOT NULL');

		if($use_custom === true)
			$this->_dso->orwhere($this->_table['trunkcustom'].'.id IS NOT NULL');

		$this->_dso->aggregate_where(false,'AND');

		$this->_dso->limit($limit);

		$r = $this->_dso->select_all(null,true,true);

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count(null,'id',$this->_table['trunkfeatures'])) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function search($value,$protocols,$disable=null,$order=null,$limit=null)
	{
		$this->_cnt = 0;

		if(dwho_has_len($value) === false)
			return(false);

		return($this->get_all($protocols,$disable,$order,$limit,$value));
	}
}

?>
