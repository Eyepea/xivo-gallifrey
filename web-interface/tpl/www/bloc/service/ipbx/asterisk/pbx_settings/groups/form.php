<?php
	$form = &$this->get_module('form');
	$element = $this->vars('element');
	$info = $this->vars('info');
?>

<?=$form->text(array('desc' => $this->bbf('fm_gfeatures_name'),'name' => 'gfeatures[name]','labelid' => 'gfeatures-name','size' => 25,'default' => $element['gfeatures']['name']['default'],'value' => $info['gfeatures']['name']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_gfeatures_number'),'name' => 'gfeatures[number]','labelid' => 'gfeatures-number','size' => 25,'default' => $element['gfeatures']['number']['default'],'value' => $info['gfeatures']['number']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_gfeatures_context'),'name' => 'gfeatures[context]','labelid' => 'gfeatures-context','size' => 25,'default' => $element['gfeatures']['context']['default'],'value' => $info['gfeatures']['context']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_queue_timeout'),'name' => 'queue[timeout]','labelid' => 'queue-timeout','bbf' => array('paramkey','fm_queue_timeout-opt'),'key' => false,'default' => $element['queue']['timeout']['default'],'value' => (isset($info['queue']['timeout']) === true ? (int) $info['queue']['timeout'] : null)),$element['queue']['timeout']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

