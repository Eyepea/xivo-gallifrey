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

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract','datastorage','abstract','sql.inc'),true);

class xivo_application_service_asterisk_user_sql extends xivo_application_service_asterisk_abstract_sql
{
	var $_table	= array(
				'userfeatures'		=> '',
				'usersip'		=> '',
				'useriax'		=> '',
				'usercustom'		=> '',
				'callfilter'		=> '',
				'callfiltermember'	=> '',
				'phonefunckey'		=> '',
				'agentfeatures'		=> '',
				'groupfeatures'		=> '',
				'queuefeatures'		=> '',
				'meetmefeatures'	=> '');

	function get_nb($arr=null,$disable=null,$initialized=null)
	{
		if(is_array($arr) === false)
			$arr = array();

		$this->_dso->new_select($this->_table['userfeatures']);
		$this->_dso->leftjoin($this->_table['usersip'],
				      $this->_table['userfeatures'].'.protocolid = '.$this->_table['usersip'].'.id');
		$this->_dso->leftjoin($this->_table['useriax'],
				      $this->_table['userfeatures'].'.protocolid = '.$this->_table['useriax'].'.id');
		$this->_dso->leftjoin($this->_table['usercustom'],
				      $this->_table['userfeatures'].'.protocolid = '.$this->_table['usercustom'].'.id');

		$this->_dso->where($this->_table['usersip'].'.id IS NOT NULL');
		$this->_dso->orwhere($this->_table['useriax'].'.id IS NOT NULL');
		$this->_dso->orwhere($this->_table['usercustom'].'.id IS NOT NULL');

		$this->_dso->aggregate_where();

		$sip = $iax = $custom = array();
		$sip['category'] = $iax['category'] = $custom['category'] = 'user';

		$arr['internal'] = 0;

		if($disable !== null)
		{
			$disable = intval((bool) $disable);
			$arr['commented'] = $disable;
			$sip['commented'] = $disable;
			$iax['commented'] = $disable;
			$custom['commented'] = $disable;
		}

		$this->_dso->andwhere($arr,$this->_table['userfeatures']);
		$this->_dso->joinwhere($this->_table['usersip'],$sip);
		$this->_dso->joinwhere($this->_table['useriax'],$iax);
		$this->_dso->joinwhere($this->_table['usercustom'],$custom);

		$this->_dso->andjoinwhere($this->_table['usersip'],
					  $this->_table['userfeatures'].'.protocol = '.$this->_table['usersip'].'.protocol');

		$this->_dso->andjoinwhere($this->_table['useriax'],
					  $this->_table['userfeatures'].'.protocol = '.$this->_table['useriax'].'.protocol');

		$this->_dso->andjoinwhere($this->_table['usercustom'],
					  $this->_table['userfeatures'].'.protocol = '.$this->_table['usercustom'].'.protocol');

		if($initialized !== null)
		{
			if((bool) $initialized === false)
				$this->_dso->andjoinwhere($this->_table['usersip'],
							  'IFNULL('.$this->_table['usersip'].'.fullcontact,\'\') = \'\'');
			else
				$this->_dso->andjoinwherenot($this->_table['usersip'],
							  'IFNULL('.$this->_table['usersip'].'.fullcontact,\'\') = \'\'');
		}

		return(xivo_ulongint($this->_dso->select_count(null,'id',$this->_table['userfeatures'])));
	}

	function get_all($protocols,$disable=null,$initialized=null,$order=null,$limit=null,$value=null,$context=null,$mode=null)
	{
		$this->_cnt = 0;

		if($mode !== 'number')
			$mode = null;

		$arr = array();

		$this->_dso->new_select($this->_table['userfeatures']);

		if(is_array($protocols) === false)
			$use_sip = $use_iax = $use_custom = true;
		else
		{
			$use_sip = in_array(XIVO_SRE_IPBX_AST_PROTO_SIP,$protocols);
			$use_iax = in_array(XIVO_SRE_IPBX_AST_PROTO_IAX,$protocols);
			$use_custom = in_array(XIVO_SRE_IPBX_AST_PROTO_CUSTOM,$protocols);
			$this->_dso->wherein('protocol',$protocols,$this->_table['userfeatures']);
		}

		if($use_sip === false && $use_iax === false && $use_custom === false)
			return(false);

		$arr['internal'] = 0;

		$sip = $iax = $custom = array();
		$sip['category'] = $iax['category'] = $custom['category'] = 'user';

		if($disable !== null)
		{
			$disable = intval((bool) $disable);
			$arr['commented'] = $disable;
			$sip['commented'] = $disable;
			$iax['commented'] = $disable;
			$custom['commented'] = $disable;
		}

		$this->_dso->andwhere($arr,$this->_table['userfeatures']);

		if($mode === 'number')
			$this->_dso->andwherenot('IFNULL('.$this->_table['userfeatures'].'.number,\'\') = \'\'');

		if($use_sip === true)
		{
			$this->_dso->leftjoin($this->_table['usersip'],
					      $this->_table['userfeatures'].'.protocolid = '.$this->_table['usersip'].'.id',
					      null,
					      '*');

			$this->_dso->joinwhere($this->_table['usersip'],$sip);

			$this->_dso->andjoinwhere($this->_table['usersip'],
						  $this->_table['userfeatures'].'.protocol = '.$this->_table['usersip'].'.protocol');

			if($initialized !== null)
			{
				if((bool) $initialized === false)
					$this->_dso->andjoinwhere($this->_table['usersip'],
								  'IFNULL('.$this->_table['usersip'].'.fullcontact,\'\') = \'\'');
				else
					$this->_dso->andjoinwherenot($this->_table['usersip'],
								  'IFNULL('.$this->_table['usersip'].'.fullcontact,\'\') = \'\'');
			}
		}

		if($use_iax === true)
		{
			$this->_dso->leftjoin($this->_table['useriax'],
					      $this->_table['userfeatures'].'.protocolid = '.$this->_table['useriax'].'.id',
					      null,
					      '*');

			$this->_dso->joinwhere($this->_table['useriax'],$iax);

			$this->_dso->andjoinwhere($this->_table['useriax'],
						  $this->_table['userfeatures'].'.protocol = '.$this->_table['useriax'].'.protocol');
		}

		if($use_custom === true)
		{
			$this->_dso->leftjoin($this->_table['usercustom'],
					      $this->_table['userfeatures'].'.protocolid = '.$this->_table['usercustom'].'.id',
					      null,
					      'interface');

			$this->_dso->joinwhere($this->_table['usercustom'],$custom);

			$this->_dso->andjoinwhere($this->_table['usercustom'],
						  $this->_table['userfeatures'].'.protocol = '.$this->_table['usercustom'].'.protocol');
		}

		$this->_dso->aggregate_where();

		if(($has_context = xivo_has_len($context)) === true)
			$this->_dso->where($this->_table['userfeatures'].'.context = %s',$context);

		if(xivo_has_len($value) === true)
		{
			$this->_dso->aggregate_where();
			$this->_dso->wherecontain('firstname',$value,true,$this->_table['userfeatures']);
			$this->_dso->orwherecontain('lastname',$value,true,$this->_table['userfeatures']);
			$this->_dso->orwherecontain('number',$value,true,$this->_table['userfeatures']);
			$this->_dso->orwherecontain('name',$value,true,$this->_table['userfeatures']);

			if(($concat = $this->_dso->concat(array((object) ($this->_table['userfeatures'].'.firstname'),
								' ',
								(object) ($this->_table['userfeatures'].'.lastname')))) !== false)
				$this->_dso->orwherecontain((object) $concat,$value,true);

			if($mode === null && $has_context === false && $context !== false)
				$this->_dso->orwherecontain('context',$value,true,$this->_table['userfeatures']);
		}

		$this->_dso->aggregate_where();

		if($use_sip === true)
			$this->_dso->orwhere($this->_table['usersip'].'.id IS NOT NULL');

		if($use_iax === true)
			$this->_dso->orwhere($this->_table['useriax'].'.id IS NOT NULL');

		if($use_custom === true)
			$this->_dso->orwhere($this->_table['usercustom'].'.id IS NOT NULL');

		$this->_dso->aggregate_where(false,'AND');

		$this->_dso->new_order($order,null,$this->_table['userfeatures']);
		$this->_dso->limit($limit);

		$r = $this->_dso->select_all(null,true,true);

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count(null,'id',$this->_table['userfeatures'])) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function search($value,$context,$protocols,$disable=null,$initialized=null,$order=null,$limit=null,$mode=null)
	{
		$this->_cnt = 0;

		if(xivo_has_len($value) === false && $mode === null)
			return(false);

		return($this->get_all($protocols,$disable,$initialized,$order,$limit,$value,$context,$mode));
	}

	function get_bsfilter($id,$context,$bsfilter)
	{
		if(($id = xivo_ulongint($id)) === 0)
			return(false);

		$this->_dso->new_select($this->_table['userfeatures'],null);

		$this->_dso->innerjoin($this->_table['callfiltermember'],
				       $this->_table['userfeatures'].'.id = '.$this->_table['callfiltermember'].'.typeval',
				       null,
				       'callfilterid');

		$this->_dso->where(array('id' => $id,'internal' => 0),$this->_table['userfeatures']);

		$this->_dso->joinwhere($this->_table['callfiltermember'],array('type' => 'user','bstype' => $bsfilter));
		$this->_dso->andjoinwhere($this->_table['callfiltermember'],
					  $this->_table['userfeatures'].'.bsfilter = '.$this->_table['callfiltermember'].'.bstype');

		$callfilterids = $this->_dso->select_singlecolumn();

		if(isset($callfilterids[0]) === false)
			return(false);

		$this->_dso->new_select($this->_table['callfiltermember'],null);
		$this->_dso->where(array('type' => 'user'),$this->_table['callfiltermember']);
		$this->_dso->andwherenot(array('bstype' => $bsfilter),$this->_table['callfiltermember']);

		$this->_dso->innerjoin($this->_table['userfeatures'],
				       $this->_table['callfiltermember'].'.typeval = '.$this->_table['userfeatures'].'.id',
				       null,
				       '*');

		$this->_dso->joinwhere($this->_table['userfeatures'],array('context' => $context,'internal' => 0));
		$this->_dso->andjoinwhere($this->_table['userfeatures'],
					  $this->_table['callfiltermember'].'.bstype = '.$this->_table['userfeatures'].'.bsfilter');
		$this->_dso->andjoinwherenot($this->_table['userfeatures'],
					     'IFNULL('.$this->_table['userfeatures'].'.number,\'\') = \'\'');

		$this->_dso->innerjoin($this->_table['callfilter'],
				       $this->_table['callfiltermember'].'.callfilterid = '.$this->_table['callfilter'].'.id',
				       null,
				       array('callfilterid'	=> 'id',
				             'callfiltername'	=> 'name'));

		$this->_dso->joinwhere($this->_table['callfilter'],array('type' => 'bosssecretary'));
		$this->_dso->andjoinwhere($this->_table['callfilter'],$this->_table['callfilter'].'.bosssecretary IS NOT NULL');
		$this->_dso->andjoinwherein($this->_table['callfilter'],'id',$callfilterids);

		$r = $this->_dso->select_all();

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function chk_bsfilter($id,$context,$bsfilter,$user2id)
	{
		if(($id = xivo_ulongint($id)) === 0
		|| ($user2id = xivo_ulongint($user2id)) === 0)
			return(false);

		$this->_dso->new_select($this->_table['callfilter'],null);
		$this->_dso->where(array('type' => 'bosssecretary'),$this->_table['callfilter']);
		$this->_dso->andwhere($this->_table['callfilter'].'.bosssecretary IS NOT NULL');

		$this->_dso->innerjoin(array('cfilterm1' => $this->_table['callfiltermember']),
				       $this->_table['callfilter'].'.id = cfilterm1.callfilterid');
		$this->_dso->joinwhere('cfilterm1',array('type'		=> 'user',
							 'bstype'	=> $bsfilter));

		$this->_dso->innerjoin(array('user1' => $this->_table['userfeatures']),
				       'cfilterm1.typeval = user1.id');
		$this->_dso->joinwhere('user1','cfilterm1.bstype = user1.bsfilter');
		$this->_dso->andjoinwhere('user1',array('id' => $id,'internal' => 0));
		$this->_dso->andjoinwherenot('user1','IFNULL(user1.number,\'\') = \'\'');

		$this->_dso->innerjoin(array('cfilterm2' => $this->_table['callfiltermember']),
				       $this->_table['callfilter'].'.id = cfilterm2.callfilterid',
				       null,
				       'id');
		$this->_dso->joinwhere('cfilterm2',array('type' => 'user'));
		$this->_dso->andjoinwherenot('cfilterm2',array('bstype' => $bsfilter));

		$this->_dso->innerjoin(array('user2' => $this->_table['userfeatures']),
				       'cfilterm2.typeval = user2.id');
		$this->_dso->joinwhere('user2','cfilterm2.bstype = user2.bsfilter');
		$this->_dso->andjoinwhere('user2',array('id'		=> $user2id,
							'context'	=> $context,
							'internal'	=> 0));
		$this->_dso->andjoinwherenot('user2','IFNULL(user2.number,\'\') = \'\'');

		return($this->_dso->select_one());
	}

	function get_phonefunckey($id)
	{
		if(($id = xivo_ulongint($id)) === 0)
			return(false);

		$this->_dso->new_select($this->_table['phonefunckey']);
		$this->_dso->where(array('iduserfeatures' => $id),$this->_table['phonefunckey']);
		$this->_dso->leftjoin($this->_table['userfeatures'],
				      $this->_table['phonefunckey'].'.typevalextenumbersright = '.$this->_table['userfeatures'].'.id',
				      null,
				      array('id','*'));
		$this->_dso->joinwhere($this->_table['userfeatures'],
				       $this->_table['phonefunckey'].'.typeextenumbersright = %s',
				       'user');

		$this->_dso->leftjoin($this->_table['agentfeatures'],
				      $this->_table['phonefunckey'].'.typevalextenumbersright = '.$this->_table['agentfeatures'].'.id',
				      null,
				      array('id','*'));
		$this->_dso->joinwhere($this->_table['agentfeatures'],
				       $this->_table['phonefunckey'].'.typeextenumbersright = %s',
				       'agent');

		$this->_dso->leftjoin($this->_table['groupfeatures'],
				      $this->_table['phonefunckey'].'.typevalextenumbersright = '.$this->_table['groupfeatures'].'.id',
				      null,
				      array('id','*'));
		$this->_dso->joinwhere($this->_table['groupfeatures'],
				       $this->_table['phonefunckey'].'.typeextenumbersright = %s',
				       'group');
		$this->_dso->andjoinwhere($this->_table['groupfeatures'],
					  array('deleted' => 0));

		$this->_dso->leftjoin($this->_table['queuefeatures'],
				      $this->_table['phonefunckey'].'.typevalextenumbersright = '.$this->_table['queuefeatures'].'.id',
				      null,
				      array('id','*'));
		$this->_dso->joinwhere($this->_table['queuefeatures'],
				       $this->_table['phonefunckey'].'.typeextenumbersright = %s',
				       'queue');

		$this->_dso->leftjoin($this->_table['meetmefeatures'],
				      $this->_table['phonefunckey'].'.typevalextenumbersright = '.$this->_table['meetmefeatures'].'.id',
				      null,
				      array('id','*'));
		$this->_dso->joinwhere($this->_table['meetmefeatures'],
				       $this->_table['phonefunckey'].'.typeextenumbersright = %s',
				       'meetme');

		$this->_dso->new_order(array('fknum' => SORT_ASC),null,$this->_table['phonefunckey']);

		$r = $this->_dso->select_all(null,true,true);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
