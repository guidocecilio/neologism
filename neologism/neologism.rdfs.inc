<?php

/**
 * Implementation of hook_access().
 */
function neologism_rdfs_access($op, $node, $account) {
  // Currently, 'edit vocabularies' means you can do absolutely anything to any vocabulary.
  if ($op == 'create' || $op == 'update' || $op == 'delete') {
    return user_access('edit vocabularies', $account);
  }
} 

function neologism_rdfs_load($node) {

  // Load class/property-specific fields
  // These are in the evoc tables
  if ($node->type == NEOLOGISM_CT_CLASS) {
    $result = db_fetch_object(db_query("SELECT * FROM {evoc_rdf_classes} WHERE nid=%d", $node->nid));
    $result->superclasses = _neologism_fetch_objects('superclasses', 'superclass', $result->prefix, $result->id);
    $result->disjoints = _neologism_fetch_objects('disjointwith', 'disjointwith', $result->prefix, $result->id);
    unset($result->ndisjointwith);
  }
  else if ($node->type == NEOLOGISM_CT_PROPERTY) {
    $result = db_fetch_object(db_query("SELECT * FROM {evoc_rdf_properties} WHERE nid=%d", $node->nid));
    $result->superproperties = _neologism_fetch_objects('superproperties', 'superproperty', $result->prefix, $result->id);
    $result->domains = _neologism_fetch_objects('propertiesdomains', 'rdf_domain', $result->prefix, $result->id);
    $result->ranges = _neologism_fetch_objects('propertiesranges', 'rdf_range', $result->prefix, $result->id);
    $result->inverses = _neologism_fetch_objects('inversesproperties', 'inverseof', $result->prefix, $result->id);
  }

  // Load the vocabulary node
  $vocabulary_nid = db_result(db_query("SELECT nid FROM {neologism_vocabularies} WHERE prefix='%s'", $result->prefix));
  if (!$vocabulary_nid) {
    // HACK ALERT
    // The vocabulary will be NULL if we are just in the
    // process of deleting the vocabulary. In that case,
    // we get it from a global variable prepared in
    // neologism_delete().
    global $_neologism_vocabulary;
    $node->vocabulary = $_neologism_vocabulary;
  }
  $result->vocabulary = node_load($vocabulary_nid);

  // For convenience
  $result->qname = $result->vocabulary->prefix . ':' . $node->title;

  // Terms are stored by prefix:id in evoc, and in case we
  // change the prefix or id during editing, we need the
  // original values so we can update them in evoc.
  $result->original_id = $node->title;

  return $result;
}

function neologism_rdfs_view($node, $teaser, $page) {
  if ($page) {
    drupal_set_title($node->qname);
    drupal_set_breadcrumb(_neologism_build_breadcrumb($node->vocabulary->path, $node->vocabulary->title));
  }
}

function neologism_rdfs_form(&$node, $form_state) {

  if (!isset($node->vocabulary)) {
    drupal_set_message('Something went wrong! No vocabulary selected!', 'error');
    return array();
  }

  $class_or_property = substr($node->type, 4);
  $related_term = null;
  if (isset($node->nid)) {    // Are we editing an existing term?
    $mode = 'edit';
    drupal_set_title(t('Editing ' . $class_or_property . ' %qname', array('%qname' => $node->qname)));
    drupal_set_breadcrumb(_neologism_build_breadcrumb($node->vocabulary->path, $node->vocabulary->title));
  } else if (arg(4)) {    // Creating a new subclass, subproperty or inverse of an existing term ...
    // Select the term and mode if provided in the url of type node/1/mode/2
    $modes = array('sub', 'inverse');
    $mode = arg(3); 
    if (in_array($mode, $modes)) {
      $related_term = node_load(arg(4));
    }
    else {
      $mode = 'new';
    }
    drupal_set_breadcrumb(_neologism_build_breadcrumb());
  } else {    // We are creating a new term
    $mode = 'new';
    drupal_set_breadcrumb(_neologism_build_breadcrumb());
  }

  $form = array();

  $form['id'] = array(
      '#type' => 'textfield',
      '#title' => t(ucwords($class_or_property) . ' URI'),
      '#description' => t('Only letters, numbers, dashes, and underscores.'),
      '#size' => 20,
      '#field_prefix' => $node->vocabulary->namespace_uri,
      '#element_validate' => array('_neologism_validate_id'),
      '#attributes' => array('style' => 'width: auto; display: inline'),
  );

  $form['body_field'] = node_body_field($node, 'Body', 0);
  // Hide the "Split summary at cursor" widget
  $form['body_field']['teaser_include']['#access'] = FALSE;
  $form['body_field']['body']['#rows'] = 4;
  $form['body_field']['body']['#title'] = 'Details';
  $form['body_field']['body']['#description'] = 'Will only show in HTML views.';

  // We need to pass through some values so we can
  // use them during insert/update
  $form['qname'] = array('#type' => 'value', '#value' => $form['#node']->qname);
  $form['original_id'] = array('#type' => 'value', '#value' => $form['#node']->original_id);
  $form['vocabulary'] = array('#type' => 'value', '#value' => $form['#node']->vocabulary);

  return $form;
}

/**
 * Form validation handler for the neo_class node form.
 *
 * @param object $form
 * @param object $form_state
 * @return 
 */
function _neologism_form_validate_class($form, &$form_state) {

  // @todo: Check that a term with this ID doesn't exist yet in this vocab

    if(!empty($form_state['values']['field_superclass2']) ) {
      // TODO do a global select and then compare with the selected value. 
      // This way consume too much resources whether there exist many classes.
      foreach( $form_state['values']['field_superclass2'] as $value ) {
        // FIXME there is some error with external superclasses that doesn't has superclasses
        if( !evoc_term_exists(NULL, $value['evoc_term'], TRUE) ) {
            form_set_error('field_superclass2', t('You must select a super class from one of the class that match your criterial. If that class does not exists in the system, please import it.'));
        }
      }
    }
}
/**
 * Handler for validation form for the neo_property node form
 * 
 * @param object $form
 * @param object $form_state
 * @return 
 */
function _neologism_form_validate_property($form, &$form_state) {

  // @todo: Check that a term with this ID doesn't exist yet in this vocab

  // check for a new property
  //if( !$form_state['values']['nid'] ) {
    $vocabulary_nid = $form_state['values']['field_vocabulary'][0]['nid'];
    $property_title = $form_state['values']['title'];
    
    // check for the property's domain. if the class selected already exists in the system
    foreach( $form_state['values']['field_domain2'] as $value ) {
      if( !evoc_term_exists(NULL, $value['evoc_term'], TRUE) ) {
        form_set_error('field_domain2', t('You must select a domain from one of the class that match your criterial. If that class does not exists in the system, please import it.'));
      }
    }
    
    // if $form_state['values']['resource_type'] == '1' everithing come fine
    if( $form_state['values']['resource_type'] == '2' ) {
      $form_state['values']['field_range2'][0]['evoc_term'] = $form_state['values']['data_types'];
      $form_state['values']['field_inverse2'] = array();
    }
    else if( $form_state['values']['resource_type'] == '3' ) {
      $form_state['values']['field_range2'][0] = NULL;
    }
    
    // Comment by Guido Cecilio 2nd Febraury 2010
    // we check at validation time if there is no domain or range and add by default rdfs:Resource (DEFAULT_RDFS_SUPERCLASS)
    if( empty($form_state['values']['field_domain2']) ) {
        $form_state['values']['field_domain2'][0]['evoc_term'] = DEFAULT_RDFS_SUPERCLASS;
    }
    if( empty($form_state['values']['field_range2']) ) {
        $form_state['values']['field_range2'][0]['evoc_term'] = DEFAULT_RDFS_SUPERCLASS;
    }

    /*
    if( $form_state['values']['field_literal_as_range'][0]['value'] ) {
      $range = "rdfs:Literal";
      // this would be used when save the property to the repository
      $form_state['values']['field_range2'][0]['evoc_term'] = $range;
    }
    */
    
    /*
     * We don't need to check something that is filtered for the system. So we need to remove this 
     * commented piece of code afeter be sure of that
     * 
    // check for the property's ranges. if the range already exists in the system
    if( !empty($form_state['values']['field_range2']) ) {
      foreach( $form_state['values']['field_range2'] as $value ) {
        if( !evoc_term_exists(NULL, $value['evoc_term'], TRUE) ) {
          form_set_error('field_range2', t('You must select a range from one of the terms that match your criterial. If that class does not exists in the system, please import it.'));
        }
      }
    }
    */
    
    // check for the property's superproperty field. 
    foreach( $form_state['values']['field_superproperty2'] as $value ) {
      if( !evoc_term_exists(NULL, $value['evoc_term'], FALSE) ) {
        form_set_error('field_superproperty2', t('There is some error selecting your superproperty, the term does not exists in the system.'));
      }
    }
  //}
}
/*
function _neologism_form_alter_term(&$form, &$form_state, $class_or_property) {

  // Hide unused fields and widgets
  $form['field_vocabulary']['#access'] = FALSE;
  $form['buttons']['preview']['#access'] = FALSE;
  $form['menu']['#access'] = FALSE;
  $form['path']['#access'] = FALSE;
  $form['author']['#access'] = FALSE;
  $form['options']['#access'] = FALSE;
  $form['revision_information']['#access'] = FALSE;
  
  // Add a Cancel button
  $form['buttons']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#weight' => 20,
    '#attributes' => array('onclick' => 'window.location = \''.url($current_voc->path, array('absolute' => TRUE)).'\'; return false;'),
  );
  $form['buttons']['submit']['#submit'][] = '_neologism_term_form_submit';
  $form['buttons']['delete']['#submit'][] = '_neologism_term_form_delete';

  if ($class_or_property == 'class') {
    _neologism_form_alter_class($form, $form_state, $mode, $related_term);
  } else {
    _neologism_form_alter_property($form, $form_state, $mode, $related_term);
  }
}

function _neologism_form_alter_class(&$form, &$form_state, $mode, &$related_term) {

  if ($mode == 'edit') {
    // Tell the widget that we are editing a term
    $form['field_superclass2']['#mxcbs_behaviour']['editing_value'] = $form['#node']->qname;
    $form['field_disjointwith2']['#mxcbs_behaviour']['editing_value'] = $form['#node']->qname;
  } else if ($mode == 'sub') {
    // extra_values is an optional parameter for #mxcbs_behaviour attribute that contain extra default values for the widget tree
    $form['field_superclass2']['#mxcbs_behaviour']['extra_values'] = array($related_term->qname);
    $form['field_superclass2']['#mxcbs_behaviour']['execute_preprocess'] = TRUE;
  }
  
  $form['field_superclass2']['#mxcbs_behaviour']['owntitle'] = FALSE;
  $form['field_superclass2']['#mxcbs_behaviour']['handler'] = 'Neologism.createSuperclassSelecctionWidget';
  $form['field_superclass2']['#mxcbs_behaviour']['url_gateway'] = 'neologism/json/fulltree';

  $form['field_disjointwith2']['#mxcbs_behaviour']['owntitle'] = FALSE;
  $form['field_disjointwith2']['#mxcbs_behaviour']['handler'] = 'Neologism.createDisjointwithSelecctionWidget'; 
  $form['field_disjointwith2']['#mxcbs_behaviour']['url_gateway'] = 'neologism/json/fulltree'; 

  $form['#validate'][] = '_neologism_form_validate_class';
}

function _neologism_form_alter_property(&$form, &$form_state, $mode, &$related_term) {

  // add behaviours for evoc fields if they are using the MXCheckBox selection widget
  $form['field_domain2']['#mxcbs_behaviour']['owntitle'] = FALSE;
  $form['field_domain2']['#mxcbs_behaviour']['handler'] = 'Neologism.createDomainSelecctionWidget';
  $form['field_domain2']['#mxcbs_behaviour']['url_gateway'] = 'neologism/json/fulltree';
  
  $form['field_range2']['#mxcbs_behaviour']['owntitle'] = TRUE;
  $form['field_range2']['#mxcbs_behaviour']['handler'] = 'Neologism.createRangeSelecctionWidget';
  $form['field_range2']['#mxcbs_behaviour']['url_gateway'] = 'neologism/json/fulltree';
  $form['field_range2']['#prefix'] = '<div id="range-treeview">';
  $form['field_range2']['#suffix'] = '</div>';
  
  // add behaviours for evoc fields if they are using the MXCheckBox selection widget
  $form['field_superproperty2']['#mxcbs_behaviour']['owntitle'] = FALSE;
  $form['field_superproperty2']['#mxcbs_behaviour']['handler'] = 'Neologism.createSuperpropertySelecctionWidget';
  $form['field_superproperty2']['#mxcbs_behaviour']['url_gateway'] = 'neologism/json/propertiesfulltree';
  
  // add behaviours for field_inverse2 field
  $form['field_inverse2']['#mxcbs_behaviour']['owntitle'] = FALSE;
  $form['field_inverse2']['#mxcbs_behaviour']['handler'] = 'Neologism.createInverseSelecctionWidget';
  $form['field_inverse2']['#mxcbs_behaviour']['url_gateway'] = 'neologism/json/propertiesfulltree';
  $form['field_inverse2']['#prefix'] = '<div id="inverse-treeview">';
  $form['field_inverse2']['#suffix'] = '</div>';
  
  // add the group for ranges
  $form['range_group']['#type'] = 'fieldset';
  $form['range_group']['#weight'] = $form['field_range2']['#weight'] - 0.5;
  $form['range_group']['resource_type'] = array(
    '#type' => 'radios',
    '#title' => t('Range'),
    '#options' => array(
      1 => t('The value of this property is another resource.'),
      2 => t('The value of this property is a literal (string, number, date, ...).'),
      3 => t('The value of this property can be either.')
    ),
    //'#description' => t('Description should goes here....'),
    '#default_value' => 1,
    '#prefix' => '<div id="range-group-resourcetype">',
    '#suffix' => '</div>',
    '#attributes' => array('onClick' => 'Neologism.checkResourceType()'),
  );
  
  $form['range_group']['data_types'] = array(
    '#title' => '',//t('Data Types'),
    '#type' => 'select',
    '#description' => t('Select the datatype for this property.'),
    '#options' => neologism_get_literal_datatype_list(),
    '#multiple' => FALSE,
    '#prefix' => '<div id="range-group-datatypes">',
    '#suffix' => '</div>'
  );

  // Move Range selector into the fieldset
  $form['range_group']['field_range2'] = $form['field_range2'];
  unset($form['field_range2']);
 
  if ($mode == 'edit') {
    // why need to inform to the widget that there is comming an edition for a value, so the widget can
    // handle the situation.
    $form['field_domain2']['#mxcbs_behaviour']['editing_value'] = $form['#node']->qname;
    $form['field_range2']['#mxcbs_behaviour']['editing_value'] = $form['#node']->qname;
    $form['field_superproperty2']['#mxcbs_behaviour']['editing_value'] = $form['#node']->qname;
    $form['field_inverse2']['#mxcbs_behaviour']['editing_value'] = $form['#node']->qname;

    // set the default value depending of the current property range value
    $range = $form['#node']->field_range2[0]['evoc_term'];
    if( isset($range) && _neologism_is_literal_datatype($range) ) {
      $form['range_group']['data_types']['#default_value'] = $range;
      // set the radio button to The value of this property is a literal (string, number, date, ...).
      $form['range_group']['resource_type']['#default_value'] = 2;
    }
  }
  else if ($mode == 'sub') {
    // Check the related term in superproperty widget
    $form['field_superproperty2']['#mxcbs_behaviour']['extra_values'] = array($related_term->qname);
    $form['field_superproperty2']['#mxcbs_behaviour']['execute_preprocess'] = TRUE;
  }
  else if ($mode == 'inverse') {
    // Check the related term in inverse widget
    $form['field_inverse2']['#mxcbs_behaviour']['extra_values'] = array($related_term->qname);
    $form['field_inverse2']['#mxcbs_behaviour']['execute_preprocess'] = TRUE;
  }

  $form['#validate'][] = '_neologism_form_validate_property';
}
*/


/**
 * Callback for submission of the class/property forms. Redirects to the vocabulary.
 */
function _neologism_term_form_submit($form, &$form_state) {
  $form_state['redirect'] = 'node/'.$form_state['values']['field_vocabulary'][0]['nid'];
}

function _neologism_term_form_delete($form, &$form_state) {
  $path = $form['#node']->vocabulary->path;
  $form_state['redirect'] = array('node/' . $form['#node']->nid . '/delete', array('destination' => $path));
}

/*
    case 'update':
      // Update the term in the evoc repository
      $term = _neologism_make_evoc_term_array($node);
      if ($node->type == NEOLOGISM_CT_CLASS) {
        evoc_update_class($term);  
      }
      else if ($node->type == NEOLOGISM_CT_PROPERTY) {
        evoc_update_property($term);
      }

      // update revision date of the vocabulary
      _neologism_vocabulary_save($node->vocabulary);
      break;
    
    case 'insert':
      // load vocabulary node
      $node->vocabulary = node_load($node->field_vocabulary[0][nid]);

      // Insert the term into the evoc repository
      $term = _neologism_make_evoc_term_array($node);
      if ($node->type == NEOLOGISM_CT_CLASS) {
        evoc_write_class($term);  
      }
      else if ($node->type == NEOLOGISM_CT_PROPERTY) {
        evoc_write_property($term);
      }

      // update revision date of the vocabulary
      _neologism_vocabulary_save($node->vocabulary);
      break;
   
    case 'delete':
      if ($node->type == NEOLOGISM_CT_CLASS) {
        // Check for other classes or properties that reference
        // this class as a superclass, disjoint class, domain or range.
        _neologism_delete_evoc_references('field_superclass2', $node->qname, true);
        _neologism_delete_evoc_references('field_disjointwith2', $node->qname, true);
        _neologism_delete_evoc_references('field_domain2', $node->qname, true);
        _neologism_delete_evoc_references('field_range2', $node->qname, true);
        evoc_remove_class(array('prefix' => $node->vocabulary->prefix, 'id' => $node->title));
      }
      else if ($node->type == NEOLOGISM_CT_PROPERTY) {
        // Check for other properties that reference
        // this property as a superproperty or inverse.
        _neologism_delete_evoc_references('field_superproperty2', $node->qname, true);
        _neologism_delete_evoc_references('field_inverse2', $node->qname, true);
        evoc_remove_property(array('prefix' => $node->vocabulary->prefix, 'id' => $node->title));
      }

      // update revision date of the vocabulary
      _neologism_vocabulary_save($node->vocabulary);
      break;
  }
}
*/

function _neologism_fetch_objects($table, $column, $prefix, $id) {
  $result = array();
  $q = db_query("SELECT $column FROM {evoc_rdf_" . $table . "} WHERE prefix='%s' AND reference='%s'", $prefix, $id);
  while ($a = db_fetch_array($super)) {
    $result[] = $a[$column];
  }
  return $result;
}
