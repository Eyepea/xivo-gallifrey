<?php

#
# XiVO Web-Interface
# Copyright (C) 2006, 2007, 2008  Proformatique <technique@proformatique.com>
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

echo	'var xivo_date_month = new Array();',"\n",
	'xivo_date_month[0] = \'',$this->bbf('date_month_january'),'\';',"\n",
	'xivo_date_month[1] = \'',$this->bbf('date_month_february'),'\';',"\n",
	'xivo_date_month[2] = \'',$this->bbf('date_month_march'),'\';',"\n",
	'xivo_date_month[3] = \'',$this->bbf('date_month_april'),'\';',"\n",
	'xivo_date_month[4] = \'',$this->bbf('date_month_may'),'\';',"\n",
	'xivo_date_month[5] = \'',$this->bbf('date_month_june'),'\';',"\n",
	'xivo_date_month[6] = \'',$this->bbf('date_month_july'),'\';',"\n",
	'xivo_date_month[7] = \'',$this->bbf('date_month_august'),'\';',"\n",
	'xivo_date_month[8] = \'',$this->bbf('date_month_september'),'\';',"\n",
	'xivo_date_month[9] = \'',$this->bbf('date_month_october'),'\';',"\n",
	'xivo_date_month[10] = \'',$this->bbf('date_month_november'),'\';',"\n",
	'xivo_date_month[11] = \'',$this->bbf('date_month_december'),'\';',"\n\n";

echo	'var xivo_date_day = new Array();',"\n",
	'xivo_date_day[0] = \'',$this->bbf('date_day_sunday'),'\';',"\n",
	'xivo_date_day[1] = \'',$this->bbf('date_day_monday'),'\';',"\n",
	'xivo_date_day[2] = \'',$this->bbf('date_day_tuesday'),'\';',"\n",
	'xivo_date_day[3] = \'',$this->bbf('date_day_wesnesday'),'\';',"\n",
	'xivo_date_day[4] = \'',$this->bbf('date_day_thursday'),'\';',"\n",
	'xivo_date_day[5] = \'',$this->bbf('date_day_friday'),'\';',"\n",
	'xivo_date_day[6] = \'',$this->bbf('date_day_saturday'),'\';';

?>
