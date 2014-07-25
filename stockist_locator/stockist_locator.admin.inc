<?php

/**
 * @file
 * Admin page callbacks for the stockist_locator module.
 */

/**
 * Admin settings form for stockist_locator.
 */
function stockist_locator_admin_settings() {
  $form['message'] = array(
    '#title'       => t('Stockist locator default location for the centre of the map'),
    '#type'        => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
  );
  $form['message']['stockist_locator_longitude'] = array(
    '#title'            => t('Longitude'),
    '#type'             => 'textfield',
    '#size'             => 20,
    '#maxlength'        => 15,
    '#required'         => TRUE,
    '#default_value'    => variable_get('stockist_locator_longitude', ''),
    '#element_validate' => array('element_validate_number'),
    '#description'      => t('Enter longitude for the centre of the map'),
  );
  $form['message']['stockist_locator_latitude'] = array(
    '#title'            => t('Latitude'),
    '#type'             => 'textfield',
    '#size'             => 20,
    '#maxlength'        => 15,
    '#required'         => TRUE,
    '#default_value'    => variable_get('stockist_locator_latitude', ''),
    '#element_validate' => array('element_validate_number'),
    '#description'      => t('Enter latitude for the centre of the map'),
  );
  $form['options'] = array(
    '#type'             => 'fieldset',
    '#collapsible'      => FALSE,
    '#collapsed'        => FALSE,
  );
  $form['options']['stockist_locator_radius'] = array(
    '#type'             => 'textfield',
    '#title'            => t('Searching Radius in KM'),
    '#size'             => 5,
    '#maxlength'        => 8,
    '#element_validate' => array('element_validate_integer_positive'),
    '#default_value'    => variable_get('stockist_locator_radius', '5'),
    '#description'      => t('Enter Searching radius value, map will show stockists within this radius of location entered.'),
  );
  // Image icon.
  $form['options']['stockist_locator_icon_image'] = array(
    '#type'                    => 'managed_file',
    '#name'                    => 'icon_image',
    '#title'                   => t('Stockist locator icon image'),
    '#default_value'           => variable_get('stockist_locator_icon_image', ''),
    '#description'             => t("Here you can upload an icon used to indicate the locations of stockists. Allowed extensions: png"),
    '#upload_validators'       => array(
      'file_validate_extensions' => array('png'),
    ),
    '#required'                => TRUE,
    '#upload_location'         => 'public://images_stockist_locator/',
  );
  return system_settings_form($form);
}
/**
 * Validation function for the general configuration form.
 */
function stockist_locator_admin_settings_validate($form, &$form_state) {
  // Validate icon image selection.
  if (!isset($form_state['values']['stockist_locator_icon_image']) || $form_state['values']['stockist_locator_icon_image'] == 0) {
    form_set_error('stockist_locator_icon_image', t('Please select an icon image to upload.'));
  }
}
/**
 * CCV export function.
 */
function stockist_locator_csv_export() {
  $custom_content_type  = "stockist_locator";
  $fields               = array();
  $fields               = field_info_instances("node", "stockist_locator");
  $headers              = array();
  $output               = "";
  $lang                 = "";
  // Display only text, integer and term reference field types allowed.
  // in export script to restrict additional fields dynamically.
  $field_types  = array("text", "list_text", "taxonomy_term_reference");
  if (!empty($fields)) {
    $headers[]  = "\"Stockist Name\"";
    foreach ($fields as $field) {
      if (in_array($field['type'], $field_types)) {
        $headers[] = "\"" . $field['label'] . "\"";
      }
    }
    $headers[] = "\"Language\"";
    drupal_add_http_header('Content-Type', 'text/csv');
    drupal_add_http_header('Content-Disposition', 'attachment;filename=export_stockist_locator.csv');
    $output .= implode(",", $headers) . "\n";
    $results = db_query('SELECT nid FROM {node} WHERE type=:type order by nid', array(':type' => $custom_content_type))->fetchAll();
    foreach ($results as $result) {
      $record = array();
      $node = node_load($result->nid);
      $record[] = "\"{$node->title}\"";
      $lang = $node->language;
      foreach ($fields as $field) {
        if (in_array($field['type'], $field_types)) {
          if ($field['type'] == 'taxonomy_term_reference') {
            $term = taxonomy_term_load($node->{$field['field_name']}[LANGUAGE_NONE]['0']['tid']);
            if ($term) {
              $record[] = "\"{$term->name}\"";
            }
            else {
              $record[] = "\"\"";
            }
          }
          elseif ($field['type'] == 'list_text') {
            // This will be country field.
            $country_name = stockist_locator_get_country_name($node->{$field['field_name']}[LANGUAGE_NONE]['0']['value']);
            $record[] = "\"{$country_name}\"";
          }
          else {
            if (isset($node->{$field['field_name']})) {
              $record[] = "\"{$node->{$field['field_name']}[LANGUAGE_NONE]['0']['value']}\"";
            }
          }
        }
      }
      $record[] = "\"{$lang}\"";
      $output .= implode(",", $record) . "\n";
    }
    print $output;
    drupal_exit();
  }
}
