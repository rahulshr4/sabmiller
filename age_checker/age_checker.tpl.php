<?php

/**
 * @file
 * Default template file for age gate popup.
 *
 * Available variables:
 * - $header: The header text to output in the popup.
 * - $message: The age verification message to display to the user.
 * - $widget: The age verification widget.
 */
  ?>
  
<div id="age_checker_verification_popup">
  <div id="age_checker_message">
    <?php print $beforemessage; ?>
  </div>
  <div id="age_checker_widget">
    <?php print $widget; ?>
  </div>
   <div id="age_checker_message">
    <?php print $aftermessage; ?>
  </div>
</div>
