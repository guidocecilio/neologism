<?php
// $Id: rdf.admin.inc,v 1.8 2008/06/04 15:40:32 arto Exp $
/**
 * rdf.admin.inc - RDF API admin page callbacks.
 *
 * @author Arto Bendiken <http://bendiken.net/>
 * @copyright Copyright (c) 2007-2008 Arto Bendiken. All rights reserved.
 * @license GPL <http://creativecommons.org/licenses/GPL/2.0/>
 * @package rdf.module
 */

//////////////////////////////////////////////////////////////////////////////
// RDF settings form

function rdf_admin_settings() {
  $form = array();

  $formats = array();
  foreach (rdf_get_formats('names') as $key => $value) {
    $formats[$key] = '';
  }
  $form['formats'] = array('#type' => 'fieldset', '#title' => t('Formats'), '#collapsible' => TRUE, '#collapsed' => FALSE, '#description' => rdf_help('admin/settings/rdf#formats'));
  $form['formats']['rdf_format'] = array('#type' => 'radios', '#options' => $formats, '#default_value' => RDF_FORMAT);

  return array_merge_recursive(system_settings_form($form), array('#theme' => 'rdf_admin_settings', 'buttons' => array('#weight' => 99)));
}

function theme_rdf_admin_settings($form) {
  $head = array(t('Default'), t('Name'), t('MIME type'), t('Readable'), t('Writable'));
  $rows = array();
  foreach (rdf_get_formats() as $format) {
    $rows[] = array(
      drupal_render($form['formats']['rdf_format'][$format->name]),
      !empty($format->link) ? l($format->title, $format->link) : $format->title,
      $format->mime_type,
      !empty($format->unserialize) ? t('Yes') : t('No'),
      !empty($format->serialize) ? t('Yes') : t('No')
    );
  }

  $form['formats']['#value'] = theme('table', $head, $rows, array('class' => 'formats'));
  unset($form['formats']['rdf_format']);

  return drupal_render($form);
}

//////////////////////////////////////////////////////////////////////////////
// RDF data management

function rdf_admin_data() {
  $subject   = isset($_GET['s']) ? urldecode($_GET['s']) : url(NULL, array('absolute' => TRUE));
  $predicate = isset($_GET['p']) ? urldecode($_GET['p']) : '';
  $object    = isset($_GET['o']) ? urldecode($_GET['o']) : '';
  $context   = isset($_GET['g']) ? urldecode($_GET['g']) : '';

  $form['query'] = array('#type' => 'fieldset', '#title' => t('Query'), '#prefix' => '<div class="container-inline">', '#suffix' => '</div>');
  $form['query']['subject'] = array('#type' => 'textfield', '#title' => '', '#default_value' => $subject, '#maxlength' => 255, '#size' => 20);
  $form['query']['predicate'] = array('#type' => 'textfield', '#title' => '', '#default_value' => $predicate, '#maxlength' => 255, '#size' => 20);
  $form['query']['object'] = array('#type' => 'textfield', '#title' => '', '#default_value' => $object, '#maxlength' => 255, '#size' => 20);
  $form['query']['submit'] = array('#type' => 'submit', '#value' => t('Find'), '#submit' => array('rdf_admin_data_submit'));

  $form['advanced'] = array('#type' => 'fieldset', '#title' => t('Advanced options'), '#collapsible' => TRUE, '#collapsed' => TRUE);
  $form['advanced']['context'] = array('#type' => 'select', '#title' => 'Context', '#default_value' => $context, '#options' => array_merge(array('' => t('(all)')), array_combine(rdf_get_contexts(), rdf_get_contexts())));

  return $form;
}

function rdf_admin_data_submit($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);

  $form_state['redirect'] = array('admin/content/rdf', drupal_query_string_encode(array('s' => $subject, 'p' => $predicate, 'o' => $object)));
}

function theme_rdf_admin_data($form) {
  $subject   = $form['query']['subject']['#value'];
  $predicate = $form['query']['predicate']['#value'];
  $object    = $form['query']['object']['#value'];
  $data      = rdf_normalize(rdf_query($subject, $predicate, $object));

  $output = drupal_render($form['query']);
  $output .= drupal_render($form['advanced']);
  $output .= theme('rdf_triple_table', $data, array('link' => 'theme_rdf_admin_link'));
  $output .= drupal_render($form);
  return $output;
}

function theme_rdf_admin_link($title, $uri, array $attributes = array()) {
  return l($title, 'admin/content/rdf', array('query' => drupal_query_string_encode(array('s' => $uri))));
}

//////////////////////////////////////////////////////////////////////////////
// RDF context management

function rdf_admin_contexts() {
  $header = array(t('URI'), t('Statements'));

  // FIXME: need something more loosely-coupled and robust for this:
  $mutables = module_exists('rdf_db') ? rdf_db_rdf_contexts() : array();

  $rows = array();
  foreach (rdf_get_contexts() as $uri) {
    $mutable = in_array($uri, $mutables);
    $rows[] = array(
      l($uri, 'admin/content/rdf', array('query' => array('g' => $uri))),
      !$mutable ? t('n/a') : rdf_count(NULL, NULL, NULL, array('context' => $uri)),
    );
  }

  return theme('table', $header, $rows);
}

//////////////////////////////////////////////////////////////////////////////
// RDF namespace management

function rdf_admin_namespaces() {
  $header = array(t('Prefix'), t('Base URI'), array('data' => t('Operations'), 'colspan' => '2'));

  // FIXME: need something more loosely-coupled and robust for this:
  $mutables = module_exists('rdf_db') ? rdf_db_rdf_namespaces() : array();

  $rows = array();
  foreach (rdf_get_namespaces() as $prefix => $uri) {
    $mutable = isset($mutables[$prefix]);
    $rows[] = array(
      $prefix, //l($prefix, 'admin/settings/rdf/predicates/' . $prefix),
      check_plain($uri),
      !$mutable ? '' : l(t('edit'), 'admin/settings/rdf/namespaces/edit/' . $prefix),
      !$mutable ? '' : l(t('delete'), 'admin/settings/rdf/namespaces/delete/' . $prefix),
    );
  }

  return theme('table', $header, $rows);
}

//////////////////////////////////////////////////////////////////////////////
// RDF repository management

function rdf_admin_repositories() {
  $header = array(t('Name'), t('Statements'), t('Mutable'), t('Module'), array('data' => t('Operations'), 'colspan' => '2'));

  $rows = array();
  foreach (rdf_get_repositories() as $name => $info) {
    $mutable = !empty($info['mutable']) && $name != 'default'; // FIXME
    $rows[] = array(
      l($info['title'], 'admin/settings/rdf/repositories/'. $info['module'] .'/edit/'. $name,
        array('attributes' => array('title' => @$info['dc:description']))),
      $name == 'system' ? t('n/a') : (empty($info['statements']) ? '-' : number_format($info['statements'])),
      !empty($info['mutable']) ? t('Yes') : t('No'),
      $info['module'],
      !$mutable ? '' : l(t('edit'), 'admin/settings/rdf/repositories/'. $info['module'] .'/edit/'. $name),
      !$mutable ? '' : l(t('delete'), 'admin/settings/rdf/repositories/'. $info['module'] .'/delete/'. $name),
    );
  }

  return theme('table', $header, $rows);
}

//////////////////////////////////////////////////////////////////////////////
// RDF DB data management

function rdf_db_admin_data_form(&$form_state, $edit = array('subject' => '', 'predicate' => '', 'object' => '', 'repository' => 'local')) {
  $edit = (object)$edit;
  $form = array();

  $form['subject'] = array('#type' => 'textfield', '#title' => t('Subject'), '#default_value' => $edit->subject, '#maxlength' => 255, '#required' => TRUE, '#description' => t('The subject of the statement: the entity or resource that the statement is about, i.e. the who or what. This should be given as either a <a href="http://en.wikipedia.org/wiki/URI" title="Uniform Resource Identifier">URI</a> or a <a href="http://en.wikipedia.org/wiki/CURIE" title="Compact URI">CURIE</a>.'));
  $form['predicate'] = array('#type' => 'textfield', '#title' => t('Predicate'), '#default_value' => $edit->predicate, '#maxlength' => 255, '#required' => TRUE, '#description' => t('The predicate of the statement: the relation between subject and object, i.e. the property name or verb. This should be given as either a <a href="http://en.wikipedia.org/wiki/URI" title="Uniform Resource Identifier">URI</a> or a <a href="http://en.wikipedia.org/wiki/CURIE" title="Compact URI">CURIE</a>.'));
  $form['object'] = array('#type' => 'textfield', '#title' => t('Object'), '#default_value' => $edit->object, '#maxlength' => 255, '#required' => TRUE, '#description' => t('The object of the statement: the entity or resource that relates to the subject as described by the predicate; i.e. the property value. This should be given as either a <a href="http://en.wikipedia.org/wiki/URI" title="Uniform Resource Identifier">URI</a>, a <a href="http://en.wikipedia.org/wiki/CURIE" title="Compact URI">CURIE</a>, or a literal.'));

  //$form['repository'] = array('#type' => 'select', '#title' => t('Repository'), '#default_value' => $edit->repository, '#options' => array(), '#description' => t('')); // TODO

  $form['submit'] = array('#type' => 'submit', '#value' => t('Add statement'));
  return $form;
}

function rdf_db_admin_data_form_validate($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);

  if (!rdf_is_valid_uri($subject) && !rdf_is_valid_curie($subject)) {
    form_set_error('subject', t('Subject value is not a valid URI or CURIE.'));
  }

  if (!rdf_is_valid_uri($predicate) && !rdf_is_valid_curie($predicate)) {
    form_set_error('predicate', t('Predicate value is not a valid URI or CURIE.'));
  }

  // TODO: relax this once we have a specific input box for literals.
  if (!rdf_is_valid_uri($object) && !rdf_is_valid_curie($object)) {
    //form_set_error('object', t('Object value is not a valid URI or CURIE.'));
  }
}

function rdf_db_admin_data_form_submit($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);

  // TODO: properly sanitize/expand/convert subject, predicate and object.
  $subject   = rdf_uriref($subject);
  $predicate = rdf_uriref($predicate);
  $object    = rdf_is_valid_uri($object) ? rdf_uriref($object) : $object;

  if (rdf_insert($subject, $predicate, $object)) {
    drupal_set_message(t('The statement has been added.'));
  }
  else {
    drupal_set_message(t('The statement was not added due to an error reported by the repository.'), 'error');
  }

  $form_state['redirect'] = array('admin/content/rdf', drupal_query_string_encode(array('s' => (string)$subject)));
}

//////////////////////////////////////////////////////////////////////////////
// RDF DB namespace management

function rdf_db_admin_ns_edit($prefix = '') {
  if (empty($prefix)) {
    return drupal_get_form('rdf_db_admin_ns_form');
  }
  else {
    return drupal_get_form('rdf_db_admin_ns_form', rdf_db_load_namespace($prefix));
  }
}

function rdf_db_admin_ns_form(&$form_state, $edit = array('prefix' => '', 'uri' => '')) {
  $edit = (object)$edit;
  $form = array();

  $form['prefix'] = array('#type' => 'textfield', '#title' => t('Prefix'), '#default_value' => $edit->prefix, '#maxlength' => 64, '#required' => TRUE, '#description' => t('The short abbreviation to use in the place of the base URI. Keep it short, and use only lowercase, alphanumeric letters.'));
  $form['uri'] = array('#type' => 'textfield', '#title' => t('Base URI'), '#default_value' => $edit->uri, '#maxlength' => 255, '#required' => TRUE, '#description' => t('The absolute base URI of the RDF vocabulary. Make sure that the URI terminates with a hash or slash character.'));

  $form['key'] = array('#type' => 'hidden', '#value' => $edit->prefix);
  $form['submit'] = array('#type' => 'submit', '#value' => empty($edit->prefix) ? t('Create new namespace') : t('Update namespace'));

  return $form;
}

function rdf_db_admin_ns_form_validate($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);

  if (!preg_match('/^[a-z_]+[a-z\d\._]*$/', $prefix)) {
    form_set_error('prefix', t('Prefix %prefix contains illegal characters.', array('%prefix' => $prefix)));
  }

  if (!rdf_is_valid_uri($uri)) {
    form_set_error('uri', t('Base URI %uri contains illegal characters.', array('%uri' => $uri)));
  }

  if (!preg_match('@[#/]$@', $uri)) {
    form_set_error('uri', t('Base URI %uri must terminate with "#" or "/".', array('%uri' => $uri)));
  }
}

function rdf_db_admin_ns_form_submit($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);

  if (empty($key)) {
    db_query("INSERT INTO {rdf_namespaces} (prefix, uri) VALUES ('%s', '%s')", $prefix, $uri);
    drupal_set_message(t('The namespace has been created.'));
  }
  else {
    db_query("UPDATE {rdf_namespaces} SET prefix = '%s', uri = '%s' WHERE prefix = '%s'", $prefix, $uri, $key);
    drupal_set_message(t('The namespace has been updated.'));
  }

  $form_state['redirect'] = 'admin/settings/rdf/namespaces';
}

function rdf_db_admin_ns_delete($form_state, $prefix) {
  if (user_access('administer RDF namespaces') && rdf_db_load_namespace($prefix)) {
    $form['prefix'] = array('#type' => 'value', '#value' => $prefix);
    $output = confirm_form($form,
      t('Are you sure you want to delete the RDF namespace %title?', array('%title' => $prefix)),
      isset($_GET['destination']) ? $_GET['destination'] : 'admin/settings/rdf/namespaces');
  }
  return $output;
}

function rdf_db_admin_ns_delete_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    db_query("DELETE FROM {rdf_namespaces} WHERE prefix = '%s'", $form_state['values']['prefix']);
    drupal_set_message(t('The namespace has been deleted.'));

    $form_state['redirect'] = 'admin/settings/rdf/namespaces';
  }
}

//////////////////////////////////////////////////////////////////////////////
// RDF DB repository management

function rdf_db_admin_db_edit($name = '') {
  if (empty($name)) {
    return drupal_get_form('rdf_db_admin_db_form');
  }
  else {
    return drupal_get_form('rdf_db_admin_db_form', rdf_db_load_repository($name));
  }
}

function rdf_db_admin_db_form(&$form_state, $edit = array('name' => '', 'title' => '', 'description' => '')) {
  $edit = (object)$edit;
  $form = array();

  $form['identity'] = array('#type' => 'fieldset', '#title' => t('Identification'));

  $form['identity']['title'] = array('#type' => 'textfield', '#title' => t('Name'), '#default_value' => $edit->title, '#maxlength' => 64, '#required' => TRUE, '#description' => t('The human-readable name of this repository. It is recommended that this name begins with a capital letter and consists only of letters, numbers, and spaces.'));
  $form['identity']['name'] = array('#type' => 'textfield', '#title' => t('ID'), '#default_value' => $edit->name, '#maxlength' => 32, '#required' => TRUE, '#description' => t('The machine-readable name of this repository. This text will be used for constructing the unique URI identifying this repository. This name may consist of only of lowercase letters, numbers, and underscores. Hyphens are not allowed. Underscores will be converted into hyphens when constructing the URI for the repository. This name must be unique to this repository.'));
  $form['identity']['description'] = array('#title' => t('Description'), '#type' => 'textarea', '#default_value' => $edit->description, '#rows' => 2, '#description' => t('A brief description of this repository.'));

  $form['key'] = array('#type' => 'hidden', '#value' => $edit->name);
  $form['submit'] = array('#type' => 'submit', '#value' => empty($edit->name) ? t('Create new repository') : t('Update repository'));

  return $form;
}

function rdf_db_admin_db_form_validate($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);

  if (!preg_match('/^[a-z]+[a-z\d_]*$/', $name)) {
    form_set_error('name', t('The machine-readable name can only consist of lowercase letters, underscores, and numbers.', array('%name' => $name)));
  }

  if (array_search($name, rdf_db_get_repository_names()) !== FALSE) { // FIXME
    form_set_error('name', t('The machine-readable name %name is already used by another repository.', array('%name' => $name)));
  }
}

function rdf_db_admin_db_form_submit($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);

  if (empty($key)) {
    rdf_db_create_repository($name, array('dc:title' => $title, 'dc:description' => $description));
    drupal_set_message(t('The repository has been created.'));
  }
  else {
    if ($key != $name) {
      rdf_db_rename_repository($key, $name);
      rdf_db_delete_repository_metadata($key);
    }
    rdf_db_update_repository_metadata($name, array('dc:title' => $title, 'dc:description' => $description));
    drupal_set_message(t('The repository has been updated.'));
  }

  $form_state['redirect'] = 'admin/settings/rdf/repositories';
}

function rdf_db_admin_db_delete($form_state, $name) {
  if (user_access('administer RDF repositories')) {
    $form['name'] = array('#type' => 'value', '#value' => $name);
    $output = confirm_form($form,
      t('Are you sure you want to delete the RDF repository %title?', array('%title' => $name)),
      isset($_GET['destination']) ? $_GET['destination'] : 'admin/settings/rdf/repositories',
      t('This action will destroy all data contained in the repository and cannot be undone.'));
  }
  return $output;
}

function rdf_db_admin_db_delete_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    rdf_db_delete_repository($form_state['values']['name']);
    drupal_set_message(t('The repository has been deleted.'));

    $form_state['redirect'] = 'admin/settings/rdf/repositories';
  }
}

//////////////////////////////////////////////////////////////////////////////
// RDF data import

function rdf_import_admin_screen(&$form_state, $edit = array('repository' => 'local', 'url' => 'http://', 'format' => RDF_FORMAT)) {
  $edit = (object)$edit;
  $form = array();

  foreach (array('repository', 'url', 'format') as $key) {
    if (isset($_GET[$key])) {
      // This is idiotic, but due to drupal_urlencode() the URL has a
      // double-escaped slash character that we need to get rid of...
      // @see http://api.drupal.org/api/function/drupal_urlencode/6
      $edit->$key = ($key == 'url') ? str_replace('/%2F', '//', $_GET[$key]) : $_GET[$key];
    }
  }

  $form['import'] = array('#type' => 'fieldset', '#title' => t('Import RDF data from a URL'));
  $form['import']['repository'] = array('#type' => 'select', '#title' => t('Repository'), '#default_value' => $edit->repository, '#options' => rdf_get_repositories('names', array('persistent' => TRUE, 'mutable' => TRUE)), '#description' => t(''), '#disabled' => TRUE);
  $form['import']['url'] = array('#type' => 'textfield', '#title' => 'URL', '#default_value' => $edit->url, '#maxlength' => 255, '#required' => TRUE, '#description' => t(''));
  $form['import']['format'] = array('#type' => 'select', '#title' => t('Format'), '#default_value' => $edit->format, '#options' => rdf_get_formats('names') /*FIXME*/, '#description' => t(''));

  $form['import']['submit'] = array('#type' => 'submit', '#value' => t('Import'));
  return $form;
}

function rdf_import_admin_screen_validate($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);

  if (!rdf_is_valid_url($url)) {
    form_set_error('url', t('URL is not valid.'));
  }
}

function rdf_import_admin_screen_submit($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);

  // Fetch the serialized representation from the given URL:
  if (!($text = file_get_contents($url))) {
    drupal_set_message(t('Error while fetching RDF data from %url.', array('%url' => $url)), 'error');
    $form_state['redirect'] = array('admin/content/rdf/import', drupal_query_string_encode(array('url' => $url)));
    return;
  }

  // Parse into triples using the specified RDF parser:
  if (($data = rdf_unserialize($text, array('format' => $format, 'uri' => $url))) === FALSE) {
    drupal_set_message(t('Error while parsing RDF data from %url.', array('%url' => $url)), 'error');
    $form_state['redirect'] = array('admin/content/rdf/import', drupal_query_string_encode(array('url' => $url)));
    return;
  }

  // Assert each triple in the specified repository, while also preserving
  // provenance. We keep track of any failures during assertions.
  $counter = 0;
  foreach ($data as $stmt) {
    if (call_user_func_array('rdf_insert', array_merge($stmt, array(array('graph' => $url, 'repository' => $repository))))) {
      $counter++;
    }
  }

  drupal_set_message(t('!count statements imported from %url.', array('!count' => $counter, '%url' => $url)));

  if (($errors = (count($data) - $counter))) {
    drupal_set_message(t('!count statements were not imported due to an error when asserting them.', array('!count' => $errors)), 'error');
    $form_state['redirect'] = array('admin/content/rdf/import', drupal_query_string_encode(compact('repository', 'url', 'format')));
  }
  else {
    $form_state['redirect'] = array('admin/content/rdf', drupal_query_string_encode(array('g' => $url)));
  }
}

//////////////////////////////////////////////////////////////////////////////
// RDF data export

function rdf_export_admin_screen(&$form_state) {
  $form = array();
  $form['export'] = array('#type' => 'fieldset', '#title' => t('Export RDF data'));

  $repos = rdf_get_repositories('names');
  $form['export']['repos'] = array('#type' => 'checkboxes', '#title' => t('Repositories'), '#default_value' => array_keys($repos), '#options' => $repos);

  /*$namespaces = array_keys(rdf_get_namespaces('names'));
  $namespaces = array_combine($namespaces, $namespaces);
  unset($namespaces['_']); // always export bnodes
  $form['export']['vocabs'] = array('#type' => 'checkboxes', '#title' => t('Vocabularies'), '#default_value' => array_keys($namespaces), '#options' => $namespaces);*/

  $formats = rdf_get_formats('names', 'w');
  $form['export']['format'] = array('#type' => 'select', '#title' => t('RDF format'), '#options' => $formats, '#default_value' => RDF_FORMAT, '#description' => t('Select the output format.'));

  $form['export']['sort'] = array('#type' => 'checkbox', '#title' => t('Sort by subject'), '#default_value' => '1', );

  $form['export']['submit'] = array('#type' => 'submit', '#value' => t('Export'));
  return $form;
}

function rdf_export_admin_screen_validate($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);
  $repos = array_keys(array_filter($repos, 'is_string'));
  //$vocabs = array_keys(array_filter($vocabs, 'is_string'));

  if (empty($repos)) {
    form_set_error('repos', t('No repositories selected.'));
  }

  /*if (empty($vocabs)) {
    form_set_error('vocabs', t('No vocabularies selected.'));
  }*/
}

function rdf_export_admin_screen_submit($form, &$form_state) {
  require_once drupal_get_path('module', 'rdf') . '/rdf.pages.inc';

  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);
  $repos = array_values(array_filter($repos, 'is_string'));
  //$vocabs = array_values(array_filter($vocabs, 'is_string'));

  // TODO: filter by vocabulary selections
  $data = rdf_query(NULL, NULL, NULL, array('repository' => $repos));
  rdf_export($data, 'export', $format, array('log' => TRUE));
}
