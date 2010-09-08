<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_extensions extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'extensions';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;
	var $_exten		= array();
	var $_context		= array();
	var $_filter_appdata	= false;

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

		$dir = dirname(__FILE__).DWHO_SEP_DIR.'config';

		if(($this->_conf = dwho_gat::get_file($dir)) === false)
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
		else if(isset($where['exten']) === true)
			$where['exten'] = $this->_sre->clean_extension($where['exten']);

		return(parent::get_element_where($where));
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		$r = array();

		if(($id = dwho_ulongint($id)) !== 0)
			$r['id'] = $id;

		$r['context'] = $arr['context'];
		$r['exten'] = $this->_sre->clean_extension($arr['exten']);
		$r['app'] = $arr['app'];

		if(isset($arr['priority']) === false)
			$r['priority'] = 1;
		else
			$r['priority'] = dwho_uint($arr['priority'],1);

		if(isset($arr['appdata']) === true)
			$r['appdata'] = (string) $arr['appdata'];

		if(isset($arr['name']) === true)
			$r['name'] = (string) $arr['name'];

		if(isset($arr['commented']) === false)
			$arr['commented'] = false;

		$r['commented'] = intval((bool) $arr['commented']);

		return($this->_dso->add($r));
	}

	function edit($id,$arr,$disable=null)
	{
		if(($id = dwho_ulongint($id)) === 0 || is_array($arr) === false)
			return(false);

		$r = array();

		if(isset($arr['context']) === true)
			$r['context'] = (string) $arr['context'];

		if(isset($arr['exten']) === true)
			$r['exten'] = $this->_sre->clean_extension($arr['exten']);

		if(isset($arr['priority']) === true)
			$r['priority'] = dwho_uint($arr['priority'],1);

		if(isset($arr['app']) === true)
			$r['app'] = (string) $arr['app'];

		if(isset($arr['appdata']) === true)
			$r['appdata'] = (string) $arr['appdata'];

		if(isset($arr['name']) === true)
			$r['name'] = (string) $arr['name'];

		if(isset($arr['commented']) === true)
			$r['commented'] = intval((bool) $arr['commented']);

		return($this->_dso->edit($id,$r,$disable));
	}

	function edit_where($where,$arr,$disable=null,$id=null,$chkunique=true)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['exten']) === true)
			$arr['exten'] = $this->_sre->clean_extension($arr['exten']);

		return(parent::edit_where($where,$arr,$disable,$id,$chkunique));
	}

	function new_exten($exten,$context,$app,$arr=null)
	{
		return($this->set_exten($app,0,$arr,$exten,$context));
	}

	function set_exten($app,$id=0,$arr=null,$exten='',$context='')
	{
		if(($id = dwho_uint($id)) === 0)
		{
			if(dwho_has_len($exten) === false
			|| dwho_has_len($context) === false)
				return(false);

			$id = count($this->_exten) + 1;
			$this->_exten[$id] = array();
		}
		else if(dwho_issa($id,$this->_exten) === false
		|| dwho_issa(0,$this->_exten[$id]) === false)
			return(false);
		else
		{
			$exten = $this->_exten[$id][0]['exten'];
			$context = $this->_exten[$id][0]['context'];
		}

		if(($r = $this->chk_exten($exten,$context,$app,$arr)) === false)
			return(false);
		else if(isset($r['priority']) === false)
			$r['priority'] = count($this->_exten[$id]) + 1;

		if (defined('XIVO_ACTIF_ACTIF') === true
                && $r['priority'] == 1
                && $r['app'] == 'Macro'
                && substr($r['appdata'],0,4) === 'user'
		&& XIVO_ACTIF_ACTIF === 1)
                        $r['priority'] = 2;

		 $this->_exten[$id][] = $r;

		return($id);
	}

	function chk_exten($exten,$context,$app,$arr=null)
	{
		return($this->_chk_element_value($exten,$context,$app,$arr));
	}

	function _chk_element_value($exten,$context,$app,$arr=null)
	{
		if(is_array($arr) === false)
			$arr = array();

		$r = array();

		if(isset($arr['appdata']) === true)
			$appdata = (array) $arr['appdata'];
		else
			$appdata = null;

		if(isset($arr['name']) === true)
			$r['name'] = $arr['name'];

		if(($rs = $this->chk_app_appdata($app,$appdata)) === false)
			return(false);

		$r['app'] = $rs['app'];
		$r['appdata'] = $rs['appdata'];
		$r['exten'] = $this->_sre->clean_extension($exten);
		$r['context'] = $context;

		if(isset($arr['commented']) === true)
			$r['commented'] = $arr['commented'];

		if(isset($arr['priority']) === true)
			$r['priority'] = dwho_uint($arr['priority']);

		return($this->chk_values($r,true,false,false));
	}

	function get_loaded_exten($id)
	{
		if(is_array($this->_exten) === false
		|| ($id = dwho_uint($id)) === 0
		|| dwho_issa($id,$this->_exten) === false
		|| dwho_issa(0,$this->_exten[$id]) === false)
			return(false);

		return($this->_exten[$id]);
	}

	function get_exten($exten,$context,$app='',$arr=null)
	{
		if(is_array($arr) === false)
			$arr = array();

		$where = array();
		$where['exten'] = $this->_sre->clean_extension($exten);
		$where['context'] = $context;

		if(dwho_has_len($app) === true)
		{
			if(($app = $this->get_app_displayname($app)) === false)
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
		|| ($id = dwho_uint($id)) === 0
		|| dwho_issa($id,$this->_exten) === false
		|| dwho_issa(0,$this->_exten[$id]) === false)
			return(false);

		$nb = count($this->_exten[$id]);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($ret = $this->add($this->_exten[$id][$i])) !== false)
				$r[] = $ret;
		}

		if(isset($r[0]) === false)
			return(false);
		else if($ret === false)
		{
			for($j = 0;$j < $i;$j++)
				$this->delete($r[$j]);
		}

		return($id);
	}

	function delete_exten($id)
	{
		if(is_array($this->_exten) === false
		|| ($id = dwho_uint($id)) === 0
		|| dwho_issa($id,$this->_exten) === false
		|| dwho_issa(0,$this->_exten[$id]) === false)
			return(false);

		$where = array();
		$where['exten'] = $this->_exten[$id][0]['exten'];
		$where['context'] = $this->_exten[$id][0]['context'];

		return($this->delete_where($where));
	}

	function new_context($context,$exten,$app,$arr=null)
	{
		return($this->set_context($exten,$app,0,$arr,$context));
	}

	function set_context($exten,$app,$id=0,$arr=null,$context='')
	{
		if(($id = dwho_uint($id)) === 0)
		{
			if(dwho_has_len($exten) === false
			|| dwho_has_len($context) === false)
				return(false);

			$id = count($this->_context) + 1;
			$this->_context[$id] = array();
		}
		else if(dwho_issa($id,$this->_context) === false
		|| dwho_issa(0,$this->_context[$id]) === false)
			return(false);
		else
			$context = $this->_context[$id][0]['context'];

		if(($r = $this->chk_context($context,$exten,$app,$arr)) === false)
			return(false);
		else if(isset($r['priority']) === false)
			$r['priority'] = count($this->_context[$id]) + 1;

		$this->_context[$id][] = $r;

		return($id);
	}

	function chk_context($context,$exten,$app,$arr=null)
	{
		return($this->_chk_element_value($exten,$context,$app,$arr));
	}

	function get_loaded_context($id)
	{
		if(is_array($this->_context) === false
		|| ($id = dwho_uint($id)) === 0
		|| dwho_issa($id,$this->_context) === false
		|| dwho_issa(0,$this->_context[$id]) === false)
			return(false);

		return($this->_context[$id]);
	}

	function get_context($context,$exten='',$app='',$arr=null,$order=null)
	{
		if(is_array($arr) === false)
			$arr = array();

		$where = array();
		$where['context'] = $context;

		if(dwho_has_len($exten) === true)
			$where['exten'] = $this->_sre->clean_extension($exten);

		if(dwho_has_len($app) === true)
		{
			if(($app = $this->get_app_displayname($app)) === false)
				return(false);

			$where['app'] = $app;
		}

		if(isset($arr['appdata']) === true)
			$where['appdata'] = $arr['appdata'];

		if(isset($arr['name']) === true)
			$where['name'] = $arr['name'];

		return($this->get_all_where($where,null,true,$order));
	}

	function add_context($id)
	{
		if(is_array($this->_context) === false
		|| ($id = dwho_uint($id)) === 0
		|| dwho_issa($id,$this->_context) === false
		|| dwho_issa(0,$this->_context[$id]) === false)
			return(false);

		$nb = count($this->_context[$id]);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($ret = $this->add($this->_context[$id][$i])) !== false)
				$r[] = $ret;
		}

		if(isset($r[0]) === false)
			return(false);
		else if($ret === false)
		{
			for($j = 0;$j < $i;$j++)
				$this->delete($r[$j]);
		}

		return($id);
	}

	function delete_context($id)
	{
		if(is_array($this->_context) === false
		|| ($id = dwho_uint($id)) === 0
		|| dwho_issa($id,$this->_context) === false
		|| dwho_issa(0,$this->_context[$id]) === false)
			return(false);

		$where = array();
		$where['context'] = $this->_context[$id][0]['context'];

		return($this->delete_where($where));
	}

	function new_hints($name,$protocol,$exten,$disable=null)
	{
		$id = count($this->_exten) + 1;

		if(($r = $this->chk_hints($name,$protocol,$exten,$disable)) === false)
			return(false);

		$this->_exten[$id][0] = $r;

		return($id);
	}

	function chk_hints($name,$protocol,$exten,$disable=null)
	{
		$r = array();
		$r['context'] = XIVO_SRE_IPBX_AST_CONTEXT_HINTS;

		if(($r['app'] = $this->_sre->mk_interface($name,$protocol)) === false)
			return(false);

		$r['exten'] = $this->_sre->clean_extension($exten);

		if($disable !== null)
			$r['commented'] = (bool) $disable;

		return($this->chk_values($r,true,false,false));
	}

	function get_hints($name,$protocol,$exten)
	{
		$where = array();

		if(($where['app'] = $this->_sre->mk_interface($name,$protocol)) === false)
			return(false);

		$where['exten'] = $this->_sre->clean_extension($exten);
		$where['context'] = XIVO_SRE_IPBX_AST_CONTEXT_HINTS;

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

	function _load_filter_appdata()
	{
		if(isset($this->_filter_appdata) === false)
			return(false);
		else if(is_object($this->_filter_appdata) === true)
			return(true);

		$this->_filter_appdata = new dwho_filter();

		return(true);
	}

	function get_app_displayname($app)
	{
		if(($cfg = $this->chk_app_exists($app,true)) === false
		|| is_array($cfg) === false
		|| isset($cfg['displayname']) === false)
			return(false);

		return($cfg['displayname']);
	}

	function chk_app_exists($app,$ret=false)
	{
		$app = strtolower(strval($app));

		if(($r = $this->_chk_exists_cfg('app',$app,$ret)) === false)
			return(false);

		return(($r === true ? $app : $r));
	}

	function chk_app_appdata($app,$appdata=array())
	{
		$r = array();
		$r['appdata'] = '';

		if(($cfg = $this->chk_app_exists($app,true)) === false
		|| is_array($cfg) === false)
			return(false);
		else if(isset($cfg['app']) === true)
			$r['app'] = $cfg['app'];
		else if(isset($cfg['displayname']) === true)
			$r['app'] = $cfg['displayname'];
		else
			return(false);

		if(dwho_issa('arg',$cfg) === false)
			return($r);

		$nbarg = count($cfg['arg']);

		if(count($appdata) > $nbarg)
		{
			$apparg1 = array_slice($appdata,0,$nbarg);
			$apparg2 = array_slice($appdata,$nbarg);

			if(empty($apparg2) === false)
				$apparg1[$nbarg-1] .= '|'.implode('|',$apparg2);

			$appdata = $apparg1;
		}

		if($this->_load_filter_appdata() === false
		|| $this->_filter_appdata->chk($appdata,true,$cfg['arg']) === false
		|| $this->_filter_appdata->get_errnb() > 0)
			return(false);

		$rs = $this->_filter_appdata->get_result();

		if(isset($cfg['appdata']) === true)
			$r['appdata'] = vsprintf(dwho_printf_escape($cfg['appdata'],count($rs)),$rs);
		else if(strcasecmp('macro',$r['app']) !== 0)
			$r['appdata'] = rtrim(implode('|',$rs),'|');
		else
			$r['appdata'] = implode('|',$rs);

		return($r);
	}

	function get_all_name_by_context($arr,$disable=null,$display=false)
	{
		$display = (bool) $display;

		if(is_array($arr) === false
		|| isset($arr['context']) === false
		|| ($list = $this->get_all_where($arr,$disable)) === false)
			return(false);

		$r = array();

		$nb = count($list);

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$list[$i];

			if(isset($r[$ref['name']]) === false)
			{
				$r[$ref['name']] = array();
				$r[$ref['name']]['name'] = $ref['name'];

				if($display === true)
					$r[$ref['name']]['exten'] = $this->_sre->display_extension($ref['exten']);
				else
					$r[$ref['name']]['exten'] = $ref['exten'];

				$r[$ref['name']]['context'] = $ref['context'];
				$r[$ref['name']]['commented'] = $ref['commented'];
				$r[$ref['name']]['line'] = array();
			}

			$r[$ref['name']]['line'][$ref['priority']] = $ref;
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_all_by_name_context($arr,$disable=null,$display=false)
	{
		if(is_array($arr) === false
		|| isset($arr['name'],$arr['context']) === false
		|| ($list = $this->get_all_where($arr,$disable)) === false)
			return(false);

		$r = array();
		$r['name'] = $list[0]['name'];

		if((bool) $display === false)
			$r['exten'] = $list[0]['exten'];
		else
			$r['exten'] = $this->_sre->display_extension($list[0]['exten']);

		$r['context'] = $list[0]['context'];
		$r['commented'] = $list[0]['commented'];
		$r['line'] = array();

		$nb = count($list);

		for($i = 0;$i < $nb;$i++)
			$r['line'][$list[$i]['priority']] = $list[$i];

		return($r);
	}
}

?>
