<?php

/**
 * @file
 * Admin page callbacks for the stockist_locator module.
 */

/**
 * Implements hook_form().
 */
function stockist_locator_ajax_form($form, &$form_state) {
  drupal_add_css(drupal_get_path('module', 'stockist_locator') . '/css/stockist_locator.css');
  $stockist_categories = stockist_locator_term_options('stockist_category');
  $form['stockist_locator_frontend_category'] = array(
    '#type'    => 'checkboxes',
    '#title'    => t('Type of Stockist:'),
    '#options'    => stockist_locator_term_options('stockist_category'),
    '#default_value'    => array_keys($stockist_categories),
    '#attributes'    => array('onclick' => 'customSearch()'),
  );

  $form['zipcode'] = array(
    '#type'          => 'textfield',
    '#required'      => FALSE,
    '#maxlength'     => 255,
    '#size'          => 42,
    '#default_value' => t('Please enter your postal code, and your city'),
    '#attributes'    => array('onclick' => 'hideText()', 'onblur' => 'hideText()'),
  );

  $form['add_button'] = array(
    '#type'          => 'label',
    '#prefix'        => '<div class="stockist-button"><button type="button" onclick="customSearch();" >' . t('Find nearest stockists') . '</button>',
    '#suffix'        => '</div>',
  );
  return $form;
}

/**
 * Implements custom ajax call.
 */
function stockist_getresultxml() {
  // Empty variables.
  $categories = $zipcode = $lat = $lng = $baseqry = $qrycategory = $qryradius = '';
  $get = drupal_get_query_parameters();
  $page       = $get['page'];
  $radius     = variable_get('stockist_locator_radius');

  if ($radius == '') {
    $radius = '5';
  }
  $units = '6371';
  if ($page == 'first') {
    $lat = $get['lat'];
    $lng = $get['lng'];
  }
  elseif ($page == 'second') {
    $zipcode    = $get['zipcode'];
    $categories = $get['categories'];

    if ($zipcode == '') {
      $lat = variable_get('stockist_locator_latitude', '');
      $lng = variable_get('stockist_locator_longitude', '');
    }
    else {
      // Get lat and long by address.
      $address  = $zipcode;
      $prepaddr = str_replace(' ', '+', $address);
      $geocode  = file_get_contents('http://maps.google.com/maps/api/geocode/json?address=' . $prepaddr . '&sensor=false');
      $output   = json_decode($geocode);
      $lat      = $output->results[0]->geometry->location->lat;
      $lng      = $output->results[0]->geometry->location->lng;
    }
  }

  $baseqry = "SELECT node.nid as Nid, node.title as storename, 
			sla1.stockist_locator_address1_value as address1,
			sla2.stockist_locator_address2_value as address2,
			slcat.stockist_locator_category_tid as category,
			sllo.stockist_locator_longitude_value as longitude,
			slla.stockist_locator_latitude_value as latitude,
			slpc.stockist_locator_postal_code_value as zipcode, 
			slc.stockist_locator_city_value as city, 
			sls.stockist_locator_state_value as state,
			slp.stockist_locator_phone_value as phone,
			sle.stockist_locator_email_value as email,
			slwu.stockist_locator_website_url_value as website_url,
			( " . $units . " * acos( cos( radians('" . $lat . "') ) * cos( radians( slla.stockist_locator_latitude_value ) ) * cos( radians( sllo.stockist_locator_longitude_value )
			- radians('" . $lng . "') ) + sin( radians('" . $lat . "') ) * sin( radians( slla.stockist_locator_latitude_value ) ) ) ) AS distance FROM {node} node
			LEFT JOIN {field_data_stockist_locator_category} slcat ON node.nid = slcat.entity_id AND (slcat.entity_type = 'node' AND slcat.deleted = '0')
			LEFT JOIN {field_data_stockist_locator_address1} sla1 ON node.nid = sla1.entity_id AND (sla1.entity_type = 'node' AND sla1.deleted = '0')
			LEFT JOIN {field_data_stockist_locator_address2} sla2 ON node.nid = sla2.entity_id AND (sla2.entity_type = 'node' AND sla2.deleted = '0')
			LEFT JOIN {field_data_stockist_locator_longitude} sllo ON node.nid = sllo.entity_id AND (sllo.entity_type = 'node' AND sllo.deleted = '0')
            LEFT JOIN {field_data_stockist_locator_latitude} slla ON node.nid = slla.entity_id AND (slla.entity_type = 'node' AND slla.deleted = '0')
            LEFT JOIN {field_data_stockist_locator_postal_code} slpc ON node.nid = slpc.entity_id AND (slpc.entity_type = 'node' AND slpc.deleted = '0')
            LEFT JOIN {field_revision_stockist_locator_city slc} ON node.nid = slc.entity_id AND (slc.entity_type = 'node' AND slc.deleted = '0')
            LEFT JOIN {field_revision_stockist_locator_state sls} ON node.nid = sls.entity_id AND (sls.entity_type = 'node' AND sls.deleted = '0')
            LEFT JOIN {field_revision_stockist_locator_phone slp} ON node.nid = slp.entity_id AND (slp.entity_type = 'node' AND slp.deleted = '0')
            LEFT JOIN {field_revision_stockist_locator_email sle} ON node.nid = sle.entity_id AND (sle.entity_type = 'node' AND sle.deleted = '0')
            LEFT JOIN {field_revision_stockist_locator_website_url} slwu ON node.nid = slwu.entity_id AND (slwu.entity_type = 'node' AND slwu.deleted = '0')
			WHERE node.status = '1' AND node.type ='stockist_locator' ";
  if ($categories != '') {
    $qrycategory = "and slcat.stockist_locator_category_tid in(" . $categories . ") ";
  }

  $qryradius = "HAVING distance < '" . $radius . "' ORDER BY distance";
  $finalqry = $baseqry . $qrycategory . $qryradius;
  $result   = db_query($finalqry);
  $count    = $result->rowCount();

  if ($count > 0) {
    $collection = $result;
  }
  else {
    $qryany5    = $baseqry . $qrycategory . " HAVING distance > '" . $radius . "' ORDER BY distance ASC LIMIT 0,5 ";
    $result     = db_query($qryany5);
    $collection = $result;
  }

  $str = '';
  if (!empty($collection)) {
    // Xml format creation.
    $dom     = new DOMDocument('1.0', 'utf-8');
    $node    = $dom->createElement('markers');
    $parnode = $dom->appendChild($node);

    try {
      $i = 0;
      foreach ($collection as $loc) {
        $node    = $dom->createElement('marker');
        $newnode = $parnode->appendChild($node);
        $newnode->setAttribute('marker_label', '');
        foreach ($loc as $k => $v) {
          if ($k == 'category') {
            $url  = stockist_locator_category_image($v);
            $term = taxonomy_term_load($v);
            $newnode->setAttribute('category_url', $url);
            $newnode->setAttribute('category_name', $term->name);
          }
          else {
            $newnode->setAttribute($k, $v);
          }
        }
        if ($page == 'second') {
          $newnode->setAttribute('custom_lng', $lng);
          $newnode->setAttribute('custom_lat', $lat);
        }
      }
    }
    catch (Exception $e) {
      $node    = $dom->createElement('error');
      $newnode = $parnode->appendChild($node);
      $newnode->setAttribute('message', $e->getMessage());
    }
    $str = str_replace(array("\r", "\n"), '', $dom->saveXml());
  }
  echo $str;
}

/**
 * Implements custom function to fetch category url from $tid.
 */
function stockist_locator_category_image($tid) {
  $icon = variable_get('stockist_locator_icon_image');
  if (isset($icon)) {
    $file_obj = file_load($icon);
    if ($file_obj) {
      $icon = file_create_url($file_obj->uri);
    }
  }
  $term = taxonomy_term_load($tid);
  if (isset($term->field_category_icon)&&!empty($term->field_category_icon)) {
    if (!empty($term->field_category_icon['und'][0]['uri'])) {
      $icon = file_create_url($term->field_category_icon['und'][0]['uri']);
    }
  }
  return $icon;
}
