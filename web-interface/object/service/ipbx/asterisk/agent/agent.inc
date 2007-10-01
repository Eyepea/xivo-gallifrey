<?php

xivo_service_asterisk::required(array('staticconf','staticconf.inc'),true);

class xivo_service_asterisk_agent extends xivo_service_asterisk_staticconf
{
	var $_dso		= null;
	var $_name		= 'agent';
	var $_filename		= 'agents.conf';
	var $_category		= 'agents';
	var $_cat_metric	= 1;
	var $_filter		= null;
	var $_callpath		= '';
	var $_origin		= false;
	var $_agent		= array();

	function xivo_service_asterisk_agent(&$sre,&$dso)
	{
		$this->_load_staticconf_config();

		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_dso->set_param('filename',$this->_filename);

		if($this->_chk_callpath() === false)
			trigger_error('Invalid or missing call path in '.$this->_sre->get_type().' ini file',E_USER_ERROR);

		$this->_load_config();
		$this->_agent['conf'] = $this->_conf;
	}

	function _chk_callpath()
	{
		if(xivo_issa($this->_name,$this->_sre->_ini) === false
		|| isset($this->_sre->_ini[$this->_name]['callpath']) === false
		|| ($this->_callpath = xivo_file::is_d_rwx($this->_sre->_ini[$this->_name]['callpath'])) === false)
			return(false);
	}

	function get($id,$disable=null)
	{
		if(($r = parent::get($id,$disable)) === false)
			return(false);

		$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function get_where($arr,$disable=null)
	{
		$arr = (array) $arr;
		$arr['category'] = $this->_category;

		return(parent::get_where($arr,$disable));
	}

	function _prepare($data)
	{
		if($data['var_name'] === 'agent')
		{
			$val = explode(',',$data['var_val']);

			$data['number'] = isset($val[0]) === true ? $val[0] : '';
			$data['passwd'] = isset($val[1]) === true ? $val[1] : '';
			$data['fullname'] = isset($val[2]) === true ? $val[2] : '';
		}

		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function add_group($group,$list=array(),$id=0)
	{
		if(($group = xivo_uint($group)) === 0)
			return(false);

		$list = array_values((array) $list);

		if(isset($list[0]) === false)
			$list = $group;
		else
			$list = $group.','.implode(',',$list);

		$arr = array();

		$arr['category'] = $this->_category;
		$arr['cat_metric'] = $this->_cat_metric;
		$arr['var_metric'] = $group * $this->_conf['var_metric']['range']; 
		$arr['var_name'] = 'group';
		$arr['var_val'] = $list;

		return(parent::add($arr,$id));
	}

	function edit_group($id,$group,$list=array())
	{
		if(($group = xivo_uint($group)) === 0)
			return(false);

		$list = array_values((array) $list);

		if(isset($list[0]) === false)
			$list = $group;
		else
			$list = $group.','.implode(',',$list);

		$arr = array();

		$arr['category'] = $this->_category;
		$arr['cat_metric'] = $this->_cat_metric;
		$arr['var_metric'] = $group * $this->_conf['var_metric']['range'];
		$arr['var_name'] = 'group';
		$arr['var_val'] = $list;

		return(parent::edit($id,$arr));
	}

	function get_agent($id,$disable=null)
	{
		if(($r = $this->_get_agent($id,$disable)) === false)
			return(false);

		if(($opt = $this->_get_opt_by_agent($r['var_metric'])) === false)
			return($r);

		$nb = count($opt);

		for($i = 0;$i < $nb;$i++)
			$r[$opt[$i]['var_name']] = $opt[$i]['var_val'];

		return($r);
	}

	function _get_agent($id,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$arr = array();
		$arr['id'] = $id;
		$arr['var_name'] = 'agent';

		if(($r = $this->get_where($arr,$disable)) === false)
			return(false);

		$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function _origin($type,$val=null)
	{
		if($val !== null)
			$orig = $val;
		else
			$orig = $this->_origin;

		if(($type !== 'add' && $type !== 'edit') === true
		|| is_array($orig) === false)
			return(false);

		if($type === 'add')
			$r = parent::add($orig,$orig['id']);
		else
			$r = parent::edit($orig['id'],$orig);

		return($r);
	}

	function add_agent($arr)
	{
		if(is_array($arr) === false || isset($arr['group']) === false)
			return(false);

		$free = false;

		if(($last = $this->_get_last_var_metric($arr['group'])) !== false)
			$last++;
		else if(($free = $this->_get_free_var_metric($arr['group'])) !== false)
			$last = $free['var_metric'] - 1;
		else
			return(false);

		$max = ($arr['group'] + 1) * $this->_conf['var_metric']['range'] - 2;

		if($last >= $max)
			return(false);

		unset($arr['group']);

		$r = $add = array();

		$add['category'] = $this->_category;
		$add['cat_metric'] = $this->_cat_metric;

		$arr['savecallsin'] = $this->_callpath;

		if(($a = xivo_get_aks($arr)) === false)
			return(false);

		for($i = 0;$i < $a['cnt'];$i++)
		{
			$key = &$a['keys'][$i];
			$add['var_name'] = $key;
			$add['var_val'] = &$arr[$key];

			if($key === 'agent')
				$add['var_metric'] = $last + 1;
			else
				$add['var_metric'] = $last;

			if(($id = $this->add($add)) === false)
				break;

			if($key === 'agent')
				$agentid = $id;

			$r[$i] = $id;
		}

		if(isset($r[0]) === false)
			return(false);

		if($id !== false)
		{
			if($free !== false)
				$this->delete($free['id']);

			return($agentid);
		}

		for($j = 0;$j < $i;$j++)
			$this->delete($r[$j]);

		return(false);
	}

	function edit_agent($id,$arr)
	{
		if(is_array($arr) === false || ($info = $this->_get_agent($id)) === false)
			return(false);

		$opt = $this->_get_opt_by_agent($info['var_metric']);
			
		$ret = $last = null;
		$free = false;

		if(isset($arr['group']) === true)
		{
			if(($free = $this->_get_free_var_metric($arr['group'])) !== false)
				$last = $free['var_metric'] - 1;
			else if(($last = $this->_get_last_var_metric($arr['group'])) !== false)
				$last++;
			else
				return(false);

			$max = ($arr['group'] + 1) * $this->_conf['var_metric']['range'] - 2;

			if($last >= $max)
				return(false);

			$var_metric = $last;
		}
		else
			$var_metric = $info['var_metric'] - 1;

		$agent_val = '';
		$r = $edit = array();

		$edit['category'] = $this->_category;
		$edit['cat_metric'] = $this->_cat_metric;

		if(isset($arr['agent']) === true)
			$agent_val = $arr['agent'];

		unset($arr['agent'],$arr['group']);

		$arr['savecallsin'] = $this->_callpath;

		if($opt !== false)
		{
			$nb = count($opt);

			for($i = 0;$i < $nb;$i++)
			{
				$ref = &$opt[$i];
	
				$edit['var_name'] = $ref['var_name'];
				$edit['var_metric'] = $var_metric;

				if(isset($arr[$ref['var_name']]) === true)
					$edit['var_val'] = $arr[$ref['var_name']];
				else
					$edit['var_val'] = $ref['var_val'];

				unset($arr[$ref['var_name']]);

				if(($ret = $this->edit($ref['id'],$edit)) === false)
					break;

				$r[] = $opt[$i];
			}

			if($ret === false)
			{
				if(isset($r[0]) === false)
					return(false);

				for($i = 0;$i < $opt;$i++)
				{
					$ref = &$opt[$i]['id'];
					$this->edit($opt['id'],$opt);
				}

				return(false);
			}
		}

		if(($a = xivo_get_aks($arr)) !== false)
		{
			$ret = null;

			$add = $r = array();
			$add['category'] = $this->_category;
			$add['cat_metric'] = $this->_cat_metric;
			$add['var_metric'] = $var_metric;

			for($i = 0;$i < $a['cnt'];$i++)
			{
				$key = &$a['keys'][$i];
				$add['var_name'] = $key;
				$add['var_val'] = &$arr[$key];

				if(($ret = $this->add($add)) === false)
					break;

				$r[$i] = $ret;
			}

			if($ret === false)
			{
				if(isset($r[0]) === false)
					return(false);

				for($j = 0;$j < $i;$j++)
					$this->delete($r[$j]);

				return(false);
			}
		}
		
		unset($edit['var_val']);

		if($agent_val !== '' || $last !== null)
		{
			$edit['var_metric'] = $var_metric + 1;
			$edit['var_name'] = 'agent';

			if($agent_val !== '')
				$edit['var_val'] = $agent_val;

			if($last !== null)
			{
				$add = array();
				$add['commented'] = true;
				$add['cat_metric'] = $this->_cat_metric;
				$add['var_metric'] = $info['var_metric'];
				$add['category'] = $this->_category; 
				$add['var_name'] = 'deleted';
				$add['var_val'] = null;

				$this->add($add);

				if($free !== false)
					$this->delete($free['id']);
			}

			if($this->edit($id,$edit) === false)
				return(false);
		}

		return(true);
	}

	function delete_agent($id)
	{
		if(($info = $this->get($id)) === false)
			return(false);

		if(isset($info['var_metric']) === true)
		{
			$this->_conf = &$this->_staticconf['conf'];

			parent::delete_where(array('var_metric' => $info['var_metric'] - 1));

			$this->_conf = &$this->_agent['conf'];
		}
		
		$edit = array();
		$edit['commented'] = true;
		$edit['category'] = $this->_category;
		$edit['var_name'] = 'deleted';
		$edit['var_val'] = null;

		$r = $this->edit($info['id'],$edit);

		return($r);
	}

	function order_agent($group,$arr,$pos=0)
	{
		if(is_scalar($arr) === true)
			$arr = array(xivo_uint($pos) => $arr);

		$arr = (array) $arr;

		if(($a = xivo_get_aks($arr)) === false)
			return(false);

		$opt = array();

		for($i = 0;$i < $a['cnt'];$i++)
		{
			$pos = &$a['keys'][$i];
			$id = &$arr[$pos];

			$pos_metric = ($group * $this->_conf['var_metric']['range']) + (2 * $pos) + 2;

			if(($agent = $this->_get_agent($id)) === false
			|| (int) $pos_metric === (int) $agent['var_metric'])
				continue;

			$agent['pos_metric'] = $pos_metric;

			if(($agent['opt'] = $this->_get_opt_by_agent($agent['var_metric'])) === false)
				unset($agent['opt']);

			$info[] = $agent;
		}

		if(isset($info[0]) === false)
			return(null);

		$nb = count($info);

		$ret = true;

		$edit = $free = array();
		$edit['var_metric'] = 0;

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$info[$i];

			$edit['var_metric'] = $ref['pos_metric'];

			if($this->edit($ref['id'],$edit) === false)
			{
				$ret = $i;
				break;
			}

			if(isset($ref['opt']) === true)
			{
				$cnt = count($ref['opt']);

				for($j = 0;$j < $cnt;$j++)
				{
					$edit['var_metric'] = $ref['pos_metric'] - 1;
					if($this->edit($ref['opt'][$j]['id'],$edit) === false)
					{
						$ret = $i;
						break;
					}
				}
			}

			if(($pinfo = $this->_chk_free_var_metric($ref['pos_metric'])) === false)
				continue;

			if($this->delete($pinfo['id']) === false)
			{
				$ret = $i;
				break;
			}

			$free[$i] = $pinfo;
		}

		if($ret === true)
			return(true);

		for($j = 0;$j <= $i;$j++)
		{
			$ref = &$info[$j];

			$edit['var_metric'] = $ref['var_metric'];

			$this->edit($ref['id'],$edit);

			if(isset($ref['opt']) === true)
			{
				$cnt = count($ref['opt']);

				for($k = 0;$k < $cnt;$k++)
				{
					$edit['var_metric'] = $ref['var_metric'] - 1;
					$this->edit($ref['opt'][$k]['id'],$edit);
				}
			}

			if(isset($free[$j]) === false)
				continue;

			$this->add($free[$j],$free[$j]['id']);
		}

		return(false);
	}

	function _get_last_var_metric($group)
	{
		$min = $group * $this->_conf['var_metric']['range'];
		$max = $min + $this->_conf['var_metric']['range'] - 1;

		$r = $this->_dso->get_last_var_metric($min,$max);

		return($r);
	}

	function _get_free_var_metric($group)
	{
		$min = $group * $this->_conf['var_metric']['range'];
		$max = $min + $this->_conf['var_metric']['range'] - 1;

		$r = $this->_dso->get_free_var_metric($min,$max);

		return($r);
	}

	function _get_opt_by_agent($var_metric,$disable=null)
	{
		if(($var_metric = xivo_uint($var_metric)) === 0)
			return(false);

		$var_metric--;

		$r = $this->_dso->get_all_by_var_metric($var_metric,$disable);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function _chk_free_var_metric($var_metric)
	{
		if(($var_metric = xivo_uint($var_metric)) === 0)
			return(false);

		$arr = array();
		$arr['var_metric'] = $var_metric;
		$arr['var_name'] = 'deleted';
		$arr['commented'] = true;

		return($this->get_where($arr));
	}
}

?>
