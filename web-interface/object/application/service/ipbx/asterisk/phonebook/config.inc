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

$array = array();

$array['import'] = array();
$array['import']['file'] = array();
$array['import']['file']['name'] = '/^[a-z0-9_-]{1,50}(?:\.[a-z]{1,10})?$/i';
$array['import']['file']['mime'] = array('text/plain','text/csv');

$array['import']['field'] = array();
$array['import']['field']['title'] = array('phonebook','title');
$array['import']['field']['firstname'] = array('phonebook','firstname');
$array['import']['field']['lastname'] = array('phonebook','lastname');
$array['import']['field']['displayname'] = array('phonebook','displayname');
$array['import']['field']['society'] = array('phonebook','society');
$array['import']['field']['mobilenumber'] = array('phonebooknumber','mobile');
$array['import']['field']['email'] = array('phonebook','email');
$array['import']['field']['url'] = array('phonebook','url');
$array['import']['field']['description'] = array('phonebook','description');
$array['import']['field']['officenumber'] = array('phonebooknumber','office');
$array['import']['field']['faxnumber'] = array('phonebooknumber','fax');
$array['import']['field']['officeaddress1'] = array('phonebookaddress','office','address1');
$array['import']['field']['officeaddress2'] = array('phonebookaddress','office','address2');
$array['import']['field']['officecity'] = array('phonebookaddress','office','city');
$array['import']['field']['officestate'] = array('phonebookaddress','office','state');
$array['import']['field']['officezipcode'] = array('phonebookaddress','office','zipcode');
$array['import']['field']['officecountry'] = array('phonebookaddress','office','country');
$array['import']['field']['homenumber'] = array('phonebooknumber','home');
$array['import']['field']['homeaddress1'] = array('phonebookaddress','home','address1');
$array['import']['field']['homeaddress2'] = array('phonebookaddress','home','address2');
$array['import']['field']['homecity'] = array('phonebookaddress','home','city');
$array['import']['field']['homestate'] = array('phonebookaddress','home','state');
$array['import']['field']['homezipcode'] = array('phonebookaddress','home','zipcode');
$array['import']['field']['homecountry'] = array('phonebookaddress','home','country');
$array['import']['field']['othernumber'] = array('phonebooknumber','other');
$array['import']['field']['otheraddress1'] = array('phonebookaddress','other','address1');
$array['import']['field']['otheraddress2'] = array('phonebookaddress','other','address2');
$array['import']['field']['othercity'] = array('phonebookaddress','other','city');
$array['import']['field']['otherstate'] = array('phonebookaddress','other','state');
$array['import']['field']['otherzipcode'] = array('phonebookaddress','other','zipcode');
$array['import']['field']['othercountry'] = array('phonebookaddress','other','country');

?>
