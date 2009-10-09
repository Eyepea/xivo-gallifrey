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

class dwho_tpl_form extends dwho_tpl_module
{
	var $_param		= array();
	var $_render		= '';
	var $_tpl		= null;
	var $_selected		= false;
	var $_optgroup		= array();
	var $_uniqoptgroup	= array();

	function dwho_tpl_form(&$tpl)
	{
		$this->_tpl = &$tpl;
	}

	function _set_param(&$v)
	{
		$v = (array) $v;

		$this->_param['name'] = 'undefined';
		$this->_param['type'] = 'text';
		$this->_param['desc'] = false;
		$this->_param['default'] = false;
		$this->_param['value'] = false;
		$this->_param['checked'] = false;
		$this->_param['size'] = 10;
		$this->_param['rows'] = 5;
		$this->_param['cols'] = 50;
		$this->_param['multiple'] = false;
		$this->_param['disabled'] = false;
		$this->_param['id'] = false;
		$this->_param['fieldid'] = false;
		$this->_param['class'] = false;
		$this->_param['label'] = true;
		$this->_param['field'] = false;
		$this->_param['key'] = null;
		$this->_param['altkey'] = false;
		$this->_param['empty'] = false;
		$this->_param['optgroup'] = false;
		$this->_param['null'] = false;
		$this->_param['bbf'] = false;
		$this->_param['bbfopt'] = false;
		$this->_param['browse'] = false;
		$this->_param['jump'] = 0;
		$this->_param['jump_strict'] = true;
		$this->_param['invalid'] = false;

		if(isset($v['name']) === true)
			$this->_param['name'] = dwho_htmlsc($v['name']);

		if(isset($v['type']) === true)
			$this->_param['type'] = (string) $v['type'];

		if(isset($v['desc']) === true)
			$this->_param['desc'] = $v['desc'];

		if(isset($v['size']) === true)
		{
			if($v['size'] !== false)
				$this->_param['size'] = dwho_uint($v['size']);
			else
				$this->_param['size'] = false;
		}

		if(isset($v['rows']) === true)
		{
			if($v['rows'] !== false)
				$this->_param['rows'] = dwho_uint($v['rows']);
			else
				$this->_param['rows'] = false;
		}

		if(isset($v['cols']) === true)
		{
			if($v['cols'] !== false)
				$this->_param['cols'] = dwho_uint($v['cols']);
			else
				$this->_param['cols'] = false;
		}

		if(isset($v['disabled']) === true)
			$this->_param['disabled'] = (bool) $v['disabled'];

		if(isset($v['multiple']) === true)
			$this->_param['multiple'] = true;

		if(isset($v['key']) === true)
			$this->_param['key'] = $v['key'];

		if(isset($v['altkey']) === true)
			$this->_param['altkey'] = $v['altkey'];

		if(isset($v['empty']) === true && $v['empty'] !== false)
			$this->_param['empty'] = $v['empty'] === true ? true : $this->_mk_param_value($v['empty'],$v);

		if(isset($v['optgroup']) === true && is_array($v['optgroup']) === true)
		{
			$ref_ogrp = &$v['optgroup'];

			$this->_param['optgroup'] = array();

			if(isset($ref_ogrp['key']) === true)
				$this->_param['optgroup']['key'] = $ref_ogrp['key'];
			else
				$this->_param['optgroup']['key'] = null;

			if(isset($ref_ogrp['altkey']) === true)
				$this->_param['optgroup']['altkey'] = $ref_ogrp['altkey'];
			else
				$this->_param['optgroup']['altkey'] = false;

			if(isset($ref_ogrp['bbf']) === true && is_bool($ref_ogrp['bbf']) === false)
				$this->_param['optgroup']['bbf'] = $this->_mk_param_bbf($ref_ogrp);
			else
				$this->_param['optgroup']['bbf'] = false;

			if(isset($ref_ogrp['bbfopt']) === true && is_bool($ref_ogrp['bbfopt']) === false)
				$this->_param['optgroup']['bbfopt'] = $this->_mk_param_bbfopt($ref_ogrp['bbfopt'],true);
			else
				$this->_param['optgroup']['bbfopt'] = false;

			if(isset($ref_ogrp['unique']) === true && is_bool($ref_ogrp['unique']) === true)
				$this->_param['optgroup']['unique'] = $ref_ogrp['unique'];
			else
				$this->_param['optgroup']['unique'] = false;
		}

		if(isset($v['default']) === true)
			$this->_param['default'] = $this->_mk_param_value($v['default'],$v);

		if(isset($v['value']) === true)
			$this->_param['value'] = $this->_mk_param_value($v['value'],$v);

		if(isset($v['field']) === false)
			$this->_param['field'] = true;

		if(($this->_param['value'] === false || $this->_param['value'] === '') === true
		&& $this->_param['default'] !== false)
			$this->_param['value'] = $this->_param['default'];

		if(isset($v['label']) === true)
		{
			if(is_string($v['label']) === true)
				$this->_param['label'] = dwho_htmlsc($v['label']);
			else
				$this->_param['label'] = false;
		}

		if(isset($v['fieldid']) === true)
		{
			if(is_string($v['fieldid']) === true)
				$this->_param['fieldid'] = dwho_htmlsc($v['fieldid']);
			else
				$this->_param['fieldid'] = false;
		}

		if(isset($v['id']) === true)
		{
			if($v['id'] !== false)
				$this->_param['id'] = dwho_htmlsc($v['id']);
		}
		else if(isset($v['labelid']) === true && is_string($v['labelid']) === true)
		{
			$v['labelid'] = dwho_htmlsc($v['labelid']);

			if($this->_param['label'] === true)
				$this->_param['label'] = 'lb-'.$v['labelid'];
			if($this->_param['field'] !== false)
				$this->_param['fieldid'] = 'fd-'.$v['labelid'];

			$this->_param['id'] = 'it-'.$v['labelid'];
		}
		else $this->_param['id'] = $this->_param['name'];

		if(isset($v['class']) === true)
			$this->_param['class'] = dwho_htmlsc($v['class']);

		if(isset($v['null']) === true)
			$this->_param['null'] = true;

		if(isset($v['bbf']) === true && is_bool($v['bbf']) === false)
			$this->_param['bbf'] = $this->_mk_param_bbf($v);

		if(isset($v['bbfopt']) === true && is_bool($v['bbfopt']) === false)
			$this->_param['bbfopt'] = $this->_mk_param_bbfopt($v['bbfopt']);

		if(isset($v['browse']) === true)
		{
			$v['browse'] = array_values((array) $v['browse']);

			if(isset($v['browse'][0]) === true)
				$this->_param['browse'] = $v['browse'];
		}

		if(isset($v['jump']) === true)
			$this->_param['jump'] = dwho_uint($v['jump']);

		if(isset($v['jump_strict']) === true)
			$this->_param['jump_strict'] = (bool) $v['jump_strict'];

		if(isset($v['invalid']) === true)
			$this->_param['invalid'] = (bool) $v['invalid'];
	}

	function _mk_param_value($value,&$v)
	{
		$r = '';

		switch(gettype($value))
		{
			case 'object':
				$value = (array) $value;
			case 'array':
				if(isset($v['name'],$value[$v['name']]) === false)
					return($r);

				$r = $this->_mk_param_value($value[$v['name']],$v);
				break;
			case 'string':
				if(isset($v['notag']) === false)
					$r = dwho_notag($value);
				else if(isset($v['notagscript']) === true)
					$r = dwho_notagscript($value);
				else
					$r = $value;

				if(isset($v['schars']) === false)
					$r = dwho_htmlsc($r);
				break;
			case 'boolean':
					$r = (int) $value;
			case 'integer':
			case 'double':
					$r = (string) $value;
				break;
			default:
				$r = '';
		}

		return($r);
	}

	function _mk_param_bbf(&$v)
	{
		$r = false;

		if(is_scalar($v['bbf']) === true)
			$r = array('concatvalue',$v['bbf']);
		else if(is_array($v['bbf']) === true && isset($v['bbf'][0],$v['bbf'][1]) === true)
		{
			switch($v['bbf'][0])
			{
				case 'paramkey':
				case 'paramvalue':
				case 'concatkey':
					break;
				default:
					$v['bbf'][0] = 'concatvalue';
			}

			$r = array($v['bbf'][0],$v['bbf'][1]);

			if(isset($v['bbf'][2]) === true
			&& (is_array($v['bbf'][2]) === true
			   || is_scalar($v['bbf'][2]) === true) === true)
				$r[2] = (array) $v['bbf'][2];
		}

		return($r);
	}

	function _mk_param_bbfopt(&$v,$optgroup=false)
	{
		$r = array();

		if(is_array($v) === true && isset($v['argmode']) === true)
		{
			if((bool) $optgroup === false)
				$this->_param['bbf'][0] = $v['argmode'];
			else
				$this->_param['optgroup']['bbf'][0] = $v['argmode'];
		}

		if(is_string($v) === true)
			$v = array('argmode' => $v);
		else if(is_array($v) === false)
			return(false);
		else if(isset($v['argmode']) === false)
			$v['argmode'] = 'concatvalue';

		switch($v['argmode'])
		{
			case 'paramkey':
			case 'paramvalue':
			case 'concatkey':
				$r['argmode'] = $v['argmode'];
				break;
			default:
				$r['argmode'] = 'concatvalue';
		}

		if(dwho_issa('time',$v) === true)
		{
			if(isset($v['time']['from'],$v['time']['format']) === true)
				$r['time'] = array('from'	=> $v['time']['from'],
						   'format'	=> $v['time']['format']);
		}
		else if(isset($v['argvalue']) === true)
			$r['argvalue'] = true;

		return($r);
	}

	function _mk_param_checked($checked)
	{
		if(dwho_bool($checked) === true
		|| (dwho_issa($this->_param['name'],$checked) === true
		&& (bool) $checked[$this->_param['name']] === true) === true)
			$this->_param['checked'] = true;
	}

	function _mk_input(&$o)
	{
		$o = (string) $o;

		$this->_render = '';

		$r = &$this->_render;

		$r .= '<input type="'.$this->_param['type'].'" name="'.$this->_param['name'].'"';

		if($this->_param['id'] !== false)
			$r .= ' id="'.$this->_param['id'].'"';

		if($this->_param['class'] !== false)
			$r .= ' class="'.$this->_param['class'].'"';

		if($this->_param['size'] !== false)
			$r .= ' size="'.$this->_param['size'].'"';

		if($this->_param['disabled'] !== false)
			$r .= ' disabled="disabled"';

		if($this->_param['checked'] !== false)
			$r .= ' checked="checked"';

		if($this->_param['value'] !== false)
		{
			if($this->_param['invalid'] === true)
				$value = '';
			else
				$value = $this->_param['value'];

			$r .= ' value="'.$value.'"';
		}

		if(isset($o{0}) === true)
			$r .= ($o{0} === ' ' ? '' : ' ').$o;

		$r .= ' />';
	}

	function text($v,$o='')
	{
		$v = (array) $v;

		$v['type'] = 'text';
		$v['checked'] = null;

		if(isset($v['value']) === true && $v['value'] === false)
			$v['value'] = '';

		$this->_set_param($v);
		$this->_mk_input($o);

		$this->_mk_desc();
		$this->_mk_label();
		$this->_mk_field();

		return($this->_render);
	}

	function file($v,$o='')
	{
		$v = (array) $v;

		$v['checked'] = null;
		$v['type'] = 'file';

		$this->_set_param($v);
		$this->_mk_input($o);

		$this->_mk_desc();
		$this->_mk_label();
		$this->_mk_field();

		return($this->_render);
	}

	function hidden($v)
	{
		$v = (array) $v;

		if(isset($v['id']) === true)
			$v['id'] = $v['id'];
		else
			$v['id'] = false;

		$v['size'] = false;
		$v['desc'] = $v['class'] = $v['checked'] = null;
		$v['type'] = 'hidden';

		$this->_set_param($v);
		$this->_mk_input($o);

		return($this->_render."\n");
	}

	function button($v,$o='')
	{
		$v = (array) $v;
		$o = (string) $o;

		$v['size'] = false;
		$v['checked'] = null;
		$v['class'] = 'itb-button';
		$v['type'] = 'button';

		$this->_set_param($v);
		$this->_mk_input($o);

		if($this->_param['field'] === true)
			$this->_render = '<p class="fm-fieldbutton">'."\n".$this->_render."\n".'</p>';

		$this->_mk_desc();

		return($this->_render);
	}

	function submit($v,$o='')
	{
		$v = (array) $v;
		$o = (string) $o;

		$v['size'] = false;
		$v['checked'] = null;
		$v['class'] = 'itb-submit';
		$v['type'] = 'submit';

		$this->_set_param($v);
		$this->_mk_input($o);

		if($this->_param['field'] === true)
			$this->_render = '<p class="fm-fieldsubmit">'."\n".$this->_render."\n".'</p>';

		$this->_mk_desc();

		return($this->_render);
	}

	function image($v,$o='')
	{
		$v = (array) $v;
		$o = (string) $o;

		$v['value'] = $v['checked'] = null;
		$v['size'] = false;
		$v['class'] = 'itb-image';
		$v['type'] = 'image';

		$o .= isset($v['src']) === true ? ' src="'.(string) $v['src'].'"' : '';
		$o .= isset($v['alt']) === true ? ' alt="'.dwho_alttitle($v['alt']).'"' : ' alt=""';

		$this->_set_param($v);
		$this->_mk_input($o);

		if($this->_param['field'] === true)
			$this->_render = '<p class="fm-fieldimgsubmit">'."\n".$this->_render."\n".'</p>';

		$this->_mk_desc();

		return($this->_render);
	}

	function password($v,$o='')
	{
		$v = (array) $v;
		$o = (string) $o;

		$v['type'] = 'password';
		$v['checked'] = null;

		$this->_set_param($v);
		$this->_mk_input($o);

		$this->_mk_desc();
		$this->_mk_label();
		$this->_mk_field();

		return($this->_render);
	}

	function checkbox($v,$o='')
	{
		$v = (array) $v;
		$o = (string) $o;

		$value = isset($v['value']) === false ? 1 : $this->_mk_param_value($v['value'],$v);

		$v['value'] = 1;
		$v['size'] = false;
		$v['type'] = 'checkbox';

		if(isset($v['class']) === false)
			$v['class'] = 'it-checkbox';

		$this->_set_param($v);

		$this->_param['value'] = $value;

		if(isset($v['checked']) === false || $v['checked'] === '')
		{
			if(isset($v['default']) === true)
				$this->_mk_param_checked($v['default']);
		}
		else
			$this->_mk_param_checked($v['checked']);

		$this->_mk_input($o);

		$this->_mk_desc();
		$this->_mk_label();
		$this->_mk_field();

		return($this->_render);
	}

	function radio($v,$o='')
	{
		$v = (array) $v;
		$o = (string) $o;

		$value = isset($v['value']) === false ? false : $this->_mk_param_value($v['value'],$v);

		$v['value'] = 1;
		$v['size'] = false;
		$v['type'] = 'radio';

		if(isset($v['class']) === false)
			$v['class'] = 'it-radio';

		$this->_set_param($v);

		$this->_param['value'] = $value;

		if(isset($v['checked']) === false || $v['checked'] === '')
		{
			if(isset($v['default']) === true)
				$this->_mk_param_checked($v['default']);
		}
		else
			$this->_mk_param_checked($v['checked']);

		$this->_mk_input($o);

		$this->_mk_desc();
		$this->_mk_label();
		$this->_mk_field();

		return($this->_render);
	}

	function textarea($v,$d='',$o='')
	{
		$this->_render = '';

		$r = &$this->_render;

		$v = (array) $v;
		$v['value'] = &$d;
		$o = (string) $o;

		$this->_set_param($v);

		$r .= '<textarea name="'.$this->_param['name'].'"';

		if($this->_param['disabled'] !== false)
			$r .= ' disabled="disabled"';

		if($this->_param['id'] !== false)
			$r .= ' id="'.$this->_param['id'].'"';

		if($this->_param['cols'] !== false)
			$r .= ' cols="'.$this->_param['cols'].'"';

		if($this->_param['rows'] !== false)
			$r .= ' rows="'.$this->_param['rows'].'"';

		$r .= (isset($o{0}) === true ? ' '.$o : '').'>'.$this->_param['value'].'</textarea>'."\n";

		$this->_mk_desc();
		$this->_mk_label();
		$this->_mk_field();

		return($this->_render);
	}

	function _mk_label($str=false)
	{
		if($this->_param['label'] === false)
			return((string) $str);

		$for = $this->_param['id'] !== false ? $this->_param['id'] : $this->_param['name'];
		$id = $this->_param['label'] === true ? 'lb-'.$this->_param['name'] : $this->_param['label'];

		if($str !== false)
			$str = '<label id="'.$id.'" for="'.$for.'">'.(string) $str.'</label>';
		else
			$this->_render = '<label id="'.$id.'" for="'.$for.'">'.$this->_render.'</label>';

		return($str);
	}

	function _mk_field()
	{
		if($this->_param['field'] === false)
			return(false);

		$this->_render = '<p'.
				 ($this->_param['fieldid'] !== false ? ' id="'.$this->_param['fieldid'].'"' : '').
				 ' class="fm-field">'."\n".
				 $this->_render."\n".
				 '</p>';
	}

	function _mk_desc()
	{
		if($this->_param['desc'] === false)
			return(false);
		else if(is_scalar($this->_param['desc']) === true)
		{
			$this->_render = '<span class="fm-desc clearboth">'.
					 $this->_mk_label((string) $this->_param['desc']).
					 '</span>'.
					 $this->_render;
			$this->_param['label'] = false;
			return(null);
		}
		else if(is_array($this->_param['desc']) === true
		&& isset($this->_param['desc'][0],$this->_param['desc'][1]) === true)
		{
			$this->_mk_field();

			$this->_param['desc'][1] = '<span class="fm-desc clearboth">'.
						   $this->_mk_label((string) $this->_param['desc'][1]).
						   '</span>';

			if(isset($this->_param['desc'][2]) === false)
				$this->_render = sprintf($this->_param['desc'][0],$this->_param['desc'][1],$this->_render);
			else
				$this->_render = sprintf($this->_param['desc'][0],$this->_render,$this->_param['desc'][1]);

			$this->_param['label'] = $this->_param['field'] = false;
		}
	}

	function select($v,$arr=array(),$o='',$more=array())
	{
		$this->_render = '';
		$this->_selected = false;
		$this->_optgroup = $this->_uniqoptgroup = array();

		$v = (array) $v;
		$o = (string) $o;

		$this->_set_param($v);

		$r = '<select name="'.$this->_param['name'].'"';

		if($this->_param['disabled'] !== false)
			$r .= ' disabled="disabled"';

		if($this->_param['id'] !== false)
			$r .= ' id="'.$this->_param['id'].'"';

		if($this->_param['multiple'] === true)
		{
			$r .= ' multiple="multiple"';

			if($this->_param['size'] !== false)
				$r .= ' size="'.$this->_param['size'].'"';
		}

		$r .= (isset($o{0}) === true ? ' '.$o : '').'>'."\n";

		if($this->_param['null'] === true || ($more !== '' && $more !== false) === true)
		{
			$more = (array) $more;
			$this->_mk_slt_option($more);
		}

		if($this->_param['null'] === true || ($arr !== '' && $arr !== false) === true)
		{
			$arr = (array) $arr;

			if($this->_param['jump'] > 0)
			{
				$jumparr = array();
				$this->_mk_slt_option_jump($this->_param['jump'],$arr,$jumparr);
				$arr = $jumparr;
			}

			$this->_mk_slt_option($arr);
		}

		if($this->_param['empty'] !== false
		|| ($this->_param['invalid'] === true
		   && $this->_selected === false) === true)
			$r .= $this->_mk_slt_option_empty();

		foreach($this->_optgroup as $key => $value)
		{
			if(is_int($key) === false && isset($this->_uniqoptgroup[$key]) === true)
				$r .= '<optgroup label="'.$key.'">'.$this->_uniqoptgroup[$key].'</optgroup>'."\n";
			else
				$r .= $value;
		}

		$this->_render = $r.$this->_render.'</select>';

		$this->_mk_desc();
		$this->_mk_label();
		$this->_mk_field();

		return($this->_render);
	}

	function _mk_slt_option_empty()
	{
		if(is_bool($this->_param['empty']) === true)
			$empty = '&nbsp;';
		else
			$empty = $this->_param['empty'];

		$selected = $this->_param['value'] === '' ? ' selected="selected"' : '';

		return('<option label="" value=""'.$selected.'>'.$empty.'</option>'."\n");
	}

	function _is_slt_key(&$arr,$key=null)
	{
		if($key === null)
			$key = $this->_param['key'];

		if(is_bool($key) === false
		&& is_array($arr) === true
		&& isset($arr[$key]) === true
		&& is_scalar($arr[$key]) === true)
			return(true);

		return(false);
	}

	function _is_slt_altkey(&$arr,$altkey=null)
	{
		if($altkey === null)
			$altkey = $this->_param['altkey'];

		if($altkey !== false
		&& is_array($arr) === true
		&& isset($arr[$altkey]) === true
		&& is_scalar($arr[$altkey]) === true)
			return(true);

		return(false);
	}

	function _set_slt_option_render($render)
	{
		if(is_array($render) === false)
			return(null);

		$render['key'] = dwho_htmlsc($render['key']);

		if($this->_param['optgroup'] === true)
		{
			$this->_render .= sprintf('<option label="%s" value="%s"%s>%s</option>'."\n",
						  dwho_alttitle($render['value']),
						  $render['key'],
						  $render['selected'],
						  preg_replace('/^ $|(?<= ) /','&nbsp;',dwho_htmlsc($render['value'])));
			return(null);
		}

		$this->_render .= sprintf('<option value="%s"%s>%s</option>'."\n",
					  $render['key'],
					  $render['selected'],
					  preg_replace('/^ $|(?<= ) /','&nbsp;',dwho_htmlsc($render['value'])));
	}

	function _mk_slt_option_render($render,$param=null)
	{
		if(is_array($render) === false)
			return(null);
		else if(dwho_has_len($render,'value') === false)
		{
			$render['value'] = ' ';
			return($this->_set_slt_option_render($render));
		}
		else if(is_array($this->_param['bbf']) === false)
			return($this->_set_slt_option_render($render));

		$key = $this->_param['bbf'][0];

		$bbfkey = $this->_param['bbf'][1];
		$bbfkeyorig = $this->_param['bbf'][1];
		$renderkey = $render['key'];
		$rendervalue = $render['value'];

		if(($time = $this->_mk_slt_option_bbf_time($render['key'])) !== false)
		{
			$bbfkey .= $time['bbfkey'];
			$bbfkeyorig .= $time['bbfkey'];
			$renderkey = $time['timevalue'];
			$rendervalue = $time['timevalue'];
		}

		if(isset($this->_param['bbf'][2]) === false)
			$oparam = null;
		else
			$oparam = $this->_param['bbf'][2];

		switch($key)
		{
			case 'paramkey':
				if($oparam !== null && is_array($param) === false)
					$param = array_merge((array) $renderkey, $oparam);

				$render['value'] = $this->bbf($bbfkey,
							(is_array($param) === true ? $param : $renderkey));
				break;
			case 'paramvalue':
				if($oparam !== null && is_array($param) === false)
					$param = array_merge((array) $rendervalue, $oparam);

				$render['value'] = $this->bbf($bbfkey,
							(is_array($param) === true ? $param : $rendervalue));
				break;
			case 'concatkey':
				if(is_scalar($render['key']) === true)
					$bbfkey .= $render['key'];

				$render['value'] = $this->bbf($bbfkey,$oparam);
				break;
			case 'concatvalue':
				if(is_scalar($render['value']) === true)
					$bbfkey .= $render['value'];

				$render['value'] = $this->bbf($bbfkey,$oparam);
				break;
		}

		return($this->_set_slt_option_render($render));
	}

	function _mk_slt_option_bbf_time($time)
	{
		if(isset($this->_param['bbfopt']['time']) === false
		|| ($time = dwho_calc_time($this->_param['bbfopt']['time']['from'],
					   $time,
					   $this->_param['bbfopt']['time']['format'],
					   false)) === false)
			return(false);

		$arr = array();

		if(isset($time['d']) === true)
			$arr[] = 'day'.($time['d'] > 1 ? 's' : '');

		if(isset($time['H']) === true)
			$arr[] = 'hour'.($time['H'] > 1 ? 's' : '');

		if(isset($time['M']) === true)
			$arr[] = 'minute'.($time['M'] > 1 ? 's' : '');

		if(isset($time['s']) === true)
			$arr[] = 'second'.($time['s'] > 1 ? 's' : '');

		if(isset($time['ms']) === true)
			$arr[] = 'millisecond'.($time['ms'] > 1 ? 's' : '');

		if(isset($time['f']) === true)
			$arr[] = 'microsecond'.($time['f'] > 1 ? 's' : '');

		if(isset($arr[0]) === false)
			return(false);

		if(isset($arr[1]) === false)
			$time = current($time);

		return(array('bbfkey'		=> '-'.implode('-',$arr),
			     'timevalue'	=> $time));
	}

	function _mk_slt_option_jump($jump,$arr,&$ret)
	{
		if(is_array($arr) === false)
			return(false);

		foreach($arr as $key => $val)
		{
			if(is_array($val) === false)
			{
				if($this->_param['jump_strict'] === false)
					$ret[$key] = $val;
				else if($this->_param['jump'] === $jump)
					$ret[$key] = $val;
				else
					return(true);
			}
			else if($jump === 0)
				$ret[$key] = $val;
			else if($this->_mk_slt_option_jump($jump-1,$val,&$ret) === true
			&& $this->_param['jump_strict'] === true
			&& $this->_param['jump'] === $jump)
				$ret[$key] = $val;
		}
	}

	function _mk_slt_optgroup_render($render)
	{
		if(is_array($this->_param['optgroup']) === false)
			return(null);

		$param = &$this->_param['optgroup'];

		if(is_scalar($render) === true && dwho_has_len($render) === false)
			$render = ' ';
		else if(is_array($param['bbf']) === true)
		{
			switch($param['bbf'][0])
			{
				case 'param':
				case 'paramkey':
				case 'paramvalue':
					$render = $this->bbf($param['bbf'][1],$render);
					break;
				case 'concat':
				case 'concatkey':
				case 'concatvalue':
					if(is_scalar($render) === true)
						$render = $this->bbf($param['bbf'][1].$render);
					else
						$render = $this->bbf($param['bbf'][1]);
					break;
			}
		}

		if(is_scalar($render) === false)
			return(null);

		$render = preg_replace('/^ $|(?<= ) /','&nbsp;',dwho_htmlsc($render));

		if($param['unique'] === true)
			return($render);

		$this->_render .= sprintf('<optgroup label="%s">',$render);

		return(true);
	}

	function _mk_slt_option_selected($key,$val)
	{
		if(($this->_selected === true
		   && $this->_param['multiple'] === false) === true
		|| is_scalar($key) === false)
			return('');
		else if(is_bool($key) === true)
			$key = (int) $key;

		if((string) $key !== $val)
			return('');

		$this->_selected = true;
		return(' selected="selected"');
	}

	function _mk_slt_option_param_array($key,$ref)
	{
		$r = array();

		if($this->_is_slt_altkey($ref) === true)
			$key = $ref[$this->_param['altkey']];

		if($this->_is_slt_key($ref) === true)
		{
			$r['key'] = $key;
			$r['value'] = $ref[$this->_param['key']];
			$r['selected'] = $this->_mk_slt_option_selected($key,$this->_param['value']);
		}
		else if($this->_param['key'] === true)
		{
			$r['key'] = $r['value'] = $key;
			$r['selected'] = $this->_mk_slt_option_selected($key,$this->_param['value']);
		}
		else if($this->_param['key'] === false && is_scalar($ref) === true)
		{
			$r['key'] = $r['value'] = $ref;
			$r['selected'] = $this->_mk_slt_option_selected($ref,$this->_param['value']);
		}
		else if($this->_param['key'] === false
		&& $this->_param['altkey'] === ''
		&& is_array($ref) === true
		&& count($ref) === 1)
		{
			$r['key'] = key($ref);
			$r['value'] = current($ref);
			$r['selected'] = $this->_mk_slt_option_selected($r['key'],$this->_param['value']);
		}
		else
			return(false);

		return($r);
	}

	function _mk_slt_option_param($key,$ref)
	{
		$r = array();

		if($this->_param['key'] === true || is_scalar($ref) === false)
		{
			$r['key'] = $r['value'] = $key;
			$r['selected'] = $this->_mk_slt_option_selected($key,$this->_param['value']);
		}
		else if($this->_param['key'] === false && is_scalar($ref) === true)
		{
			$r['key'] = $r['value'] = $ref;
			$r['selected'] = $this->_mk_slt_option_selected($ref,$this->_param['value']);
		}
		else
		{
			$r['key'] = $key;
			$r['value'] = $ref;
			$r['selected'] = $this->_mk_slt_option_selected($key,$this->_param['value']);
		}

		return($r);
	}

	function _mk_slt_option(&$arr,$prevkey=null)
	{
		if(is_array($arr) === false || empty($arr) === true)
			return(false);
		else if($this->_is_slt_key($arr) === true)
		{
			if($this->_is_slt_altkey($arr) === true)
				$key = $arr[$this->_param['altkey']];
			else
				$key = $this->_param['key'];

			$this->_mk_slt_option_render(
					$this->_mk_slt_option_param($key,$arr[$this->_param['key']]));

			return(null);
		}

		$optgroup = $this->_param['optgroup'];

		reset($arr);

		while(list($key) = each($arr))
		{
			$ref = &$arr[$key];

			if($this->_param['browse'] !== false
			&& ($cnt = count($this->_param['browse'])) !== 0)
			{
				for($i = 0;$i < $cnt;$i++)
				{
					if(dwho_issa($this->_param['browse'][$i],$ref) === false)
						break;

					$ref = &$ref[$this->_param['browse'][$i]];
				}
			}

			if($this->_param['optgroup'] !== true)
				$this->_param['optgroup'] = $optgroup;
			else if($prevkey !== null)
				$key = $prevkey;

			if(is_array($ref) === true)
			{
				if(is_array($this->_param['optgroup']) === true
				&& $this->_mk_slt_optgroup($key,$ref) !== false)
					continue;
				else
					$this->_param['optgroup'] = false;

				if(($render = $this->_mk_slt_option_param_array($key,$ref)) !== false)
					$this->_mk_slt_option_render($render);
				else if(isset($this->_param['bbfopt']['argvalue']) === true)
					$this->_mk_slt_option_render(
						$this->_mk_slt_option_param($key,$ref),
						$ref);
				else
					$this->_mk_slt_option($ref);
			}
			else
				$this->_mk_slt_option_render($this->_mk_slt_option_param($key,$ref));
		}
	}

	function _mk_slt_optgroup($key,&$arr)
	{
		if(is_array($this->_param['optgroup']) === false)
			return(null);

		$label = $prevkey = null;

		$param = $this->_param['optgroup'];

		if($this->_is_slt_altkey($arr,$param['altkey']) === true)
		{
			$key = $arr[$param['altkey']];
			unset($arr[$param['altkey']]);
		}

		$render = false;

		if($this->_is_slt_key($arr,$param['key']) === true)
		{
			$value = $arr[$param['key']];
			$prevkey = $key;
			unset($arr[$param['key']]);
			$render = $value;
		}
		else if($param['key'] === true)
			$render = $key;
		else if($param['key'] === false && is_scalar($arr) === true)
			$render = $arr;

		if($render !== false
		&& ($label = $this->_mk_slt_optgroup_render($render)) !== null
		&& $param['unique'] === true
		&& dwho_has_len($this->_render) === true)
		{
			$this->_optgroup[] = $this->_render;
			$this->_render = '';
		}

		if($render === false && $label === null)
			return(false);
		else if(empty($arr) === true)
			$this->_mk_slt_option_render($this->_mk_slt_option_param($prevkey,$arr));
		else
		{
			$this->_param['optgroup'] = true;
			$this->_mk_slt_option($arr,$prevkey);
			$this->_param['optgroup'] = $param;
		}

		if($render === false || $label === null)
			return(null);
		else if($label === true && $param['unique'] === false)
			$this->_render .= '</optgroup>'."\n";
		else
		{
			if(isset($this->_uniqoptgroup[$label]) === false)
				$this->_uniqoptgroup[$label] = '';

			$this->_optgroup[$label] = 1;
			$this->_uniqoptgroup[$label] .= $this->_render;
			$this->_render = '';
		}
	}
}

?>
