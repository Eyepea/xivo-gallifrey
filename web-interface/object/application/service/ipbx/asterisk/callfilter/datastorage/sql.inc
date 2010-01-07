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

class xivo_application_service_asterisk_callfilter_sql extends xivo_application_abstract_datastorage_sql
{
	var $_table	= array(
				'callfilter'		=> '',
				'callfiltermember'	=> '',
				'userfeatures'		=> '');

	function get_all($type,$disable=null,$order=null,$limit=null)
	{
		$arr = array();

		$this->_dso->new_select($this->_table['callfilter']);

		$this->_dso->leftjoin(array('cfilterm1' => $this->_table['callfiltermember']),
				      $this->_table['callfilter'].'.id = cfilterm1.callfilterid');

		$this->_dso->leftjoin(array('user1' => $this->_table['userfeatures']),
				      'cfilterm1.typeval = user1.id',
				      null,
				      array('member_id' => 'id',
					    'member_context' => 'context',
					    'member_firstname' => 'firstname',
					    'member_lastname' => 'lastname',
					    'member_number' => 'number',
					    'member_name' => 'name',
					    'linked' => (object) 'MAX(IFNULL(user2.id,0))'));

		$this->_dso->leftjoin(array('cfilterm2' => $this->_table['callfiltermember']),
				      $this->_table['callfilter'].'.id = cfilterm2.callfilterid');

		$this->_dso->leftjoin(array('user2' => $this->_table['userfeatures']),
				      'cfilterm2.typeval = user2.id');

		$arr['type'] = $type;

		if($disable !== null)
			$arr['commented'] = intval((bool) $disable);

		$this->_dso->where($arr,$this->_table['callfilter']);

		$this->_dso->joinwhere('user1',array('internal' => 0));
		$this->_dso->joinwhere('user2',array('internal' => 0));

		if($type === 'bosssecretary')
		{
			$this->_dso->joinwhere('cfilterm1',array('type' => 'user','bstype' => 'boss'));

			$this->_dso->andjoinwhere('user1',array('bsfilter' => 'boss'));
			$this->_dso->andjoinwherenot('user1','IFNULL(user1.number,\'\') = \'\'');

			$this->_dso->joinwhere('cfilterm2',array('type' => 'user','bstype' => 'secretary'));

			$this->_dso->andjoinwhere('user2',array('bsfilter' => 'secretary'));
			$this->_dso->andjoinwhere('user2','user1.context = user2.context');
			$this->_dso->andjoinwherenot('user2','IFNULL(user2.number,\'\') = \'\'');
		}

		$this->_dso->new_group('id',$this->_table['callfilter']);
		$this->_dso->new_order($order,null,$this->_table['callfilter']);
		$this->_dso->limit($limit);

		$r = $this->_dso->select_all();

		$this->_dso->reset_group();
		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count(null,'id',$this->_table['callfilter'])) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_callfiltermember_user($type,$id)
	{
		$this->_dso->new_select($this->_table['callfiltermember']);

		$this->_dso->innerjoin($this->_table['userfeatures'],
				       $this->_table['callfiltermember'].'.typeval = '.$this->_table['userfeatures'].'.id',
				       null,
				       array('member_context' => 'context',
					     'member_firstname' => 'firstname',
					     'member_lastname' => 'lastname',
					     'member_number' => 'number',
					     'member_name' => 'name'));

		$this->_dso->where(array('callfilterid' => $id,'type' => 'user'),$this->_table['callfiltermember']);

		$this->_dso->joinwhere($this->_table['userfeatures'],array('internal' => 0));

		if($type === 'bosssecretary')
			$this->_dso->andjoinwherein($this->_table['userfeatures'],array('boss','secretary'));

		$this->_cnt = 0;

		if(($r = $this->_dso->select_all(null,false)) === false
		|| isset($r[0]) === false)
			return(false);

		$this->_cnt = $this->_dso->num_rows();
		$this->_dso->free();

		return($r);
	}

	function get_boss_users($disable=null,$order=null,$limit=null)
	{
		return($this->_get_boss_secretary_users(true,$disable,$order,$limit));
	}

	function get_secretary_users($disable=null,$order=null,$limit=null)
	{
		return($this->_get_boss_secretary_users(false,$disable,$order,$limit));
	}

	function _get_boss_secretary_users($boss=true,$disable=null,$order=null,$limit=null)
	{
		$this->_dso->new_select($this->_table['userfeatures']);

		$where = array();
		$where['internal'] = 0;
		$where['bsfilter'] = (bool) $boss === true ? 'boss' : 'secretary';

		if($disable !== null)
			$where['disable'] = intval((bool) $disable);

		$this->_dso->where($where);
		$this->_dso->andwherenot('IFNULL(number,\'\') = \'\'');

		$this->_dso->new_order($order);
		$this->_dso->limit($limit);

		if(($r = $this->_dso->select_all()) !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_free_boss_users($order=null,$limit=null)
	{
		$this->_dso->new_select($this->_table['userfeatures']);
		$this->_dso->leftjoin($this->_table['callfiltermember'],
				      $this->_table['userfeatures'].'.id = '.$this->_table['callfiltermember'].'.typeval');

		$this->_dso->where(array('bsfilter' => 'boss','internal' => 0));
		$this->_dso->joinwhere($this->_table['callfiltermember'],array('bstype' => 'boss','type' => 'user'));
		$this->_dso->andwherenot('IFNULL('.$this->_table['userfeatures'].'.number,\'\') = \'\'');
		$this->_dso->andwhere($this->_table['callfiltermember'].'.id IS NULL');

		if(($r = $this->_dso->select_all()) !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
