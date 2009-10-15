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

$form = &$this->get_module('form');
$url = &$this->get_module('url');

$element = $this->get_var('element');
$list = $this->get_var('list');
$type = $this->get_var('type');

echo	$form->text(array('desc'	=> $this->bbf('fm_phonebooknumber_'.$type),
			  'name'	=> 'phonebooknumber['.$type.']',
			  'labelid'	=> 'phonebooknumber-'.$type,
			  'size'	=> 15,
			  'default'	=> $element['phonebooknumber']['number']['default'],
			  'value'	=> $this->get_var('phonebooknumber',$type,'number')));

if($type === 'office'):
	echo	$form->text(array('desc'	=> $this->bbf('fm_phonebooknumber_fax'),
				  'name'	=> 'phonebooknumber[fax]',
				  'labelid'	=> 'phonebooknumber-fax',
				  'size'	=> 15,
				  'default'	=> $element['phonebooknumber']['number']['default'],
				  'value'	=> $this->get_var('phonebooknumber','fax','number')));
endif;

echo	$form->text(array('desc'	=> $this->bbf('fm_phonebookaddress_address1'),
			  'name'	=> 'phonebookaddress['.$type.'][address1]',
			  'labelid'	=> 'phonebookaddress-'.$type.'-address1',
			  'size'	=> 15,
			  'default'	=> $element['phonebookaddress']['address1']['default'],
			  'value'	=> $this->get_var('phonebookaddress',$type,'address1'))),

	$form->text(array('desc'	=> $this->bbf('fm_phonebookaddress_address2'),
			  'name'	=> 'phonebookaddress['.$type.'][address2]',
			  'labelid'	=> 'phonebookaddress-'.$type.'-address2',
			  'size'	=> 15,
			  'default'	=> $element['phonebookaddress']['address2']['default'],
			  'value'	=> $this->get_var('phonebookaddress',$type,'address2'))),

	$form->text(array('desc'	=> $this->bbf('fm_phonebookaddress_city'),
			  'name'	=> 'phonebookaddress['.$type.'][city]',
			  'labelid'	=> 'phonebookaddress-'.$type.'-city',
			  'size'	=> 15,
			  'default'	=> $element['phonebookaddress']['city']['default'],
			  'value'	=> $this->get_var('phonebookaddress',$type,'city'))),

	$form->text(array('desc'	=> $this->bbf('fm_phonebookaddress_state'),
			  'name'	=> 'phonebookaddress['.$type.'][state]',
			  'labelid'	=> 'phonebookaddress-'.$type.'-state',
			  'size'	=> 15,
			  'default'	=> $element['phonebookaddress']['state']['default'],
			  'value'	=> $this->get_var('phonebookaddress',$type,'state'))),

	$form->text(array('desc'	=> $this->bbf('fm_phonebookaddress_zipcode'),
			  'name'	=> 'phonebookaddress['.$type.'][zipcode]',
			  'labelid'	=> 'phonebookaddress-'.$type.'-zipcode',
			  'size'	=> 15,
			  'default'	=> $element['phonebookaddress']['zipcode']['default'],
			  'value'	=> $this->get_var('phonebookaddress',$type,'zipcode'))),

	$form->select(array('desc'	=> $this->bbf('fm_phonebookaddress_country'),
			    'name'	=> 'phonebookaddress['.$type.'][country]',
			    'labelid'	=> 'phonebookaddress-'.$type.'-country',
			    'empty'	=> true,
			    'size'	=> 15,
			    'default'	=> $element['phonebookaddress']['country']['default'],
			    'selected'	=> $this->get_var('phonebookaddress',$type,'country')),
		      $this->get_var('territory'));
?>
