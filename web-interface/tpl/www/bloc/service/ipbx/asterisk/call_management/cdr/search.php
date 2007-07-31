<?php
	$form = &$this->get_module('form');

	$element = $this->vars('element');
?>
<div id="sr-cdr" class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>
<?=$form->hidden(array('name' => 'act','value' => 'add'));?>

<div class="fm-field fm-desc-inline">
<?=$form->text(array('desc' => $this->bbf('fm_dbeg'),'field' => false,'name' => 'dbeg','labelid' => 'dbeg','default' => $element['dbeg']['default'],'value' => $this->varra('info','dbeg')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_dend'),'field' => false,'name' => 'dend','labelid' => 'dend','value' => $this->varra('info','dend')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<?=$form->slt(array('desc' => $this->bbf('fm_channel'),'name' => 'channel','labelid' => 'channel','empty' => true,'key' => false,'default' => $element['channel']['default'],'value' => $this->varra('info','channel')),$element['channel']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_disposition'),'name' => 'disposition','labelid' => 'disposition','empty' => true,'key' => false,'default' => $element['disposition']['default'],'value' => $this->varra('info','disposition')),$element['disposition']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<div class="fm-field fm-multifield">
<?=$form->text(array('desc' => $this->bbf('fm_src'),'field' => false,'name' => 'src','labelid' => 'src','size' => 15,'default' => $element['src']['default'],'value' => $this->varra('info','src')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
<?=$form->slt(array('field' => false,'name' => 'srcformat','labelid' => 'srcformat','key' => false,'default' => $element['srcformat']['default'],'value' => $this->varra('info','srcformat')),$element['srcformat']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>


<div class="fm-field fm-multifield">
<?=$form->text(array('desc' => $this->bbf('fm_dst'),'field' => false,'name' => 'dst','labelid' => 'dst','size' => 15,'default' => $element['dst']['default'],'value' => $this->varra('info','dst')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
<?=$form->slt(array('field' => false,'name' => 'dstformat','labelid' => 'dstformat','key' => false,'default' => $element['dstformat']['default'],'value' => $this->varra('info','dstformat')),$element['dstformat']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<div class="fm-field fm-multifield">
<?=$form->text(array('desc' => $this->bbf('fm_clid'),'field' => false,'name' => 'clid','labelid' => 'clid','size' => 15,'default' => $element['clid']['default'],'value' => $this->varra('info','clid')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
<?=$form->slt(array('field' => false,'name' => 'clidformat','labelid' => 'clidformat','key' => false,'default' => $element['clidformat']['default'],'value' => $this->varra('info','clidformat')),$element['clidformat']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<div class="fm-field fm-multifield">
<?=$form->text(array('desc' => $this->bbf('fm_accountcode'),'field' => false,'name' => 'accountcode','labelid' => 'accountcode','size' => 15,'default' => $element['accountcode']['default'],'value' => $this->varra('info','accountcode')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
<?=$form->slt(array('field' => false,'name' => 'accountcodeformat','labelid' => 'accountcodeformat','key' => false,'default' => $element['accountcodeformat']['default'],'value' => $this->varra('info','accountcodeformat')),$element['accountcodeformat']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<div class="fm-field fm-multifield">
<?=$form->text(array('desc' => $this->bbf('fm_userfield'),'field' => false,'name' => 'userfield','labelid' => 'userfield','size' => 15,'default' => $element['userfield']['default'],'value' => $this->varra('info','userfield')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
<?=$form->slt(array('field' => false,'name' => 'userfieldformat','labelid' => 'userfieldformat','key' => false,'default' => $element['userfieldformat']['default'],'value' => $this->varra('info','userfieldformat')),$element['userfieldformat']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<div class="fm-field fm-desc-inline">
<div class="fm-multifield">
<?=$form->text(array('desc' => $this->bbf('fm_dubeg'),'field' => false,'name' => 'dubeg','labelid' => 'dubeg','default' => $element['dubeg']['default'],'value' => $this->varra('info','dubeg')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
<?=$form->slt(array('field' => false,'name' => 'dubegunit','id' => 'dubegunit','label' => false,'key' => false,'default' => $element['dubegunit']['default'],'value' => $this->varra('info','dubegunit')),$element['dubegunit']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<div class="fm-multifield">
<?=$form->text(array('desc' => $this->bbf('fm_duend'),'field' => false,'name' => 'duend','labelid' => 'duend','value' => $this->varra('info','duend')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
<?=$form->slt(array('field' => false,'name' => 'duendunit','id' => 'duendunit','label' => false,'key' => false,'default' => $element['duendunit']['default'],'value' => $this->varra('info','duendunit')),$element['duendunit']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>
</div>

<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>

</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
