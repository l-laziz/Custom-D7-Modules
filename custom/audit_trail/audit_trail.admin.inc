<?php

/**
 * @file
 * Administration page callbacks for the user track module.
 */

/**
 * Form builder. Configure annotations.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function audit_trail_admin_settings() {
  $form['other_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Other'),
    '#weight' => 6,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['other_fieldset']['audit_trail_ip_address'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track IP address'),
    '#description' => 'Check to track IP address.',
    '#default_value' => variable_get('audit_trail_ip_address', FALSE),
  );
  $form['email_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Email settings'),
    '#weight' => 5,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['email_fieldset']['audit_trail_send_email'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable notification'),
    '#description' => 'Check to enable email notification on failed login.',
    '#default_value' => variable_get('audit_trail_send_email', FALSE),
  );
  $form['email_fieldset']['audit_trail_emails'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#description' => 'Enter email address to send to. If multiple addresses, separate by comma followed by space.',
    '#default_value' => variable_get('audit_trail_emails', NULL),
     // Set this element enabled if the "Enable notification" checkbox above is checked.
    '#states' => array(
      // Action to take.
      'visible' => array(
        ':input[name="audit_trail_send_email"]' => array('checked' => TRUE),
      ),
    ),
  );
  
  return system_settings_form($form);
}

/**
 * Validate audit_trail_admin_settings() form values on submit.
 */
function audit_trail_admin_settings_validate($form, &$form_state) {
  $emails = $form_state['values']['audit_trail_emails'];
  $send_enable = $form_state['values']['audit_trail_send_email'];
  if($send_enable == TRUE && empty($emails)){
    form_set_error('audit_trail_emails','Please enter email address or disable notification');
  }
  if(!empty($emails)) {
    //Run through isEmail() custom email vaidation function.
    isEmail($emails);
  }
}

/**
 * Custom Validation for email address.
 */
function isEmail($address) {
  $invalid = array();
  // @explode() - Splits string by comma into an array.
  $emailAddresses = explode(",", $address);

  // Iterates over each array value.
  foreach($emailAddresses as $emailAdd) {
    $emailAdd = trim($emailAdd);
    // Test input whether it's valid address.
    if (empty($emailAdd) || !filter_var($emailAdd, FILTER_VALIDATE_EMAIL)) {
    // if validation fails, add failed email to an array.
    $invalid[] = $emailAdd;
    }
  }
  // No invalid address found in an array.
  if(empty($invalid)) {
    return true;
  }
  elseif (count($invalid) == 1) {
  form_set_error('audit_trail_emails', t('%email is not a valid e-mail address.', array('%email' => reset($invalid))));
  }
  else {
   form_set_error('audit_trail_emails', t('%emails are not valid e-mail addresses.', array('%emails' => implode(', ', $invalid))));
  }
}

/**
 * Display results in Table format.
 */
function audit_trail_report_callback($account = NULL) {
  //Configure sortable $header columns;
  $header = array(array('data' => 'Username', 'field' => 'name'),
    array('data' => 'Date/Time','field' => 'access','sort' => 'desc'),
    array('data' => 'Status','field' => 'success'),
  );
  $rows = array();
  // Set the database table for query.
  $query = db_select('audit_trail', 'at')
    ->fields('at')
    ->extend('PagerDefault')->limit(10)
    ->extend('TableSort')->orderByHeader($header);
    // Execute the query.
  $result = $query->execute();
  // Put the query result objects into arrays. 
  foreach ($result as $record) {
    $rows[] = array(
      $record->name,
      format_date($record->access),
      //check_plain($record->ip),
      $record->success,
    );
  }
  // Output the table.
  $output = theme('table', array('header' => $header, 'rows' => $rows));
  // Output the pager.
  $output .= theme('pager');

  return $output;
}