<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_context extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_context		= false;
	var $_contextnumbers	= false;
	var $_contextnummember	= false;
	var $_contextmember	= false;
	var $_entity		= null;
	var $_modules		= array();

	function xivo_application_service_asterisk_context(&$service,$param=null)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_context = &$this->_service->get_module('context')) === false
		|| ($this->_contextnumbers = &$this->_service->get_module('contextnumbers')) === false
		|| ($this->_contextnummember = &$this->_service->get_module('contextnummember')) === false
		|| ($this->_contextmember = &$this->_service->get_module('contextmember')) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['context'] = $this->_context->get($id,$disable)) === false)
			return(false);

		$this->_origin['context'] = $this->_context->get_origin();

		$this->_get_contextnumbers();
		$this->_get_contextnummember();
		$this->_get_contextmember();

		return($this->_info);
	}

	function _get_contextnumbers()
	{
		$this->_info['contextnumbers'] = false;
		$this->_origin['contextnumbers'] = false;

		if(xivo_haslen($this->_info['context']['entity']) === false)
			return(null);
		else if(($this->_info['contextnumbers'] = $this->_contextnumbers->get_all_by_type(
								array('context' => $this->_info['context']['name']))) === false)
			return(false);

		$this->_origin['contextnumbers'] = $this->_contextnumbers->get_origin_list();

		return($this->_info['contextnumbers']);
	}

	function _get_contextnummember()
	{
		$this->_origin['contextnummember'] = false;

		if(($this->_info['contextnummember'] = $this->_contextnummember->get_all_by_type(
								array('context' => $this->_info['context']['name']))) === false)
			return(false);

		$this->_origin['contextnummember'] = $this->_contextnummember->get_origin_list();

		return($this->_info['contextnummember']);
	}

	function _get_contextmember()
	{
		$this->_origin['contextmember'] = false;

		if(($this->_info['contextmember'] = $this->_contextmember->get_all_by_type(
								array('context' => $this->_info['context']['name']))) === false)
			return(false);

		$this->_origin['contextmember'] = $this->_contextmember->get_origin_list();

		return($this->_info['contextmember']);
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
			|| is_object($this->_entity) === false
			|| ($info['contextnumbers'] = $this->_contextnumbers->get_all_where(
								array('context' => $info['context']['name']))) === false
			|| ($info['entity'] = $this->_entity->get_where(
			   					array('name' => $info['context']['entity']))) === false)
				$info['contextnumbers'] = $info['entity'] = false;

			if($this->_contextnummember->get_where(array('context' => $info['context']['name'])) !== false
			|| $this->_contextmember->get_where(array('context' => $info['context']['name'])) !== false)
				$info['deletable'] = false;
			else
				$info['deletable'] = true;

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

		$name = (string) $this->get_result_var('context','name');

		if(xivo_issa('contextnumbers',$arr) === true)
		{
			$arr['contextnumbers']['context'] = $name;
			$this->set_contextnumbers($action,$arr['contextnumbers']);
		}
		else
		{
			$this->_set_result('contextnumbers',null);
			$this->_set_error('contextnumbers','invalid data');
		}

		$this->set_contextnummember($action,$name);
		$this->set_contextmember($action,$name);

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

	function set_contextnumbers($action,$arr)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| is_array($arr) === false)
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

		if(xivo_issa('contextnummember',$this->_info) === true)
			$contextnummember = $this->_info['contextnummember'];
		else
			$contextnummember = false;

		foreach($arr as $key => $list)
		{
			if(isset($reslist[$key]) === false)
				$reslist[$key] = array();

			$nb = count($list);

			foreach($list as $value)
			{
				if(isset($incr[$key]) === false)
					$incr[$key] = 0;

				$i = &$incr[$key];

				$infotmp = $info;
				$infotmp['type'] = $key;

				if(xivo_ak('numberbeg',$value) === false)
				{
					$errlist[$key][$i] = 'missing numberbeg';
					$reslist[$key][$i++] = $infotmp;
					$nberr++;
					$nb--;
					continue;
				}

				$infotmp['numberbeg'] = $value['numberbeg'];

				if($key === 'incall')
				{
					if(xivo_haslen($value,'didlength') === false)
					{
						$errlist['incall'][$i] = 'wrong didlength';
						$reslist['incall'][$i++] = $infotmp;
						$nberr++;
						$nb--;
						continue;
					}

					$infotmp['didlength'] = $value['didlength'];
				}

				if(xivo_haslen($value,'numberend') === true
				&& $value['numberbeg'] !== $value['numberend'])
				{
					if(xivo_is_max_digit($value['numberbeg'],$value['numberend'],true) !== true)
					{
						$errlist[$key][$i] = 'wrong interval';
						$reslist[$key][$i++] = $infotmp;
						$nberr++;
						$nb--;
						continue;
					}

					$infotmp['numberend'] = $value['numberend'];
				}
				else if(xivo_haslen($infotmp['numberbeg']) === false)
				{
					$nb--;
					continue;
				}
				else
					$infotmp['numberend'] = '';

				$uniq = $info['context'].'-'.$infotmp['type'].'-'.$infotmp['numberbeg'].'-'.$infotmp['numberend'];

				if(isset($tmp[$uniq]) === true)
				{
					$nb--;
					continue;
				}
				else if(($rs = $this->_contextnumbers->chk_values($infotmp)) === false)
				{
					$nb--;
					$err = $this->_contextnumbers->get_filter_error();
					
					if(isset($err['type']) === false && isset($err['context']) === false)
					{
						$errlist[$key][$i] = $err;
						$reslist[$key][$i++] = $this->_contextnumbers->get_filter_result();
						$nberr++;
					}
					continue;
				}
				else if($contextnummember !== false && isset($contextnummember[$key]) === true)
				{
					$rs['numberbeg'] = (string) $rs['numberbeg'];
					$rs['numberend'] = (string) $rs['numberend'];

					foreach($contextnummember[$key] as $k => $v)
					{
						$number = (string) $v['number'];

						if(isset($number{0}) === false)
						{
							unset($contextnummember[$key][$k]);
							continue;
						}

						$numberbeg = $rs['numberbeg'];
						$numberend = $rs['numberend'];

						if($key === 'incall')
						{
							$pattern = '/^'.$this->_service->_regexten_pattern($number).'$/';

							$didlength = -($rs['didlength']);
							$numberbeg = substr($numberbeg,$didlength);
							$numberend = substr($numberend,$didlength);

							if(preg_match($pattern,$numberbeg) === 1)
								$number = $numberbeg;
							else if(preg_match($pattern,$numberend) === 1)
								$number = $numberend;
							else if(xivo_digit($number) === false)
								continue;
						}

						if((isset($numberend{0}) === false && $numberbeg === $number) === true
						|| $this->_contextnumbers->chk_number_in_interval($numberbeg,$number,$numberend) === true)
							unset($contextnummember[$key][$k]);
					}

					if($nb < 2 && empty($contextnummember[$key]) === false)
					{
						$errlist[$key][$i] = 'missing interval';
						$reslist[$key][$i++] = $rs;
						$nberr++;
						continue;
					}
				}

				$nb--;
				$tmp[$uniq] = 1;
				$reslist[$key][$i++] = $rs;
			}

			if(empty($reslist[$key]) === true)
				unset($reslist[$key]);
		}

		if($nberr > 0)
		{
			$this->_set_error('contextnumbers',$errlist);
			$this->_set_result('contextnumbers',$reslist);
			return(false);
		}
		else if(empty($reslist) === true)
		{
			$this->_set_error('contextnumbers','empty');
			$this->_set_result('contextnumbers',false);
			return(false);
		}
		
		$this->_set_result('contextnumbers',$reslist);

		return(true);
	}

	function set_contextnummember($action,$context)
	{
		if($action !== 'edit'
		|| xivo_issa('context',$this->_origin) === false
		|| (string) $context === (string) $this->_origin['context']['name']
		|| xivo_issa('contextnummember',$this->_origin) === false)
			return(null);
		else if($this->_contextnummember->chk_value('context',$context) !== false)
		{
			$this->_set_result_var('contextnummember','context',$context);
			return(true);
		}

		$this->_set_error('contextnummember',$this->_contextnummember->get_filter_error());
		$this->_set_result('contextnummember',$this->_contextnummember->get_filter_result());

		return(false);
	}

	function set_contextmember($action,$context)
	{
		if($action !== 'edit'
		|| xivo_issa('context',$this->_origin) === false
		|| (string) $context === (string) $this->_origin['context']['name']
		|| xivo_issa('contextmember',$this->_origin) === false)
			return(null);
		else if($this->_contextmember->chk_value('context',$context) !== false)
		{
			$this->_set_result_var('contextmember','context',$context);
			return(true);
		}

		$this->_set_error('contextmember',$this->_contextmember->get_filter_error());
		$this->_set_result('contextmember',$this->_contextmember->get_filter_result());

		return(false);
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
			foreach($val as $value)
			{
				if($this->_contextnumbers->add($value) === false)
					return(false);
				else
					$this->_return['contextnumbers'][] = $value;
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
		|| $this->edit_contextnumbers() === false
		|| $this->edit_contextnummember() === false
		|| $this->edit_contextmember() === false)
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

		$where = array();
		$where['context'] = $this->_info['context']['name'];

		$this->_return['contextnumbers'] = $this->_contextnumbers->delete_where($where);

		if(($rs = $this->get_result('contextnumbers')) === null)
			return($this->_return['contextnumbers']);
		else if(is_array($rs) === false || empty($rs) === true)
			return(false);

		$this->_return['contextnumbers'] = array();

		foreach($rs as $val)
		{
			foreach($val as $value)
			{
				if($this->_contextnumbers->add($value) === false)
					return(false);
				else
					$this->_return['contextnumbers'][] = $value;
			}
		}

		if(isset($this->_return['contextnumbers'][0]) === false)
			return(($this->_return['contextnumbers'] = false));

		return(true);
	}

	function edit_contextnummember()
	{
		if(xivo_issa('context',$this->_info) === false)
			return(false);
		else if(($newcontext = (string) $this->get_result_var('contextnummember','context')) === ''
		|| xivo_issa('contextnummember',$this->_info) === false)
			return(null);

		$context = $this->_info['context']['name'];

		$this->_return['contextnummember'] = array();

		if($this->_contextnummember->edit_where(array('context'	=> $context),
							array('context'	=> $newcontext),
							null,
							null,
							false) !== false)
			$this->_return['contextnummember']['_context'] = $newcontext;
		else
			return(($this->_return['contextnummember'] = false));

		$refinfo = &$this->_info['contextnummember'];

		if($this->_update_extensions_contextnummember($context,$newcontext) === false
		|| $this->_update_extenumbers_contextnummember($context,$newcontext) === false
		|| (isset($refinfo['user']) === true
		   && $this->_update_user_contextnummember($context,$newcontext) === false) === true
		|| (isset($refinfo['group']) === true
		   && $this->_update_group_contextnummember($context,$newcontext) === false) === true
		|| (isset($refinfo['queue']) === true
		   && $this->_update_queue_contextnummember($context,$newcontext) === false) === true
		|| (isset($refinfo['meetme']) === true
		   && $this->_update_meetme_contextnummember($context,$newcontext) === false) === true
		|| (isset($refinfo['incall']) === true
		   && $this->_update_incall_contextnummember($context,$newcontext) === false) === true)
			return(false);

		return($this->_return['contextnummember']);
	}

	function edit_contextmember()
	{
		if(xivo_issa('context',$this->_info) === false)
			return(false);
		else if(($newcontext = (string) $this->get_result_var('contextmember','context')) === ''
		|| xivo_issa('contextmember',$this->_info) === false)
			return(null);

		$context = $this->_info['context']['name'];

		$this->_return['contextmember'] = array();

		if($this->_contextmember->edit_where(array('context' => $this->_info['context']['name']),
						     array('context' => $newcontext),
						     null,
						     null,
						     false) !== false)
			$this->_return['contextmember']['_context'] = $newcontext;
		else
			return(($this->_return['contextmember'] = false));

		$refinfo = &$this->_info['contextmember'];

		if((isset($refinfo['callfilter']) === true
		   && $this->_update_callfilter_contextmember($context,$newcontext) === false) === true
		|| (isset($refinfo['generalsip']) === true
		   && $this->_update_generalsip_contextmember($context,$newcontext,$refinfo['generalsip']) === false) === true
		|| (isset($refinfo['generaliax']) === true
		   && $this->_update_generaliax_contextmember($context,$newcontext,$refinfo['generaliax']) === false) === true
		|| (isset($refinfo['generalvoicemail']) === true
		   && $this->_update_generalvoicemail_contextmember($context,$newcontext,$refinfo['generalvoicemail']) === false) === true
		|| (isset($refinfo['meetme']) === true
		   && $this->_update_meetme_contextmember($context,$newcontext) === false) === true
		|| (isset($refinfo['outcall']) === true
		   && $this->_update_outcall_contextmember($context,$newcontext) === false) === true
		|| (isset($refinfo['queue']) === true
		   && $this->_update_queue_contextmember($context,$newcontext) === false) === true
		|| (isset($refinfo['rightcall']) === true
		   && $this->_update_rightcall_contextmember($context,$newcontext) === false) === true
		|| (isset($refinfo['schedule']) === true
		   && $this->_update_schedule_contextmember($context,$newcontext) === false) === true
		|| (isset($refinfo['trunksip']) === true
		   && $this->_update_trunksip_contextmember($context,$newcontext) === false) === true
		|| (isset($refinfo['trunkiax']) === true
		   && $this->_update_trunkiax_contextmember($context,$newcontext) === false) === true
		|| (isset($refinfo['trunkcustom']) === true
		   && $this->_update_trunkcustom_contextmember($context,$newcontext) === false) === true)
			return(false);

		return($this->_return['contextmember']);
	}

	function _update_extensions_contextnummember($context,$newcontext)
	{
		if(xivo_issa('contextnummember',$this->_return) === false)
			return(false);

		$ref = &$this->_return['contextnummember'];

		$where = array('context' => $context);
		$value = array('context' => $newcontext);

		if(((isset($this->_modules['extensions']) === false
		    || is_object($this->_modules['extensions']) === false) === true
		   && ($this->_modules['extensions'] = $this->_service->get_module('extensions')) === false) === true
		|| ($ref['extensions'] = $this->_modules['extensions']->edit_where($where,$value,
										   null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_extenumbers_contextnummember($context,$newcontext)
	{
		if(xivo_issa('contextnummember',$this->_return) === false)
			return(false);

		$ref = &$this->_return['contextnummember'];

		$where = array('context' => $context);
		$value = array('context' => $newcontext);

		if(((isset($this->_modules['extenumbers']) === false
		    || is_object($this->_modules['extenumbers']) === false) === true
		   && ($this->_modules['extenumbers'] = $this->_service->get_module('extenumbers')) === false) === true
		|| ($ref['extenumbers'] = $this->_modules['extenumbers']->edit_where($where,$value,
										     null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_user_contextnummember($context,$newcontext)
	{
		if(xivo_issa('contextnummember',$this->_return) === false)
			return(false);

		$ref = &$this->_return['contextnummember'];

		$where = array('context' => $context);
		$value = array('context' => $newcontext);

		if(((isset($this->_modules['usersip']) === false
		    || is_object($this->_modules['usersip']) === false) === true
		   && ($this->_modules['usersip'] = $this->_service->get_module('usersip')) === false) === true
		|| ((isset($this->_modules['useriax']) === false
		    || is_object($this->_modules['useriax']) === false) === true
		   && ($this->_modules['useriax'] = $this->_service->get_module('useriax')) === false) === true
		|| ((isset($this->_modules['usercustom']) === false
		    || is_object($this->_modules['usercustom']) === false) === true
		   && ($this->_modules['usercustom'] = $this->_service->get_module('usercustom')) === false) === true
		|| ((isset($this->_modules['userfeatures']) === false
		    || is_object($this->_modules['userfeatures']) === false) === true
		   && ($this->_modules['userfeatures'] = $this->_service->get_module('userfeatures')) === false) === true
		|| ($ref['usersip'] = $this->_modules['usersip']->edit_where($where,$value,
									     null,null,false)) === false
		|| ($ref['useriax'] = $this->_modules['useriax']->edit_where($where,$value,
									     null,null,false)) === false
		|| ($ref['usercustom'] = $this->_modules['usercustom']->edit_where($where,$value,
										   null,null,false)) === false
		|| ($ref['userfeatures'] = $this->_modules['userfeatures']->edit_where($where,$value,
										       null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_group_contextnummember($context,$newcontext)
	{
		if(xivo_issa('contextnummember',$this->_return) === false)
			return(false);

		$ref = &$this->_return['contextnummember'];

		$where = array('context' => $context);
		$value = array('context' => $newcontext);

		if(((isset($this->_modules['groupfeatures']) === false
		    || is_object($this->_modules['groupfeatures']) === false) === true
		   && ($this->_modules['groupfeatures'] = $this->_service->get_module('groupfeatures')) === false) === true
		|| ($ref['groupfeatures'] = $this->_modules['groupfeatures']->edit_where($where,$value,
											 false,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_queue_contextnummember($context,$newcontext)
	{
		if(xivo_issa('contextnummember',$this->_return) === false)
			return(false);

		$ref = &$this->_return['contextnummember'];

		$where = array('context' => $context);
		$value = array('context' => $newcontext);

		if(((isset($this->_modules['queuefeatures']) === false
		    || is_object($this->_modules['queuefeatures']) === false) === true
		   && ($this->_modules['queuefeatures'] = $this->_service->get_module('queuefeatures')) === false) === true
		|| ($ref['queuefeatures'] = $this->_modules['queuefeatures']->edit_where($where,$value,
											 null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_meetme_contextnummember($context,$newcontext)
	{
		if(xivo_issa('contextnummember',$this->_return) === false)
			return(false);

		$ref = &$this->_return['contextnummember'];

		$where = array('context' => $context);
		$value = array('context' => $newcontext);

		if(((isset($this->_modules['meetmefeatures']) === false
		    || is_object($this->_modules['meetmefeatures']) === false) === true
		   && ($this->_modules['meetmefeatures'] = $this->_service->get_module('meetmefeatures')) === false) === true
		|| ($ref['meetmefeatures'] = $this->_modules['meetmefeatures']->edit_where($where,$value,
											   null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_incall_contextnummember($context,$newcontext)
	{
		if(xivo_issa('contextnummember',$this->_return) === false)
			return(false);

		$ref = &$this->_return['contextnummember'];

		$where = array('context' => $context);
		$value = array('context' => $newcontext);

		if(((isset($this->_modules['incall']) === false
		    || is_object($this->_modules['incall']) === false) === true
		   && ($this->_modules['incall'] = $this->_service->get_module('incall')) === false) === true
		|| ($ref['incall'] = $this->_modules['incall']->edit_where($where,$value,
									   null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_callfilter_contextmember($context,$newcontext)
	{
		if(xivo_issa('contextmember',$this->_return) === false)
			return(false);

		$ref = &$this->_return['contextmember'];

		$where = array('context' => $context);
		$value = array('context' => $newcontext);

		if(((isset($this->_modules['callfilter']) === false
		    || is_object($this->_modules['callfilter']) === false) === true
		   && ($this->_modules['callfilter'] = $this->_service->get_module('callfilter')) === false) === true
		|| ($ref['callfilter'] = $this->_modules['callfilter']->edit_where($where,$value,
										   null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_generalsip_contextmember($context,$newcontext,$data)
	{
		if(xivo_issa('contextmember',$this->_return) === false
		|| is_array($data) === false)
			return(false);

		$ref = &$this->_return['contextmember'];

		if(((isset($this->_modules['sip']) === false
		    || is_object($this->_modules['sip']) === false) === true
		   && ($this->_modules['sip'] = $this->_service->get_realstatic('sip')) === false) === true)
			return(false);
		else if(isset($data['context']) === true
		&& ($ref['generalsip']['context'] = $this->_modules['sip']->edit_where(
							array('var_name'	=> 'context',
							      'var_val'		=> $context),
							array('var_val'		=> $newcontext),
							null,null,false)) === false)
			return(false);
		else if(isset($data['regcontext']) === true
		&& ($ref['generalsip']['regcontext'] = $this->_modules['sip']->edit_where(
							array('var_name'	=> 'regcontext',
							      'var_val'		=> $context),
							array('var_val'		=> $newcontext),
							null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_generaliax_contextmember($context,$newcontext,$data)
	{
		if(xivo_issa('contextmember',$this->_return) === false
		|| is_array($data) === false)
			return(false);

		$ref = &$this->_return['contextmember'];

		if(((isset($this->_modules['iax']) === false
		    || is_object($this->_modules['iax']) === false) === true
		   && ($this->_modules['iax'] = $this->_service->get_realstatic('iax')) === false) === true)
			return(false);
		else if(isset($data['regcontext']) === true
		&& ($ref['generaliax']['regcontext'] = $this->_modules['iax']->edit_where(
							array('var_name'	=> 'regcontext',
							      'var_val'		=> $context),
							array('var_val'		=> $newcontext),
							null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_generalvoicemail_contextmember($context,$newcontext,$data)
	{
		if(xivo_issa('contextmember',$this->_return) === false
		|| is_array($data) === false)
			return(false);

		$ref = &$this->_return['contextmember'];

		$value = array('var_val' => $newcontext);

		if(((isset($this->_modules['voicemail']) === false
		    || is_object($this->_modules['voicemail']) === false) === true
		   && ($this->_modules['voicemail'] = $this->_service->get_realstatic('voicemail')) === false) === true)
			return(false);
		else if(isset($data['cidinternalcontexts']) === true
		&& ($ref['generalvoicemail']['cidinternalcontexts'] = $this->_modules['voicemail']->edit_where(
							array('var_name'	=> 'cidinternalcontexts',
							      'var_val'		=> $context),
							$value,
							null,null,false)) === false)
			return(false);
		else if(isset($data['dialout']) === true
		&& ($ref['generalvoicemail']['dialout'] = $this->_modules['voicemail']->edit_where(
							array('var_name'	=> 'dialout',
							      'var_val'		=> $context),
							$value,
							null,null,false)) === false)
			return(false);
		else if(isset($data['callback']) === true
		&& ($ref['generalvoicemail']['callback'] = $this->_modules['voicemail']->edit_where(
							array('var_name'	=> 'callback',
							      'var_val'		=> $context),
							$value,
							null,null,false)) === false)
			return(false);
		else if(isset($data['exitcontext']) === true
		&& ($ref['generalvoicemail']['exitcontext'] = $this->_modules['voicemail']->edit_where(
							array('var_name'	=> 'exitcontext',
							      'var_val'		=> $context),
							$value,
							null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_meetme_contextmember($context,$newcontext)
	{
		if(xivo_issa('contextmember',$this->_return) === false)
			return(false);

		$ref = &$this->_return['contextmember'];

		$where = array('exitcontext' => $context);
		$value = array('exitcontext' => $newcontext);

		if(((isset($this->_modules['meetmefeatures']) === false
		    || is_object($this->_modules['meetmefeatures']) === false) === true
		   && ($this->_modules['meetmefeatures'] = $this->_service->get_module('meetmefeatures')) === false) === true
		|| ($ref['meetmefeatures'] = $this->_modules['meetmefeatures']->edit_where($where,$value,
											   null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_outcall_contextmember($context,$newcontext)
	{
		if(xivo_issa('contextmember',$this->_return) === false)
			return(false);

		$ref = &$this->_return['contextmember'];

		$where = array('context' => $context);
		$value = array('context' => $newcontext);

		if(((isset($this->_modules['extensions']) === false
		    || is_object($this->_modules['extensions']) === false) === true
		   && ($this->_modules['extensions'] = $this->_service->get_module('extensions')) === false) === true
		|| ((isset($this->_modules['extenumbers']) === false
		    || is_object($this->_modules['extenumbers']) === false) === true
		   && ($this->_modules['extenumbers'] = $this->_service->get_module('extenumbers')) === false) === true
		|| ((isset($this->_modules['outcall']) === false
		    || is_object($this->_modules['outcall']) === false) === true
		   && ($this->_modules['outcall'] = $this->_service->get_module('outcall')) === false) === true
		|| ($ref['extensions'] = $this->_modules['extensions']->edit_where($where,$value,
										   null,null,false)) === false
		|| ($ref['extenumbers'] = $this->_modules['extenumbers']->edit_where($where,$value,
										     null,null,false)) === false
		|| ($ref['outcall'] = $this->_modules['outcall']->edit_where($where,$value,
									     null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_queue_contextmember($context,$newcontext)
	{
		if(xivo_issa('contextmember',$this->_return) === false)
			return(false);

		$ref = &$this->_return['contextmember'];

		$where = array('context' => $context);
		$value = array('context' => $newcontext);

		if(((isset($this->_modules['queue']) === false
		    || is_object($this->_modules['queue']) === false) === true
		   && ($this->_modules['queue'] = $this->_service->get_module('queue')) === false) === true
		|| ($ref['queue'] = $this->_modules['queue']->edit_where($where,$value,
									 null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_rightcall_contextmember($context,$newcontext)
	{
		if(xivo_issa('contextmember',$this->_return) === false)
			return(false);

		$ref = &$this->_return['contextmember'];

		$where = array('context' => $context);
		$value = array('context' => $newcontext);

		if(((isset($this->_modules['rightcall']) === false
		    || is_object($this->_modules['rightcall']) === false) === true
		   && ($this->_modules['rightcall'] = $this->_service->get_module('rightcall')) === false) === true
		|| ($ref['rightcall'] = $this->_modules['rightcall']->edit_where($where,$value,
										 null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_schedule_contextmember($context,$newcontext)
	{
		if(xivo_issa('contextmember',$this->_return) === false)
			return(false);

		$ref = &$this->_return['contextmember'];

		$where = array('context' => $context);
		$value = array('context' => $newcontext);

		if(((isset($this->_modules['schedule']) === false
		    || is_object($this->_modules['schedule']) === false) === true
		   && ($this->_modules['schedule'] = $this->_service->get_module('schedule')) === false) === true
		|| ($ref['schedule'] = $this->_modules['schedule']->edit_where($where,$value,
									       null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_trunksip_contextmember($context,$newcontext)
	{
		if(xivo_issa('contextmember',$this->_return) === false)
			return(false);

		$ref = &$this->_return['contextmember'];

		$where = array('context' => $context);
		$value = array('context' => $newcontext);

		if(((isset($this->_modules['trunksip']) === false
		    || is_object($this->_modules['trunksip']) === false) === true
		   && ($this->_modules['trunksip'] = $this->_service->get_module('trunksip')) === false) === true
		|| ($ref['trunksip'] = $this->_modules['trunksip']->edit_where($where,$value,
									       null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_trunkiax_contextmember($context,$newcontext)
	{
		if(xivo_issa('contextmember',$this->_return) === false)
			return(false);

		$ref = &$this->_return['contextmember'];

		$where = array('context' => $context);
		$value = array('context' => $newcontext);

		if(((isset($this->_modules['trunkiax']) === false
		    || is_object($this->_modules['trunkiax']) === false) === true
		   && ($this->_modules['trunkiax'] = $this->_service->get_module('trunkiax')) === false) === true
		|| ($ref['trunkiax'] = $this->_modules['trunkiax']->edit_where($where,$value,
									       null,null,false)) === false)
			return(false);

		return(true);
	}

	function _update_trunkcustom_contextmember($context,$newcontext)
	{
		if(xivo_issa('contextmember',$this->_return) === false)
			return(false);

		$ref = &$this->_return['contextmember'];

		$where = array('context' => $context);
		$value = array('context' => $newcontext);

		if(((isset($this->_modules['trunkcustom']) === false
		    || is_object($this->_modules['trunkcustom']) === false) === true
		   && ($this->_modules['trunkcustom'] = $this->_service->get_module('trunkcustom')) === false) === true
		|| ($ref['trunkcustom'] = $this->_modules['trunkcustom']->edit_where($where,$value,
										     null,null,false)) === false)
			return(false);

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
					$this->_context->delete($val);

					if(isset($this->_origin['context']) === true
					&& $this->_origin['context'] !== false)
						$this->_context->add_origin($this->_origin['context']);
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
				case 'contextnummember':
					if(is_array($val) === false
					|| isset($val['_context']) === false)
						continue;

					$context = $val['_context'];

					$where = array('context' => $context);

					if(isset($this->_origin['contextnummember']) === false
					|| xivo_issa(0,$this->_origin['contextnummember']) === false)
					{
						$this->_contextnummember->delete_where($where);
						continue;
					}

					$newcontext = $this->_origin['contextnummember'][0]['context'];

					$this->_contextnummember->edit_where($where,array('context' => $newcontext),
									     null,null,false);

					$this->_update_extensions_contextnummember($context,$newcontext);
					$this->_update_extenumbers_contextnummember($context,$newcontext);

					if(isset($val['user']) === true)
						$this->_update_user_contextnummember($context,$newcontext);

					if(isset($val['group']) === true)
						$this->_update_group_contextnummember($context,$newcontext);

					if(isset($val['queue']) === true)
						$this->_update_queue_contextnummember($context,$newcontext);

					if(isset($val['meetme']) === true)
						$this->_update_meetme_contextnummember($context,$newcontext);

					if(isset($val['incall']) === true)
						$this->_update_incall_contextnummember($context,$newcontext);
					break;
				case 'contextmember':
					if(is_array($val) === false
					|| isset($val['_context']) === false)
						continue;

					$context = $val['_context'];

					$where = array('context' => $context);

					if(isset($this->_origin['contextmember']) === false
					|| xivo_issa(0,$this->_origin['contextmember']) === false)
					{
						$this->_contextmember->delete_where($where);
						continue;
					}

					$newcontext = $this->_origin['contextmember'][0]['context'];

					$this->_contextmember->edit_where($where,array('context' => $newcontext),
									  null,null,false);

					if(isset($val['callfilter']) === true)
						$this->_update_callfilter_contextmember($context,$newcontext);

					if(isset($val['generalsip']) === true)
						$this->_update_generalsip_contextmember($context,$newcontext,$val['generalsip']);

					if(isset($val['generaliax']) === true)
						$this->_update_generaliax_contextmember($context,$newcontext,$val['generaliax']);

					if(isset($val['generalvoicemail']) === true)
						$this->_update_generalvoicemail_contextmember($context,$newcontext,$val['generalvoicemail']);

					if(isset($val['meetme']) === true)
						$this->_update_meetme_contextmember($context,$newcontext);

					if(isset($val['outcall']) === true)
						$this->_update_outcall_contextmember($context,$newcontext);

					if(isset($val['queue']) === true)
						$this->_update_queue_contextmember($context,$newcontext);

					if(isset($val['rightcall']) === true)
						$this->_update_rightcall_contextmember($context,$newcontext);

					if(isset($val['schedule']) === true)
						$this->_update_schedule_contextmember($context,$newcontext);

					if(isset($val['trunksip']) === true)
						$this->_update_trunksip_contextmember($context,$newcontext);

					if(isset($val['trunkiax']) === true)
						$this->_update_trunkiax_contextmember($context,$newcontext);

					if(isset($val['trunkcustom']) === true)
						$this->_update_trunkcustom_contextmember($context,$newcontext);
					break;					   
			}
		}

		return(true);
	}

	function delete()
	{
		if(isset($this->_info['contextnummember'],$this->_info['contextmember']) === false)
			return(false);
		else if(is_array($this->_info['contextnummember']) === true
		|| is_array($this->_info['contextmember']) === true)
			return(null);

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
