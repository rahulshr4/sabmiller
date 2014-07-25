<?php

/**
 * @file
 * Admin functionality for age_checker module.
 */

/**
 * Form callback for age_checker admin settings.
 */
function age_checker_admin_form() {
  $form = array();
  // Message options for age checker.
  $form['message'] = array(
    '#title'       => t('Age Checker Message'),
    '#type'        => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
  );
  // Message Before the Form.
  $message_beforeform = variable_get('age_checker_before_form_text', array('value' => '', 'format' => NULL));
  $form['message']['age_checker_before_form_text'] = array(
    '#title'         => t('Enter before text'),
    '#type'          => 'text_format',
    '#rows'          => 6,
    '#default_value' => $message_beforeform['value'] ? $message_beforeform['value'] : '',
    '#description'   => t('The message displayed before the Form.'),
    '#format'        => $message_beforeform['format'],
  );
  // Message After the Form.
  $message_afterform = variable_get('age_checker_after_form_text', array('value' => '', 'format' => NULL));
  $form['message']['age_checker_after_form_text'] = array(
    '#title'         => t('Enter after Text'),
    '#type'          => 'text_format',
    '#rows'          => 6,
    '#default_value' => $message_afterform['value'] ? $message_afterform['value'] : '',
    '#description'   => t('The message displayed after the Form'),
    '#format'        => $message_afterform['format'],
  );
  // Verification options for age checker.
  $form['options'] = array(
    '#title'         => t('Age Checker Options'),
    '#type'          => 'fieldset',
    '#collapsible'   => TRUE,
    '#collapsed'     => FALSE,
  );
  // Minimum Age Checker.
  $age_checker_threshold_age = variable_get('age_checker_threshold_age', 18);
  $form['options']['age_checker_threshold_age'] = array(
    '#title'            => t('Enter threshold age limit in whole years ie. 18'),
    '#type'             => 'textfield',
    '#size'             => 3,
    '#element_validate' => array('element_validate_integer'),
    '#default_value'    => isset($age_checker_threshold_age) ? $age_checker_threshold_age : '18',
    '#description'      => t('The minimum age a user can be to pass the age checker.'),
  );
  // Cookie Expiration Time.
  $age_checker_cookie_expiration_time = variable_get('age_checker_cookie_expiration_time', '15');
  $form['options']['age_checker_cookie_expiration_time'] = array(
    '#title'            => t('Cookie expiration days'),
    '#type'             => 'textfield',
    '#field_suffix'     => t('Days'),
    '#size'             => 6,
    '#element_validate' => array('element_validate_integer'),
    '#default_value'    => isset($age_checker_cookie_expiration_time) ? $age_checker_cookie_expiration_time : '15',
    '#description'      => t('The number of days before the cookie set by age checker module expires, and the user must verify their age again (0 days will expire at end of session).'),
  );
  // Age Checker URL.
  $age_checker_url = variable_get('age_checker_url', '');
  $form['options']['age_checker_url'] = array(
    '#title'         => t('Enter underage page url'),
    '#type'          => 'textfield',
    '#default_value' => isset($age_checker_url) ? $age_checker_url : '',
    '#required'      => TRUE,
    '#description'   => t('Please add http:// or https:// for external url  or create a drupal CMS page and enter Drupal path for internal CMS Page. E.g "under-age" for  http://www.example.com/sitename/under-age'),
  );
  // Age Checker Visibility.
  $age_checker_visibility = variable_get('age_checker_visibility', AGE_CHECKER_VISIBILITY_NOTLISTED);
  $form['options']['age_checker_visibility'] = array(
    '#type'       => 'radios',
    '#title'      => t('Show Age Gate on specific pages'),
    '#options'    => array(
      AGE_CHECKER_VISIBILITY_NOTLISTED => t('Show on all pages except those listed'),
      AGE_CHECKER_VISIBILITY_LISTED    => t('Show only on the listed pages'),
    ),
    '#default_value' => isset($age_checker_visibility) ? $age_checker_visibility : AGE_CHECKER_VISIBILITY_NOTLISTED,
  );
  // Age checker specific pages.
  $age_checker_pages = variable_get('age_checker_pages', 'admin/*');
  $form['options']['age_checker_pages'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Type Age Gate specific pages'),
    '#default_value' => isset($age_checker_pages) ? $age_checker_pages : 'admin/*\nuser',
    '#description'   => t("Specify pages by using their paths. Enter one path per line. The '*' character is a wildcard. Example paths are blog for the blog page and blog/* for every personal blog. <front> is the front page."),
  );
  // Date format options.
  $age_checker_date_format = variable_get('age_checker_date_format', '0');
  $form['options']['age_checker_date_format'] = array(
    '#type'     => 'radios',
    '#title'    => t('Age Checker Date Format'),
    '#options'  => array(
      '0'  => t('dd/mm/yyyy'),
      '1'  => t('mm/dd/yyyy'),
    ),
    '#default_value' => isset($age_checker_date_format) ? $age_checker_date_format : 0,
  );
  // Age checker validation message.
  $form['options']['age_checker_blank_error_msg'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Age Checker Blank Validation Message'),
    '#required'      => TRUE,
    '#maxlength'     => 255,
    '#default_value' => variable_get('age_checker_blank_error_msg', 'Date field can not be blank.'),
    '#description'   => t('Please enter message text to populate in age checker validation.'),
  );
  $form['options']['age_checker_dateformat_error_msg'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Incorrect Date Format Validation Message'),
    '#required'      => TRUE,
    '#maxlength'     => 255,
    '#default_value' => variable_get('age_checker_dateformat_error_msg', 'Please enter your date of birth in correct format.'),
    '#description'   => t('Please enter message text to populate in age checker validation.'),
  );
  $form['options']['age_checker_daterange_error_msg'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Date Out Of Range Validation Message'),
    '#required'      => TRUE,
    '#maxlength'     => 255,
    '#default_value' => variable_get('age_checker_daterange_error_msg', 'Please enter your date of birth in valid date range.'),
    '#description'   => t('Please enter message text to populate in age checker validation.'),
  );
  // Error message for under age.
  $form['options']['age_checker_underage_error_msg'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Under Age Validation Message'),
    '#required'      => TRUE,
    '#maxlength'     => 255,
    '#default_value' => variable_get('age_checker_underage_error_msg', 'Sorry, you are Under age limit and are prohibited from entering this site!'),
    '#description'   => t('Please enter error message for under age checker validation'),
  );
  // Age checker submit button text.
  $form['options']['age_checker_button_text'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Button Text'),
    '#maxlength'     => 50,
    '#required'      => TRUE,
    '#default_value' => variable_get('age_checker_button_text', 'Begin your discovery'),
    '#description'   => t('Please enter submit button text.'),
  );
  // Images Age Checker.
  $age_checker_background_image = variable_get('age_checker_background_image', '');
  $form['options']['age_checker_background_image'] = array(
    '#type'                    => 'managed_file',
    '#name'                    => 'backgroundimage_image',
    '#title'                   => t('Change the background'),
    '#default_value'           => isset($age_checker_background_image) ? $age_checker_background_image : '',
    '#description'             => t("Here you can upload an image to the background!"),
    '#upload_location'         => 'public://images_age_checker/',
    '#upload_validators'       => array(
      'file_validate_extensions' => array('gif png jpg jpeg'),
      'file_validate_size'       => array(1000000),
    ),
  );
  return system_settings_form($form);
}
