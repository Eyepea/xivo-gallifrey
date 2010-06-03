<?php
	$url = &$this->get_module('url');

	$presencename = $this->get_var('presencename');
	$status = $this->get_var('status');
?>
<div id="ctistatus-<?=$presencename?>">
    <div class="sb-list">
        <table id="table-presence-listing">
        <tr class="sb-top">
            <th class="th-left">status</th>
            <th class="th-center">dispname</th>
            <th class="th-right col-action">&nbsp</th>
        </tr>
        <?php
            foreach($status as $p => $v) 
            {   
                echo 
                    '<tr><td>', $p, '</td><td>', $v['display'], '</td>',
                    '<td>',$url->href_html($url->img_html('img/site/button/edit.gif',
                                $this->bbf('opt_modify'),
                                'border="0"'),
                            'service/ipbx/call_center/agents',
                            array('act' => 'edit',
                                    'group'   => $ref['agentgroup']['id']),
                            null,
                            $this->bbf('opt_modify')),"\n</td></tr>";
            }
        ?>  
        </table>
    </div>
</div>
