<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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

define("TOK_SPACE" , 0); // space, tabulation
define("TOK_BOOL"  , 1); // &, |
define("TOK_CMP"   , 2); // >, <, >=, <=
define("TOK_PAROP" , 3); // (
define("TOK_PARCL" , 4); // )
define("TOK_SKILL" , 5); // skill name
define("TOK_WEIGHT", 6); // 0 <= x <= 100


class xivo_service_asterisk_queueskillrule extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'queueskillrule';

	var $_filter		= false;
	
	function xivo_service_asterisk_queueskillrule(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	/** 
		Parser & Lexer to check a rule string
	*/

	function check_rule($rule)
	{
		$tokens = $this->_tokenize($rule);
		return($this->_validate($tokens));
	}

	/**
		lexer. transform string into tokens array
	*/		
	function _tokenize($rule)
	{
		$_space  = " \t";
		$_opcmp  = "><";
		$_opbool = '&|';

		$tokens = array();

		$len = strlen($rule);
		$start = 0;	
		$value = null;

		for($i = 0; $i < $len; $i++)
		{
			$value = null;

			// space
			if(strpos($_space, $rule[$i]) 		!== false)
			{ $value = TOK_SPACE; }
			else if($rule[$i] == '(')
			{ $value = TOK_PAROP; }
			else if($rule[$i] == ')')
			{ $value = TOK_PARCL; }
			else if(strpos($_opcmp, $rule[$i])	!== false)
			{ $value = TOK_CMP; }
			else if(strpos($_opbool, $rule[$i])	!== false)
			{ $value = TOK_BOOL; }

			if(!is_null($value))
			{
				if($i - $start > 0)
				{
					$literal = substr($rule, $start, $i - $start);
					if(ctype_digit($literal))
					{ $tokens[] = array(TOK_WEIGHT, (int) $literal); }
					else
					{
						// check this skill exist
						$tokens[] = array(TOK_SKILL, $literal);
					}
				}

				if($value == TOK_CMP && $i < $len-1 && $rule[$i+1] == '=')
				{ $tokens[] = array(TOK_CMP, $rule[$i] . '='); $i++; }
				else if($value != TOK_SPACE)
				{ $tokens[] = array($value, $rule[$i]); }

				$start = $i+1;				
			}
		}

		if($i - $start > 0)
		{
			$literal = substr($rule, $start, $i - $start);
			// check this skill exist

			if(ctype_digit($literal))	
			{ $tokens[] = array(TOK_WEIGHT, (int) $literal); }
			else
			{ $tokens[] = array(TOK_SKILL, $literal); }
		}

		return($tokens);
	}

	/**
		Rule parser.
		validate the rule syntax

		@returns bool valid rule or not
	*/
	function _validate($tokens)
	{
		$i = 0;
		return($this->_match_boolexp($tokens, $i));
	}

	/**

		CMPEXP: TOK_SKILL 
		      | TOK_SKILL TOK_CMP TOK_WEIGHT
	*/
	function _match_cmpexp($tokens, &$i)
	{
		if($this->_eos($tokens, $i))
		{ return false; }

		// 1st token must be a skillname
		if(!$this->_isa($tokens, $i, TOK_SKILL))
		{ return false; }

		// the 2d token is facultative. If present, must be a comparison operator		
		if(!$this->_isa($tokens, ++$i, TOK_CMP))
		{ return true; }

		// 3d token must be a weight
		$isaweight = $this->_isa($tokens, ++$i, TOK_WEIGHT);
		if($isaweight)
		{ $i++; }

		return $isaweight;
	}

	/**
		BOOLEXP: CMPEXP
		       | '(' BOOLEXP ')'
		       | CMPEXP BOOLOP BOOLEXP

	*/
	function _match_boolexp($tokens, &$i, $inpar=false)
	{
		//if($this->_eos($tokens, $i) || ($inpar && $this->_isa($tokens, $i, TOK_PARCL)))
		if($inpar && $this->_isa($tokens, $i, TOK_PARCL))
		{ return true; }
	
		// expression with parenthesis
		if($this->_isa($tokens, $i, TOK_PAROP))
		{
			if(!$this->_match_boolexp($tokens, ++$i, true) ||
			   !$this->_isa($tokens, $i++, TOK_PARCL))
			{ return false; }

		// simple comparison expression.
		} else {	
			if(!$this->_match_cmpexp($tokens, $i))
			{ return false; }
		}

		//
		if($this->_eos($tokens, $i) || ($inpar && $this->_isa($tokens, $i, TOK_PARCL)))
		{ return true; }

		// the comparison expression, if not alone, must be followed
		// by a boolean operator then a boolean expression
		return($this->_isa($tokens, $i, TOK_BOOL) && 
		       $this->_match_boolexp($tokens, ++$i, $inpar));	
	}

	/**
		end-of-expression
	*/
	function _eos($tokens, $i)
	{ return($i >= count($tokens)); }

	/**
		match a token
	*/		
	function _isa($tokens, $i, $token)
	{ return($i < count($tokens) && $tokens[$i][0] == $token); }
}	

?>
