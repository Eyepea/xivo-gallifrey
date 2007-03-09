<?php
	$url = &$this->get_module('url');
	$menu = &$this->get_module('menu');
	$this->file_include('bloc/menu/top/user/loginbox');
?>

<div id="toolbox">
<div id="logo"><?=$url->img_html('img/menu/top/logo.gif');?></div>
<div class="nav">
         <ul>
	 	<li onmouseout="this.className='moo';" onmouseover="this.className='mov';"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('mn_top_services');?></span><span class="span-right">&nbsp;</span><ul><li><?=$url->href_html($this->bbf('mn_sub_top_services_ipbx'),'service/ipbx',null,'');?></li></ul></li>
		<li onmouseout="this.className='moo';" onmouseover="this.className='mov';"><?=$url->href_html('<span class="span-left">&nbsp;</span><span class="span-center">'.$this->bbf('mn_top_configuration').'</span><span class="span-right">&nbsp;</span>','xivo/configuration');?></li>
                <li onmouseout="this.className='moo';" onmouseover="this.className='mov';"><?=$url->href_html('<span class="span-left">&nbsp;</span><span class="span-center">'.$this->bbf('mn_top_preferences').'</span><span class="span-right">&nbsp;</span>','xivo/preferences');?></li>
                <li onmouseout="this.className='moo';" onmouseover="this.className='mov';"><?=$url->href_html('<span class="span-left">&nbsp;</span><span class="span-center">'.$this->bbf('mn_top_help').'</span><span class="span-right">&nbsp;</span>','#');?></li>
                <li onmouseout="this.className='moo';" onmouseover="this.className='mov';"><?=$url->href_html('<span class="span-left">&nbsp;</span><span class="span-center">'.$this->bbf('mn_top_contact').'</span><span class="span-right">&nbsp;</span>','xivo/contact');?></li>
	</ul>	
</div>
<div id="tooltips">Tooltips</div>
<div id="toolbar"><?=$menu->mk_toolbar();?></div>
</div>
