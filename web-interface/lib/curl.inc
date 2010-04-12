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

if(defined('CURLOPT_FTPASCII') === false)
	define('CURLOPT_FTPASCII',CURLOPT_TRANSFERTEXT);

if(defined('CURLOPT_MUTE') === false)
	define('CURLOPT_MUTE',false);

if(defined('CURLOPT_PASSWDFUNCTION') === false)
	define('CURLOPT_PASSWDFUNCTION',false);


class dwho_curl
{
	var $_url	= null;
	var $_option	= array(
				'connecttimeout'	=> 5,
#				'timeout'		    => 30,
				'timeout'		    => 180,
				'returntransfer'	=> true);
	var $_error	= '';
	var $_errno	= 0;
	var $_link	= null;
	var $_uri	= null;

	function dwho_curl()
	{
		if(dwho::load_extension('curl') === false)
			trigger_error('Unable to load CURL',E_USER_ERROR);
	}

	function load($url=null,$option=array(),$open=true)
	{
		$option = (array) $option;
		$open = (bool) $open;

		if(dwho_has_len($url) === false)
			$url = $this->_url;

		if($url !== null && $this->_chk_url($url) === false)
			return(false);

		$this->_url = $url;

		$option = array_merge($this->_option,$option);

		if(empty($option) === true)
		{
			if($open === true)
				return($this->open());
			else
				return($this->create());
		}
		else if(($r = $this->create()) === false)
			return(false);

		foreach($option as $key => $val)
			$this->set_option($key,$val);

		if($open === true)
			return($this->connect());

		return($r);
	}

	function _chk_scheme($scheme)
	{
		$scheme = strtolower($scheme);

		switch($scheme)
		{
			case 'http':
			case 'https':
			case 'ftp':
			case 'ldap':
				return($scheme);
		}

		return(false);
	}

	function _chk_url($str)
	{
		if(is_object($this->_uri) === false)
		{
			dwho::load_class('dwho_uri');
			$this->_uri = new dwho_uri();
		}

		if(($arr = $this->_uri->parse_uri($str)) === false
		|| isset($arr['scheme']) === false
		|| ($arr['scheme'] = $this->_chk_scheme($arr['scheme'])) === false
		|| isset($arr['authority'],$arr['authority']['host']) === false)
			return(false);

		return($arr);
	}

	function _chk_info($info)
	{
		if($info === null)
			return(null);

		switch($info)
		{
			case 'effective_url':
			case CURLINFO_EFFECTIVE_URL:
				return(CURLINFO_EFFECTIVE_URL);
			case 'http_code':
			case CURLINFO_HTTP_CODE:
				return(CURLINFO_HTTP_CODE);
			case 'filetime':
			case CURLINFO_FILETIME:
				return(CURLINFO_FILETIME);
			case 'total_time':
			case CURLINFO_TOTAL_TIME:
				return(CURLINFO_TOTAL_TIME);
			case 'namelookup_time':
			case CURLINFO_NAMELOOKUP_TIME:
				return(CURLINFO_NAMELOOKUP_TIME);
			case 'connect_time':
			case CURLINFO_CONNECT_TIME:
				return(CURLINFO_CONNECT_TIME);
			case 'pretransfer_time':
			case CURLINFO_PRETRANSFER_TIME:
				return(CURLINFO_PRETRANSFER_TIME);
			case 'starttransfer_time':
			case CURLINFO_STARTTRANSFER_TIME:
				return(CURLINFO_STARTTRANSFER_TIME);
			case 'redirect_time':
			case CURLINFO_REDIRECT_TIME:
				return(CURLINFO_REDIRECT_TIME);
			case 'size_upload':
			case CURLINFO_SIZE_UPLOAD:
				return(CURLINFO_SIZE_UPLOAD);
			case 'size_download':
			case CURLINFO_SIZE_DOWNLOAD:
				return(CURLINFO_SIZE_DOWNLOAD);
			case 'speed_download':
			case CURLINFO_SPEED_DOWNLOAD:
				return(CURLINFO_SPEED_DOWNLOAD);
			case 'speed_upload':
			case CURLINFO_SPEED_UPLOAD:
				return(CURLINFO_SPEED_UPLOAD);
			case 'header_size':
			case CURLINFO_HEADER_SIZE:
				return(CURLINFO_HEADER_SIZE);
//			case 'header_out':
//			case CURLINFO_HEADER_OUT:
//				return(CURLINFO_HEADER_OUT);
			case 'request_size':
			case CURLINFO_REQUEST_SIZE:
				return(CURLINFO_REQUEST_SIZE);
			case 'ssl_verifyresult':
			case CURLINFO_SSL_VERIFYRESULT:
				return(CURLINFO_SSL_VERIFYRESULT);
			case 'content_length_download':
			case CURLINFO_CONTENT_LENGTH_DOWNLOAD:
				return(CURLINFO_CONTENT_LENGTH_DOWNLOAD);
			case 'content_length_upload':
			case CURLINFO_CONTENT_LENGTH_UPLOAD:
				return(CURLINFO_CONTENT_LENGTH_UPLOAD);
			case 'content_type':
			case CURLINFO_CONTENT_TYPE:
				return(CURLINFO_CONTENT_TYPE);
		}

		return(false);
	}

	function _chk_option($option,$value=null)
	{
		$option = strtolower($option);

		switch($option)
		{
//			case 'autoreferer':
//			case CURLOPT_AUTOREFERER:
//				return(CURLOPT_AUTOREFERER);
//			case 'cookiesession':
//			case CURLOPT_COOKIESESSION:
//				return(CURLOPT_COOKIESESSION);
			case 'dns_use_global_cache':
			case CURLOPT_DNS_USE_GLOBAL_CACHE:
				return(CURLOPT_DNS_USE_GLOBAL_CACHE);
			case 'dns_cache_timeout':
			case CURLOPT_DNS_CACHE_TIMEOUT:
				return(CURLOPT_DNS_CACHE_TIMEOUT);
//			case 'ftp_ssl':
//			case CURLOPT_FTP_SSL:
//				return(CURLOPT_FTP_SSL);
//			case 'private':
//			case CURLOPT_PRIVATE:
//				return(CURLOPT_PRIVATE);
//			case 'ftpsslauth':
//			case CURLOPT_FTPSSLAUTH:
//				return(CURLOPT_FTPSSLAUTH);
//			case 'ftpsslauth':
//			case CURLOPT_FTPSSLAUTH:
//				return(CURLOPT_FTPSSLAUTH);
			case 'port':
			case CURLOPT_PORT:
				return(CURLOPT_PORT);
			case 'file':
			case CURLOPT_FILE:
				return(CURLOPT_FILE);
			case 'infile':
			case CURLOPT_INFILE:
				return(CURLOPT_INFILE);
			case 'infilesize':
			case CURLOPT_INFILESIZE:
				return(CURLOPT_INFILESIZE);
			case 'url':
			case CURLOPT_URL:
				if($value !== null
				&& $this->_chk_url($value) === false)
					return(false);
				return(CURLOPT_URL);
			case 'proxy':
			case CURLOPT_PROXY:
				return(CURLOPT_PROXY);
			case 'verbose':
			case CURLOPT_VERBOSE:
				return(CURLOPT_VERBOSE);
			case 'header':
			case CURLOPT_HEADER:
				return(CURLOPT_HEADER);
			case 'httpheader':
			case CURLOPT_HTTPHEADER:
				return(CURLOPT_HTTPHEADER);
			case 'noprogress':
			case CURLOPT_NOPROGRESS:
				return(CURLOPT_NOPROGRESS);
			case 'nobody':
			case CURLOPT_NOBODY:
				return(CURLOPT_NOBODY);
			case 'failonerror':
			case CURLOPT_FAILONERROR:
				return(CURLOPT_FAILONERROR);
			case 'upload':
			case CURLOPT_UPLOAD:
				return(CURLOPT_UPLOAD);
			case 'post':
			case CURLOPT_POST:
				return(CURLOPT_POST);
			case 'ftplistonly':
			case CURLOPT_FTPLISTONLY:
				return(CURLOPT_FTPLISTONLY);
			case 'ftpappend':
			case CURLOPT_FTPAPPEND:
				return(CURLOPT_FTPAPPEND);
//			case 'ftp_create_missing_dirs':
//			case CURLOPT_FTP_CREATE_MISSING_DIRS:
//				return(CURLOPT_FTP_CREATE_MISSING_DIRS);
			case 'netrc':
			case CURLOPT_NETRC:
				return(CURLOPT_NETRC);
			case 'followlocation':
			case CURLOPT_FOLLOWLOCATION:
				if(DWHO_PHP_SAFE === true
				|| dwho_empty(ini_get('open_basedir')) === true)
					return(CURLOPT_FOLLOWLOCATION);
				else
					return(false);
			case 'ftpascii':
			case CURLOPT_FTPASCII:
				return(CURLOPT_FTPASCII);
			case 'put':
			case CURLOPT_PUT:
				return(CURLOPT_PUT);
			case 'mute':
			case CURLOPT_MUTE:
				return(CURLOPT_MUTE);
			case 'userpwd':
			case CURLOPT_USERPWD:
				return(CURLOPT_USERPWD);
			case 'proxyuserpwd':
			case CURLOPT_PROXYUSERPWD:
				return(CURLOPT_PROXYUSERPWD);
			case 'range':
			case CURLOPT_RANGE:
				return(CURLOPT_RANGE);
			case 'timeout':
			case CURLOPT_TIMEOUT:
				return(CURLOPT_TIMEOUT);
//			case 'timeout_ms':
//			case CURLOPT_TIMEOUT_MS:
//				return(CURLOPT_TIMEOUT_MS);
//			case 'tcp_nodelay':
//			case CURLOPT_TCP_NODELAY:
//				return(CURLOPT_TCP_NODELAY);
			case 'postfields':
			case CURLOPT_POSTFIELDS:
				return(CURLOPT_POSTFIELDS);
			case 'referer':
			case CURLOPT_REFERER:
				return(CURLOPT_REFERER);
			case 'useragent':
			case CURLOPT_USERAGENT:
				return(CURLOPT_USERAGENT);
			case 'ftpport':
			case CURLOPT_FTPPORT:
				return(CURLOPT_FTPPORT);
			case 'ftp_use_epsv':
			case CURLOPT_FTP_USE_EPSV:
				return(CURLOPT_FTP_USE_EPSV);
			case 'low_speed_limit':
			case CURLOPT_LOW_SPEED_LIMIT:
				return(CURLOPT_LOW_SPEED_LIMIT);
			case 'low_speed_time':
			case CURLOPT_LOW_SPEED_TIME:
				return(CURLOPT_LOW_SPEED_TIME);
			case 'resume_from':
			case CURLOPT_RESUME_FROM:
				return(CURLOPT_RESUME_FROM);
			case 'cookie':
			case CURLOPT_COOKIE:
				return(CURLOPT_COOKIE);
			case 'sslcert':
			case CURLOPT_SSLCERT:
				return(CURLOPT_SSLCERT);
			case 'sslcertpasswd':
			case CURLOPT_SSLCERTPASSWD:
				return(CURLOPT_SSLCERTPASSWD);
			case 'writeheader':
			case CURLOPT_WRITEHEADER:
				return(CURLOPT_WRITEHEADER);
			case 'ssl_verifyhost':
			case CURLOPT_SSL_VERIFYHOST:
				return(CURLOPT_SSL_VERIFYHOST);
			case 'cookiefile':
			case CURLOPT_COOKIEFILE:
				return(CURLOPT_COOKIEFILE);
			case 'sslversion':
			case CURLOPT_SSLVERSION:
				return(CURLOPT_SSLVERSION);
			case 'timecondition':
			case CURLOPT_TIMECONDITION:
				return(CURLOPT_TIMECONDITION);
			case 'timevalue':
			case CURLOPT_TIMEVALUE:
				return(CURLOPT_TIMEVALUE);
			case 'customrequest':
			case CURLOPT_CUSTOMREQUEST:
				return(CURLOPT_CUSTOMREQUEST);
			case 'stderr':
			case CURLOPT_STDERR:
				return(CURLOPT_STDERR);
			case 'transfertext':
			case CURLOPT_TRANSFERTEXT:
				return(CURLOPT_TRANSFERTEXT);
			case 'returntransfer':
			case CURLOPT_RETURNTRANSFER:
				return(CURLOPT_RETURNTRANSFER);
			case 'quote':
			case CURLOPT_QUOTE:
				return(CURLOPT_QUOTE);
			case 'postquote':
			case CURLOPT_POSTQUOTE:
				return(CURLOPT_POSTQUOTE);
			case 'interface':
			case CURLOPT_INTERFACE:
				return(CURLOPT_INTERFACE);
			case 'krb4level':
			case CURLOPT_KRB4LEVEL:
				return(CURLOPT_KRB4LEVEL);
			case 'httpproxytunnel':
			case CURLOPT_HTTPPROXYTUNNEL:
				return(CURLOPT_HTTPPROXYTUNNEL);
			case 'filetime':
			case CURLOPT_FILETIME:
				return(CURLOPT_FILETIME);
			case 'writefunction':
			case CURLOPT_WRITEFUNCTION:
				return(CURLOPT_WRITEFUNCTION);
			case 'readfunction':
			case CURLOPT_READFUNCTION:
				return(CURLOPT_READFUNCTION);
			case 'passwdfunction':
			case CURLOPT_PASSWDFUNCTION:
		  		return(CURLOPT_PASSWDFUNCTION);
			case 'headerfunction':
			case CURLOPT_HEADERFUNCTION:
				return(CURLOPT_HEADERFUNCTION);
			case 'maxredirs':
			case CURLOPT_MAXREDIRS:
				return(CURLOPT_MAXREDIRS);
			case 'maxconnects':
			case CURLOPT_MAXCONNECTS:
				return(CURLOPT_MAXCONNECTS);
			case 'closepolicy':
			case CURLOPT_CLOSEPOLICY:
				return(CURLOPT_CLOSEPOLICY);
			case 'fresh_connect':
			case CURLOPT_FRESH_CONNECT:
				return(CURLOPT_FRESH_CONNECT);
			case 'forbid_reuse':
			case CURLOPT_FORBID_REUSE:
				return(CURLOPT_FORBID_REUSE);
			case 'random_file':
			case CURLOPT_RANDOM_FILE:
				return(CURLOPT_RANDOM_FILE);
			case 'egdsocket':
			case CURLOPT_EGDSOCKET:
				return(CURLOPT_EGDSOCKET);
			case 'connecttimeout':
			case CURLOPT_CONNECTTIMEOUT:
				return(CURLOPT_CONNECTTIMEOUT);
//			case 'connecttimeout_ms':
//			case CURLOPT_CONNECTTIMEOUT_MS:
//				return(CURLOPT_CONNECTTIMEOUT_MS);
			case 'ssl_verifypeer':
			case CURLOPT_SSL_VERIFYPEER:
				return(CURLOPT_SSL_VERIFYPEER);
			case 'cainfo':
			case CURLOPT_CAINFO:
				return(CURLOPT_CAINFO);
			case 'capath':
			case CURLOPT_CAPATH:
				return(CURLOPT_CAPATH);
			case 'cookiejar':
			case CURLOPT_COOKIEJAR:
				return(CURLOPT_COOKIEJAR);
			case 'ssl_cipher_list':
			case CURLOPT_SSL_CIPHER_LIST:
				return(CURLOPT_SSL_CIPHER_LIST);
			case 'binarytransfer':
			case CURLOPT_BINARYTRANSFER:
				return(CURLOPT_BINARYTRANSFER);
//			case 'nosignal':
//			case CURLOPT_NOSIGNAL:
//				return(CURLOPT_NOSIGNAL);
//			case 'proxytype':
//			case CURLOPT_PROXYTYPE:
//				return(CURLOPT_PROXYTYPE);
//			case 'buffersize':
//			case CURLOPT_BUFFERSIZE:
//				return(CURLOPT_BUFFERSIZE);
			case 'httpget':
			case CURLOPT_HTTPGET:
				return(CURLOPT_HTTPGET);
			case 'http_version':
			case CURLOPT_HTTP_VERSION:
				return(CURLOPT_HTTP_VERSION);
			case 'sslkey':
			case CURLOPT_SSLKEY:
				return(CURLOPT_SSLKEY);
			case 'sslkeytype':
			case CURLOPT_SSLKEYTYPE:
				return(CURLOPT_SSLKEYTYPE);
			case 'sslkeypasswd':
			case CURLOPT_SSLKEYPASSWD:
				return(CURLOPT_SSLKEYPASSWD);
			case 'sslengine':
			case CURLOPT_SSLENGINE:
				return(CURLOPT_SSLENGINE);
			case 'sslengine_default':
			case CURLOPT_SSLENGINE_DEFAULT:
				return(CURLOPT_SSLENGINE_DEFAULT);
//			case 'sslcerttype':
//			case CURLOPT_SSLCERTTYPE:
//				return(CURLOPT_SSLCERTTYPE);
			case 'crlf':
			case CURLOPT_CRLF:
				return(CURLOPT_CRLF);
			case 'encoding':
			case CURLOPT_ENCODING:
				return(CURLOPT_ENCODING);
//			case 'proxyport':
//			case CURLOPT_PROXYPORT:
//				return(CURLOPT_PROXYPORT);
//			case 'unrestricted_auth':
//			case CURLOPT_UNRESTRICTED_AUTH:
//				return(CURLOPT_UNRESTRICTED_AUTH);
//			case 'ftp_use_eprt':
//			case CURLOPT_FTP_USE_EPRT:
//				return(CURLOPT_FTP_USE_EPRT);
//			case 'http200aliases':
//			case CURLOPT_HTTP200ALIASES:
//				return(CURLOPT_HTTP200ALIASES);
			case 'httpauth':
			case CURLOPT_HTTPAUTH:
				return(CURLOPT_HTTPAUTH);
			case 'proxyauth':
			case CURLOPT_PROXYAUTH:
				return(CURLOPT_PROXYAUTH);
//			case 'recently_used':
//			case CURLOPT_RECENTLY_USED:
//				return(CURLOPT_RECENTLY_USED);
//			case 'traffic':
//			case CURLOPT_TRAFFIC:
//				return(CURLOPT_TRAFFIC);
		}

		return(false);
	}

	function open()
	{
		if($this->create() === false)
			return(false);

		return($this->connect());
	}

	function is_open()
	{
		return(is_resource($this->_link));
	}

	function halt($str='')
	{
		$str = (string) $str;

		if(($this->_errno = $this->errno()) === 0)
			$this->_errno = 'unknown';

		if(($this->_error = $this->error()) === '')
			$this->_error = 'unknown';

		trigger_error($str.'Error: '.$this->_error.' - Errno: '.$this->_errno,E_USER_WARNING);
	}

	function create()
	{
		if(is_resource($this->_link) === true
		|| ($this->_link = curl_init($this->_url)) === false)
		{
			$this->halt();
			return(false);
		}

		return($this->_link);
	}

	function connect()
	{
		$r = false;

		if(is_resource($this->_link) === false
		|| ($r = curl_exec($this->_link)) === false)
			$this->halt();

		return($r);
	}

	function close()
	{
		$r = null;

		if(is_resource($this->_link) === true)
			$r = curl_close($this->_link);

		$this->_link = null;

		return($r);
	}

	function errno()
	{
		$r = 0;

		if(is_resource($this->_link) === true)
			$r = curl_errno($this->_link);

		return($r);
	}

	function error()
	{
		$r = '';

		if(is_resource($this->_link) === true)
			$r = curl_error($this->_link);

		return($r);
	}

	function get_info($info=null)
	{
		if(is_resource($this->_link) === false 
		|| ($info = $this->_chk_info($info)) === false)
			return(false);
		else if($info === null)
			return(curl_getinfo($this->_link));

		return(curl_getinfo($this->_link,$info));
	}

	function set_option($option,$value)
	{
		$r = false;

		if(is_resource($this->_link) === true
		&& ($option = $this->_chk_option($option,$value)) !== false)
			$r = curl_setopt($this->_link,$option,$value);

		return($r);
	}

	function get_version($name='')
	{
		if(dwho_has_len($name) === true)
			$name = strtolower($name);
		else
			$name = '';

		$list = explode(' ',curl_version());

		if(($nb = count($list)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$version = explode('/',$list[$i],2);

			if(isset($version[0]) === false)
				continue;

			$key = strtolower($version[0]);

			if(isset($version[1]) === true)
				$r[$key] = $version[1];
			else
				$r[$key] = '';
		}

		if(empty($r) === true)
			$r = false;
		else if(isset($r[$name]) === true)
			$r = &$r[$name];

		return($r);
	}
}

?>
