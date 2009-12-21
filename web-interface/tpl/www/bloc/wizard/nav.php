<?php
$step = $this->get_var('step');
$form = &$this->get_module('form');
?>

<div id="wz-nav-bar">
	<div id="wz-nav-prev">
		<?php
		if(($step !== 'welcome')
			&& (($step !== 'send') || (($step === 'send') && ($this->get_var('send-result') === false))))
		{
			echo $form->submit(array('name' => 'prev',
									'value' => $this->bbf('prev'),
									'paragraph' => false));
		}
		else
			echo "&nbsp;";
		?>
	</div>
	<div id="wz-nav-step">
	<?php
		echo $this->bbf('wz-steps-' . $this->get_var('step'));
	?>
	</div>
	<div id="wz-nav-next">
		<?php
		if($step !== 'send')
			echo $form->submit(array('name' => 'next',
									'value' => $this->bbf('next'),
									'paragraph' => false));
		else
			echo "&nbsp;";
		?>
	</div>
</div>

