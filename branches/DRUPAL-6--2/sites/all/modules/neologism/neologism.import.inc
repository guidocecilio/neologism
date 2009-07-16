<?php

function neologism_import_form($form_state) {

/// import test for myclass evoc reference
        $node_type = 'myclass';
        $node['values']['title'] = 'title test import myclass';
        $node['values']['body'] = '';
        $node['values']['field_superclass'][0]['evoc_term']['evoc_term'] = 'sioc:User';
//        $node['values']['field_evoc'][0]['evoc_term'] = 'sioc:Item';
        $node['values']['field_text'][0]['value'] = 'text value imported';
        $node['values']['field_node'][0]['nid']['nid'] = 'test import myclass';
        $node['values']['field_user'][0]['uid']['uid'] = 'admin';
        //$node['values']['field_comment'][0]['value'] = 'new comment here';
        // Nodereference field use the nid key instead of value.
	
///    neologism_create_node($node_type, $node);
        
/// end of import test        
        
	$form['vocabulary_uri'] = array(
    '#type' => 'textfield',
    '#title' => t('Vocabulary URI'),
    '#required' => TRUE,
    '#default_value' => $form_state['values']['vocabulary_uri'],
    '#description' => "Enter the URI of the vocabulary to import in Neologism. Make sure it finishes by either / or #.",
  );
  $form['prefix'] = array(
    '#type' => 'textfield',
    '#title' => t('Prefix'),
    '#required' => TRUE,
    '#default_value' => $form_state['values']['prefix'],
    '#description' => "Choose a prefix for this vocabulary. This prefix will be used by default in Neologism.",
  );
  $form['ns_uri'] = array(
    '#type' => 'textfield',
    '#title' => t('Namespace URI'),
    '#required' => FALSE,
    '#default_value' => isset($form_state['values']['ns_uri']) ? $form_state['values']['ns_uri'] : NULL,
    '#description' => "Do not use unless the vocabulary defines a different namespace than the one above. Make sure it finishes by either / or #.",
  );
  $form['vocabulary_id'] = array(
    '#type' => 'select',
    '#title' => t('Vocabulary to import in'),
    '#required' => TRUE,
    '#default_value' => $form_state['values']['vocabulary_id'],
    '#options' => array('<create>', 1),
    '#description' => 'Select the existing vocabulary to import the new vocabulary in. If you want to create a new vocabulary, select &lt;create&gt;.',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );
  return $form;
}

function neologism_import_form_submit($form, &$form_state) {
  $form_state['storage']['values'] = $form_state['values'];
  $form_state['rebuild'] = true;

  $fetched_terms = evoc_fetch_vocabulary($form_state['values']['vocabulary_uri'], $form_state['values']['prefix'], TRUE, $form_state['values']['ns_uri']);

  // TODO auto create a new vocab and the options in the form to alter an existing voc.
$vocabulary_id = '1';

  // A vocabulary representation is built in memory.
  $voc = array();

  // Create a node for each term belonging to the vocabulary.
  foreach ($fetched_terms as $term) {
//var_dump($term);
    // Cleanup the terms in a vocabulary array.
    $tid = $term['id'];
    // Remove the id if found in the label.
//     if ($term['label'] == $tid) {
//       $term['label'] = NULL;
//     }
    // TODO this might not be necessary since evoc does it.
    $term['label'] = $term['label'] ? $term['label'] : $term['id'];
    // TODO check this, looks we are losing the term if TRUE
    if ($voc[$tid] && $term['label']) {
      // Concatenate the label information (BIRF).
      $voc[$tid]['label'] .= '. ' . $term['label'];
    }
    else {
      $voc[$tid] = $term;
    }

  }
//var_dump($voc);

neologism_create_vocabulary($vocabulary_id, $voc);

}

// TODO
function neologism_create_vocabulary ($vocabulary_id, $voc) {
  foreach ($voc as $term) {
//  var_dump($term);
    switch ($term['type']) {
      // Store the term as a class in memory.
      case 'class' :
        $node_type = NEOLOGISM_CT_CLASS;
        $node['values']['title'] = $term['id'];
        $node['values']['body'] = '';
        $node['values']['field_label'][0]['value'] = $term['label'];
        $node['values']['field_superclass'][0]['value'] = $term['superclass'];
        //$node['values']['field_comment'][0]['value'] = 'new comment here';
        // Nodereference field use the nid key instead of value.
        $node['values']['field_vocabulary'][0]['nid'] = $vocabulary_id;
        break;

      case 'property' :
        $node_type = NEOLOGISM_CT_PROPERTY;
        $node['values']['title'] = $term['id'];
        $node['values']['body'] = '';
        $node['values']['field_label'][0]['value'] = $term['id'];
        $node['values']['field_domain2'][0]['value'] = $term['domain'];
        $node['values']['field_range2'][0]['value'] = $term['range'];
        //$node['values']['field_comment'][0]['value'] = 'new comment here';
        // Nodereference field use the nid key instead of value.
        $node['values']['field_vocabulary'][0]['nid'] = $vocabulary_id;
        break;
    }

    neologism_create_node($node_type, $node);


  }
}


function neologism_create_node($node_type, $node_values) {
  module_load_include('inc', 'node', 'node.pages');
  $node = array('type' => $node_type);
// when editing a node, use
// $node = node_load(12);

  if (is_array($node_values)) {
    $node_values['values']['op'] = t('Save');
  }
//print_r($node_values);
  drupal_execute($node_type . '_node_form', $node_values, (object)$node);



}



