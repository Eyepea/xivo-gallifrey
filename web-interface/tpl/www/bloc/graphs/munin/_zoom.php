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

$url     = &$this->get_module('url');
$basedir = $this->get_var('basedir');
$zoom    = $this->get_var('zoom');
$prev    = $this->get_var('prev');
$next    = $this->get_var('next');

$elts    = split('-', $zoom);

function bbf_format($ctx, $link)
{
    list($domain, $mod, $freq) = split('-', $link);
    return $ctx->bbf('title_content_name').' - '.$ctx->bbf($mod).' ('.
        $ctx->bbf('per-'.$freq).')';
}

?>
<div id="sr-users" class="b-infos b-form">
	<h3 class="sb-top xspan">
		<span class="span-left">&nbsp;</span>
		</span>
		<span class="span-center"><?= bbf_format($this, $zoom); ?></span>
		<span class="span-right">&nbsp;</span>
	</h3>
	<div class="sb-content">
<?php
    if(!is_null($prev))
        echo $url->href_html($this->bbf('previous'), null, array("zoom" => $prev), 
                null, bbf_format($this, $prev)),
            '&nbsp;&nbsp;';
    
    echo $url->img_html("$basedir/XIVO.$zoom.png", "$zoom graph");

    if(!is_null($next))
        echo '&nbsp;&nbsp;',
            $url->href_html($this->bbf('next'), null, array("zoom" => $next),
                null, bbf_format($this, $next));
            
?>
    </div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>

