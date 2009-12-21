<?php
$form = &$this->get_module('form');

$ent_name = $this->get_var('ent-name');
$ent_dispname = $this->get_var('ent-dispname');

echo 
	$form->hidden(array('name' => 'fm_send',
						'value' => 1)),

	$form->hidden(array('name' => 'step',
						'value' => $this->get_var('step')));

#	$this->file_include('bloc/xivo/configuration/manage/entity/form');

?>
<div class="wz-check-tb-title"><?=$this->bbf('wz-entctx-entity')?></div>
<div class="wz-entctx-tb-left">&nbsp;</div>
<div class="wz-entctx-tb-right">
	<?=$form->text(array('desc' => $this->bbf('wz-entctx-ent-name'),
						'name' => 'ent[name]',
						'labelid' => 'ent-name',
						'default' => $ent_name))?>
</div>
<div class="wz-entctx-tb-right">
	<?=$form->text(array('desc' => $this->bbf('wz-entctx-ent-dispname'),
						'name' => 'ent[dispname]',
						'labelid' => 'ent-dispname',
						'default' => $ent_dispname))?>
</div>

<div class="wz-check-tb-title"><?=$this->bbf('wz-entctx-context')?></div>

<div class="wz-entctx-tb-left">&nbsp;</div>
<div class="wz-entctx-tb-right"><?=$this->bbf('wz-entctx-ctx-name')?></div>
<div class="wz-entctx-tb-right"><?=$this->bbf('wz-entctx-ctx-numbeg')?></div>
<div class="wz-entctx-tb-right"><?=$this->bbf('wz-entctx-ctx-numend')?></div>

<div class="wz-entctx-tb-left"><?=$this->bbf('wz-entctx-incoming')?></div>
<div class="wz-entctx-tb-right">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-name'),
						'name' => 'ctx[incoming][name]',
						'labelid' => 'wz-ctx-incoming-name'))?>
</div>
<div class="wz-entctx-tb-right">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-numbeg'),
						'name' => 'ctx[incoming][numbeg]',
						'labelid' => 'wz-ctx-incoming-numbeg'))?>
</div>
<div class="wz-entctx-tb-right">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-numend'),
						'name' => 'ctx[incoming][numend]',
						'labelid' => 'wz-ctx-incoming-numend'))?>
</div>

<div class="wz-entctx-tb-left"><?=$this->bbf('wz-entctx-outgoing')?></div>
<div class="wz-entctx-tb-right">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-name'),
						'name' => 'ctx[outgoing][name]',
						'labelid' => 'wz-ctx-outgoing-name'))?>
</div>
<div class="wz-entctx-tb-right">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-numbeg'),
						'name' => 'ctx[outgoing][numbeg]',
						'labelid' => 'wz-ctx-outgoing-numbeg'))?>
</div>
<div class="wz-entctx-tb-right">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-numend'),
						'name' => 'ctx[outgoing][numend]',
						'labelid' => 'wz-ctx-outgoing-numend'))?>
</div>

<div class="wz-entctx-tb-left"><?=$this->bbf('wz-entctx-internal')?></div>
<div class="wz-entctx-tb-right">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-name'),
						'name' => 'ctx[internal][name]',
						'labelid' => 'wz-ctx-internal-name'))?>
</div>
<div class="wz-entctx-tb-right">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-numbeg'),
						'name' => 'ctx[internal][numbeg]',
						'labelid' => 'wz-ctx-internal-numbeg'))?>
</div>
<div class="wz-entctx-tb-right">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-numend'),
						'name' => 'ctx[internal][numend]',
						'labelid' => 'wz-ctx-internal-numend'))?>
</div>

<div class="wz-check-tb-title">&nbsp</div>
