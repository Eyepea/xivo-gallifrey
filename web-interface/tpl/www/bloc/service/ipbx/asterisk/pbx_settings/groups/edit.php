<?php
	$form = &$this->get_module('form');
	$info = $this->vars('info');
	$queue_elt = $this->vars('queue_elt');
?>

<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => 'edit'));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>
<?=$form->hidden(array('name' => 'id','value' => $info['gfeatures']['id']));?>
<?=$form->text(array('desc' => $this->bbf('fm_gfeatures_name'),'name' => 'gfeatures[name]','id' => 'it-gfeatures-name','label' => 'lb-gfeatures-name','size' => 25,'value' => $info['gfeatures']['name']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
<?php
	if(xivo_ak('ringseconds',$queue_elt) === true):
?>
<p class="fm-field">
<label id="lb-queue-ringseconds" for="it-queue-ringseconds"><?=$this->bbf('fm_queue_ringseconds');?></label>
<select name="queue[ringseconds]" id="it-queue-ringseconds" onfocus="this.className='it-mfocus';" onblur="this.className='it-mblur';">
<?php
		$nb = count($queue_elt['ringseconds']);

		for($i = 0;$i < $nb;$i++):
			$val = $queue_elt['ringseconds'][$i];

			echo '<option ',($val === (int) $info['queue']['ringseconds'] ? 'selected="selected" ' : ''),'value="',xivo_alttitle($val),'">',$this->bbf('fm_queue_ringseconds-opt',$val),'</option>';
		endfor;
?>
</select>
</p>
<?php
	endif;
?>
<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
