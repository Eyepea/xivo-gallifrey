<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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

class xivo_i18n_mediator
{
	var $_mediator = array(
				'language'	=> null,
				'locale'	=> null,
				'territory'	=> null,
				'timezone'	=> null);

	var $_attrib = array(
				'browser'	=> array(),
				'language'	=> '',
				'locale'	=> '',
				'territory'	=> '',
				'timezone'	=> '');

	function xivo_i18n_mediator()
	{
		if(xivo::load_class('xivo::i18n::language') === false
		|| xivo::load_class('xivo::i18n::locale') === false
		|| xivo::load_class('xivo::i18n::territory') === false
		|| xivo::load_class('xivo::i18n::timezone') === false)
			return(false);

		$this->_mediator['language'] = new xivo_i18n_language();
		$this->_mediator['locale'] = new xivo_i18n_locale();
		$this->_mediator['territory'] = new xivo_i18n_territory();
		$this->_mediator['timezone'] = new xivo_i18n_timezone();
		$this->_get_browser_info();
	}

	function chk($name)
	{
		$name = (string) $name;

		return((isset($this->_mediator[$name]) === true && is_object($this->_mediator[$name])));
	}

	function get($name)
	{
		$name = (string) $name;

		if($this->chk($name) === true)
			return($this->_mediator[$name]);

		return(false);
	}

	function set_attrib($name,$value)
	{
		$name = (string) $name;

		$this->_attrib[$name] = $value;
	}

	function get_attrib($name)
	{
		$name = (string) $name;

		if(isset($this->_attrib[$name]) === true)
			return($this->_attrib[$name]);
		else
			return(null);
	}

	function _get_browser_info()
	{
		if(isset($_SERVER['HTTP_ACCEPT_LANGUAGE']) === false)
			return(null);

		$arr = explode(',',$_SERVER['HTTP_ACCEPT_LANGUAGE']);

		if(($nb = count($arr)) === 0)
			return(null);

		for($i = 0;$i < $nb;$i++)
		{
			$l = $arr[$i];

			if(($pos = strpos($l,';')) !== false)
				$l = substr($l,0,$pos);

			if(($lc = $this->chk_locale($l)) !== false)
			{
				$pos = strpos($lc,'_');
				$t = substr($lc,$pos+1);
				$l = substr($lc,0,$pos);

				if(isset($this->_attrib['browser'][$l]) === false)
					$this->_attrib['browser'][$l] = array();

				$this->_attrib['browser'][$l][] = $t;
			}
			else if(($la = $this->chk_language($l)) !== false)
			{
				if(isset($this->_attrib['browser'][$la]) === false)
					$this->_attrib['browser'][$la] = array();

				$this->_attrib['browser'][$la][] = '';
			}
		}

		$this->_mediator['language']->set_attrib('browser',$this->_attrib['browser']);
		$this->_mediator['territory']->set_attrib('browser',$this->_attrib['browser']);
	}

	function get_browser_language()
	{
		if(empty($this->_attrib['browser']) === true)
			return(false);

		reset($this->_attrib['browser']);
		return(key($this->_attrib['browser']));
	}

	function get_browser_territory($lang='')
	{
		if($lang !== ''
		&& ($lang = $this->chk_language($lang)) !== false
		&& isset($this->_attrib['browser'][$lang]) === true)
		{
			$v = &$this->_attrib['browser'][$lang];

			if(is_array($v) === true && ($nb = count($v)) > 1)
			{
				for($i = 0;$i < $nb;$i++)
				{
					if($v[$i] !== ''
					&& ($r = $this->chk_territory($v[$i])) !== false)
						return($r);
				}
			}
		}
		else if(empty($this->_attrib['browser']) === false)
		{
			foreach($this->_attrib['browser'] as $k => $v)
			{
				if(($k = $this->chk_language($k)) === false
				|| is_array($v) === false
				|| ($nb = count($v)) < 2)
					continue;

				for($i = 0;$i < $nb;$i++)
				{
					if($v[$i] !== ''
					&& ($r = $this->chk_territory($v[$i])) !== false)
						return($r);
				}
			}
		}

		return(false);
	}

	function set_language($lang='')
	{
		$r = false;

		$lang = (string) $lang;

		if($lang !== '' && ($lang = $this->chk_language($lang)) !== false)
			$r = $lang;
		else if(($lang = $this->get_browser_language()) !== false)
			$r = $lang;
		else if(defined('XIVO_LC_LANGUAGE') === true && ($lang = $this->chk_language(XIVO_LC_LANGUAGE)) !== false)
			$r = $lang;

		return(($this->_attrib['language'] = $r));
	}

	function chk_language($lang)
	{
		return($this->_mediator['language']->is_valid($lang));
	}

	function get_language_info($lang='')
	{
		$lang = (string) $lang;

		if($lang === '')
			$lang = $this->set_language();

		return($this->_mediator['language']->get_info($lang));
	}

	function set_territory($territo='')
	{
		$r = false;

		$territo = (string) $territo;

		if($territo !== '' && ($territo = $this->chk_territory($territo)) !== false)
			$r = $territo;
		else if($this->_attrib['language'] === '' && ($lang = $this->set_language()) !== false)
		{
			if(($r = $this->get_browser_territory($lang)) === false)
				$r = $this->get_def_territory($lang);
		}

		if($r === false && defined('XIVO_LC_TERRITORY') === true
		&& ($territo = $this->chk_territory(XIVO_LC_TERRITORY)) !== false)
			$r = $territo;

		return(($this->_attrib['territory'] = $r));
	}

	function chk_territory($territo)
	{
		return($this->_mediator['territory']->is_valid($territo));
	}

	function get_territory_info($territo='')
	{
		$territo = (string) $territo;

		if($territo === '')
			$territo = $this->set_territory();

		return($this->_mediator['territory']->get_info($territo));
	}

	function set_timezone($tz='')
	{
		$r = false;

		$tz = (string) $tz;

		if($tz !== '' && ($tz = $this->chk_timezone($tz)) !== false)
			$r = $tz;
		else if(empty($this->_attrib['territory']) === true)
			$this->set_territory();

		if($r === false && empty($this->_attrib['territory']) === false
		&& ($tz = $this->get_def_timezone($this->_attrib['territory'])) !== false)
			$r = $tz;

		return($r);
	}

	function chk_timezone($tz)
	{
		return($this->_mediator['timezone']->is_valid($tz));
	}

	function get_timezone_info($tz='')
	{
		$tz = (string) $tz;

		if($tz === '')
			$tz = $this->set_timezone();

		return($this->_mediator['timezone']->get_info($tz));
	}

	function set_locale($lang=false,$territo=false)
	{
		$r = false;

		$lang = $lang !== false ? $this->chk_language($lang) : false;
		$territo = $territo !== false ? $this->chk_territory($territo) : false;

		if($lang !== false)
			$lang = $this->set_language($lang);
		else if(empty($this->_attrib['language']) === false)
			$lang = $this->_attrib['language'];
		else
			$lang = false;

		if($territo !== false)
			$territo = $this->set_territory($territo);
		else if(empty($this->_attrib['territory']) === false)
			$territo = $this->_attrib['territory'];
		else
			$territo = false;

		if($lang !== false)
		{
			if(($territo !== false
			   && ($r = $this->chk_locale($lang.'_'.$territo)) !== false) === true
			|| (($nterrito = $this->get_browser_territory($lang)) !== false
			   && ($r = $this->chk_locale($lang.'_'.$nterrito)) !== false) === true
			|| (($nterrito = $this->get_def_territory($lang)) !== false
			   && ($r = $this->chk_locale($lang.'_'.$nterrito)) !== false) === true);
		}

		if($r === false
		&& $territo !== false
		&& ($nlang = $this->get_def_language($territo)) !== false)
			$r = $this->chk_locale($nlang.'_'.$territo);

		if($r === false && $this->_attrib['locale'] === '')
		{
			$this->set_territory($this->set_language());

			if(empty($this->_attrib['language']) === false || empty($this->_attrib['territory']) === false)
				$r = $this->set_locale();
		}

		if(empty($this->_attrib['territory']) === false)
			$this->set_timezone();

		return(($this->_attrib['locale'] = $r));
	}

	function get_real_locale($locale)
	{
		$locale = (string) $locale;

		if($locale === '' || ($info = $this->get_locale_info($locale)) === false)
			return(false);

		$locales = array();

		if(($value = xivo_issa_val('charset',$info)) !== false
		&& ($nb = count($value)) !== 0)
		{
			for($i = 0;$i < $nb;$i++)
				$locales[] = $info['locale'].'.'.$info['charset'][$i];
		}

		$locales[] = $info['locale'];
		$locales[] = $info['lang'];

		$loc = setlocale(LC_ALL,null);
		$r = setlocale(LC_ALL,$locales);
		setlocale(LC_ALL,$loc);

		return($r);
	}

	function chk_locale($locale)
	{
		return($this->_mediator['locale']->is_valid($locale));
	}

	function get_locale_info($locale='')
	{
		$locale = (string) $locale;

		if($locale === '')
			$locale = $this->set_locale();

		return($this->_mediator['locale']->get_info($locale));
	}

	function get_language($locale='')
	{
		$r = false;

		$locale = $locale === '' ? $this->_attrib['locale'] : (string) $locale;

		if(empty($locale) === false && ($locale = $this->chk_locale($locale)) !== false)
			$r = $this->_mediator['locale']->get_language($locale);

		return($r);
	}

	function get_territory($locale='')
	{
		$r = false;

		$locale = $locale === '' ? $this->_attrib['locale'] : (string) $locale;

		if(empty($locale) === false && ($locale = $this->chk_locale($locale)) !== false)
			$r = $this->_mediator['locale']->get_territory($locale);

		return($r);
	}

	function get_timezone($locale='')
	{
		$r = false;

		$locale = $locale === '' ? $this->_attrib['locale'] : (string) $locale;

		if(empty($locale) === false && ($locale = $this->chk_locale($locale)) !== false
		&& ($territo = $this->_mediator['locale']->get_territory($locale)) !== false)
			$r = $this->get_def_timezone($territo);

		return($r);
	}

	function get_def_territory($lang)
	{
		$r = false;

		if(($lang = $this->chk_language($lang)) !== false
		&& ($locale = $this->_mediator['language']->exists_default($lang)) !== false)
			$r = $this->_mediator['locale']->get_territory($locale);

		return($r);
	}

	function get_def_language($territo)
	{
		$r = false;

		if(($territo = $this->chk_territory($territo)) !== false
		&& ($locale = $this->_mediator['territory']->exists_default($territo)) !== false)
			$r = $this->_mediator['locale']->get_language($locale);

		return($r);
	}

	function get_def_timezone($territo)
	{
		return($this->_mediator['territory']->get_timezone($territo));
	}

	function get_territory_list()
	{
		return($this->_mediator['territory']->get_list());
	}

	function get_language_list()
	{
		return($this->_mediator['language']->get_list());
	}

	function get_locale_list()
	{
		return($this->_mediator['locale']->get_list());
	}

	function get_timezone_list()
	{
		return($this->_mediator['timezone']->get_list());
	}

	function set_babelfish_path()
	{
		if(defined('XIVO_I18N_BABELFISH_PATH') === true)
			return(true);
		else if(xivo_file::is_d_r(XIVO_PATH_I18N) === false)
		{
			trigger_error('Invalid I18N path',E_USER_ERROR);
			return(false);
		}
		else if(defined('XIVO_USER_I18N_LOCALE') === true
		&& $this->_chk_babelfish_path(XIVO_USER_I18N_LOCALE,true) !== false)
			return(true);
		else if(defined('XIVO_USER_I18N_LANGUAGE') === true)
		{
			if(($locale = $this->_mediator['language']->exists_default(XIVO_USER_I18N_LANGUAGE)) !== false
			&& $this->_chk_babelfish_path($locale,true) !== false)
				return(true);
			else if(($locales = $this->_mediator['language']->get_locale_list(XIVO_USER_I18N_LANGUAGE)) !== false
			&& $this->_chk_babelfish_path($locales,true) !== false)
				return(true);
		}

		$this->set_locale();

		if(empty($this->_attrib['locale']) === false
		&& $this->_chk_babelfish_path($this->_attrib['locale'],true) !== false)
			return(true);
		else if(empty($this->_attrib['language']) === false)
		{
			if(($locale = $this->_mediator['language']->exists_default($this->_attrib['language'])) !== false
			&& $this->_chk_babelfish_path($locale,true) !== false)
				return(true);
			else if(($locales = $this->_mediator['language']->get_locale_list($this->_attrib['language'])) !== false
			&& $this->_chk_babelfish_path($locales,true) !== false)
				return(true);
		}

		if(defined('XIVO_LC_LANGUAGE') === true)
		{
			if(($locale = $this->_mediator['language']->exists_default(XIVO_LC_LANGUAGE)) !== false
			&& $this->_chk_babelfish_path($locale,true) !== false)
				return(true);
			else if(($locales = $this->_mediator['language']->get_locale_list(XIVO_LC_LANGUAGE)) !== false
			&& $this->_chk_babelfish_path($locales,true) !== false)
				return(true);
		}

		if($this->_chk_babelfish_path('fr_FR',true) !== false)
			return(true);

		return(false);
	}

	function _chk_babelfish_path($locale,$define=false)
	{
		$define = (bool) $define;

		if(is_array($locale) === true)
		{
			if(empty($locale) === true)
				return(false);

			reset($locale);

			while(list($key) = each($locale))
			{
				if(($nlocale = $this->chk_locale($key)) === false
				|| ($dir = xivo_file::is_d_r(XIVO_PATH_I18N.XIVO_SEP_DIR.$nlocale)) === false)
					continue;
				else if($define === true)
				{
					if(($real_locale = $this->get_real_locale($nlocale)) === false)
						$real_locale = $nlocale;

					define('XIVO_I18N_BABELFISH_LANGUAGE',
						$this->_mediator['locale']->get_language($nlocale));
					define('XIVO_I18N_BABELFISH_TERRITORY',
						$this->_mediator['locale']->get_territory($nlocale));
					define('XIVO_I18N_BABELFISH_LOCALE',$nlocale);
					define('XIVO_I18N_BABELFISH_REAL_LOCALE',$real_locale);
					define('XIVO_I18N_BABELFISH_PATH',$dir);
				}
				return($dir);
			}

			return(false);
		}
		else if(($locale = $this->chk_locale($locale)) !== false
		&& ($dir = xivo_file::is_d_r(XIVO_PATH_I18N.XIVO_SEP_DIR.$locale)) !== false)
		{
			if($define === true)
			{
				if(($real_locale = $this->get_real_locale($locale)) === false)
					$real_locale = $locale;

				define('XIVO_I18N_BABELFISH_LANGUAGE',$this->_mediator['locale']->get_language($locale));
				define('XIVO_I18N_BABELFISH_TERRITORY',$this->_mediator['locale']->get_territory($locale));
				define('XIVO_I18N_BABELFISH_LOCALE',$locale);
				define('XIVO_I18N_BABELFISH_REAL_LOCALE',$real_locale);
				define('XIVO_I18N_BABELFISH_PATH',$dir);
			}
			return($dir);
		}

		return(false);
	}
}

?>
