<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','realstatic','abstract.inc'),true);

class xivo_application_service_asterisk_realstatic_sip_register extends xivo_application_service_asterisk_realstatic_abstract
{
	var $_name		= 'register';
	var $_modulename	= 'staticsip';
	var $_category		= 'general';
	var $_cat_metric	= 0;
	var $_filename		= 'sip.conf';
	var $_var_name		= 'register';

	function xivo_application_service_asterisk_realstatic_sip_register(&$service)
	{
		return($this->_init(&$service));
	}

	function _load_config()
	{
		return(parent::_load_config(dirname(__FILE__)));
	}

	function get_register($id,$disable=null)
	{
		$this->_info = $this->_origin = false;

		if(($info = $this->_realstatic->get($id,$disable)) === false)
			return(false);

		$this->_info = $this->_origin = array();
		$this->_info[$this->_name] = $info;
		$this->_origin[$this->_name] = $this->_realstatic->get_origin();

		return($this->_info[$this->_name]);
	}

	function set_register($arr)
	{
		$this->_reset();

		if(is_array($arr) === false)
		{
			$this->_set_result($this->_name,null);
			$this->_set_error($this->_name,'invalid data');
			return(false);
		}
		else if(($rs = $this->build_register($arr)) === false)
		{
			$arr['commented'] = false;
			$this->_set_result($this->_name,$arr);
			$this->_set_error($this->_name,'invalid data');
			return(false);
		}

		$rs['arr']['commented'] = false;

		$this->_set_result($this->_name,$rs);

		return(true);
	}

	function add_register()
	{
		if(($rs = $this->get_result($this->_name)) === null || $rs === false)
			return($rs);
		else if(isset($rs['str'],$rs['arr']) === false)
			return(false);

		$info = array();
		$info['category'] = $this->_category;
		$info['cat_metric'] = $this->_cat_metric;
		$info['var_name'] = $this->_var_name;
		$info['var_val'] = $rs['str'];

		if(isset($rs['arr']['commented']) === true)
			$info['commented'] = intval((bool) $rs['arr']['commented']);

		return(($this->_return[$this->_name] = $this->_realstatic->add($info)));
	}

	function edit_register($id)
	{
		if(xivo_issa($this->_name,$this->_info) === true)
			$id = $this->_info[$this->_name]['id'];
		else if(($id = xivo_ulongint($id)) === 0)
			return(false);

		if(($rs = $this->get_result($this->_name)) === null)
			return(null);
		else if(is_array($rs) === false || isset($rs['str'],$rs['arr']) === false)
			return(false);

		$where = array();
		$where['category'] = $this->_category;
		$where['cat_metric'] = $this->_cat_metric;
		$where['var_name'] = $this->_var_name;

		$info = $where;
		$info['var_val'] = $rs['str'];

		if(isset($rs['arr']['commented']) === true)
			$info['commented'] = intval((bool) $rs['arr']['commented']);

		return(($this->_return[$this->_name] = $this->_realstatic->edit_where($where,$info,null,$id)));
	}

	function delete_register($status,$id=0)
	{
		if(xivo_issa($this->_name,$this->_info) === true)
			$id = $this->_info[$this->_name]['id'];
		else if(($id = xivo_ulongint($id)) === 0)
			return(null);

		$r = $this->_realstatic->delete($id);

		if($status === 'delete')
			$this->_return[$this->_name] = $r;

		return($r);
	}

	function _enable_disable_register($status,$disable=false,$id=0)
	{
		if(xivo_issa($this->_name,$this->_info) === true)
			$id = $this->_info[$this->_name]['id'];
		else if(($id = xivo_ulongint($id)) === 0)
			return(null);

		if((bool) $disable === false)
			$r = $this->_realstatic->enable($id);
		else
			$r = $this->_realstatic->disable($id);

		if($status === 'enable' || $status === 'disable')
			$this->_return[$this->_name] = $r;

		return($r);
	}

	function enable_register($status,$id=0)
	{
		return($this->_enable_disable_register($status,false,$id));
	}

	function disable_register($status,$id=0)
	{
		return($this->_enable_disable_register($status,true,$id));
	}

	function build_register($arr)
	{
		$r = array('str' => '','arr' => array());

		if(is_array($arr) === false
		|| isset($arr['username'],$arr['host']) === false
		|| ($r['arr']['username'] = $this->chk_value('username',$arr['username'])) === false
		|| ($r['arr']['host'] = $this->chk_value('host',$arr['host'])) === false)
			return(false);

		$r['str'] = $r['arr']['username'];

		if(isset($arr['password']) === true
		&& ($r['arr']['password'] = $this->set_chk_value('password',$arr['password'])) !== '')
		{
			$r['str'] .= ':'.$r['arr']['password'];

			if(isset($arr['authuser']) === true
			&& ($r['arr']['authuser'] = $this->set_chk_value('authuser',$arr['authuser'])) !== '')
				$r['str'] .= ':'.$r['arr']['authuser'];
		}

		$r['str'] .= '@'.$r['arr']['host'];

		if(isset($arr['port']) === true
		&& ($r['arr']['port'] = $this->set_chk_value('port',$arr['port'])) !== '')
			$r['str'] .= ':'.$r['arr']['port'];

		if(isset($arr['contact']) === true
		&& ($r['arr']['contact'] = $this->set_chk_value('contact',$arr['contact'])) !== '')
			$r['str'] .= '/'.$r['arr']['contact'];

		return($r);
	}
}

?>
