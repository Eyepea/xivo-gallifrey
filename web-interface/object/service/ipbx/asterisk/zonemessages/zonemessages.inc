<?php

xivo_service_asterisk::required(array('staticconf','staticconf.inc'),true);

class xivo_service_asterisk_zonemessages extends xivo_service_asterisk_staticconf
{
	var $_dso		= null;
	var $_name		= 'zonemessages';
	var $_filename		= 'voicemail.conf';
	var $_category		= 'zonemessages';
	var $_cat_metric	= 1;
	var $_var_metric	= 0;
	var $_filter		= false;
	var $_origin		= false;
	var $_zonemessages	= array();

	function xivo_service_asterisk_zonemessages(&$sre,&$dso)
	{
		$this->_load_staticconf_config();

		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_dso->set_param('filename',$this->_filename);

		$this->_load_config();
		$this->_zonemessages['conf'] = $this->_conf;
	}

	function get($id,$disable=null)
	{
		$this->_origin = false;

		if(($r = parent::get($id,$disable)) === false)
			return(false);

		$this->_origin = $r;

		return($this->_prepare($r));
	}

	function get_where($arr,$disable=null)
	{
		$this->_origin = false;

		$arr = (array) $arr;
		$arr['category'] = $this->_category;
		$arr['cat_metric'] = $this->_cat_metric;
		$arr['var_metric'] = $this->_var_metric;

		if(($r = parent::get_where($arr,$disable)) === false)
			return(false);

		$this->_origin = $r;

		return($this->_prepare($r));
	}

	function _prepare($data)
	{
		$data['name'] = $data['var_name'];
		$data['msg_format'] = '';

		if(($pos = strpos($data['var_val'],'|')) !== false)
		{
			$data['timezone'] = substr($data['var_val'],0,$pos);
			$data['msg_format'] = substr($data['var_val'],$pos+1);
		}
		else
			$data['timezone'] = $data['var_val'];

		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function _origin($type,$val=null)
	{
		if($val !== null)
			$orig = $val;
		else
			$orig = $this->_origin;

		if(($type !== 'add' && $type !== 'edit') === true
		|| is_array($orig) === false)
			return(false);

		if($type === 'add')
			$r = parent::add($orig,$orig['id']);
		else
			$r = parent::edit($orig['id'],$orig);

		return($r);
	}

	function get_by_name($name,$disable=null)
	{
		return($this->get_where(array('var_name' => $name),$disable));
	}

	function add($arr,$id=0)
	{
		$arr = (array) $arr;
		$arr['category'] = $this->_category;
		$arr['cat_metric'] = $this->_cat_metric;
		$arr['var_metric'] = $this->_var_metric;

		return(parent::add($arr,$id));
	}

	function edit($id,$arr)
	{
		$arr = (array) $arr;
		$arr['category'] = $this->_category;
		$arr['cat_metric'] = $this->_cat_metric;
		$arr['var_metric'] = $this->_var_metric;
		
		return(parent::edit($id,$arr));
	}

	function replace_val_list($arr,$disable=null)
	{
		$arr = (array) $arr;

		if(empty($arr) === true)
			return(false);

		foreach($arr as $name => $val)
			$this->replace_val_by_name($name,$val,$disable);

		return(true);
	}

	function replace_by_name($name,$arr)
	{
		$arr = (array) $arr;
		$arr['category'] = $this->_category;
		$arr['cat_metric'] = $this->_cat_metric;
		$arr['var_metric'] = $this->_var_metric;
		$arr['var_name'] = $name;

		return(parent::replace_by_name($name,$arr));
	}

	function replace_val_by_name($name,$val,$disable=null)
	{
		$disable = $disable === null ? $disable : intval((bool) $disable);

		$arr = array();
		$arr['var_val'] = $val;
		$arr['cat_metric'] = $this->_cat_metric;
		$arr['var_metric'] = $this->_var_metric;

		if($val !== null)
			$arr['commented'] = $disable !== null ? $disable : 0;
		else
			$arr['commented'] = $disable !== null ? $disable : 1;

		return($this->replace_by_name($name,$arr));
	}

	function _prepare_get_all_where($arr=array(),$disable=null)
	{
		$this->_conf = &$this->_staticconf['conf'];

		$r = parent::_prepare_get_all_where($arr,$disable);

		$this->_conf = &$this->_zonemessages['conf'];

		return($r);
	}

	function get_all_name($arr=array(),$disable=null,$prepare=true)
	{
		$prepare = (bool) $prepare;

		$arr = (array) $arr;
		$arr['category'] = $this->_category;
		$arr['cat_metric'] = $this->_cat_metric;

		if(($list = $this->_prepare_get_all_where($arr,$disable)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[$list[$i]['var_name']] = $prepare === true ? $this->_prepare($list[$i]) : $list[$i];

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function chk_msg_format($str)
	{
		$str = (string) $str;

		if(($len = strlen($str)) === 0)
			return(false);

		if(($nb = preg_match_all('/\'/',$str,$match,PREG_OFFSET_CAPTURE)) === 0)
			return($this->_chk_letter_format($str));

		if($nb % 2 === 1)
			return(false);

		$match = $match[0];

		if($match[0][1] > 0)
		{
			$msg = substr($str,0,$match[0][1]);

			if($this->_chk_letter_format($msg) === false)
				return(false);
		}

		for($i = 0;$i < $nb;$i += 2)
		{
			$offset1 = $match[$i][1]+1;
			$offset2 = $match[$i+1][1];

			if($offset1 === $offset2)
				return(false);

			$msg = substr($str,$offset1,$offset2-$offset1);

			$regex = '[\/'.XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN.']'.
				  XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_LENGTH;

			if(preg_match('/^'.$regex.'$/',$msg) === 0)
				return(false);

			if(isset($match[$i+2]) === false)
				break;

			$offset3 = $match[$i+2][1];

			if($offset3 <= $offset2+1)
				continue;

			$msg = substr($str,$offset2+1,$offset3-($offset2+1));

			if($this->_chk_letter_format($msg) === false)
				return(false);
		}

		if($len <= $offset2+1)
			return(true);

		$msg = substr($str,$offset2+1);
		
		return($this->_chk_letter_format($msg));
	}

	function _chk_letter_format($str)
	{
		$str = (string) $str;

		if(preg_match('/^[ AaBbcdehHIklmMNpPqQRSTxXY]+$/',$str) === 1)
			return(true);

		return(false);
	}
}

?>
