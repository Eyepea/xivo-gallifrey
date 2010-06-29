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

class xivo_application_service_asterisk_incall_sql extends xivo_application_abstract_datastorage_sql
{
	var $_table	= array(
				'incall'		=> '',
				'dialaction'		=> '',
				'userfeatures'		=> '',
				'groupfeatures'		=> '',
				'queuefeatures'		=> '',
				'meetmefeatures'	=> '',
				'voicemail'		=> '',
				'schedule'		=> '',
				'voicemenu'		=> '');

	function get_all($disable=null,$order=null,$limit=null,$value=null)
	{
		$this->_dso->new_select($this->_table['incall']);

		$this->_dso->innerjoin($this->_table['dialaction'],
				       $this->_table['incall'].'.id = '.$this->_table['dialaction'].'.categoryval',
				       null,
				       array('linked','action','actionarg1','actionarg2'));

		$this->_dso->leftjoin($this->_table['userfeatures'],
				      $this->_table['dialaction'].'.actionarg1 = '.$this->_table['userfeatures'].'.id',
				      null,
				      array('userfirstname'	=> 'firstname',
					    'userlastname'	=> 'lastname',
					    'usernumber'	=> 'number',
					    'usercontext'	=> 'context',
					    'username'		=> 'name'));

		$this->_dso->joinwhere($this->_table['userfeatures'],
				       $this->_table['dialaction'].'.action = %s',
				       'user');

		$this->_dso->leftjoin($this->_table['groupfeatures'],
				      $this->_table['dialaction'].'.actionarg1 = '.$this->_table['groupfeatures'].'.id',
				      null,
				      array('groupname'		=> 'name',
					    'groupnumber'	=> 'number',
					    'groupcontext'	=> 'context'));

		$this->_dso->joinwhere($this->_table['groupfeatures'],
				       $this->_table['dialaction'].'.action = %s',
				       'group');

		$this->_dso->andjoinwhere($this->_table['groupfeatures'],
					  $this->_table['groupfeatures'].'.deleted = 0');

		$this->_dso->leftjoin($this->_table['queuefeatures'],
				      $this->_table['dialaction'].'.actionarg1 = '.$this->_table['queuefeatures'].'.id',
				      null,
				      array('queuename'		=> 'name',
					    'queuenumber'	=> 'number',
					    'queuecontext'	=> 'context'));

		$this->_dso->joinwhere($this->_table['queuefeatures'],
				       $this->_table['dialaction'].'.action = %s',
				       'queue');

		$this->_dso->leftjoin($this->_table['meetmefeatures'],
				      $this->_table['dialaction'].'.actionarg1 = '.$this->_table['meetmefeatures'].'.id',
				      null,
				      array('meetmename'	=> 'name',
					    'meetmenumber'	=> 'number',
					    'meetmecontext'	=> 'context'));

		$this->_dso->joinwhere($this->_table['meetmefeatures'],
				       $this->_table['dialaction'].'.action = %s',
				       'meetme');

		$this->_dso->leftjoin($this->_table['voicemail'],
				      $this->_table['dialaction'].'.actionarg1 = '.$this->_table['voicemail'].'.uniqueid',
				      null,
				      array('voicemailfullname'	=> 'fullname',
					    'voicemailmailbox'	=> 'mailbox',
					    'voicemailcontext'	=> 'context'));

		$this->_dso->joinwhere($this->_table['voicemail'],
				       $this->_table['dialaction'].'.action = %s',
				       'voicemail');

		$this->_dso->leftjoin($this->_table['schedule'],
				      $this->_table['dialaction'].'.actionarg1 = '.$this->_table['schedule'].'.id',
				      null,
				      array('schedulename'	=> 'name',
					    'schedulecontext'	=> 'context'));

		$this->_dso->joinwhere($this->_table['schedule'],
				       $this->_table['dialaction'].'.action = %s',
				       'schedule');

		$this->_dso->leftjoin($this->_table['voicemenu'],
				      $this->_table['dialaction'].'.actionarg1 = '.$this->_table['voicemenu'].'.id',
				      null,
				      array('voicemenuname'	=> 'name',
					    'voicemenunumber'	=> 'number',
					    'voicemenucontext'	=> 'context'));

		$this->_dso->joinwhere($this->_table['voicemenu'],
				       $this->_table['dialaction'].'.action = %s',
				       'voicemenu');

		$where = array();
		$where['event'] = 'answer';
		$where['category'] = 'incall';

		$this->_dso->where($where,$this->_table['dialaction']);

		if($disable !== null)
			$this->_dso->andwhere(array('commented' => intval((bool) $disable)),
					      $this->_table['incall']);

		if(dwho_has_len($value) === true)
		{
			$this->_dso->aggregate_where();
			$this->_dso->wherecontain('exten',$value,true,$this->_table['incall']);
			$this->_dso->orwherecontain('description',$value,true,$this->_table['incall']);
			$this->_dso->orwherecontain('firstname',$value,true,$this->_table['userfeatures']);
			$this->_dso->orwherecontain('lastname',$value,true,$this->_table['userfeatures']);
			$this->_dso->orwherecontain('number',$value,true,$this->_table['userfeatures']);
			$this->_dso->orwherecontain('name',$value,true,$this->_table['userfeatures']);
			$this->_dso->orwherecontain('name',$value,true,$this->_table['groupfeatures']);
			$this->_dso->orwherecontain('number',$value,true,$this->_table['groupfeatures']);
			$this->_dso->orwherecontain('name',$value,true,$this->_table['queuefeatures']);
			$this->_dso->orwherecontain('number',$value,true,$this->_table['queuefeatures']);
			$this->_dso->orwherecontain('name',$value,true,$this->_table['meetmefeatures']);
			$this->_dso->orwherecontain('number',$value,true,$this->_table['meetmefeatures']);
			$this->_dso->orwherecontain('fullname',$value,true,$this->_table['voicemail']);
			$this->_dso->orwherecontain('mailbox',$value,true,$this->_table['voicemail']);
			$this->_dso->orwherecontain('email',$value,true,$this->_table['voicemail']);
			$this->_dso->orwherecontain('name',$value,true,$this->_table['schedule']);
			$this->_dso->orwherecontain('name',$value,true,$this->_table['voicemenu']);
			$this->_dso->orwherecontain('number',$value,true,$this->_table['voicemenu']);
			$this->_dso->aggregate_where();
		}

		$this->_dso->new_order($order,null,$this->_table['incall']);
		$this->_dso->limit($limit);

		$r = $this->_dso->select_all();

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count(null,'id',$this->_table['incall'])) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function search($value,$disable=null,$order=null,$limit=null)
	{
		$this->_cnt = 0;

		if(dwho_has_len($value) === false)
			return(false);

		return($this->get_all($disable,$order,$limit,$value));
	}
}

?>
