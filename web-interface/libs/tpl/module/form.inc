<?php

class xivo_tpl_form extends xivo_tpl_module
{
	var $_param	= array();
	var $_render	= '';
	var $_tpl	= null;

	function xivo_tpl_form(&$tpl)
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
		$this->_param['id'] = false;
		$this->_param['fieldid'] = false;
		$this->_param['class'] = false;
		$this->_param['label'] = true;
		$this->_param['field'] = false;
		$this->_param['key'] = null;
		$this->_param['key_val'] = false;
		$this->_param['empty'] = false;
		$this->_param['optgroup'] = false;
		$this->_param['null'] = false;
		$this->_param['bbf'] = false;

		if(isset($v['name']) === true)
			$this->_param['name'] = xivo_htmlsc($v['name']);
			
		if(isset($v['type']) === true)
			$this->_param['type'] = (string) $v['type'];

		if(isset($v['desc']) === true)
			$this->_param['desc'] = $v['desc'];

		if(isset($v['size']) === true)
		{
 			if($v['size'] !== false)
				$this->_param['size'] = xivo_uint($v['size']);
			else
				$this->_param['size'] = false;
		}

		if(isset($v['rows']) === true)
		{
			if($v['rows'] !== false)
				$this->_param['rows'] = xivo_uint($v['rows']);
			else
				$this->_param['rows'] = false;
		}

		if(isset($v['cols']) === true)
		{
			if($v['cols'] !== false)
				$this->_param['cols'] = xivo_uint($v['cols']);
			else
				$this->_param['cols'] = false;
		}

		if(isset($v['multiple']) === true)
			$this->_param['multiple'] = true;

		if(isset($v['key']) === true)
			$this->_param['key'] = $v['key'];

		if(isset($v['key_val']) === true)
			$this->_param['key_val'] = $v['key_val'];

		if(isset($v['empty']) === true && $v['empty'] !== false)
			$this->_param['empty'] = $v['empty'] === true ? true : $this->_mk_param_value($v['empty'],$v);

		if(isset($v['optgroup']) === true && $v['optgroup'] !== false)
			$this->_param['optgroup'] = true;

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
				$this->_param['label'] = xivo_htmlsc($v['label']);
			else
				$this->_param['label'] = false;
		}

		if(isset($v['fieldid']) === true)
		{
			if(is_string($v['fieldid']) === true)
				$this->_param['fieldid'] = xivo_htmlsc($v['fieldid']);
			else
				$this->_param['fieldid'] = false;
		}

		if(isset($v['id']) === true)
		{
			if($v['id'] !== false)
				$this->_param['id'] = xivo_htmlsc($v['id']);
		}
		else if(isset($v['labelid']) === true && is_string($v['labelid']) === true)
		{
			$v['labelid'] = xivo_htmlsc($v['labelid']);

			if($this->_param['label'] === true)
				$this->_param['label'] = 'lb-'.$v['labelid'];
			if($this->_param['field'] !== false)
				$this->_param['fieldid'] = 'fd-'.$v['labelid'];

			$this->_param['id'] = 'it-'.$v['labelid'];
		}
		else $this->_param['id'] = $this->_param['name'];

		if(isset($v['class']) === true)
			$this->_param['class'] = xivo_htmlsc($v['class']);

		if(isset($v['null']) === true)
			$this->_param['null'] = true;

		if(isset($v['bbf']) === true && is_bool($v['bbf']) === false)
		{
			if(is_scalar($v['bbf']) === true)
				$this->_param['bbf'] = array('concatvalue',$v['bbf']);
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

				$this->_param['bbf'] = array($v['bbf'][0],$v['bbf'][1]);
			}
		}
	}

	function _mk_param_value($value,&$v)
	{
		$r = '';

		switch(gettype($value))
		{
			case 'array':
				if(isset($v['name'],$value[$v['name']]) === false)
					return($r);

				$r = $this->_mk_param_value($value[$v['name']],$v);
				break;
			case 'string':
				if(isset($v['notag']) === false)
					$r = xivo_notag($value);
				else if(isset($v['notagscript']) === true)
					$r = xivo_notagscript($value);
				else
					$r = $value;

				if(isset($v['schars']) === false)
					$r = xivo_htmlsc($r);
				break;
			case 'integer':
			case 'double':
					$r = $value;
				break;
			case 'boolean':
					$r = (int) $value;
				break;
		}

		return($r);
	}

	function _mk_param_checked($checked)
	{
		if(xivo_bool($checked) === true
		|| (xivo_issa($this->_param['name'],$checked) === true
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

		if($this->_param['checked'] !== false)
			$r .= ' checked="checked"';

		if($this->_param['value'] !== false)
			$r .= ' value="'.$this->_param['value'].'"';

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

		$v['size'] = $v['id'] = false;
		$v['desc'] = $v['class'] = $v['checked'] = null;
		$v['type'] = 'hidden';

		$this->_set_param($v);
		$this->_mk_input($o);

		return($this->_render);
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

	function bimg($v,$o='')
	{
		$v = (array) $v;
		$o = (string) $o;

		$v['value'] = $v['checked'] = null;
		$v['size'] = false;
		$v['class'] = 'itb-image';
		$v['type'] = 'image';

		$o .= isset($v['src']) === true ? ' src="'.(string) $v['src'].'"' : '';
		$o .= isset($v['alt']) === true ? ' alt="'.xivo_alttitle($v['alt']).'"' : ' alt=""';

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

	function radio()
	{
		$v = (array) $v;
		$o = (string) $o;

		$value = isset($v['value']) === false ? false : $this->_mk_param_value($v['value'],$v);

		$v['value'] = 1;
		$v['size'] = false;
		$v['type'] = 'radio';
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
			return(($str !== false ? (string) $str : false));

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

		$this->_render = '<p'.($this->_param['fieldid'] !== false ? ' id="'.$this->_param['fieldid'].'"' : '').' class="fm-field">'.
		"\n".$this->_render."\n".'</p>';
	}

	function _mk_desc()
	{
		if($this->_param['desc'] === false)
			return(false);

		if(is_scalar($this->_param['desc']) === true)
		{
			$this->_render = '<span class="fm-desc">'.(string) $this->_param['desc'].'</span>'.$this->_render;	
			return(null);
		}

		if(is_array($this->_param['desc']) === true && isset($this->_param['desc'][0],$this->_param['desc'][1]) === true)
		{
			$this->_mk_field();

			$this->_param['desc'][1] = '<span class="fm-desc">'.(string) $this->_param['desc'][1].'</span>';

			if(isset($this->_param['desc'][2]) === false)
				$this->_render = sprintf((string) $this->_param['desc'][0],$this->_mk_label($this->_param['desc'][1]),$this->_render);
			else
				$this->_render = sprintf((string) $this->_param['desc'][0],$this->_render,$this->_mk_label($this->_param['desc'][1]));

			$this->_param['label'] = $this->_param['field'] = false;
		}
	}

	function slt($v,$a=array(),$o='',$more=array())
	{
		$this->_render = '';

		$r = &$this->_render;

		$v = (array) $v;
		$o = (string) $o;

		$this->_set_param($v);

		$r .= '<select name="'.$this->_param['name'].'"';

		if($this->_param['id'] !== false)
			$r .= ' id="'.$this->_param['id'].'"';

		if($this->_param['multiple'] === true)
		{
			$r .= ' multiple="multiple"';

			if($this->_param['size'] !== false)
				$r .= ' size="'.$this->_param['size'].'"';
		}

		$r .= (isset($o{0}) === true ? ' '.$o : '').'>'."\n";

		if($this->_param['empty'] !== false)
		{
			if($this->_param['empty'] === true)
				$empty = '&nbsp;';
			else
				$empty = $this->_param['empty'];

			$r .= '<option label="" value=""'.($this->_param['value'] === '' ? ' selected="selected"' : '').'>'.$empty.'</option>'."\n";
		}

		if($this->_param['null'] === true || ($more !== '' && $more !== false) === true)
		{
			$more = (array) $more;
			$this->_mk_slt_option($more);
		}

		if($this->_param['null'] === true || ($a !== '' && $a !== false) === true)
		{
			$a = (array) $a;
			$this->_mk_slt_option($a);
		}

		$r .= '</select>';

		$this->_mk_desc();
		$this->_mk_label();
		$this->_mk_field();

		return($this->_render);
	}

	function _mk_slt_option_render($render)
	{
		if(is_array($render) === false)
			return(null);

		if($this->_param['optgroup'] === true)
		{
			if(isset($render[3]{0}) === false)
				$render[3] = '&nbsp;';
			else if(is_array($this->_param['bbf']) === true)
			{
				switch($this->_param['bbf'][0])
				{
					case 'paramkey':
						$render[3] = $this->bbf($this->_param['bbf'][1],$render[1]);
						break;
					case 'paramvalue':
						$render[3] = $this->bbf($this->_param['bbf'][1],$render[3]);
						break;
					case 'concatkey':
						$render[3] = $this->bbf($this->_param['bbf'][1].$render[1]);
						break;
					case 'concatvalue':
						$render[3] = $this->bbf($this->_param['bbf'][1].$render[3]);
						break;
				}
			}

			$this->_render .= vsprintf('<option label="%s" value="%s"%s>%s</option>',$render);
		}
		else
		{
			if(isset($render[2]{0}) === false)
				$render[2] = '&nbsp;';
			else if(is_array($this->_param['bbf']) === true)
			{
				switch($this->_param['bbf'][0])
				{
					case 'paramkey':
						$render[2] = $this->bbf($this->_param['bbf'][1],$render[0]);
						break;
					case 'paramvalue':
						$render[2] = $this->bbf($this->_param['bbf'][1],$render[2]);
						break;
					case 'concatkey':
						$render[2] = $this->bbf($this->_param['bbf'][1].$render[0]);
						break;
					case 'concatvalue':
						$render[2] = $this->bbf($this->_param['bbf'][1].$render[2]);
						break;
				}
			}

			$this->_render .= vsprintf('<option value="%s"%s>%s</option>',$render);
		}
	}

	function _mk_slt_option(&$a)
	{
		if(($arr = xivo_get_aks($a)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = &$arr['keys'][$i];

			$ref = &$a[$k];

			$o = '';

/*
			if(xivo_issa('__param',$ref) === true && is_string($ref['__param']) === true)
			{
				$o = ' '.$ref['__param'];
				unset($ref['__param']);

				if(count($ref) === 1)
					$ref = $ref[key($ref)];
			}

			if(isset($k{0}) === true && $k{0} === ' ' && strlen(trim($k)) === 0)
				$k = '-';
*/
	
			if(is_array($ref) === true)
			{
				if($this->_param['key_val'] !== false && isset($ref[$this->_param['key_val']]) === true)
					$k = $ref[$this->_param['key_val']];

				$render = false;
				if(is_bool($this->_param['key']) === false && isset($ref[$this->_param['key']]) === true)
				{
					$v = &$ref[$this->_param['key']];
					$selected = $this->_param['value'] === $k ? ' selected="selected"'.$o : $o;

					if($this->_param['optgroup'] === true)
						$render = array(xivo_alttitle($v),xivo_alttitle($k),$selected,xivo_htmlsc($v));
					else
						$render = array(xivo_alttitle($k),$selected,xivo_htmlsc($v));
				}
				else if($this->_param['key'] === true)
				{
					$selected = $this->_param['value'] === $k ? ' selected="selected"'.$o : $o;

					if($this->_param['optgroup'] === true)
						$render = array(xivo_alttitle($k),xivo_alttitle($k),$selected,xivo_htmlsc($k));
					else
						$render = array(xivo_alttitle($k),$selected,xivo_htmlsc($k));
				}
				else if($this->_param['key'] === false && is_scalar($ref) === true)
				{
					$selected = $this->_param['value'] === $ref ? ' selected="selected"'.$o : $o;

					if($this->_param['optgroup'] === true)
						$render = array(xivo_alttitle($ref),xivo_alttitle($ref),$selected,xivo_htmlsc($ref));
					else
						$render = array(xivo_alttitle($ref),$selected,xivo_htmlsc($ref));
				}

				if($render !== false)
					$this->_mk_slt_option_render($render);
				else
					$this->_mk_slt_option($ref);
			}
			else
			{
				if($this->_param['key'] === true)
				{
					$selected = $this->_param['value'] === $k ? ' selected="selected"'.$o : $o;

					if($this->_param['optgroup'] === true)
						$render = array(xivo_alttitle($k),xivo_alttitle($k),$selected,xivo_htmlsc($k));
					else
						$render = array(xivo_alttitle($k),$selected,xivo_htmlsc($k));
				}
				else if($this->_param['key'] === false && is_scalar($ref) === true)
				{
					$selected = $this->_param['value'] === $ref ? ' selected="selected"'.$o : $o;

					if($this->_param['optgroup'] === true)
						$render = array(xivo_alttitle($ref),xivo_alttitle($ref),$selected,xivo_htmlsc($ref));
					else
						$render = array(xivo_alttitle($ref),$selected,xivo_htmlsc($ref));
				}
				else
				{
					$selected = $this->_param['value'] === $k ? ' selected="selected"'.$o : $o;

					if($this->_param['optgroup'] === true)
						$render = array(xivo_alttitle($ref),xivo_alttitle($k),$selected,xivo_htmlsc($ref));
					else
						$render = array(xivo_alttitle($k),$selected,xivo_htmlsc($ref));
				}

				$this->_mk_slt_option_render($render);
			}
		}
	}
}

?>
