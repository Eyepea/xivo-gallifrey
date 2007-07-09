<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','abstract.inc'),true);

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
		if(is_array($id) === true && ($id = $this->get_where($id)) === false)
			return(false);

		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id,$disable)) === false)
			return(false);

		$this->_origin = $r;
		
		$r = $this->_prepare($r);

		return($r);
	}

	function _prepare($data)
	{
		$data['commented'] = (bool) $data['commented'];

		return($data);
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

	function get_where($arr)
	{
		$arr = (array) $arr;

		if(($where = $this->get_elements_where($arr)) === false)
			return(false);

		return($this->_dso->get_where($where));
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

        function disable($id,$value=false)
        {
                if(($id = xivo_uint($id)) === 0)
                        return(false);

                $value = (bool) $value === false ? 0 : 1;

                return($this->_dso->edit($id,array('commented' => $value)));
        }

	function get_name_exten($arr=array(),$disable=null)
	{
		$r = false;

		$arr = (array) $arr;

		if(($list = $this->get_all_where($arr,$disable)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[$list[$i]['name']] = $list[$i]['exten'];

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_list_where($arr=array(),$disable=null)
	{
		$arr = (array) $arr;

		if(($where = $this->get_elements_where($arr)) === false
		|| ($r = $this->_dso->get_list_where($where,$disable)) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_where($arr=array(),$disable=null,$prepare=true)
	{
		$arr = (array) $arr;
		$prepare = (bool) $prepare;

		if(($where = $this->get_elements_where($arr)) === false
		|| ($list = $this->_dso->get_all_where($where,$disable)) === false
		|| isset($list[0]) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $prepare === true ? $this->_prepare($list[$i]) : $list[$i];

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
