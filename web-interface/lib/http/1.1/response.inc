<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

if(defined('UPLOAD_ERR_NO_TMP_DIR') === false)
	define('UPLOAD_ERR_NO_TMP_DIR',6);

if(defined('UPLOAD_ERR_CANT_WRITE') === false)
	define('UPLOAD_ERR_CANT_WRITE',7);

require_once(dwho_file::joinpath(DWHO_PATH_ROOT,'http','1.1','abstract.inc'));

class dwho_http_1_1_response extends dwho_http_1_1_abstract
{
	var $_status_line	= null;
	var $_upload_file	= array();

	function dwho_http_1_1_response()
	{
		$this->dwho_http_1_1_abstract();
	}

	function set_status_line($code)
	{
		$code = intval($code);

		if(isset($this->_status_code[$code]) === false)
			return(false);

		$this->_code = $code;
		$this->_status_line = sprintf('HTTP/%s %d %s',
					      $this->_version,
					      $this->_code,
					      $this->_status_code[$code]);

		return(true);
	}

	function set_cache_control($parameters)
	{
		if(is_scalar($parameters) === true)
			$parameters = array($parameters => null);

		if(is_array($parameters) === false || empty($parameters) === true)
			return(false);

		$params = array();

		foreach($parameters as $attribute => $value)
		{
			$attrlower = strtolower($attribute);

			switch($attrlower)
			{
				case 'public':
				case 'no-store':
				case 'no-transform':
				case 'must-revalidate':
				case 'proxy-revalidate':
					$attribute = $attrlower;
					$value = null;
					break;
				case 'private':
				case 'no-cache':
					$attribute = $attrlower;

					if($value === null)
						break;
					else if($this->valid_token($value) === false)
						return(false);

					$value = '"'.$value.'"';
					break;
				case 'max-age':
				case 's-maxage':
					$attribute = $attrlower;

					if(dwho_is_uint($value) === false)
						return(false);
					break;
				default:
					if($this->valid_token($attribute) === false)
						return(false);
					else if($value !== null)
						$value = $this->quote_word($value);
			}

			if($value !== null)
				$params[$attribute] = $attribute.'='.$value;
			else
				$params[$attribute] = $attribute;
		}

		$this->_headers['cache-control'] = sprintf('Cache-Control: %s',implode(', ',$params));

		return(true);
	}

	function set_content_disposition($type,$parameters=null)
	{
		if(is_scalar($type) === false)
			return(false);
		else if(strcasecmp('attachment',$type) === 0)
			$type = 'attachment';
		else if(strcasecmp('inline',$type) === 0)
			$type = 'inline';
		else if($this->valid_token($type) === false)
			return(false);

		if(is_array($parameters) === false || empty($parameters) === true)
		{
			$this->_headers['content-disposition'] = sprintf('Content-Disposition: %s',$type);
			return(true);
		}

		$params = array();

		foreach($parameters as $attribute => $value)
		{
			$attrlower = strtolower($attribute);

			switch($attrlower)
			{
				case 'filename':
					$attribute = $attrlower;

					if(($value = $this->quote_word($value)) === false)
						return(false);
					break;
				case 'creation-date':
				case 'modification-date':
				case 'read-date':
					$attribute = $attrlower;

					if($this->valid_rfc2822_date($value) === false
					|| ($value = $this->quote_word($value)) === false)
						return(false);
					break;
				case 'size':
					$attribute = $attrlower;

					if(dwho_is_digit($value) === false)
						return(false);

					$value = dwho_ulongint($value);
					break;
				default:
					if($this->valid_token($attribute) === false
					|| ($value = $this->quote_word($value)) === false)
						return(false);
			}

			$params[$attribute] = $attribute.'='.$value;
		}

		$this->_headers['content-disposition'] = sprintf('Content-Disposition: %s; %s',
								 $type,
								 implode('; ',$params));

		return(false);
	}

	function authent_basic($realm)
	{
		if(($realm = $this->quote_word($realm)) === false)
			return(false);

		$this->_headers['www-authenticate'] = sprintf('WWW-Authenticate: Basic realm=%s',
							      $realm);

		return(true);
	}

	function send_file_download($file)
	{
		if(dwho_file::is_f_r($file) === false
		|| ($info = dwho_file::get_info($file)) === false)
			return(false);

		$content_type = explode('/',$info['mime'],2);

		$this->set_pragma('no-cache');
		$this->set_expires($this->build_rfc1123_date(gmstrftime('%s')));
		$this->set_last_modified($this->build_rfc1123_date($info['mtime']));
		$this->set_cache_control(array('no-cache'		=> null,
					       'must-revalidate'	=> null));
		$this->set_content_type($content_type[0],$content_type[1]);
		$this->set_content_length($info['size']);
		$this->set_content_disposition('attachment',array('filename'	=> $info['name']));
		$this->send();
		readfile($info['path']);
		die();
	}

	function verify_upload_file_error($name)
	{
		if(dwho_issa($name,$_FILES) === false
		|| isset($_FILES[$name]['error']) === false)
			return(false);

		if(($maxfilesize = dwho_get_memory('upload_max_filesize')) === false)
			trigger_error('Invalid upload_max_filesize php.ini directive',E_USER_ERROR);

		if(($postmaxsize = dwho_get_memory('post_max_size')) === false)
			trigger_error('Invalid post_max_size php.ini directive',E_USER_ERROR);

		if(($mlimit = dwho_get_memory('memory_limit')) === false)
			trigger_error('Invalid memory_limit php.ini directive',E_USER_ERROR);

		if($postmaxsize < $maxfilesize)
			trigger_error('post_max_size php.ini directive shoud be less than upload_max_filesize php.ini directive',E_USER_ERROR);

		if($mlimit < $postmaxsize)
			trigger_error('memory_limit php.ini directive shoud be less than post_max_size php.ini directive',E_USER_ERROR);

		switch($_FILES[$name]['error'])
		{
			case UPLOAD_ERR_OK:
				return(true);
			case UPLOAD_ERR_INI_SIZE:
				//trigger_error('Uploaded file exceeds the upload_max_filesize directive',E_USER_WARNING);
				break;
			case UPLOAD_ERR_FORM_SIZE:
				//trigger_error('Uploaded file exceeds the MAX_FILE_SIZE directive',E_USER_WARNING);
				break;
			case UPLOAD_ERR_PARTIAL:
				//trigger_error('Uploaded file was only partially uploaded',E_USER_WARNING);
				break;
			case UPLOAD_ERR_NO_FILE:
				//trigger_error('No file was uploaded',E_USER_WARNING);
				break;
			case UPLOAD_ERR_NO_TMP_DIR:
				trigger_error('Missing a temporary folder',E_USER_WARNING);
				break;
			case UPLOAD_ERR_CANT_WRITE:
				trigger_error('Failed to write file to disk',E_USER_WARNING);
				break;
			default:
				trigger_error('Unknown File Error',E_USER_WARNING);
		}

		return(false);
	}

	function valid_upload_file_info($params)
	{
		if(is_array($params) === false)
			return(false);

		if(isset($params['size']) === true)
		{
			$params['size'] = dwho_uint($params['size']);

			if($this->_upload_file['size'] > $params['size'])
				return(false);
		}

		if(isset($params['mime'],$this->_upload_file['mime']) === true)
		{
			$params['mime'] = (array) $params['mime'];

			if(isset($params['mime'][0]) === false
			|| in_array($this->_upload_file['mime'],$params['mime']) === false)
				return(false);
		}

		if(isset($params['name']) === true)
		{
			$params['name'] = (string) $params['name'];

			if(preg_match($params['name'],$this->_upload_file['name']) !== 1)
				return(false);
		}

		if(isset($params['extension']) === true)
		{
			$params['extension'] = (array) $params['extension'];

			if(($pos = strrpos($this->_upload_file['name'],'.')) === false)
				return(false);

			$ext = strtolower(substr($this->_upload_file['name'],$pos + 1));

			if(isset($params['extension'][0]) === true
			&& in_array($ext,$params['extension']) === false)
				return(false);

			if(dwho_issa($ext,$params['extension']) === true
			&& in_array($this->_upload_file['mime'],$params['extension'][$ext]) === false)
				return(false);
		}

		return(true);
	}

	function upload_file($name,$valid_info=array())
	{
		if(is_string($name) === false
		|| $this->verify_upload_file_error($name) === false
		|| isset($_FILES[$name]['tmp_name'],
			 $_FILES[$name]['name'],
			 $_FILES[$name]['type'],
			 $_FILES[$name]['size']) === false)
			return(false);
		else if(is_uploaded_file($_FILES[$name]['tmp_name']) === false)
		{
			dwho_file::rm($_FILES[$name]['tmp_name']);
			return(false);
		}

		$this->_upload_file = $_FILES[$name];

		if(($this->_upload_file['mime'] = dwho_file::get_mime($this->_upload_file['tmp_name'])) === false)
			$this->_upload_file['mime'] = $this->_upload_file['type'];

		if(empty($valid_info) === false
		&& $this->valid_upload_file_info($valid_info) === false)
		{
			dwho_file::rm($this->_upload_file['tmp_name']);
			return(false);
		}

		return($this->_upload_file);
	}

	function send($die=false)
	{
		if(isset($this->_status_line) === true)
			header($this->_status_line);

		foreach($this->_headers as $value)
			header($value);

		if((bool) $die === true)
			die();
	}
}

?>
