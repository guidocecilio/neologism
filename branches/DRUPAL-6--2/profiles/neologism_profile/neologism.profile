<?php
// $Id:  Exp $

/**
 * @file
 * Install Profile for Neologism 
 */
 
/**
 * Return an array of the modules to be enabled when this profile is installed.
 *
 * @return
 *  An array of modules to be enabled.
 */
function neologism_profile_modules() {
  return array(
    // Core - optional
    'color', 'help', 'menu', 
    'path', 
    'taxonomy', 'dblog',

    // Core - required
    'block', 'filter', 'node', 'system', 'user',

    // CCK core
    'content', 'nodereference', 'optionwidgets', 'text', 'userreference', 'content_copy', 'fieldgroup',
    
    // Contrib
    'rdf', 
    //'sparql', 
    //'evoc', 
    //'evocreference', 'ext', 'mxcheckboxselect',
    
    // Neologism
    //'neologism',
  );
}

/**
 * Return a description of the profile for the initial installation screen.
 *
 * @return
 *   An array with keys 'name' and 'description' describing this profile.
 */
function neologism_profile_details() {
  return array(
    'name' => 'Neologism',
    'description' => 'Tool to easily create and publish RDF vocabularies online.'
  );
}

/**
 * Return a list of tasks that this profile supports.
 *
 * @return
 *   A keyed array of tasks the profile will perform during
 *   the final stage. The keys of the array will be used internally,
 *   while the values will be displayed to the user in the installer
 *   task list.
 */
function neologism_profile_task_list() {
}


/**
 * Perform any final installation tasks for this profile.
 *
 * @return
 *   An optional HTML string to display to the user on the final installation
 *   screen.
 */
function neologism_profile_tasks(&$task, $url) {
  module_rebuild_cache();
  
  $modules_list = array(
    'sparql',
    'evoc', 
    'ext', 'evocreference', 'mxcheckboxselect',
    //'neologism'
  );
  
  drupal_install_modules($modules_list);
  drupal_install_modules(array('neologism'));
  
  variable_set('ext_path', drupal_get_path('module', 'ext') .'/ext-3.0.0');
  
  /*
  module_enable(array(
    'sparql',
    'evoc', 
    'evocreference', 'ext', 'mxcheckboxselect',
    'neologism'
  ));
  
  // Evoc installation
  drupal_install_schema('evoc');
  // default sparql endpoint
  $endpoint = array(
    'name' => 'SPARQLer',
    'enabled' => TRUE,
    'status' => 'alive',
    'endpoint' => 'http://www.sparql.org/sparql',
    'webform' => 'http://www.sparql.org/sparql.html',
    'comment' => 'based on ARQ and Joseki',
    'data_exposed' => '"any" — "general purpose SPARQL service"'
  );
  
  $result = db_query("insert into {evoc_sparql_endpoints} (name, enabled, status, endpoint, webform, comment, data_exposed) values ('%s', '%d', '%s', '%s', '%s', '%s', '%s')", 
    $endpoint['name'], $endpoint['enabled'], $endpoint['status'], $endpoint['endpoint'], $endpoint['webform'], $endpoint['comment'], $endpoint['data_exposed']);
      
  variable_set('evoc_sparqlendpoint', 1);
  
  // Neologism installation
  // create Neologism content types
  $ct = content_types();
  if (!isset($ct[NEOLOGISM_CT_VOCABULARY])) {
    module_load_include('install', 'neologism', 'neologism');
    _neologism_create_content('vocabulary');
  }
  if (!isset($ct[NEOLOGISM_CT_CLASS])) {
    module_load_include('install', 'neologism', 'neologism');
    _neologism_create_content('class');
  }
  if (!isset($ct[NEOLOGISM_CT_PROPERTY])) {
    module_load_include('install', 'neologism', 'neologism');
    _neologism_create_content('property');
  }
*/
}
