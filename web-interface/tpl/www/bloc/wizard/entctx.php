<?php
$form = &$this->get_module('form');

$ent_name = $this->get_var('ent-name');
$ent_dispname = $this->get_var('ent-dispname');
$ctx = $this->get_var('ctx');
$disp_digits = array($this->bbf('wz-entctx-digits-pick'), range(1, 20));

echo 
	$form->hidden(array('name' => 'fm_send',
						'value' => 1)),

	$form->hidden(array('name' => 'step',
						'value' => $this->get_var('step')));


?>
<!--
	Entity
-->

<div class="wz-check-tb-title"><?=$this->bbf('wz-entctx-entity')?></div>
<div class="wz-entctx-tb-left">&nbsp;</div>
<div class="wz-entctx-tb-right"><?=$this->bbf('wz-entctx-ent-name')?></div>
<div class="wz-entctx-tb-right"><?=$this->bbf('wz-entctx-ent-dispname')?></div>
<div class="wz-entctx-tb-left">&nbsp;</div>
<div class="wz-entctx-tb-right wz-paragraph">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ent-name'),
						'name' => 'ent[name]',
						'paragraph' => false,
						'required' => true,
						'help' => $this->bbf('hlp_ent-name'),
						'comment' => $this->bbf('cmt_ent-name'),
						'labelid' => 'ent-name',
						'default' => $ent_name))?>
</div>
<div class="wz-entctx-tb-right wz-paragraph">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ent-dispname'),
						'name' => 'ent[dispname]',
						'paragraph' => false,
						'required' => true,
						'help' => $this->bbf('hlp_ent-dispname'),
						'comment' => $this->bbf('cmt_ent-dispname'),
						'labelid' => 'ent-dispname',
						'default' => $ent_dispname))?>
</div>

<!--
	Context: header
-->
<div class="wz-check-tb-title"><?=$this->bbf('wz-entctx-context')?></div>

<div class="wz-entctx-tb-left">&nbsp;</div>
<div class="wz-entctx-tb-right"><?=$this->bbf('wz-entctx-ctx-name')?></div>
<div class="wz-entctx-tb-right"><?=$this->bbf('wz-entctx-ctx-dispname')?></div>
<div class="wz-entctx-tb-right"><?=$this->bbf('wz-entctx-ctx-numbeg')?></div>
<div class="wz-entctx-tb-right"><?=$this->bbf('wz-entctx-ctx-numend')?></div>

<!-- 
	Context: incoming 
-->
<div class="wz-entctx-tb-left"><?=$this->bbf('wz-entctx-incoming')?></div>
<div class="wz-entctx-tb-right wz-paragraph">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-name'),
						'name' => 'ctx[incoming][name]',
						'paragraph' => false,
						'required' => true,
						'help' => $this->bbf('hlp_wz-ctx-incoming-name'),
						'comment' => $this->bbf('cmt_wz-ctx-incoming-name'),
						'default' => $ctx['incoming']['name'],
						'labelid' => 'wz-ctx-incoming-name'))?>
</div>
<div class="wz-entctx-tb-right wz-paragraph">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-name'),
						'name' => 'ctx[incoming][dispname]',
						'paragraph' => false,
						'required' => true,
						'help' => $this->bbf('hlp_wz-ctx-incoming-dispname'),
						'comment' => $this->bbf('cmt_wz-ctx-incoming-dispname'),
						'default' => $ctx['incoming']['dispname'],
						'labelid' => 'wz-ctx-incoming-dispname'))?>
</div>
<div class="wz-entctx-tb-right wz-paragraph">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-numbeg'),
						'name' => 'ctx[incoming][numbeg]',
						'paragraph' => false,
						'required' => true,
						'regexp' => '[[:int:]]',
						'help' => $this->bbf('hlp_wz-ctx-incoming-numbeg'),
						'comment' => $this->bbf('cmt_wz-ctx-incoming-numbeg'),
						'default' => $ctx['incoming']['numbeg'],
						'labelid' => 'wz-ctx-incoming-numbeg'))?>
</div>
<div class="wz-entctx-tb-right wz-paragraph">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-numend'),
						'name' => 'ctx[incoming][numend]',
						'paragraph' => false,
						'required' => true,
						'regexp' => '[[:int:]]',
						'help' => $this->bbf('hlp_wz-ctx-incoming-numend'),
						'comment' => $this->bbf('cmt_wz-ctx-incoming-numend'),
						'default' => $ctx['incoming']['numend'],
						'labelid' => 'wz-ctx-incoming-numend'))?>
	<?=$form->select(array('name' => 'ctx[incoming][dispdigits]',
						'help' => $this->bbf('hlp_wz-ctx-dispdigits'),
						'comment' => $this->bbf('cmt_wz-ctx-dispdigits'),
						'paragraph' => false,
						'labelid' => 'wz-ctx-dispdigits',
						'selected' => $ctx['incoming']['dispdigits']),
						$disp_digits)?>
</div>

<!-- 
	Context: outgoing
-->
<div class="wz-entctx-tb-left"><?=$this->bbf('wz-entctx-outgoing')?></div>
<div class="wz-entctx-tb-right wz-paragraph">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-name'),
						'name' => 'ctx[outgoing][name]',
						'paragraph' => false,
						'required' => true,
						'help' => $this->bbf('hlp_wz-ctx-outgoing-name'),
						'comment' => $this->bbf('cmt_wz-ctx-outgoing-name'),
						'default' => $ctx['outgoing']['name'],
						'labelid' => 'wz-ctx-outgoing-name'))?>
</div>
<div class="wz-entctx-tb-right wz-paragraph">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-numbeg'),
						'name' => 'ctx[outgoing][dispname]',
						'paragraph' => false,
						'required' => true,
						'help' => $this->bbf('hlp_wz-ctx-outgoing-dispname'),
						'comment' => $this->bbf('cmt_wz-ctx-outgoing-dispname'),
						'default' => $ctx['outgoing']['dispname'],
						'labelid' => 'wz-ctx-outgoing-dispname'))?>
</div>

<!--
	Context: internal
-->
<div class="wz-entctx-tb-left"><?=$this->bbf('wz-entctx-internal')?></div>
<div class="wz-entctx-tb-right wz-paragraph">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-name'),
						'name' => 'ctx[internal][name]',
						'paragraph' => false,
						'required' => true,
						'help' => $this->bbf('hlp_wz-ctx-internal-name'),
						'comment' => $this->bbf('cmt_wz-ctx-internal-name'),
						'default' => $ctx['internal']['name'],
						'labelid' => 'wz-ctx-internal-name'))?>
</div>
<div class="wz-entctx-tb-right wz-paragraph">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-name'),
						'name' => 'ctx[internal][dispname]',
						'paragraph' => false,
						'required' => true,
						'help' => $this->bbf('hlp_wz-ctx-internal-dispname'),
						'comment' => $this->bbf('cmt_wz-ctx-internal-dispname'),
						'default' => $ctx['internal']['dispname'],
						'labelid' => 'wz-ctx-internal-dispname'))?>
</div>
<div class="wz-entctx-tb-right wz-paragraph">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-numbeg'),
						'name' => 'ctx[internal][numbeg]',
						'paragraph' => false,
						'required' => true,
						'regexp' => '[[:int:]]',
						'help' => $this->bbf('hlp_wz-ctx-internal-numbeg'),
						'comment' => $this->bbf('cmt_wz-ctx-internal-numbeg'),
						'default' => $ctx['internal']['numbeg'],
						'labelid' => 'wz-ctx-internal-numbeg'))?>
</div>
<div class="wz-entctx-tb-right wz-paragraph">
	<?=$form->text(array(#'desc' => $this->bbf('wz-entctx-ctx-numend'),
						'name' => 'ctx[internal][numend]',
						'paragraph' => false,
						'required' => true,
						'regexp' => '[[:int:]]',
						'help' => $this->bbf('hlp_wz-ctx-internal-numend'),
						'comment' => $this->bbf('cmt_wz-ctx-internal-numend'),
						'default' => $ctx['internal']['numend'],
						'labelid' => 'wz-ctx-internal-numend'))?>
</div>

<div class="wz-check-tb-title">&nbsp</div>
