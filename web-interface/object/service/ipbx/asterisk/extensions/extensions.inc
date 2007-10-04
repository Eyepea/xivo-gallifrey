<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_extensions extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'extensions';
	var $_filter		= null;
	var $_origin		= false;
	var $_origin_list	= false;
	var $_exten		= null;

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

	function _prepare($data)
	{
		$data['commented'] = (bool) $data['commented'];
		$data['exten'] = $this->_sre->clean_extension($data['exten']);

		return($data);
	}

	function get_element_where($where)
	{
		if(is_array($where) === false)
			return(false);

		if(isset($where['exten']) === true)
			$where['exten'] = $this->_sre->clean_extension($where['exten']);

		return(parent::get_element_where($where));
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		$r = array();

		if(($id = xivo_uint($id)) !== 0)
			$r['id'] = $id;

		$r['context'] = $arr['context'];
		$r['exten'] = $this->_sre->clean_extension($arr['exten']);
		$r['app'] = $arr['app'];

		if(isset($arr['priority']) === false)
			$r['priority'] = 1;
		else
			$r['priority'] = xivo_uint($arr['priority'],1);

		if(isset($arr['appdata']) === true)
			$r['appdata'] = (string) $arr['appdata'];

		if(isset($arr['name']) === true)
			$r['name'] = (string) $arr['name'];

		if(isset($arr['commented']) === false)
			$r['commented'] = false;

		return($this->_dso->add($r));
	}

	function edit($id,$arr,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		$r = array();

		if(isset($arr['context']) === true)
			$r['context'] = (string) $arr['context'];

		if(isset($arr['exten']) === true)
			$r['exten'] = $this->_sre->clean_extension($arr['exten']);

		if(isset($arr['priority']) === true)
			$r['priority'] = xivo_uint($arr['priority'],1);

		if(isset($arr['app']) === true)
			$r['app'] = (string) $arr['app'];

		if(isset($arr['appdata']) === true)
			$r['appdata'] = (string) $arr['appdata'];

		if(isset($arr['name']) === true)
			$r['name'] = (string) $arr['name'];

		if(isset($arr['commented']) === false)
			$arr['commented'] = false;

		$r['commented'] = intval((bool) $arr['commented']);

		return($this->_dso->edit($id,$r,$disable));
	}

	function edit_where($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		if(isset($arr['exten']) === true)
			$arr['exten'] = $this->_sre->clean_extension($arr['exten']);

		return(parent::edit_where($id,$arr,$disable));
	}

	function new_exten($app,$arr=null,$exten='',$context='')
	{
		return($this->set_exten($app,0,$arr,$exten,$context));
	}

	function set_exten($app,$id=0,$arr=null,$exten='',$context='')
	{
		if(($id = xivo_uint($id)) === 0)
			$id = count($this->_exten) + 1;
		else if(isset($this->_exten[$id]) === false)
			return(false);

		if(isset($this->_exten[$id],$this->_exten[$id][0]) === true)
		{
			$exten = $this->_exten[$id][0]['exten'];
			$context = $this->_exten[$id][0]['context'];
		}

		if(($r = $this->chk_exten($app,$arr,$exten,$context)) === false)
			return(false);

		$this->_exten[$id][] = $r;

		return($id);
	}

	function chk_exten($app,$arr=null,$exten='',$context='')
	{
		$arr = (array) $arr;

		$r = array();

		if(isset($arr['appdata']) === true)
			$r['appdata'] = $arr['appdata'];

		if(isset($arr['name']) === true)
			$r['name'] = $arr['name'];

		if(($r['app'] = $this->_chk_app($app)) === false)
			return(false);

		$r['exten'] = $this->_sre->clean_extension($exten);
		$r['context'] = $context;
		
		return($this->chk_values($r,true,false,false));
	}

	function get_exten($app='',$exten='',$context='',$arr=null)
	{
		$arr = (array) $arr;

		$exten = $this->_sre->clean_extension($exten);
		$app = strval($app);

		$where = array();
		$where['exten'] = $exten;
		$where['context'] = $context;

		if(isset($app{0}) === true)
		{
			if(($app = $this->_chk_app($app)) === false)
				return(false);

			$where['app'] = $app;
		}

		if(isset($arr['appdata']) === true)
			$where['appdata'] = $arr['appdata'];

		if(isset($arr['name']) === true)
			$where['name'] = $arr['name'];

		return($this->get_where($where));
	}

	function add_exten($id)
	{
		if(is_array($this->_exten) === false
		|| ($id = xivo_uint($id)) === 0
		|| isset($this->_exten[$id],$this->_exten[$id][0]) === false)
			return(false);

		$nb = count($this->_exten[$id]);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$this->_exten[$id][$i]['priority'] = $i+1;

			if(($ret = $this->add($this->_exten[$id][$i])) === false)
				break;

			$r[] = $ret;
		}

		if(isset($r[0]) === false)
			return(false);

		if($ret === false)
		{
			for($j = 0;$j < $i;$j++)
				$this->delete($r[$j]);
		}

		return($id);
	}

	function delete_exten($id)
	{
		if(is_array($this->_exten) === false
		|| ($id = xivo_uint($id)) === 0
		|| isset($this->_exten[$id],$this->_exten[$id][0]) === false)
			return(false);

		$where = array();
		$where['exten'] = $this->_exten[$id][0]['exten'];
		$where['context'] = $this->_exten[$id][0]['context'];

		return($this->delete_where($where));
	}

	function new_hints($name,$protocol,$exten)
	{
		$id = count($this->_exten) + 1;

		if(($r = $this->chk_hints($name,$protocol,$exten)) === false)
			return(false);

		$this->_exten[$id][0] = $r;

		return($id);
	}

	function chk_hints($name,$protocol,$exten)
	{
		$r = array();
		$r['context'] = 'hints';

		if(($r['app'] = $this->_sre->mk_interface($name,$protocol)) === false)
			return(false);

		$r['exten'] = $this->_sre->clean_extension($exten);
		
		return($this->chk_values($r,true,false,false));
	}

	function get_hints($name,$protocol,$exten)
	{
		$where = array();

		if(($where['app'] = $this->_sre->mk_interface($name,$protocol)) === false)
			return(false);

		$where['exten'] = $this->_sre->clean_extension($exten);
		$where['context'] = 'hints';

		return($this->get_where($where));
	}

	function add_hints($id)
	{
		return($this->add_exten($id));
	}

	function delete_hints($id)
	{
		return($this->delete_exten($id));
	}

	function chk_macro($arr)
	{
		return($this->chk_application('macro',$arr));
	}

	function chk_hangup($arr)
	{
		return($this->chk_application('hangup',$arr));
	}

	function chk_application($app,$arr)
	{
		if(is_array($arr) === false)
			return(false);

		if(isset($arr['priority']) === false)
			$arr['priority'] = 1;
		else
			$arr['priority'] = xivo_uint($arr['priority'],1);

		if(isset($arr['context']) === false)
			$arr['context'] = '';

		if(($arr['app'] = $this->_chk_app($app)) === false)
			$arr['app'] = '';

		return($this->chk_values($arr));
	}

	function _chk_app($app)
	{
		$app = strtolower((string) $app);

		if($this->_chk_exists_cfg('app',$app) === false)
			return(false);

		return($this->_conf['app'][$app]['name']);
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
}

?>
