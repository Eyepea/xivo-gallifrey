<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_context extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_context		= false;
	var $_contextnumbers	= false;
	var $_entity		= null;

	function xivo_application_service_asterisk_context(&$service)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_context = &$this->_service->get_module('context')) === false
		|| ($this->_contextnumbers = &$this->_service->get_module('contextnumbers')) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['context'] = $this->_context->get($id,$disable)) === false)
			return(false);

		$this->_origin['context'] = $this->_context->get_origin();

		$this->_get_contextnumbers($disable);

		return($this->_info);
	}

	function _get_contextnumbers()
	{
		$this->_origin['contextnumbers'] = false;
		$this->_info['contextnumbers'] = false;

		if(xivo_haslen($this->_info['context']['entity']) === false)
			return(null);
		else if(($this->_info['contextnumbers'] = $this->_contextnumbers->get_all_by_type(
										array('context' => $this->_info['context']['name']))) === false)
			return(false);

		$this->_origin['contextnumbers'] = $this->_contextnumbers->get_origin_list();

		return($this->_info['contextnumbers']);
	}

	function get_elements()
	{
		$r = array();
		$r['context'] = $this->_context->get_element();
		$r['contextnumbers'] = $this->_contextnumbers->get_element();

		return($r);
	}

	function get_contexts_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		if(($rs = $this->_context->get_all($disable,true,$order,$limit)) === false
		|| ($nb = count($rs)) === 0)
		{
			$this->_cnt = $this->_context->get_cnt();
			return(false);
		}

		$this->_load_entity();

		$r = $info = array();

		$this->_cnt = $this->_context->get_cnt();

		for($i = 0;$i < $nb;$i++)
		{
			$info['context'] = $rs[$i];

			if(xivo_haslen($info['context']['entity']) === false 
			|| ($info['contextnumbers'] = $this->_contextnumbers->get_all_where(
									array('context' => $info['context']['name']))) === false
			|| ($info['entity'] = $this->_entity->get_where(
			   						array('name' => $info['context']['entity']))) === false)
				$info['contextnumbers'] = $info['entity'] = false;

			$r[$info['context']['name']] = $info;
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function _load_entity()
	{
		if(is_object($this->_entity) === false
		&& (xivo::load_class('xivo_entity',XIVO_PATH_OBJECT,null,false) === false
		   || ($this->_entity = new xivo_entity()) === false) === true)
		   return(false);

		return(true);
	}

	function get_entities_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		if($this->_load_entity() === false)
			return(false);

		return($this->_entity->get_all($disable,true,$order,$limit,$assoc));
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
		|| xivo_issa('context',$arr) === false)
			return(false);

		$this->set_context($arr['context']);

		if((string) $this->get_result_var('context','entity') === '')
			return(($this->get_errnb() < 1));
		else if(xivo_issa('contextnumbers',$arr) === true)
		{
			$arr['contextnumbers']['context'] = $this->get_result_var('context','name');
			$this->set_contextnumbers($arr['contextnumbers']);
		}
		else
		{
			$this->_set_result('contextnumbers',null);
			$this->_set_error('contextnumbers','invalid data');
		}
	
		return(($this->get_errnb() < 1));
	}

	function set_context($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('context',null);
			$this->_set_error('context','invalid data');
			return(false);
		}

		$arr['commented'] = false;

		if(($rs = $this->_context->chk_values($arr)) === false)
		{
			$this->_set_result('context',$this->_context->get_filter_result());
			$this->_set_error('context',$this->_context->get_filter_error());
			return(false);
		}

		$this->_set_result('context',$rs);

		return(true);
	}

	function set_contextnumbers($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('contextnumbers',null);
			$this->_set_error('contextnumbers','invalid data');
			return(false);
		}
		else if(isset($arr['context']) === false)
		{
			$this->_set_result('contextnumbers',false);
			$this->_set_error('contextnumbers','wrong context');
			return(false);
		}
		
		$info = $infotmp = $tmp = array();
		$info['context'] = $arr['context'];
		$info['numberbeg'] = '';
		$info['numberend'] = '';
		$info['didlength'] = '';

		unset($arr['context']);

		$reslist = $errlist = $incr = array();
		$nberr = 0;

		foreach($arr as $key => $val)
		{
			foreach($val as $val1)
			{
				if(isset($incr[$key]) === false)
					$incr[$key] = 0;

				$i = &$incr[$key];

				$infotmp = $info;
				$infotmp['type'] = $key;

				if(xivo_ak('numberbeg',$val1) === false)
				{
					$errlist[$key][$i] = 'missing numberbeg';
					$reslist[$key][$i++] = $infotmp;
					$nberr++;
					continue;
				}

				$infotmp['numberbeg'] = $val1['numberbeg'];

				if($key === 'incall')
				{
					if(xivo_haslen($val1,'didlength') === false)
					{
						$errlist['incall'][$i] = 'wrong didlength';
						$reslist['incall'][$i++] = $infotmp;
						$nberr++;
						continue;
					}

					$infotmp['didlength'] = $val1['didlength'];
				}

				if(xivo_haslen($val1,'numberend') === true
				&& $val1['numberbeg'] !== $val1['numberend'])
				{
					if(xivo_is_max_digit($val1['numberbeg'],$val1['numberend'],true) !== true)
					{
						$errlist[$key][$i] = 'wrong interval';
						$reslist[$key][$i++] = $infotmp;
						$nberr++;
						continue;
					}

					$infotmp['numberend'] = $val1['numberend'];
				}
				else if(xivo_haslen($infotmp['numberbeg']) === false)
					continue;
				else
					$infotmp['numberend'] = '';

				$uniq = $info['context'].'-'.$infotmp['type'].'-'.$infotmp['numberbeg'].'-'.$infotmp['numberend'];

				if(isset($tmp[$uniq]) === true)
					continue;
				else if(($rs = $this->_contextnumbers->chk_values($infotmp)) === false)
				{
					$err = $this->_contextnumbers->get_filter_error();
					
					if(isset($err['type']) === false && isset($err['context']) === false)
					{
						$errlist[$key][$i] = $err;
						$reslist[$key][$i++] = $this->_contextnumbers->get_filter_result();
						$nberr++;
					}
					continue;
				}

				$tmp[$uniq] = 1;
				if(isset($reslist[$key]) === false)
					$reslist[$key] = array();

				$reslist[$key][$i++] = $rs;
			}
		}

		if($nberr > 0)
		{
			$this->_set_error('contextnumbers',$errlist);
			$this->_set_result('contextnumbers',$reslist);
			return(false);
		}
		if(empty($reslist) === true)
		{
			$this->_set_error('contextnumbers','empty');
			$this->_set_result('contextnumbers',false);
			return(false);
		}
		
		$this->_set_result('contextnumbers',$reslist);

		return(true);
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$contextnumbers = null;

		if($this->get_errnb() > 0
		|| $this->add_context() === false
		|| ($contextnumbers = $this->add_contextnumbers()) === false)
		{
			if($contextnumbers !== null)
				$this->_reverse_add();

			$r = false;
		}

		$this->_status = '';

		return($r);
	}

	function add_context()
	{
		if(($rs = $this->get_result('context')) === null
		|| $rs === false)
			return(false);

		if($this->_context->add($rs) !== false)
			$this->_return['context'] = $rs['name'];
		else
			$this->_return['context'] = false;

		return($this->_return['context']);
	}

	function add_contextnumbers()
	{
		if(($rs = $this->get_result('contextnumbers')) === null
		|| (string) $this->get_result_var('context','entity') === '')
			return(null);
		else if(is_array($rs) === false || empty($rs) === true)
			return(false);

		$this->_return['contextnumbers'] = array();

		foreach($rs as $val)
		{
			foreach($val as $val1)
			{
				if($this->_contextnumbers->add($val1) === false)
					return(false);
				else
					$this->_return['contextnumbers'][] = $val1;
			}
		}

		if(isset($this->_return['contextnumbers'][0]) === false)
			return(($this->_return['contextnumbers'] = false));

		return(true);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'add'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'context':
					$this->_context->delete($val);
					break;
				case 'contextnumbers':
					if(is_array($val) === false)
						continue;

					foreach($val as $contextnumbersval)
					{
						if($contextnumbersval !== false)
							$this->_contextnumbers->delete_where($contextnumbersval);
					}
					break;
			}
		}

		return(true);
	}

	function edit()
	{
		$r = true;

		$this->_status = 'edit';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->edit_context() === false
		|| $this->edit_contextnumbers() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_context()
	{
		if(xivo_issa('context',$this->_info) === false
		|| ($rs = $this->get_result('context')) === null
		|| $rs === false)
			return(false);

		$rs['commented'] = $this->_info['context']['commented'];

		if($this->_context->edit($this->_info['context']['name'],$rs) !== false)
			$this->_return['context'] = $rs['name'];
		else
			$this->_return['context'] = false;

		return($this->_return['context']);
	}

	function edit_contextnumbers()
	{
		if(xivo_issa('context',$this->_info) === false)
			return(false);
		else if((string) $this->get_result_var('context','entity') === '')
			return($this->delete_contextnumbers());
		else if(xivo_issa('contextnumbers',$this->_info) === false)
			return($this->add_contextnumbers());

		$contextnumbers_where = array();
		$contextnumbers_where['context'] = $this->_info['context']['name'];

		$this->_return['contextnumbers'] = $this->_contextnumbers->delete_where($contextnumbers_where);

		if(($rs = $this->get_result('contextnumbers')) === null)
			return($this->_return['contextnumbers']);
		else if(is_array($rs) === false || empty($rs) === true)
			return(false);

		$this->_return['contextnumbers'] = array();

		foreach($rs as $val)
		{
			foreach($val as $val1)
			{
				if($this->_contextnumbers->add($val1) === false)
					return(false);
				else
					$this->_return['contextnumbers'][] = $val1;
			}
		}

		if(isset($this->_return['contextnumbers'][0]) === false)
			$this->_return['contextnumbers'] = false;

		return(true);
	}

	function _reverse_edit()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'edit'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'context':
					if(isset($this->_origin['context']) === false
					|| $this->_origin['context'] === false)
						$this->_context->delete($val);
					else
						$this->_context->edit_origin($this->_origin['context']);
					break;
				case 'contextnumbers':
					if(is_array($val) === true)
					{
						foreach($val as $contextnumbersval)
							$this->_contextnumbers->delete_where($contextnumbersval);
					}

					if(isset($this->_origin['contextnumbers']) === true)
						$this->_contextnumbers->add_origin_list($this->_origin['contextnumbers']);
					break;
			}
		}

		return(true);
	}

	function delete()
	{
		$r = true;

		$this->_status = 'delete';
		$this->_return = array();

		if($this->delete_context() === false
		|| $this->delete_contextnumbers() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';

		return($r);
	}

	function delete_context()
	{
		if(xivo_issa('context',$this->_info) === false)
			return(false);

		$r = $this->_context->delete($this->_info['context']['name']);

		if($this->_status === 'delete')
			$this->_return['context'] = $r;

		return($r);
	}

	function delete_contextnumbers()
	{
		if(xivo_issa('contextnumbers',$this->_info) === false)
			return(null);
		else if(xivo_issa('context',$this->_info) === false)
			return(false);

		$r = $this->_contextnumbers->delete_where(array('context' => $this->_info['context']['name']));

		if($this->_status === 'delete')
			$this->_return['contextnumbers'] = $r;

		return($r);
	}

	function _reverse_delete()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'delete'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'context':
					if(isset($this->_origin['context']) === true
					&& $this->_origin['context'] !== false)
						$this->_context->add_origin($this->_origin['context']);
					break;
				case 'contextnumbers':
					if(isset($this->_origin['contextnumbers']) === true)
						$this->_contextnumbers->add_origin_list($this->_origin['contextnumbers']);
					break;
			}
		}

		return(true);
	}

	function enable()
	{
		$r = true;

		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_context() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function disable()
	{
		$r = true;

		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_context() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function enable_context()
	{
		return($this->_enable_disable_context(false));
	}

	function disable_context()
	{
		return($this->_enable_disable_context(true));
	}

	function _enable_disable_context($disable=false)
	{
		if(xivo_issa('context',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_context->enable($this->_info['context']['name']);
		else
			$r = $this->_context->disable($this->_info['context']['name']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['context'] = $r;

		return($r);
	}
}

?>
