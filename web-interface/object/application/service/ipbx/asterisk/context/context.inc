<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_context extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_context		= false;
	var $_contextentity	= false;
	var $_entity		= null;

	function xivo_application_service_asterisk_context(&$service)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_context = &$this->_service->get_module('context')) === false
		|| ($this->_contextentity = &$this->_service->get_module('contextentity')) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['context'] = $this->_context->get($id,$disable)) === false)
			return(false);

		$this->_origin['context'] = $this->_context->get_origin();

		$this->_get_contextentity($disable);

		return($this->_info);
	}

	function _get_contextentity()
	{
		$this->_origin['contextentity'] = false;
		$this->_info['contextentity'] = false;

		if(xivo_haslen($this->_info['context']['entity']) === false)
			return(null);
		else if(($this->_info['contextentity'] = $this->_contextentity->get_all_by_type(
										array('context' => $this->_info['context']['name']))) === false)
			return(false);

		$this->_origin['contextentity'] = $this->_contextentity->get_origin_list();

		return($this->_info['contextentity']);
	}

	function get_elements()
	{
		$r = array();
		$r['context'] = $this->_context->get_element();
		$r['contextentity'] = $this->_contextentity->get_element();

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
			|| ($info['contextentity'] = $this->_contextentity->get_all_where(
									array('context' => $info['context']['name']))) === false
			|| ($info['entity'] = $this->_entity->get_where(
			   						array('name' => $info['context']['entity']))) === false)
				$info['contextentity'] = $info['entity'] = false;

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
		else if(xivo_issa('contextentity',$arr) === true)
		{
			$arr['contextentity']['context'] = $this->get_result_var('context','name');
			$this->set_contextentity($arr['contextentity']);
		}
		else
		{
			$this->_set_result('contextentity',null);
			$this->_set_error('contextentity','invalid data');
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

	function set_contextentity($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('contextentity',null);
			$this->_set_error('contextentity','invalid data');
			return(false);
		}
		else if(isset($arr['context']) === false)
		{
			$this->_set_result('contextentity',false);
			$this->_set_error('contextentity','wrong context');
			return(false);
		}
		
		$info = $infotmp = $tmp = array();
		$info['context'] = $arr['context'];
		$info['typevalbeg'] = '';
		$info['typevalend'] = '';
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

				if(xivo_ak('typevalbeg',$val1) === false)
				{
					$errlist[$key][$i] = 'missing typevalbeg';
					$reslist[$key][$i++] = $infotmp;
					$nberr++;
					continue;
				}

				$infotmp['typevalbeg'] = $val1['typevalbeg'];

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

				if(xivo_haslen($val1,'typevalend') === true
				&& $val1['typevalbeg'] !== $val1['typevalend'])
				{
					if(xivo_is_max_digit($val1['typevalbeg'],$val1['typevalend'],true) !== true)
					{
						$errlist[$key][$i] = 'wrong interval';
						$reslist[$key][$i++] = $infotmp;
						$nberr++;
						continue;
					}

					$infotmp['typevalend'] = $val1['typevalend'];
				}
				else if(xivo_haslen($infotmp['typevalbeg']) === false)
					continue;
				else
					$infotmp['typevalend'] = '';

				$uniq = $info['context'].'-'.$infotmp['type'].'-'.$infotmp['typevalbeg'].'-'.$infotmp['typevalend'];

				if(isset($tmp[$uniq]) === true)
					continue;
				else if(($rs = $this->_contextentity->chk_values($infotmp)) === false)
				{
					$err = $this->_contextentity->get_filter_error();
					
					if(isset($err['type']) === false && isset($err['context']) === false)
					{
						$errlist[$key][$i] = $err;
						$reslist[$key][$i++] = $this->_contextentity->get_filter_result();
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
			$this->_set_error('contextentity',$errlist);
			$this->_set_result('contextentity',$reslist);
			return(false);
		}
		if(empty($reslist) === true)
		{
			$this->_set_error('contextentity','empty');
			$this->_set_result('contextentity',false);
			return(false);
		}
		
		$this->_set_result('contextentity',$reslist);

		return(true);
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$contextentity = null;

		if($this->get_errnb() > 0
		|| $this->add_context() === false
		|| ($contextentity = $this->add_contextentity()) === false)
		{
			if($contextentity !== null)
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

	function add_contextentity()
	{
		if(($rs = $this->get_result('contextentity')) === null
		|| (string) $this->get_result_var('context','entity') === '')
			return(null);
		else if(is_array($rs) === false || empty($rs) === true)
			return(false);

		$this->_return['contextentity'] = array();

		foreach($rs as $val)
		{
			foreach($val as $val1)
			{
				if($this->_contextentity->add($val1) === false)
					return(false);
				else
					$this->_return['contextentity'][] = $val1;
			}
		}

		if(isset($this->_return['contextentity'][0]) === false)
			return(($this->_return['contextentity'] = false));

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
				case 'contextentity':
					if(is_array($val) === false)
						continue;

					foreach($val as $contextentityval)
					{
						if($contextentityval !== false)
							$this->_contextentity->delete_where($contextentityval);
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
		|| $this->edit_contextentity() === false)
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

	function edit_contextentity()
	{
		if(xivo_issa('context',$this->_info) === false)
			return(false);
		else if((string) $this->get_result_var('context','entity') === '')
			return($this->delete_contextentity());
		else if(xivo_issa('contextentity',$this->_info) === false)
			return($this->add_contextentity());

		$contextentity_where = array();
		$contextentity_where['context'] = $this->_info['context']['name'];

		$this->_return['contextentity'] = $this->_contextentity->delete_where($contextentity_where);

		if(($rs = $this->get_result('contextentity')) === null)
			return($this->_return['contextentity']);
		else if(is_array($rs) === false || empty($rs) === true)
			return(false);

		$this->_return['contextentity'] = array();

		foreach($rs as $val)
		{
			foreach($val as $val1)
			{
				if($this->_contextentity->add($val1) === false)
					return(false);
				else
					$this->_return['contextentity'][] = $val1;
			}
		}

		if(isset($this->_return['contextentity'][0]) === false)
			$this->_return['contextentity'] = false;

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
				case 'contextentity':
					if(is_array($val) === true)
					{
						foreach($val as $contextentityval)
							$this->_contextentity->delete_where($contextentityval);
					}

					if(isset($this->_origin['contextentity']) === true)
						$this->_contextentity->add_origin_list($this->_origin['contextentity']);
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
		|| $this->delete_contextentity() === false)
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

	function delete_contextentity()
	{
		if(xivo_issa('contextentity',$this->_info) === false)
			return(null);
		else if(xivo_issa('context',$this->_info) === false)
			return(false);

		$r = $this->_contextentity->delete_where(array('context' => $this->_info['context']['name']));

		if($this->_status === 'delete')
			$this->_return['contextentity'] = $r;

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
				case 'contextentity':
					if(isset($this->_origin['contextentity']) === true)
						$this->_contextentity->add_origin_list($this->_origin['contextentity']);
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
