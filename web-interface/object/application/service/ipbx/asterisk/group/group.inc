<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_group extends xivo_application_asterisk_abstract
{
	var $_origin	= array();
	var $_status	= '';

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($gfeatures = &$this->_ipbx->get_module('groupfeatures')) === false
		|| ($this->_info['gfeatures'] = $gfeatures->get($id)) === false
		|| ($queue = &$this->_ipbx->get_module('queue')) === false
		|| ($this->_info['queue'] = $queue->get($this->_info['gfeatures']['name'])) === false)
			return(false);

		$this->_origin['gfeatures'] = $gfeatures->get_origin();
		$this->_origin['queue'] = $queue->get_origin();

		$this->_get_localexten();
		$this->_get_extenumbers();
		$this->_get_user();
		$this->_get_rightcall();
		$this->_get_dialstatus();

		return($this->_info);
	}

	function _get_localexten()
	{
		$this->_info['localexten'] = false;
		$this->_origin['localexten'] = false;

		if(($extensions = &$this->_ipbx->get_module('extensions')) === false)
			return(false);

		$this->_info['localexten'] = $extensions->get_exten('macro',
								    $this->_info['gfeatures']['number'],
								    $this->_info['gfeatures']['context']);

		$this->_origin['localexten'] = $extensions->get_origin();

		return($this->_info['localexten']);
	}

	function _get_extenumbers()
	{
		$this->_info['extenumbers'] = false;
		$this->_origin['extenumbers'] = false;

		if(($extenumbers = &$this->_ipbx->get_module('extenumbers')) === false)
			return(false);

		$extenumbers_where = array();
		$extenumbers_where['exten'] = $this->_info['gfeatures']['number'];
		$extenumbers_where['context'] = $this->_info['gfeatures']['context'];

		$this->_info['extenumbers'] = $extenumbers->get_where($extenumbers_where);
		$this->_origin['extenumbers'] = $extenumbers->get_origin();

		return($this->_info['extenumbers']);
	}

	function _get_user()
	{
		$this->_info['user'] = false;
		$this->_origin['user'] = false;

		$where = array();
		$where['usertype'] = 'user';
		$where['category'] = 'group';
		$where['queue_name'] = $this->_info['gfeatures']['name'];

		if(($queuemember = &$this->_ipbx->get_module('queuemember')) === false
		|| ($this->_info['user'] = $queuemember->get_all_where($where)) === false
		|| isset($this->_info['user'][0]) === false)
			return(false);

		$this->_origin['user'] = $queuemember->get_origin_list();

		return($this->_info['user']);
	}

	function _get_rightcall()
	{
		$this->_info['rightcall'] = false;
		$this->_origin['rightcall'] = false;

		$where = array();
		$where['type'] = 'group';
		$where['typeval'] = $this->_info['gfeatures']['id'];

		if(($rightcallmember = &$this->_ipbx->get_module('rightcallmember')) === false
		|| ($this->_info['rightcall'] = $rightcallmember->get_all_where($where)) === false
		|| isset($this->_info['rightcall'][0]) === false)
			return(false);

		$this->_origin['rightcall'] = $rightcallmember->get_origin_list();

		return($this->_info['rightcall']);
	}

	function _get_dialstatus()
	{
		$this->_info['dialstatus'] = false;
		$this->_origin['dialstatus'] = false;

		$where = array();
		$where['category'] = 'group';
		$where['categoryval'] = $this->_info['gfeatures']['id'];

		if(($dialstatus = &$this->_ipbx->get_module('dialstatus')) === false
		|| ($info = $dialstatus->get_all_where($where)) === false
		|| ($nb = count($info)) === 0)
			return(false);

		$this->_info['dialstatus'] = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($this->_info['dialstatus'][$info[$i]['status']]) === true)
				continue;

			$this->_info['dialstatus'][$info[$i]['status']] = $info[$i];
		}

		$this->_origin['dialstatus'] = $dialstatus->get_origin_list();

		return($this->_info['dialstatus']);
	}

	function get_element()
	{
		$r = array();

		if(($gfeatures = &$this->_ipbx->get_module('groupfeatures')) !== false)
			$r['gfeatures'] = $gfeatures->get_element();
		else
			$r['gfeatures'] = false;

		if(($queue = &$this->_ipbx->get_module('queue')) !== false)
			$r['queue'] = $queue->get_element();
		else
			$r['queue'] = false;

		if(($dialstatus = &$this->_ipbx->get_module('dialstatus')) !== false)
			$r['dialstatus'] = $dialstatus->get_element();
		else
			$r['dialstatus'] = false;

		return($r);
	}

	function get_element_dialstatus_list()
	{
		$r = array();

		xivo::load_class('xivo_sort');
		$sortidentity = new xivo_sort(array('key' => 'identity'));
		$sortname = new xivo_sort(array('key' => 'name'));

		if(xivo_issa('gfeatures',$this->_info) === false)
			$id = 0;
		else
			$id = $this->_info['gfeatures']['id'];

		if(($ufeatures = &$this->_ipbx->get_module('userfeatures')) === false
		|| ($r['users'] = $ufeatures->get_all_number()) === false)
			$r['users'] = false;
		else
			usort($r['users'],array(&$sortidentity,'str_usort'));

		if(($gfeatures = &$this->_ipbx->get_module('groupfeatures')) === false
		|| ($r['groups'] = $gfeatures->get_all_number($id)) === false)
			$r['groups'] = false;
		else
			usort($r['groups'],array(&$sortidentity,'str_usort'));

		if(($qfeatures = &$this->_ipbx->get_module('queuefeatures')) === false
		|| ($r['queues'] = $qfeatures->get_all_number()) === false)
			$r['queues'] = false;
		else
			usort($r['queues'],array(&$sortidentity,'str_usort'));

		if(($mfeatures = &$this->_ipbx->get_module('meetmefeatures')) === false
		|| ($r['meetme'] = $mfeatures->get_all_number()) === false)
			$r['meetme'] = false;
		else
			usort($r['meetme'],array(&$sortidentity,'str_usort'));

		if(($schedule = &$this->_ipbx->get_module('schedule')) === false
		|| ($r['schedule'] = $schedule->get_all()) === false)
			$r['schedule'] = false;
		else
			usort($r['schedule'],array(&$sortname,'str_usort'));

		if(($sounds = &$this->_ipbx->get_module('sounds')) === false)
			$r['sounds'] = false;
		else
			$r['sounds'] = $sounds->get_list('dialstatus',true);

		return($r);
	}

	function set_add($arr)
	{
		return($this->_set('add',$arr));
	}

	function set_edit($arr)
	{
		if(empty($this->_info) === true)
			return(false);

		return($this->_set('edit',$arr));
	}

	function _set($action,$arr)
	{
		$this->_reset();

		if(($action !== 'add' && $action !== 'edit') === true
		|| xivo_issa('gfeatures',$arr) === false
		|| xivo_issa('queue',$arr) === false)
			return(false);

		$this->set_groupfeatures($arr['gfeatures']);

		$name = $this->get_result_var('gfeatures','name');
		$number = (string) $this->get_result_var('gfeatures','number');
		$context = (string) $this->get_result_var('gfeatures','context');

		$arr['queue']['name'] = $name;

		$this->set_queue($arr['queue']);

		if($this->_errnb === 0 && $number !== '')
		{
			$appdata = array('appdata' => 'supergroup');

			if(($localexten = $this->set_macro($action,'localexten',$number,$appdata,$context)) === false)
				$this->_set_error('localexten');

			$this->_set_result('localexten',$localexten);

			$this->set_extenumbers($action,$number,$context);
		}

		if(xivo_issa('user',$arr) === true)
			$this->set_user($action,$arr['user'],$name);

		if(xivo_issa('rightcall',$arr) === true)
			$this->set_rightcall($arr['rightcall']);

		if(xivo_issa('dialstatus',$arr) === true)
			$this->set_dialstatus($arr['dialstatus']);

		if($this->get_errnb() > 0)
			$r = false;
		else
			$r = true;

		return($r);
	}

	function set_groupfeatures($arr)
	{
		if(is_array($arr) === false
		|| ($gfeatures = &$this->_ipbx->get_module('groupfeatures')) === false)
		{
			$this->_set_result('gfeatures',null);
			$this->_set_error('gfeatures','invalid module');
			return(false);
		}

		if(($rs = $gfeatures->chk_values($arr)) === false)
		{
			$this->_set_result('gfeatures',$gfeatures->get_filter_result());
			$this->_set_error('gfeatures',$gfeatures->get_filter_error());

			return(false);
		}

		$this->_set_result('gfeatures',$rs);

		return(true);
	}

	function set_queue($arr)
	{
		if(is_array($arr) === false
		|| ($queue = &$this->_ipbx->get_module('queue')) === false)
		{
			$this->_set_result('queue',null);
			$this->_set_error('queue','invalid module');
			return(false);
		}

		$arr['category'] = 'group';

		if(($rs = $queue->chk_values($arr)) === false)
		{
			$this->_set_result('queue',$queue->get_filter_result());
			$this->_set_error('queue',$queue->get_filter_error());

			return(false);
		}

		$this->_set_result('queue',$rs);

		return(true);
	}

	function set_user($action,$list,$name)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| is_array($list) === false
		|| is_scalar($name) === false
		|| ($ufeatures = &$this->_ipbx->get_module('userfeatures')) === false
		|| ($queuemember = &$this->_ipbx->get_module('queuemember')) === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === 0)
			return(null);

		$info = array();
		$info['usertype'] = 'user';
		$info['userid'] = 0;
		$info['category'] = 'group';

		$where = $info;

		$info['queue_name'] = $name;

		if($action === 'edit')
		{
			if(xivo_issa('gfeatures',$this->_info) === false)
				return(false);

			$where['queue_name'] = $this->_info['gfeatures']['name'];
		}

		$tmp = $reslist = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($uinfo = $ufeatures->get($list[$i])) === false)
			{
				$this->_set_error('user','wrong user');
				continue;
			}

			$info['penalty'] = 0;
			$info['userid'] = $where['userid'] = $uinfo['id'];

			if($action === 'edit'
			&& $queuemember->get_where($where) !== false
			&& ($orig = $queuemember->get_origin()) !== false)
				$info = array_merge($orig,$info);
			else
			{
				$interface = $this->mk_interface($uinfo['name'],
								 null,
								 $uinfo['context'],
								 $uinfo['number'],
								 $uinfo['protocol']);

				if($interface === false)
					continue;

				$info['interface'] = $interface['interface'];
				$info['channel'] = $interface['channel'];
			}

			if(($rs = $queuemember->chk_values($info)) === false
			|| isset($tmp[$uinfo['id']]) === true)
				continue;

			$tmp[$uinfo['id']] = 1;
			$reslist[] = $rs;
		}

		if(isset($reslist[0]) === false)
		{
			$this->_set_result('user',false);
			return(false);
		}

		$this->_set_result('user',$reslist);

		return(true);
	}

	function set_rightcall($list)
	{
		if(is_array($list) === false
		|| ($rightcallmember = &$this->_ipbx->get_module('rightcallmember')) === false
		|| ($rightcall = &$this->_ipbx->get_module('rightcall')) === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === 0)
			return(null);

		$info = array();
		$info['rightcallid'] = 0;
		$info['type'] = 'group';
		$info['typeval'] = 0;
		
		$tmp = $reslist = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($rightcallid = $rightcall->get_id($list[$i])) === false)
			{
				$this->_set_error('rightcall','wrong rightcall');
				continue;
			}

			$info['rightcallid'] = $rightcallid;

			if(($rs = $rightcallmember->chk_values($info)) === false
			|| isset($tmp[$rightcallid]) === true)
				continue;

			$tmp[$rightcallid] = 1;
			$reslist[] = $rs;
		}

		if(isset($reslist[0]) === false)
		{
			$this->_set_result('rightcall',false);
			return(false);
		}

		$this->_set_result('rightcall',$reslist);

		return(true);
	}

	function set_dialstatus($list)
	{
		if(is_array($list) === false
		|| ($dialstatus = &$this->_ipbx->get_module('dialstatus')) === false
		|| ($arr = xivo_get_aks($list,true)) === false)
			return(false);

		if($arr['cnt'] === 0)
			return(null);

		$info = array();
		$info['category'] = 'group';
		$info['categoryval'] = 0;
		$info['linked'] = true;

		$reslist = array();

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$list[$key];

			if(is_array($val) === false)
				continue;

			$info['status'] = $key;
			$info = array_merge($info,$val);

			if(($rs = $dialstatus->chk_values($info)) === false
			|| isset($tmp[$rs['status']]) === true)
				continue;

			$reslist[$rs['status']] = $rs;
		}

		if(empty($reslist) === true)
		{
			$this->_set_result('dialstatus',false);
			return(false);
		}

		$this->_set_result('dialstatus',$reslist);

		return(true);
	}

	function add()
	{
		$this->_status = 'add';
		$this->_return = array();
		$gfeaturesid = null;

		if($this->get_errnb() > 0
		|| ($gfeaturesid = $this->add_groupfeatures()) === false
		|| $this->add_queue() === false
		|| $this->add_macro('localexten') === false
		|| $this->add_extenumbers() === false
		|| $this->add_user() === false
		|| $this->add_rightcall($gfeaturesid) === false
		|| $this->add_dialstatus($gfeaturesid) === false)
		{
			if($gfeaturesid !== null)
				$this->_reverse_add();

			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function add_groupfeatures()
	{
		if(($rs = $this->get_result('gfeatures')) === null
		|| $rs === false)
			return($rs);

		$gfeatures = &$this->_ipbx->get_module('groupfeatures');

		$this->_return['groupfeatures'] = $gfeatures->add($rs);

		return($this->_return['groupfeatures']);
	}

	function add_queue()
	{
		if(($rs = $this->get_result('queue')) === null
		|| $rs === false)
			return($rs);

		$queue = &$this->_ipbx->get_module('queue');

		if($queue->add($rs) === false)
			return(false);

		$this->_return['queue'] = $rs['name'];

		return($this->_return['queue']);
	}

	function add_user()
	{
		if(($rs = $this->get_result('user')) === null || $rs === false)
			return($rs);

		$queuemember = &$this->_ipbx->get_module('queuemember');

		$id = $this->_return['user'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$id['queue_name'] = $rs[$i]['queue_name'];
			$id['interface'] = $rs[$i]['interface'];

			if($queuemember->add($rs[$i]) === false)
				return(false);

			$this->_return['user'][] = $id;
		}

		if(isset($this->_return['user'][0]) === false)
			$this->_return['user'] = false;

		return(true);
	}

	function add_rightcall($gfeaturesid)
	{
		if(($gfeaturesid = xivo_uint($gfeaturesid)) === 0)
			return(false);

		if(($rs = $this->get_result('rightcall')) === null || $rs === false)
			return($rs);

		$rightcallmember = &$this->_ipbx->get_module('rightcallmember');

		$this->_return['rightcall'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['typeval'] = $gfeaturesid;

			if(($this->_return['rightcall'][] = $rightcallmember->add($rs[$i])) === false)
				return(false);
		}

		if(isset($this->_return['rightcall'][0]) === false)
			$this->_return['rightcall'] = false;

		return(true);
	}

	function add_dialstatus($gfeaturesid)
	{
		if(($gfeaturesid = xivo_uint($gfeaturesid)) === 0)
			return(false);

		if(($rs = $this->get_result('dialstatus')) === null || $rs === false)
			return($rs);

		if(($arr = xivo_get_aks($rs)) === false)
			return(false);

		$dialstatus = &$this->_ipbx->get_module('dialstatus');

		$this->_return['dialstatus'] = array();

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$ref = &$rs[$key];
			$ref['categoryval'] = $gfeaturesid;

			if($dialstatus->is_valid($ref['category'],
						 $ref['categoryval'],
						 $ref['type'],
						 $ref['typeval']) === false
			|| ($this->_return['dialstatus'][$ref['status']] = $dialstatus->add($ref)) === false)
				return(false);
		}

		if(empty($this->_return['dialstatus']) === true)
			$this->_return['dialstatus'] = false;

		return(true);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'add'
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$this->_return[$key];

			if($val === false)
				continue;

			switch($key)
			{
				case 'localexten':
					$extensions = &$this->_ipbx->get_module('extensions');
					$extensions->delete_exten($val);
					break;
				case 'user':
					$queuemember = &$this->_ipbx->get_module('queuemember');

					if(is_array($val) === false || ($nb = count($val)) === 0)
						continue;

					for($j = 0;$j < $nb;$j++)
					{
						if($val[$j] !== false)
							$queuemember->delete($val[$j]);
					}
					break;
				case 'rightcall':
					$rightcallmember = &$this->_ipbx->get_module('rightcallmember');

					if(is_array($val) === false || ($nb = count($val)) === 0)
						continue;

					for($j = 0;$j < $nb;$j++)
					{
						if($val[$j] !== false)
							$rightcallmember->delete($val[$j]);
					}
					break;
				case 'dialstatus':
					$dialstatus = &$this->_ipbx->get_module('dialstatus');

					if(is_array($val) === false || ($nb = count($val)) === 0)
						continue;

					for($j = 0;$j < $nb;$j++)
					{
						if($val[$j] !== false)
							$dialstatus->delete($val[$j]);
					}
					break;
				default:
					if(($module = &$this->_ipbx->get_module($key)) === false)
						continue;

					$module->delete($val);
			}
		}
		return(true);
	}

	function edit()
	{
		$this->_status = 'edit';
		$this->_return = array();

		if(is_array($this->_info) === false
		|| empty($this->_info) === true
		|| $this->get_errnb() > 0
		|| $this->edit_groupfeatures() === false)
		{
			$this->_status = '';
			return(false);
		}

		if($this->edit_queue() === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		$unlinks = false;

		if($this->_info['gfeatures']['number'] !== '')
		{
			if($this->get_result('localexten') === null
			|| $this->get_result('extenumbers') === null)
			{
				$unlinks = true;
				$this->delete_macro('localexten');
				$this->delete_extenumbers();
			}
			else if($this->edit_macro('localexten') === false
			|| $this->edit_extenumbers() === false)
			{
				$this->_reverse_edit();
				$this->_status = '';
				return(false);
			}
		}
		else if($this->add_macro('localexten') === false
		|| $this->add_extenumbers() === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		if($this->edit_user($this->_info['gfeatures']['name']) === false
		|| $this->edit_rightcall() === false
		|| $this->edit_dialstatus() === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		if($unlinks === true)
			$this->unlinks();

		$this->_status = '';

		return(true);
	}

	function edit_groupfeatures()
	{
		$rs = false;

		if(xivo_issa('gfeatures',$this->_info) === false
		|| ($rs = $this->get_result('gfeatures')) === null
		|| $rs === false)
			return($rs);

		$gfeatures = &$this->_ipbx->get_module('groupfeatures');
		$this->_return['groupfeatures'] = $gfeatures->edit($this->_info['gfeatures']['id'],$rs);

		return($this->_return['groupfeatures']);
	}

	function edit_queue()
	{
		$rs = false;

		if(xivo_issa('queue',$this->_info) === false
		|| ($rs = $this->get_result('queue')) === null
		|| $rs === false)
			return($rs);

		$queue = &$this->_ipbx->get_module('queue');

		if($queue->edit($this->_info['queue']['name'],$rs) === false)
			return(false);

		$this->_return['queue'] = $rs['name'];

		return($this->_return['queue']);
	}

	function edit_user($name)
	{
		if(is_scalar($name) === false)
			return(false);

		if(xivo_issa('user',$this->_info) === false)
			return($this->add_user());

		$this->_return['user'] = $this->delete_user($name);

		if(($rs = $this->get_result('user')) === null)
			return($this->_return['user']);

		if($rs === false)
			return(false);

		$queuemember = &$this->_ipbx->get_module('queuemember');

		$id = $this->_return['user'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$id['queue_name'] = $rs[$i]['queue_name'];
			$id['interface'] = $rs[$i]['interface'];

			if($queuemember->add($rs[$i]) === false)
				return(false);

			$this->_return['user'][] = $id;
		}

		if(isset($this->_return['user'][0]) === false)
			$this->_return['user'] = false;

		return(true);
	}

	function edit_rightcall()
	{
		if(xivo_issa('gfeatures',$this->_info) === false)
			return(false);

		if(xivo_issa('rightcall',$this->_info) === false)
			return($this->add_rightcall($this->_info['gfeatures']['id']));

		$this->_return['rightcall'] = $this->delete_rightcall();

		if(($rs = $this->get_result('rightcall')) === null)
			return($this->_return['rightcall']);

		if($rs === false)
			return(false);

		$rightcallmember = &$this->_ipbx->get_module('rightcallmember');

		$this->_return['rightcall'] = array();

		$i = 0;
		$nb = count($rs);

		if(($cnt = count($this->_info['rightcall'])) !== 0)
		{
			for($i;$i < $cnt;$i++)
			{
				if(isset($rs[$i]) === false)
				{
					$i = $nb;
					break;
				}

				$id = $this->_info['rightcall'][$i]['id'];
				$rs[$i]['typeval'] = $this->_info['gfeatures']['id'];

				if(($this->_return['rightcall'][] = $rightcallmember->add($rs[$i],$id)) === false)
					return(false);
			}
		}

		if($i >= $nb)
		{
			if(isset($this->_return['rightcall'][0]) === false)
				$this->_return['rightcall'] = false;

			return(true);
		}

		for($i;$i < $nb;$i++)
		{
			$rs[$i]['typeval'] = $this->_info['gfeatures']['id'];

			if(($this->_return['rightcall'][] = $rightcallmember->add($rs[$i])) === false)
				return(false);
		}

		if(isset($this->_return['rightcall'][0]) === false)
			$this->_return['rightcall'] = false;

		return(true);
	}

	function edit_dialstatus()
	{
		if(xivo_issa('gfeatures',$this->_info) === false)
			return(false);

		if(xivo_issa('dialstatus',$this->_info) === false)
			return($this->add_dialstatus($this->_info['gfeatures']['id']));

		$this->_return['dialstatus'] = $this->delete_dialstatus();

		if(($rs = $this->get_result('dialstatus')) === null)
			return($this->_return['dialstatus']);

		if(($arr = xivo_get_aks($rs)) === false)
			return(false);

		$dialstatus = &$this->_ipbx->get_module('dialstatus');

		$this->_return['dialstatus'] = array();

		$i = 0;

		if(($list = xivo_get_aks($this->_info['dialstatus'])) !== false)
		{
			for($i;$i < $list['cnt'];$i++)
			{
				$key = &$list['keys'][$i];

				if(isset($rs[$key]) === false)
				{
					$i = $arr['cnt'];
					break;
				}

				$ref = &$rs[$key];

				$id = $this->_info['dialstatus'][$key]['id'];
				$ref['categoryval'] = $this->_info['gfeatures']['id'];

				if($dialstatus->is_valid($ref['category'],
							 $ref['categoryval'],
							 $ref['type'],
							 $ref['typeval']) === false
				|| ($this->_return['dialstatus'][$ref['status']] = $dialstatus->add($ref,$id)) === false)
					return(false);
			}

		}

		if($i >= $arr['cnt'])
		{
			if(empty($this->_return['dialstatus']) === true)
				$this->_return['dialstatus'] = false;

			return(true);
		}

		for($i;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$ref = &$rs[$key];

			$ref['categoryval'] = $this->_info['gfeatures']['id'];

			if($dialstatus->is_valid($ref['category'],
						 $ref['categoryval'],
						 $ref['type'],
						 $ref['typeval']) === false
			|| ($this->_return['dialstatus'][$ref['status']] = $dialstatus->add($ref)) === false)
				return(false);
		}

		if(empty($this->_return['dialstatus']) === true)
			$this->_return['dialstatus'] = false;

		return(true);
	}

	function _reverse_edit()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'edit'
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$this->_return[$key];

			if($val === false)
				continue;

			switch($key)
			{
				case 'groupfeatures':
					$gfeatures = &$this->_ipbx->get_module('groupfeatures');
					$gfeatures->edit_origin($this->_origin['gfeatures']);
					break;
				case 'localexten':
					$extensions = &$this->_ipbx->get_module('extensions');

					if($this->_origin['localexten'] === false)
						$extensions->delete_exten($val);
					else
						$extensions->edit_origin($this->_origin['localexten']);
					break;
				case 'user':
					$this->delete_user();

					$queuemember = &$this->_ipbx->get_module('queuemember');
					$queuemember->add_origin_list($this->_origin['user']);
					break;
				case 'rightcall':
					$this->delete_rightcall();

					$rightcallmember = &$this->_ipbx->get_module('rightcallmember');
					$rightcallmember->add_origin_list($this->_origin['rightcall']);
					break;
				case 'dialstatus':
					$this->delete_dialstatus();

					$dialstatus = &$this->_ipbx->get_module('dialstatus');
					$dialstatus->add_origin_list($this->_origin['dialstatus']);
					break;
				default:
					if(($module = &$this->_ipbx->get_module($key)) === false)
						continue;

					if($this->_origin[$key] === false)
						$module->delete($val);
					else
						$module->edit_origin($this->_origin[$key]);
			}
		}
		return(true);
	}

	function delete()
	{
		$this->_status = 'delete';
		$this->_return = array();

		if($this->delete_groupfeatures() === false
		|| $this->delete_queue() === false
		|| $this->delete_macro('localexten') === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_user($this->_info['gfeatures']['name']) === false
		|| $this->delete_rightcall() === false
		|| $this->delete_dialstatus() === false)
		{
			$this->_reverse_delete();
			$this->_status = '';

			return(false);
		}

		if(isset($this->_return['localexten']) === true
		|| isset($this->_return['extensions']) === true)
			$this->unlinks();

		$this->_status = '';

		return(true);
	}

	function delete_groupfeatures()
	{
		if(xivo_issa('gfeatures',$this->_info) === false
		|| ($gfeatures = &$this->_ipbx->get_module('groupfeatures')) === false)
			return(false);

		$r = $gfeatures->delete($this->_info['gfeatures']['id']);

		if($this->_status === 'delete')
			$this->_return['groupfeatures'] = $r;

		return($r);
	}

	function delete_queue()
	{
		if(xivo_issa('queue',$this->_info) === false
		|| ($queue = &$this->_ipbx->get_module('queue')) === false)
			return(false);

		$r = $queue->delete($this->_info['queue']['name']);

		if($this->_status === 'delete')
			$this->_return['queue'] = $r;

		return($r);
	}

	function delete_user($name)
	{
		if(xivo_issa('user',$this->_info) === false)
			return(null);

		if(xivo_issa('gfeatures',$this->_info) === false
		|| is_scalar($name) === false
		|| ($queuemember = &$this->_ipbx->get_module('queuemember')) === false)
			return(false);

		$queuemember_where = array();
		$queuemember_where['usertype'] = 'user';
		$queuemember_where['category'] = 'group';
		$queuemember_where['queue_name'] = $name;

		$r = $queuemember->delete_where($queuemember_where);

		if($this->_status === 'delete')
			$this->_return['user'] = $r;

		return($r);
	}

	function delete_rightcall()
	{
		if(xivo_issa('rightcall',$this->_info) === false)
			return(null);

		if(xivo_issa('gfeatures',$this->_info) === false
		|| ($rightcallmember = &$this->_ipbx->get_module('rightcallmember')) === false)
			return(false);

		$rightcallmember_where = array();
		$rightcallmember_where['type'] = 'group';
		$rightcallmember_where['typeval'] = $this->_info['gfeatures']['id'];

		$r = $rightcallmember->delete_where($rightcallmember_where);

		if($this->_status === 'delete')
			$this->_return['rightcall'] = $r;

		return($r);
	}

	function delete_dialstatus()
	{
		if(xivo_issa('dialstatus',$this->_info) === false)
			return(null);

		if(xivo_issa('gfeatures',$this->_info) === false
		|| ($dialstatus = &$this->_ipbx->get_module('dialstatus')) === false)
			return(false);

		$dialstatus_where = array();
		$dialstatus_where['category'] = 'group';
		$dialstatus_where['categoryval'] = $this->_info['gfeatures']['id'];

		$r = $dialstatus->delete_where($dialstatus_where);

		if($this->_status === 'delete')
			$this->_return['dialstatus'] = $r;

		return($r);
	}

	function _reverse_delete()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'delete'
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];

			if($this->_return[$key] === false)
				continue;

			switch($key)
			{
				case 'groupfeatures':
					$gfeatures = &$this->_ipbx->get_module('groupfeatures');
					$gfeatures->recover($this->_info['gfeatures']['id']);
					break;
				case 'localexten':
					$extensions = &$this->_ipbx->get_module('extensions');

					if($this->_origin['localexten'] !== false)
						$extensions->add_origin($this->_origin['localexten']);
					break;
				case 'user':
					$queuemember = &$this->_ipbx->get_module('queuemember');
					$queuemember->add_origin_list($this->_origin['user']);
					break;
				case 'rightcall':
					$rightcallmember = &$this->_ipbx->get_module('rightcallmember');
					$rightcallmember->add_origin_list($this->_origin['rightcall']);
					break;
				case 'dialstatus':
					$dialstatus = &$this->_ipbx->get_module('dialstatus');
					$dialstatus->add_origin_list($this->_origin['dialstatus']);
					break;
				default:
					if(($module = &$this->_ipbx->get_module($key)) === false)
						continue;

					if($this->_origin[$key] !== false)
						$module->add_origin($this->_origin[$key]);
			}
		}
		return(true);
	}

	function unlinks()
	{
		if(xivo_issa('gfeatures',$this->_info) === false)
			return(false);

		if(($incall = &$this->_ipbx->get_module('incall')) !== false)
		{
			$incall_where = array();
			$incall_where['type'] = 'group';
			$incall_where['typeval'] = $this->_info['gfeatures']['id'];

			$incall->unlinked_where($incall_where);
		}

		if(($schedule = &$this->_ipbx->get_module('schedule')) === false)
			return(true);
		
		$schedule_where = array();
		$schedule_where['typetrue'] = 'group';
		$schedule_where['typevaltrue'] = $this->_info['gfeatures']['id'];

		$schedule->unlinked_where($schedule_where);

		$schedule_where = array();
		$schedule_where['typefalse'] = 'group';
		$schedule_where['typevalfalse'] = $this->_info['gfeatures']['id'];

		$schedule->unlinked_where($schedule_where);
	}
}

?>
