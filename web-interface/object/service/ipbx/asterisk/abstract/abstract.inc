<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'libconfig.inc');

class xivo_service_asterisk_abstract extends xivo_libconfig_abstract
{
	var $_conf	= array();
	var $_sre	= null;

	function get_name()
	{
		return($this->_name);
	}

	function _load_config()
	{
		if(empty($this->_conf) === false)
			return(null);

		$dir = XIVO_PATH_OBJECT.XIVO_SEP_DIR.'service'.
		       XIVO_SEP_DIR.'ipbx'.
		       XIVO_SEP_DIR.'asterisk'.
		       XIVO_SEP_DIR.$this->_name.
		       XIVO_SEP_DIR.'config';

		if(($this->_conf = xivo_gat::get_file($dir)) === false)
			$this->_conf = array();
	}

	function set_origin($arr)
	{
		if(isset($this->_origin) === true)
			$this->_origin = false;
		else
			return(false);

		if(is_array($arr) === false)
			return(false);

		$this->_origin = $arr;

		return(true);
	}

	function get_origin()
	{
		if(isset($this->_origin) === false
		|| is_array($this->_origin) === false)
			return(false);

		return($this->_origin);
	}

	function get_origin_list()
	{
		if(isset($this->_origin_list) === false
		|| is_array($this->_origin_list) === false)
			return(false);

		return($this->_origin_list);
	}

	function add_origin($val=null)
	{
		return($this->_origin('add',$val));
	}

	function add_origin_list($val=null)
	{
		return($this->_origin_list('add',$val));
	}

	function edit_origin($val=null)
	{
		return($this->_origin('edit',$val));
	}

	function edit_origin_list($val=null)
	{
		return($this->_origin_list('edit',$val));
	}

	function _origin($type,$val=null)
	{
		if(($type !== 'add' && $type !== 'edit') === true
		|| isset($this->_origin) === false)
			return(false);

		if($val !== null)
			$orig = $val;
		else
			$orig = $this->_origin;

		if(is_array($orig) === false)
			return(false);

		if($type === 'add')
			$r = $this->add($orig,$orig['id']);
		else
			$r = $this->edit($orig['id'],$orig);

		return($r);
	}

	function _origin_list($type,$val=null)
	{
		if(($type !== 'add' && $type !== 'edit') === true
		|| isset($this->_origin_list) === false)
			return(false);

		if($val !== null)
			$orig = $val;
		else
			$orig = $this->_origin_list;

		if(is_array($orig) === false || ($nb = count($orig)) === 0)
			return(false);

		if($type === 'add')
		{
			for($i = 0;$i < $nb;$i++)
				$this->add($orig[$i],$orig[$i]['id']);
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
				$this->edit($orig[$i]['id'],$orig[$i]);
		}

		return(true);
	}

	function _prepare($data)
	{
		return($data);
	}

	function get($id,$disable=null,$delete=null)
	{
		if(isset($this->_origin) === true)
			$this->_origin = false;

		if(($id = xivo_uint($id)) === 0
		|| ($r = $this->_dso->get($id,$disable,$delete)) === false)
			return(false);

		if(isset($this->_origin) === true)
			$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function get_id($arr,$disable=null,$delete=null)
	{
		if(is_array($arr) === true
		&& ($arr = $this->get_element_where($arr)) === false)
			return(false);

		return($this->_dso->get_id($arr,$disable,$delete));
	}

	function get_where($arr,$disable=null,$delete=null)
	{
		if(isset($this->_origin) === true)
			$this->_origin = false;

		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($r = $this->_dso->get($where,$disable,$delete)) === false)
			return(false);

		if(isset($this->_origin) === true)
			$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function get_nb($arr=null,$disable=null,$delete=null)
	{
		if($arr !== null && is_array($arr) === false)
			return(false);

		if(is_array($arr) === true
		&& ($arr = $this->get_element_where($arr)) === false)
			return(false);

		return($this->_dso->get_nb($arr,$disable,$delete));
	}

	function get_list($disable=null,$delete=null,$order=null,$limit=null)
	{
		if(($r = $this->_dso->get_list($disable,$delete,$order,$limit)) !== false
		&& isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_list_where($arr,$disable=null,$delete=null,$order=null,$limit=null)
	{
		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($r = $this->_dso->get_list_where($where,$disable,$delete,$order,$limit)) === false
		|| isset($r[0]) === false)
			return(false);

		return($r);
	}

	function get_all($disable=null,$delete=null,$prepare=true,$order=null,$limit=null,$assoc=false)
	{
		$prepare = (bool) $prepare;

		if(isset($this->_origin_list) === true)
			$this->_origin_list = array();

		if(($list = $this->_dso->get_all($disable,$delete,$order,$limit)) === false)
			return(false);

		$nb = count($list);

		$r = array();
		
		if($prepare === true)
		{
			for($i = 0;$i < $nb;$i++)
			{
				$r[] = $this->_prepare($list[$i]);

				if(isset($this->_origin_list) === true)
					$this->_origin_list[] = $list[$i];
			}
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
			{
				$r[] = $list[$i];

				if(isset($this->_origin_list) === true)
					$this->_origin_list[] = $list[$i];
			}
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_where($arr,$disable=null,$delete=null,$prepare=true,$order=null,$limit=null)
	{
		if(isset($this->_origin_list) === true)
			$this->_origin_list = array();

		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($list = $this->_dso->get_all_where($where,$disable,$delete,$order,$limit)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		if($prepare === true)
		{
			for($i = 0;$i < $nb;$i++)
			{
				$r[] = $this->_prepare($list[$i]);

				if(isset($this->_origin_list) === true)
					$this->_origin_list[] = $list[$i];
			}
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
			{
				$r[] = $list[$i];

				if(isset($this->_origin_list) === true)
					$this->_origin_list[] = $list[$i];
			}
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		return($this->_dso->add($arr));
	}

	function edit($id,$arr,$disable=null,$delete=null)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		$r = $this->_dso->edit($id,$arr,$disable,$delete);
		
		return($r);
	}

	function edit_where($id,$arr,$disable=null,$delete=null)
	{
		if(is_array($id) === false
		|| ($where = $this->get_element_where($id)) === false
		|| ($arr = $this->chk_values($arr,true,false,false)) === false)
			return(false);

		$r = $this->_dso->edit($where,$arr,$disable,$delete);

		return($r);
	}

	function delete($id,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($id,$disable));
	}

	function delete_where($id,$disable=null)
	{
		if(is_array($id) === false
		|| ($where = $this->get_element_where($id)) === false)
			return(false);

		return($this->_dso->delete($where,$disable));
	}

	function enable($id,$value=false)
	{
		$value = intval((bool) $value);

		return($this->edit($id,array('commented' => $value)));
	}

	function disable($id,$value=true)
	{
		$value = intval((bool) $value);

		return($this->edit($id,array('commented' => $value)));
	}

	function enable_where($id,$value=false)
	{
		if(is_array($id) === false
		|| ($where = $this->get_element_where($id)) === false)
			return(false);

		$value = intval((bool) $value);

		$r = $this->_dso->edit($where,array('commented' => $value));

		return($r);
	}

	function disable_where($id,$value=true)
	{
		if(is_array($id) === false
		|| ($where = $this->get_element_where($id)) === false)
			return(false);

		$value = intval((bool) $value);

		$r = $this->_dso->edit($where,array('commented' => $value));

		return($r);
	}

	function chk_dest_endcall($val)
	{
		return($this->_chk_dest_value('endcall',$val));
	}

	function chk_dest_application($val)
	{
		return($this->_chk_dest_value('application',$val));
	}

	function _chk_dest_value($type,$val)
	{
		if(is_scalar($val) === false
		|| ($value = $this->get_element_value($type)) === false
		|| in_array($val,$value,true) === false)
			return(false);

		return(true);
	}

	function chk_dest_sound($dir,$val)
	{
		if(($sounds = $this->_sre->get_module('sounds')) === false
		|| ($list = $sounds->get_list($dir,true)) === false
		|| isset($list[$val]) === false)
			return(false);

		return($val);
	}

	function chk_dest_custom($val)
	{
		if(preg_match('/^[a-zA-Z]{3,}\((.*)\)$/',$val,$match) !== 1)
			return(false);

		if($match[1] === '' || ctype_print($match[1]) === true)
			return(true);

		return(false);
	}

	function chk_dest_schedule($val,$id=null,$disable=null)
	{
		return($this->_chk_dest('schedule',$val,$id,$disable,false));
	}

	function chk_dest_user($val,$id=null,$disable=null,$number=true)
	{
		return($this->_chk_dest('userfeatures',$val,$id,$disable,$number));	
	}

	function chk_dest_group($val,$id=null,$disable=null,$number=true)
	{
		return($this->_chk_dest_group_queue('group',$val,$id,$disable,$number));
	}

	function chk_dest_queue($val,$id=null,$disable=null,$number=true)
	{
		return($this->_chk_dest_group_queue('queue',$val,$id,$disable,$number));
	}

	function _chk_dest_group_queue($type,$val,$id=null,$disable=null,$number=true)
	{
		$val = intval($val);

		if($type !== 'group' && $type !== 'queue')
			return(false);

		if(($info = $this->_chk_dest($type.'features',$val,$id,null,$number)) === false
		|| $disable === null)
			return($info);

		$module = $this->_sre->get_module($type.'features');

		if(($info = $module->get($val)) === false
		|| ($queue = $this->_sre->get_module('queue')) === false
		|| ($queue->get($info['name'],$disable)) === false)
			return(false);

		return($info);
	}

	function chk_dest_meetme($val,$id=null,$disable=null,$number=true)
	{
		return($this->_chk_dest('meetmefeatures',$val,$id,$disable,$number));
	}

	function _chk_dest($type,$val,$id=null,$disable=null,$number=true)
	{
		$val = intval($val);
		$number = (bool) $number;

		if($id !== null)
			$id = xivo_uint($id);

		if(($module = $this->_sre->get_module($type)) === false
		|| ($info = $module->get($val,$disable)) === false
		|| ($id !== null && $id === $info['id']) === true
		|| ($number === true && xivo_empty($info['number']) === true) === true)
			return(false);

		return($info);
	}
}

?>
