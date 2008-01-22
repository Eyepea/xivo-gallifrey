<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_meetme extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_appmeetme		= null;
	var $_meetmefeatures	= null;
	var $_meetmeroom	= null;
	var $_extensions	= null;
	var $_extenumbers	= null;
	var $_musiconhold	= false;

	function xivo_application_service_asterisk_meetme(&$service,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_meetmefeatures = &$this->_service->get_module('meetmefeatures')) === false
		|| ($this->_appmeetme = &$this->_service->get_apprealstatic('meetme')) === false
		|| ($this->_meetmeroom = &$this->_appmeetme->get_module('room')) === false)
			return(false);
		
		if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_extensions = &$this->_service->get_module('extensions')) === false
		|| ($this->_extenumbers = &$this->get_abstract('extenumbers')) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['meetmeroom'] = $this->_meetmeroom->get_meetmeroom($id)) === false
		|| ($this->_info['mfeatures'] = $this->_meetmefeatures->get_where(
							array('meetmeid' => $this->_info['meetmeroom']['id']))) === false)
			return(false);

		$this->_origin['mfeatures'] = $this->_meetmefeatures->get_origin();
		$this->_origin['meetmeroom'] = $this->_meetmeroom->get_origin_var('meetmeroom');

		if($this->_components === false)
			return($this->_info);

		$this->_get_exten();
		$this->_get_extenumbers();
		$this->get_musiconhold();

		return($this->_info);
	}

	function get_nb($arr=null,$disable=null)
	{
		return($this->_meetmeroom->get_nb($arr,$disable));
	}

	function _get_exten()
	{
		$this->_info['exten'] = false;
		$this->_origin['exten'] = false;

		$this->_info['exten'] = $this->_extensions->get_exten('macro',
								    $this->_info['mfeatures']['number'],
								    $this->_info['mfeatures']['context']);

		$this->_origin['exten'] = $this->_extensions->get_origin();

		return($this->_info['exten']);
	}

	function _get_extenumbers()
	{
		return(parent::_get_extenumbers('meetme',$this->_info['mfeatures']['id']));
	}

	function get_elements()
	{
		$r = array();

		$r['mfeatures'] = $this->_meetmefeatures->get_element();
		$r['meetmeroom'] = $this->_meetmeroom->get_element();

		return($r);
	}

	function get_meetme_list($disable=null,$order=null,$limit=null)
	{
		if(($rs = $this->_meetmefeatures->get_all(null,true,$order,$limit)) === false
		|| ($nb = count($rs)) === 0)
		{
			$this->_cnt = $this->_meetmefeatures->get_cnt();
			return(false);
		}
		
		$this->_cnt = $this->_meetmefeatures->get_cnt();

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['mfeatures'] = $rs[$i];

			if(($info['meetmeroom'] = $this->_meetmeroom->get_meetmeroom(
							$info['mfeatures']['meetmeid'],
							$disable)) === false)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

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
		|| xivo_issa('meetmeroom',$arr) === false
		|| xivo_issa('mfeatures',$arr) === false)
			return(false);

		$this->set_meetmeroom($arr['meetmeroom']);

		$number = (string) $this->get_result_var('meetmeroom','number');

		$arr['mfeatures']['meetmeid'] = 0;
		$arr['mfeatures']['number'] = $number;

		$this->set_meetmefeatures($arr['mfeatures']);
	
		$context = (string) $this->get_result_var('mfeatures','context');

		$appdata = array('appdata' => XIVO_SRE_IPBX_AST_MEETME_MACRO);
		if(($rsexten = $this->set_macro($action,'exten',$number,$appdata,$context)) !== false)
			$this->_set_result('exten',$rsexten);

		$this->set_extenumbers($action,$number,$context);

		if($this->get_errnb() > 0)
			$r = false;
		else
			$r = true;

		return($r);
	}

	function set_meetmefeatures($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('mfeatures',null);
			$this->_set_error('mfeatures','invalid module');
			return(false);
		}
		else if(($rs = $this->_meetmefeatures->chk_values($arr)) === false)
		{
			$this->_set_result('mfeatures',$this->_meetmefeatures->get_filter_result());
			$this->_set_error('mfeatures',$this->_meetmefeatures->get_filter_error());
			return(false);
		}

		$this->_set_result('mfeatures',$rs);

		return(true);
	}

	function set_meetmeroom($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('meetmeroom',null);
			$this->_set_error('meetmeroom','invalid data');
			return(false);
		}

		if(($r = $this->_meetmeroom->set_meetmeroom($arr)) === false)
			$this->_set_error('meetmeroom',$this->_meetmeroom->get_error('meetmeroom'));

		$this->_set_result('meetmeroom',$this->_meetmeroom->get_result('meetmeroom'));

		return($r);
	}

	function set_extenumbers($action,$number,$context)
	{
		return(parent::set_extenumbers($action,'meetme',$number,$context));
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$meetmeroomid = null;

		if($this->get_errnb() > 0
		|| ($meetmeroomid = $this->add_meetmeroom()) === false
		|| ($meetmefeaturesid = $this->add_meetmefeatures($meetmeroomid)) === false
		|| $this->add_extenumbers($meetmefeaturesid) === false
		|| $this->add_macro('exten') === false)
		{
			$r = false;
			if($meetmeroomid !== null)
				$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
	}

	function add_meetmeroom()
	{
		if(($rs = $this->_meetmeroom->add_meetmeroom()) === null
		|| $rs === false)
			return(false);

		$this->_return['meetmeroom'] = $this->_meetmeroom->get_return_var('meetmeroom');

		return($this->_return['meetmeroom']);
	}

	function add_meetmefeatures($meetmeid)
	{
		if(($meetmeid = xivo_uint($meetmeid)) === 0
		|| ($rs = $this->get_result('mfeatures')) === null
		|| $rs === false)
			return(false);

		$rs['meetmeid'] = $meetmeid;

		$this->_return['mfeatures'] = $this->_meetmefeatures->add($rs);

		return($this->_return['mfeatures']);
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
				case 'meetmeroom':
					$this->_meetmeroom->delete_meetmeroom($this->_status,$val);
					break;
				case 'mfeatures':
					$this->_meetmefeatures->delete($val);
					break;
				case 'extenumbers':
					$this->_extenumbers->delete($this->_status,$val);
					break;
				case 'exten':
					$this->_extensions->delete_exten($val);
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
		|| $this->edit_meetmeroom() === false
		|| $this->edit_meetmefeatures() === false
		|| $this->edit_extenumbers() === false
		|| $this->edit_macro('exten') === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_meetmeroom()
	{
		if(xivo_issa('meetmeroom',$this->_info) === false
		|| ($rs = $this->_meetmeroom->edit_meetmeroom($this->_info['meetmeroom']['id'])) === null
		|| $rs === false)
			return(false);

		$this->_return['meetmeroom'] = $this->_meetmeroom->get_return_var('meetmeroom');

		return($this->_return['meetmeroom']);
	}

	function edit_meetmefeatures()
	{
		if(xivo_issa('mfeatures',$this->_info) === false
		|| ($rs = $this->get_result('mfeatures')) === null
		|| $rs === false)
			return(false);

		$rs['meetmeid'] = $this->_info['mfeatures']['meetmeid'];

		$this->_return['mfeatures'] = $this->_meetmefeatures->edit($this->_info['mfeatures']['id'],$rs);

		return($this->_return['mfeatures']);
	}

	function edit_extenumbers()
	{
		if(xivo_issa('extenumbers',$this->_info) === false)
			return(false);

		return(parent::edit_extenumbers($this->_info['extenumbers']['id']));
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
				case 'meetmeroom':
					if(isset($this->_origin['meetmeroom']) === false)
						continue;

					if($this->_origin['meetmeroom'] === false)
						$this->_meetmeroom->delete_meetmeroom($this->_status,$val);
					else
						$this->_meetmeroom->edit_origin($this->_origin['meetmeroom']);
					break;
				case 'mfeatures':
					if(isset($this->_origin['mfeatures']) === false)
						continue;

					if($this->_origin['mfeatures'] === false)
						$this->_meetmefeatures->delete($val);
					else
						$this->_meetmefeatures->edit_origin($this->_origin['mfeatures']);
					break;
				case 'extenumbers':
					if(isset($this->_origin['extenumbers']) === false)
						continue;

					if($this->_origin['extenumbers'] === false)
						$this->_extenumbers->delete($this->_status,$val);
					else
						$this->_extenumbers->edit_origin($this->_origin['extenumbers']);
					break;
				case 'exten':
					if(isset($this->_origin['exten']) === false)
						continue;

					if($this->_origin['exten'] === false)
						$this->_extensions->delete_exten($val);
					else
						$this->_extensions->edit_origin($this->_origin['exten']);
					break;
			}
		}
		return(true);
	}

	function delete()
	{
		$this->_status = 'delete';
		$this->_return = array();

		if($this->delete_meetmeroom() === false
		|| $this->delete_meetmefeatures() === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_macro('exten') === false)
		{
			$this->_reverse_delete();
			$this->_status = '';
			return(false);
		}

		$this->unlinks();
		$this->_status = '';

		return(true);
	}

	function delete_meetmeroom()
	{
		$rs = false;

		if(xivo_issa('meetmeroom',$this->_info) === false
		|| ($rs = $this->_meetmeroom->delete_meetmeroom($this->_status,$this->_info['meetmeroom']['id'])) === null
		|| $rs === false)
			return($rs);

		if($this->_status === 'delete')
			$this->_return['meetmeroom'] = $rs;

		return($this->_return['meetmeroom']);
	}

	function delete_meetmefeatures()
	{
		if(xivo_issa('mfeatures',$this->_info) === false)
			return(false);

		$r = $this->_meetmefeatures->delete($this->_info['mfeatures']['id']);

		if($this->_status === 'delete')
			$this->_return['mfeatures'] = $r;

		return($r);
	}

	function delete_extenumbers()
	{
		if(xivo_issa('extenumbers',$this->_info) === false)
			return(false);

		return(parent::delete_extenumbers($this->_info['extenumbers']['id']));
	}

	function _reverse_delete()
	{
		if($this->_status !== 'delete'
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];

			if($this->_return[$key] === false)
				continue;

			switch($key)
			{
				case 'meetmeroom':
					if(isset($this->_origin['meetmeroom']) === false)
						continue;

					$this->_meetmeroom->add_origin($this->_origin['meetmeroom']);
					break;
				case 'mfeatures':
					if(isset($this->_origin['mfeatures']) === false)
						continue;

					$this->_meetmefeatures->add_origin($this->_origin['mfeatures']);
					break;
				case 'extenumbers':
					if(isset($this->_origin['extenumbers']) === false)
						continue;

					$this->_extenumbers->add_origin($this->_origin['extenumbers']);
					break;
				case 'exten':
					if(isset($this->_origin['exten']) === false
					|| $this->_origin['exten'] === false)
						continue;

					$this->_extensions->add_origin($this->_origin['exten']);
					break;
			}
		}
		return(true);
	}

	function unlinks()
	{
		if(xivo_issa('mfeatures',$this->_info) === false)
			return(false);

		if(($incall = &$this->_service->get_module('incall')) !== false)
		{
			$incall_where = array();
			$incall_where['type'] = 'meetme';
			$incall_where['typeval'] = $this->_info['mfeatures']['id'];

			$incall->unlinked_where($incall_where);
		}

		if(($dialstatus = &$this->_service->get_module('dialstatus')) !== false)
		{
			$dialstatus_where = array();
			$dialstatus_where['type'] = 'meetme';
			$dialstatus_where['typeval'] = $this->_info['mfeatures']['id'];

			$dialstatus->unlinked_where($dialstatus_where);
		}

		if(($schedule = &$this->_service->get_module('schedule')) === false)
			return(true);
		
		$schedule_where = array();
		$schedule_where['typetrue'] = 'meetme';
		$schedule_where['typevaltrue'] = $this->_info['mfeatures']['id'];

		$schedule->unlinked_where($schedule_where);

		$schedule_where = array();
		$schedule_where['typefalse'] = 'meetme';
		$schedule_where['typevalfalse'] = $this->_info['mfeatures']['id'];

		$schedule->unlinked_where($schedule_where);

		return(true);
	}
}

?>
