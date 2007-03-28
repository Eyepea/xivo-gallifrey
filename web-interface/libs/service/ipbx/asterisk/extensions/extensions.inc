<?php

require_once(XIVO_PATH_LIBS.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'abstract.inc');

class xivo_service_asterisk_extensions extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'extensions';
	var $_filter	= null;
	var $_origin	= null;

	function xivo_service_asterisk_extensions(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function _load_config()
	{
		if(empty($this->_conf) === false)
			return(null);

		$dir = dirname(__FILE__).XIVO_SEP_DIR.'config';

		if(($this->_conf = xivo_gat::get_file($dir)) === false)
			$this->_conf = array();
	}

	function get($id,$disable=null)
	{
		$r = false;

		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id,$disable)) === false)
			return($r);

		$this->_origin = $r;

		$r['commented'] = (bool) $r['commented'];

		return($r);
	}

	function add_origin()
	{
		return($this->_origin('add'));
	}

	function edit_origin()
	{
		return($this->_origin('edit'));
	}

	function _origin($type)
	{
		if(($type !== 'add' && $type !== 'edit') === true || is_array($this->_origin) === false)
			return(false);

		if($type === 'add')
			$r = $this->add($this->_origin,$this->_origin['id']);
		else
			$r = $this->edit($this->_origin['id'],$this->_origin);

		return($r);
	}

	function get_where($arr,$disable=null)
	{
		$arr = (array) $arr;

		$where = array();

		if(isset($arr['name']) === true)
			$where['name'] = $arr['name'];

		if(isset($arr['context']) === true)
			$where['context'] = $arr['context'];

		if(isset($arr['exten']) === true)
			$where['exten'] = $arr['exten'];

		if(isset($arr['priority']) === true)
			$where['priority'] = $arr['priority'];

		if(isset($arr['app']) === true)
			$where['app'] = $arr['app'];

		if(isset($arr['appdata']) === true)
			$where['appdata'] = $arr['appdata'];

		if(($id = $this->_dso->get_where($where)) === false)
			return(false);

		return($this->get($id,$disable));
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		$r = array();

		if(($id = xivo_uint($id)) !== 0)
			$r['id'] = $id;

		$r['context'] = $arr['context'];
		$r['exten'] = $arr['exten'];
		$r['priority'] = $arr['priority'];
		$r['app'] = $arr['app'];
		$r['appdata'] = $arr['appdata'];

		if(isset($arr['name']) === true)
			$r['name'] = (string) $arr['name'];

		if(isset($arr['commented']) === true)
			$r['commented'] = (bool) $arr['commented'];
		else
			$r['commented'] = false;

		return($this->_dso->add($r));
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		$r = array();

		$r['context'] = $arr['context'];
		$r['exten'] = $arr['exten'];
		$r['priority'] = $arr['priority'];
		$r['app'] = $arr['app'];
		$r['appdata'] = $arr['appdata'];

		if(isset($arr['name']) === true)
			$r['name'] = (string) $arr['name'];

		if(isset($arr['commented']) === true)
			$r['commented'] = (bool) $arr['commented'];
		else
			$r['commented'] = false;

		return($this->_dso->edit($id,$r));
	}

	function get_name_exten($arr=array(),$disable=null)
	{
		$r = false;

		$arr = (array) $arr;

		if(($list = $this->get_list_where($arr)) === false)
			return($r);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $this->get($list[$i],$disable)) === false)
				continue;

			$r[$info['name']] = $info['exten'];
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_list_where($arr=array(),$disable=null)
	{
		$arr = (array) $arr;

		$where = array();

		if(isset($arr['name']) === true)
			$where['name'] = $arr['name'];

		if(isset($arr['context']) === true)
			$where['context'] = $arr['context'];

		if(isset($arr['exten']) === true)
			$where['exten'] = $arr['exten'];

		if(isset($arr['priority']) === true)
			$where['priority'] = $arr['priority'];

		if(isset($arr['app']) === true)
			$where['app'] = $arr['app'];

		if(isset($arr['appdata']) === true)
			$where['appdata'] = $arr['appdata'];

		if(($r = $this->_dso->get_list_where($where,$disable)) !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_where($arr=array(),$disable=null)
	{
		$r = false;

		if(($list = $this->get_list_where($arr)) === false)
			return($r);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $this->get($list[$i],$disable)) === false)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function delete($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($id));
	}

	function delete_by_name($name)
	{
		return($this->_dso->delete_where(array('name' => $name)));
	}
}

?>
