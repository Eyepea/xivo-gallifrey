<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_meetme extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_appmeetme		= false;
	var $_meetmefeatures	= false;
	var $_meetmeroom	= false;
	var $_extensions	= false;
	var $_extenumbers	= false;
	var $_contextnummember	= false;
	var $_contextmember	= false;
	var $_musiconhold	= array();
	var $_contextlist	= array();

	function xivo_application_service_asterisk_meetme(&$service,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_meetmefeatures = &$this->_service->get_module('meetmefeatures')) === false
		|| ($this->_appmeetme = &$this->_service->get_apprealstatic('meetme')) === false
		|| ($this->_meetmeroom = &$this->_appmeetme->get_module('room')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_extensions = &$this->_service->get_module('extensions')) === false
		|| ($this->_extenumbers = $this->get_abstract('extenumbers',array('type' => 'meetme'))) === false
		|| ($this->_contextnummember = $this->get_abstract('contextnummember',array('type' => 'meetme'))) === false
		|| ($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'meetme'))) === false)
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

		$this->_get_meetmemacro();
		$this->_get_extenumbers($this->_info['mfeatures']['id']);
		$this->_get_contextnummember($this->_info['mfeatures']['id']);
		$this->_get_contextmember($this->_info['mfeatures']['id']);

		return($this->_info);
	}

	function get_nb($arr=null,$disable=null)
	{
		return($this->_meetmeroom->get_nb($arr,$disable));
	}

	function _get_meetmemacro()
	{
		$this->_info['meetmemacro'] = false;
		$this->_origin['meetmemacro'] = false;

		if(($this->_info['meetmemacro'] = $this->_extensions->get_exten($this->_info['mfeatures']['number'],
										$this->_info['mfeatures']['context'],
										'macro')) !== false)
			$this->_origin['meetmemacro'] = $this->_extensions->get_origin();

		return($this->_info['meetmemacro']);
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
							$disable)) !== false)
				$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function _mk_meetmemacro_appdata($mfeaturesid)
	{
		$arr = array();
		$arr['appdata'][0] = XIVO_SRE_IPBX_AST_MEETME_MACRO;
		$arr['appdata'][1] = xivo_ulongint($mfeaturesid);

		return($arr);
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

		if(($rsmeetmemacro = $this->chk_macro('meetmemacro',
						      $number,
						      $this->_mk_meetmemacro_appdata(0),
						      $context)) !== false)
			$this->_set_result('meetmemacro',$rsmeetmemacro);

		$this->set_extenumbers($action,$number,$context);

		$this->set_contextnummember($context,$number);

		$this->set_contextmember();

		return(($this->get_errnb() < 1));
	}

	function set_meetmefeatures($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('mfeatures',null);
			$this->_set_error('mfeatures','invalid data');
			return(false);
		}

		$this->get_musiconhold();

		if(isset($arr['musiconhold'],$this->_musiconhold[$arr['musiconhold']]) === false)
			$arr['musiconhold'] = '';

		if(($rs = $this->_meetmefeatures->chk_values($arr)) === false)
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
		else if(($r = $this->_meetmeroom->set_meetmeroom($arr)) === false)
			$this->_set_error('meetmeroom',$this->_meetmeroom->get_error('meetmeroom'));

		$this->_set_result('meetmeroom',$this->_meetmeroom->get_result('meetmeroom'));

		return($r);
	}

	function _set_meetmemacro($action,$mfeaturesid)
	{
		$rs = false;

		if(($mfeaturesid = xivo_ulongint($mfeaturesid)) === 0
		|| ($rs = $this->get_result('meetmemacro')) === null
		|| $rs === false)
			return($rs);
		else if(($rsmacro = $this->set_macro($action,
						     'meetmemacro',
						     $rs['exten'],
						     $this->_mk_meetmemacro_appdata($mfeaturesid),
						     $rs['context'])) === false)
			return(false);

		$this->_set_result('meetmemacro',$rsmacro);

		return(true);
	}

	function set_contextmember()
	{
		$rs = array();
		$rs['mfeatures-exitcontext']['context'] = (string) $this->get_result_var('mfeatures','exitcontext');
		$rs['mfeatures-exitcontext']['typeval'] = 0;
		$rs['mfeatures-exitcontext']['varname'] = 'exitcontext';

		return(parent::set_contextmember($rs));
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$meetmeroomid = null;

		if($this->get_errnb() > 0
		|| ($meetmeroomid = $this->add_meetmeroom()) === false
		|| ($mfeaturesid = $this->add_meetmefeatures($meetmeroomid)) === false
		|| $this->add_extenumbers($mfeaturesid) === false
		|| $this->add_meetmemacro($mfeaturesid) === false
		|| $this->add_contextnummember($mfeaturesid) === false
		|| $this->add_contextmember($mfeaturesid) === false)
		{
			xivo_var_dump($meetmeroomid);
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

		return(($this->_return['meetmeroom'] = $this->_meetmeroom->get_return_var('meetmeroom')));
	}

	function add_meetmefeatures($meetmeid)
	{
		if(($meetmeid = xivo_ulongint($meetmeid)) === 0
		|| ($rs = $this->get_result('mfeatures')) === null
		|| $rs === false)
			return(false);

		$rs['meetmeid'] = $meetmeid;

		return(($this->_return['mfeatures'] = $this->_meetmefeatures->add($rs)));
	}

	function add_meetmemacro($mfeaturesid)
	{
		if(($rs = $this->_set_meetmemacro('add',$mfeaturesid)) !== true)
			return($rs);

		return(parent::add_macro('meetmemacro'));
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
				case 'meetmeroom':
					$this->_meetmeroom->delete_meetmeroom($this->_status,$val);
					break;
				case 'mfeatures':
					$this->_meetmefeatures->delete($val);
					break;
				case 'meetmemacro':
					$this->_extensions->delete_exten($val);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_add($this->_status);
					break;
				case 'contextnummember':
					$this->_contextnummember->reverse_add();
					break;
				case 'contextmember':
					$this->_contextmember->reverse_add();
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
		|| $this->edit_meetmemacro($this->_info['mfeatures']['id']) === false
		|| $this->edit_contextnummember() === false
		|| $this->edit_contextmember() === false)
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

		return(($this->_return['meetmeroom'] = $this->_meetmeroom->get_return_var('meetmeroom')));
	}

	function edit_meetmefeatures()
	{
		if(xivo_issa('mfeatures',$this->_info) === false
		|| ($rs = $this->get_result('mfeatures')) === null
		|| $rs === false)
			return(false);

		$rs['meetmeid'] = $this->_info['mfeatures']['meetmeid'];

		return(($this->_return['mfeatures'] = $this->_meetmefeatures->edit($this->_info['mfeatures']['id'],$rs)));
	}

	function edit_meetmemacro($mfeaturesid)
	{
		if(($rs = $this->_set_meetmemacro('edit',$mfeaturesid)) !== true)
			return($rs);

		return(parent::edit_macro('meetmemacro'));
	}

	function edit_contextnummember()
	{
		if(xivo_issa('mfeatures',$this->_info) === false)
			return(false);

		return(parent::edit_contextnummember($this->_info['mfeatures']['id']));
	}

	function edit_contextmember()
	{
		if(xivo_issa('mfeatures',$this->_info) === false)
			return(false);

		return(parent::edit_contextmember($this->_info['mfeatures']['id']));
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
				case 'meetmeroom':
					if(isset($this->_origin['meetmeroom']) === false
					|| $this->_origin['meetmeroom'] === false)
						$this->delete_meetmeroom();
					else
						$this->_meetmeroom->edit_origin($this->_origin['meetmeroom']);
					break;
				case 'mfeatures':
					if(isset($this->_origin['mfeatures']) === false
					|| $this->_origin['mfeatures'] === false)
						$this->delete_meetmefeatures();
					else
						$this->_meetmefeatures->edit_origin($this->_origin['mfeatures']);
					break;
				case 'meetmemacro':
					if(isset($this->_origin['meetmemacro']) === false
					|| $this->_origin['meetmemacro'] === false)
						$this->delete_macro('meetmemacro');
					else
						$this->_extensions->edit_origin($this->_origin['meetmemacro']);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_edit($this->_status);
					break;
				case 'contextnummember':
					$this->_contextnummember->reverse_edit($this->_status);
					break;
				case 'contextmember':
					$this->_contextmember->reverse_edit($this->_status);
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
		|| $this->delete_macro('meetmemacro') === false
		|| $this->delete_contextnummember() === false
		|| $this->delete_contextmember() === false)
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
		$r = false;

		if(xivo_issa('meetmeroom',$this->_info) === false
		|| ($r = $this->_meetmeroom->delete_meetmeroom($this->_status,$this->_info['meetmeroom']['id'])) === null
		|| $r === false)
			return($r);

		if($this->_status === 'delete')
			$this->_return['meetmeroom'] = $r;

		return($r);
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

	function _reverse_delete()
	{
		if($this->_status !== 'delete'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'meetmeroom':
					if(isset($this->_origin['meetmeroom']) === true
					&& $this->_origin['meetmeroom'] !== false)
						$this->_meetmeroom->add_origin($this->_origin['meetmeroom']);
					break;
				case 'mfeatures':
					if(isset($this->_origin['mfeatures']) === true
					&& $this->_origin['mfeatures'] !== false)
						$this->_meetmefeatures->add_origin($this->_origin['mfeatures']);
					break;
				case 'meetmemacro':
					if(isset($this->_origin['meetmemacro']) === true
					&& $this->_origin['meetmemacro'] !== false)
						$this->_extensions->add_origin($this->_origin['meetmemacro']);
					break;
				case 'extenumbers':
					if(isset($this->_origin['extenumbers']) === true
					&& $this->_origin['extenumbers'] !== false)
						$this->_extenumbers->add_origin($this->_origin['extenumbers']);
					break;
				case 'contextnummember':
					$this->_contextnummember->add_origin();
					break;
				case 'contextmember':
					$this->_contextmember->add_origin_list();
					break;
			}
		}

		return(true);
	}

	function unlinks()
	{
		if(xivo_issa('mfeatures',$this->_info) === false)
			return(false);
		else if(($dialaction = &$this->_service->get_module('dialaction')) !== false)
		{
			$dialaction_where = array();
			$dialaction_where['type'] = 'meetme';
			$dialaction_where['typeval'] = $this->_info['mfeatures']['id'];

			$dialaction->unlinked_where($dialaction_where);
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
