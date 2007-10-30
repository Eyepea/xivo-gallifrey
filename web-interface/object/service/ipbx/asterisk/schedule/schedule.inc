<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_schedule extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'schedule';
	var $_filter	= false;
	var $_origin	= false;

	function xivo_service_asterisk_schedule(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function _prepare($data)
	{
		$data['timebeg'] = (string) $data['timebeg'];
		$data['timeend'] = (string) $data['timeend'];

		$data['endcall'] = array('true' => '','false' => '');
		$data['user'] = array('true' => '','false' => '');
		$data['group'] = array('true' => '','false' => '');
		$data['queue'] = array('true' => '','false' => '');
		$data['meetme'] = array('true' => '','false' => '');
		$data['schedule'] = array('true' => '','false' => '');
		$data['application'] = array('true' => '','false' => '');
		$data['custom'] = array('true' => '','false' => '');

		if(isset($data[$data['typetrue']]) === true)
			$data[$data['typetrue']]['true'] = $data['typevaltrue'];

		if(isset($data[$data['typefalse']]) === true)
			$data[$data['typefalse']]['false'] = $data['typevalfalse'];

		$data['publicholiday'] = (bool) $data['publicholiday'];
		$data['commented'] = (bool) $data['commented'];
		$data['linked'] = (bool) $data['linked'];

		if($data['timebeg'] === '' || $data['timebeg'] === '*')
		{
			$data['timehourbeg'] = $data['timeminbeg'] = '';
			$data['timeend'] = $data['timehourend'] = $data['timeminend'] = '';

			return($data);
		}

		$hour = explode(':',$data['timebeg'],3);

		$data['timehourbeg'] = sprintf('%02u',$hour[0]);

		if(xivo_haslen($hour,1) === true)
			$data['timeminbeg'] = sprintf('%02u',$hour[1]);
		else
			$data['timeminbeg'] = '00';

		if($data['timeend'] === '' || $data['timeend'] === '*')
		{
			$data['timeend'] = $data['timehourend'] = $data['timeminend'] = '';
			return($data);
		}

		$hour = explode(':',$data['timeend'],3);

		$data['timehourend'] = sprintf('%02u',$hour[0]);

		if(xivo_haslen($hour,1) === true)
			$data['timeminend'] = sprintf('%02u',$hour[1]);
		else
			$data['timeminend'] = '00';

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		if(isset($arr['timebeg'],$arr['timeend']) === true
		&& $arr['timebeg'] === $arr['timeend'])
			$arr['timeend'] = null;

		if(isset($arr['daynamebeg'],$arr['daynameend']) === true
		&& $arr['daynamebeg'] === $arr['daynameend'])
			$arr['daynameend'] = null;

		if(isset($arr['daynumbeg'],$arr['daynumend']) === true
		&& $arr['daynumbeg'] === $arr['daynumend'])
			$arr['daynumend'] = null;

		if(isset($arr['monthbeg'],$arr['monthend']) === true
		&& $arr['monthbeg'] === $arr['monthend'])
			$arr['monthend'] = null;

		if(isset($arr['name']) === true)
		{
			$where = array('name' => $arr['name']);

			if($this->get_id($where) !== false)
				return(false);
		}

		return($this->_dso->add($arr));
	}

	function edit($id,$arr,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		if(isset($arr['timebeg'],$arr['timeend']) === true
		&& $arr['timebeg'] === $arr['timeend'])
			$arr['timeend'] = null;

		if(isset($arr['daynamebeg'],$arr['daynameend']) === true
		&& $arr['daynamebeg'] === $arr['daynameend'])
			$arr['daynameend'] = null;

		if(isset($arr['daynumbeg'],$arr['daynumend']) === true
		&& $arr['daynumbeg'] === $arr['daynumend'])
			$arr['daynumend'] = null;

		if(isset($arr['monthbeg'],$arr['monthend']) === true
		&& $arr['monthbeg'] === $arr['monthend'])
			$arr['monthend'] = null;

		if(isset($arr['name']) === true)
		{
			$where = array('name' => $arr['name']);

			if(($whereid = $this->get_id($where)) !== false && $whereid !== $id)
				return(false);
		}

		return($this->_dso->edit($id,$arr,$disable));
	}

	function linked($id,$disable=null)
	{
		return($this->edit($id,array('linked' => true),$disable));
	}

	function unlinked($id,$disable=null)
	{
		return($this->edit($id,array('linked' => false),$disable));
	}

	function linked_where($id,$disable=null)
	{
		return($this->edit_where($id,array('linked' => true),$disable));
	}

	function unlinked_where($id,$disable=null)
	{
		return($this->edit_where($id,array('linked' => false),$disable));
	}

	function mk_time($beg,$end)
	{
		$r = array('beg' => '*','end' => '');

		if(is_array($beg) === false || is_array($end) === false
		|| isset($beg['hour'],$beg['min'],$end['hour'],$end['min']) === false)
			return(false);

		$beg['hour'] = strval($beg['hour']);
		$beg['min'] = strval($beg['min']);

		$end['hour'] = strval($end['hour']);
		$end['min'] = strval($end['min']);

		$begint = $beg['hour'].$beg['min'];

		if($begint === '' || strpos($begint,'*') !== false)
			return($r);
		else if(ctype_digit($begint) === false)
			return(false);

		$r['beg'] = sprintf('%02u:%02u',$beg['hour'],$beg['min']);

		$endint = $end['hour'].$end['min'];

		if($endint === '')
			return($r);
		else if(ctype_digit($endint) === false || (int) $begint > (int) $endint)
			return(false);

		$r['end'] = sprintf('%02u:%02u',$end['hour'],$end['min']);

		return($r);
	}

	function chk_date($arr)
	{
		if(is_array($arr) === false)
			return(false);

		if(isset($arr['daynamebeg']) === false
		|| $arr['daynamebeg'] === ''
		|| $arr['daynamebeg'] === '*')
		{
			$arr['daynamebeg'] = '*';
			$arr['daynameend'] = null;
		}

		if(isset($arr['daynumbeg']) === false
		|| $arr['daynumbeg'] === ''
		|| $arr['daynumbeg'] === '*')
		{
			$arr['daynumbeg'] = '*';
			$arr['daynumend'] = null;
		}

		if(isset($arr['monthbeg']) === false
		|| $arr['monthbeg'] === ''
		|| $arr['monthbeg'] === '*')
		{
			$arr['monthbeg'] = '*';
			$arr['monthend'] = null;
		}

		if($arr['daynumbeg'] === '*' || $arr['monthbeg'] === '*')
			return($arr);

		$leapdbeg = strtolower($arr['daynumbeg'].'-'.$arr['monthbeg'].'-2000');
		$normdbeg = strtolower($arr['daynumbeg'].'-'.$arr['monthbeg'].'-2001');

		$leapdate = strtolower(date('j-M-Y',strtotime($arr['daynumbeg'].' '.$arr['monthbeg'].' 2000')));
		$normdate = strtolower(date('j-M-Y',strtotime($arr['daynumbeg'].' '.$arr['monthbeg'].' 2001')));

		if($leapdbeg !== $leapdate && $normdbeg !== $normdate)
			return(false);

		if($arr['daynumend'] === null || $arr['monthend'] === null)
			return($arr);

		$leapdend = strtolower($arr['daynumend'].'-'.$arr['monthend'].'-2000');
		$normdend = strtolower($arr['daynumend'].'-'.$arr['monthend'].'-2001');

		$leapdate = strtolower(date('j-M-Y',strtotime($arr['daynumend'].' '.$arr['monthend'].' 2000')));
		$normdate = strtolower(date('j-M-Y',strtotime($arr['daynumend'].' '.$arr['monthend'].' 2001')));

		if($leapdend !== $leapdate && $normdend !== $normdate)
			return(false);

		return($arr);
	}

	function is_valid($type,$typeval,$id=null,$disable=null)
	{
		switch($type)
		{
			case 'endcall':
			case 'application':
				return($this->_chk_dest_value($type,$typeval));
			case 'custom':
				return($this->chk_dest_custom($typeval));
			case 'schedule':
				return($this->chk_dest_schedule($typeval,$id,$disable));
			case 'user':
				return($this->chk_dest_user($typeval,null,$disable));
			case 'group':
				return($this->chk_dest_group($typeval,null,$disable));
			case 'queue':
				return($this->chk_dest_queue($typeval,null,$disable));
			case 'meetme':
				return($this->chk_dest_meetme($typeval,null,$disable));
		}

		return(false);
	}

	function get_all($id=0,$disable=null,$prepare=true,$order=null,$limit=null)
	{
		if(($list = parent::get_all($disable,$prepare,$order,$limit)) === false)
			return(false);

		if(($id = xivo_uint($id)) === 0)
			return($list);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info = $list[$i];

			if($id === (int) $info['id'])
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
