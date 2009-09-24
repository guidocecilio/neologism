<?php

/**
 * Construct a tree structure for an specific vocabulary
 * @return json with the tree structure 
 */
function neologism_gateway_get_classes_tree() {
  $voc['id'] = $_POST['voc_id'];
  $voc['title'] = $_POST['voc_title'];
  
  $node = $_POST['node'];
  $nodes = array();
  if ( $node == 'super' ) {
    $classes = db_query(db_rewrite_sql('SELECT superclass FROM {evoc_rdf_superclasses} where prefix = "%s"'), $voc['title']);

    $root_superclasses = array();
    while ($class = db_fetch_object($classes)) {
      $root_superclasses = neologism_gateway_get_root_superclasses($class->superclass);
    }
    
    foreach ($root_superclasses as $class) {
      $qname_splitted = explode(':', $class);
      $object = db_fetch_object(db_query(db_rewrite_sql("SELECT prefix, id, label, comment FROM {evoc_rdf_classes} where prefix = '%s' and id = '%s'"), $qname_splitted[0], $qname_splitted[1]));
      if( $object ) {
        $children = neologism_gateway_get_children($class, $voc['title']);
        if( $qname_splitted[0] == $voc['title'] || _neologism_gateway_in_nodes($voc['title'], $children) ) {
          $qtip = '<b>'.$object->label.'</b><br/>'.$object->comment;
          $leaf = count($children) == 0;
          $nodes[] = array(
            'text' => $class, 
            'id' => $class, 
            'leaf' => $leaf, 
            'iconCls' => 'class-samevoc',
            'cls' => ($qname_splitted[0] == $voc['title']) ? 'currentvoc' : '', 
            'children' => $children, 
            'qtip' => $qtip
          );        
        }
      }
    }
    
    //we need to add properties without superproperties belonging to the $voc as well
    $classes = db_query(db_rewrite_sql('SELECT * FROM {evoc_rdf_classes} where prefix = "%s" and superclasses = "0"'), $voc['title']);
    while ($class = db_fetch_object($classes)) {
      $qname = $class->prefix.':'.$class->id;
      $qtip = '<b>'.$class->label.'</b><br/>'.$class->comment;
      $nodes[] = array(
        'text' => $qname, 
        'id' => $qname, 
        'leaf' => true, 
        'iconCls' => 'class-samevoc',
        'cls' => 'currentvoc', 
        'qtip' => $qtip
      );
    }
  
  }

  drupal_json($nodes);
}

function _neologism_gateway_in_nodes($prefix, array $nodes) {
  foreach ($nodes as $node) {    
    $qterm_splited = explode(':', $node['id']);
    if( $prefix == $qterm_splited[0] ) {
      return true;
    }
  }
  return false;  
}

/**
 * This recurive function search for chindren of $node return class from the same $voc. 
 * If the parent does not belong to the $voc but has children that does, this parent is added as well.
 * @param object $node
 * @param object $voc
 * @param object $add_checkbox [optional]
 * @return 
 */
function neologism_gateway_get_children($node, $voc, $add_checkbox = FALSE) {
  $nodes = array();
  
  $children = db_query('select prefix, reference from {evoc_rdf_superclasses} where superclass = "'.$node.'"');
    
  while ($child = db_fetch_object($children)) {
    $class = db_fetch_object(db_query('select * from evoc_rdf_classes where prefix = "'.$child->prefix.'" and id = "'.$child->reference.'" '));
    if ( $class ) {
      $class->prefix = trim($class->prefix);
      $class->id = trim($class->id); 
      $class_qname = $class->prefix.':'.$class->id;
      $children_nodes = neologism_gateway_get_children($class_qname, $voc, $add_checkbox);  
      if( $class->prefix == $voc || _neologism_gateway_in_nodes($voc, $children_nodes) ) {
        $leaf = count($children_nodes) == 0;
        $qtip = '<b>'.$class->label.'</b><br/>'.$class->comment;
        $nodes[] = array(
          'text' => $class_qname, 
          'id' => $class_qname, 
          'leaf' => $leaf, 
          'iconCls' => 'class-samevoc', 
          'cls' => ($class->prefix == $voc) ? 'currentvoc' : '',
          'children' => $children_nodes, 
          'qtip' => $qtip
        );
        
        if( $add_checkbox ) {
          $nodes[0]['checked'] = false;
        }
      }
      
    }
  }
  
  return $nodes;
}


//-----------------------------------------------------------------------------------------------------------------
// functions for objectproperty_tree
function neologism_gateway_get_properties_tree() {
  $voc['id'] = $_POST['voc_id'];
  $voc['title'] = $_POST['voc_title'];
  
  $node = $_POST['node'];
  $nodes = array();
  
  if ( $node == 'super' ) {
    $properties = db_query(db_rewrite_sql('SELECT superproperty FROM {evoc_rdf_superproperties} where prefix = "%s"'), $voc['title']);

    $root_superproperties = array();
    while ($property = db_fetch_object($properties)) {
      $root_superproperties = neologism_gateway_get_root_superproperties($property->superproperty);
    }
    
    foreach ($root_superproperties as $property) {
      $qname_splitted = explode(':', $property);
      $object = db_fetch_object(db_query(db_rewrite_sql("SELECT label, comment FROM {evoc_rdf_properties} where prefix = '%s' and id = '%s'"), $qname_splitted[0], $qname_splitted[1]));
      if( $object ) {
        $children = neologism_gateway_get_property_children($property, $voc['title']);
        if( $qname_splitted[0] == $voc['title'] || _neologism_gateway_in_nodes($voc['title'], $children) ) {
          $qtip = '<b>'.$object->label.'</b><br/>'.$object->comment;
          $leaf = count($children) == 0;
          $nodes[] = array(
            'text' => $property, 
            'id' => $property, 
            'leaf' => $leaf, 
            'iconCls' => 'property-samevoc',
            'cls' => ($qname_splitted[0] == $voc['title']) ? 'currentvoc' : '', 
            'children' => $children, 
            'qtip' => $qtip
          );        
        }
      }
    }
    
    //we need to add properties without superproperties belonging to the $voc as well
    $properties = db_query(db_rewrite_sql('SELECT * FROM {evoc_rdf_properties} where prefix = "%s" and superproperties = "0"'), $voc['title']);
    while ($property = db_fetch_object($properties)) {
      $qname = $property->prefix.':'.$property->id;
      $qtip = '<b>'.$property->label.'</b><br/>'.$property->comment;
      $nodes[] = array(
        'text' => $qname, 
        'id' => $qname, 
        'leaf' => true, 
        'iconCls' => 'property-samevoc',
        'cls' => 'currentvoc', 
        'qtip' => $qtip
      );
    }
    
  }

  drupal_json($nodes);
}

function neologism_gateway_get_root_superproperties($property) {
  static $root_superproperties = array();
  
  $term_qname_parts = explode(':', $property);
  $prefix = $term_qname_parts[0];
  $id = $term_qname_parts[1];
  
  $object = db_fetch_object(db_query(db_rewrite_sql("select superproperties from {evoc_rdf_properties} where prefix = '%s' and id = '%s'"), $prefix, $id));
  if ( $object->superproperties > 0 ) {
    $superproperty = db_query(db_rewrite_sql("SELECT superproperty FROM {evoc_rdf_superproperties} where prefix = '%s' and reference = '%s'"), $prefix, $id);
    while ( $term = db_fetch_object($superproperty) ) {
      $term->superproperty = trim($term->superproperty);
      $root_superproperties = neologism_gateway_get_root_superclasses($term->superproperty);  
    }
  }
  else {
    if( !_neologism_gateway_in_array($property, $root_superproperties) ) {
      $root_superproperties[] = $property;  
    }
  }
  
  return $root_superproperties;
}


function neologism_gateway_get_property_children($node, $voc, $add_checkbox = FALSE) {
  $nodes = array();
  
  $children = db_query('select prefix, reference from {evoc_rdf_superproperties} where superproperty = "'.$node.'"');
    
  while ($child = db_fetch_object($children)) {
    $property = db_fetch_object(db_query('select * from evoc_rdf_properties where prefix = "'.$child->prefix.'" and id = "'.$child->reference.'" '));
    if ( $property ) {
      $property->prefix = trim($property->prefix);
      $property->id = trim($property->id); 
      $qname = $property->prefix.':'.$property->id;
      $children_nodes = neologism_gateway_get_property_children($qname, $voc, $add_checkbox);  
      if( $property->prefix == $voc || _neologism_gateway_in_nodes($voc, $children_nodes) ) {
        $leaf = count($children_nodes) == 0;
        $qtip = '<b>'.$property->label.'</b><br/>'.$property->comment;
        $nodes[] = array(
          'text' => $qname, 
          'id' => $qname, 
          'leaf' => $leaf, 
          'iconCls' => 'property-samevoc', 
          'cls' => ($property->prefix == $voc) ? 'currentvoc' : '',
          'children' => $children_nodes, 
          'qtip' => $qtip
        );
        
        if( $add_checkbox ) {
          $nodes[0]['checked'] = false;
        }
      }
      
    }
  }
  
  return $nodes;
}

//-----------------------------------------------------------------------------------------------------------
// These functions are more completed that the above. I planned to fix the function above for tunning process
//

/**
 * Construct the tree structure for a Tree using ExtJS Tree structure
 * @return json with the tree structure 
 */
function neologism_gateway_get_full_classes_tree() {
  $nodes = array();
  
  $node = $_REQUEST['node'];
    
  if ( $node == 'root' ) {
    $classes = db_query(db_rewrite_sql("SELECT prefix, id FROM {evoc_rdf_classes}"));

    $root_superclasses = array();
    while ($class = db_fetch_object($classes)) {
      $class->prefix = trim($class->prefix);
      $class->id = trim($class->id);
      $root_superclasses = neologism_gateway_get_root_superclasses($class->prefix.':'.$class->id);
    }
  
    foreach ($root_superclasses as $class) {
      $qname_parts = explode(':', $class);
      $object = db_fetch_object(db_query(db_rewrite_sql("SELECT prefix, id, label, comment FROM {evoc_rdf_classes} where prefix = '%s' and id = '%s'"), $qname_parts[0], $qname_parts[1]));
      if( $object ) {
        $children = neologism_gateway_get_children2($class);
        $qtip = '<b>'.$object->label.'</b><br/>'.$object->comment;
        $leaf = count($children) == 0;
        $nodes[] = array(
          'text' => $class, 
          'id' => $class, 
          'leaf' => $leaf, 
          'iconCls' => 'class-samevoc', 
          'children' => $children, 
          'checked' => false,
          'qtip' => $qtip
        );        
      }
    }

  }

  drupal_json($nodes);
}

function neologism_gateway_get_root_superclasses($class) {
 
  static $root_superclasses = array();
  
  $term_qname_parts = explode(':', $class);
  $prefix = $term_qname_parts[0];
  $id = $term_qname_parts[1];
  
  $object = db_fetch_object(db_query(db_rewrite_sql("select superclasses from {evoc_rdf_classes} where prefix = '%s' and id = '%s'"), $prefix, $id));
  if ( $object->superclasses > 0 ) {
    $superclass = db_query(db_rewrite_sql("SELECT superclass FROM {evoc_rdf_superclasses} where prefix = '%s' and reference = '%s'"), $prefix, $id);
    while ( $term = db_fetch_object($superclass) ) {
      $term->superclass = trim($term->superclass);
      $root_superclasses = neologism_gateway_get_root_superclasses($term->superclass);  
    }
  }
  else {
    if( !_neologism_gateway_in_array($class, $root_superclasses) ) {
      $root_superclasses[] = $class;  
    }
  }
  
  return $root_superclasses;
}

/**
 * This recursive function return all the children from $node
 * @param object $node
 * @return 
 */
/*
function neologism_gateway_get_children($node, $reset = false) {
  $nodes = array();
  
  // search for the children in all the tables
  $children = db_query(db_rewrite_sql("select n.title, n.nid from 
    content_field_superclass2 as c inner join node as n on c.nid = n.nid 
    where c.field_superclass2_evoc_term = '%s'"), $node);
   
  // get children from vocabularies on Drupal content 
  $arr_children = array();  
  while ($child = db_fetch_object($children)) {
    $classes = db_query(db_rewrite_sql("select e.prefix, e.superclass from {evoc_rdf_classes} as e where e.id = '%s'"), $child->title);
    // may there is more than one class with the same title but belong to a different vocabulary
    while ($class = db_fetch_object($classes)) {
      $class->prefix = trim($class->prefix);
      $class->superclass = trim($class->superclass); 
      // check if the current $class is well selected, because might be a class with same title/id and different vocabulary/prefix
      // otherwise we need to join more tables to make the correct query.
      // this fix when there is two class with same name but different prefix, eg: eg:Agent and foaf:Agent
      if( $class->superclass == $node ) {
        $arr = array('prefix' => $class->prefix, 'id' => $child->title);
        if( !in_array($arr, $arr_children) ) {
          $arr_children[] = $arr; 
        }
      }
    }
  }
  
  // get child from evoc classes
  $children = db_query(db_rewrite_sql("select e.prefix, e.id from {evoc_rdf_classes} as e where e.superclass = '%s'"), $node);
  while ($child = db_fetch_object($children)) {
    $child->prefix = trim($child->prefix);
    $child->id = trim($child->id);
    // may there is more than one class with the same title but belong to a different vocabulary
    $arr = array('prefix' => $child->prefix, 'id' => $child->id);
    if( !in_array($arr, $arr_children) ) {
      $arr_children[] = $arr; 
    }
  }
  
  // at this point we are finished to search all children of current node.
  // now  we need to expand the children
  
  
  // iterate through the children
  foreach( $arr_children as $child ) {
    $class_qname = $child['prefix'].':'.$child['id'];
    $children = neologism_gateway_get_children($class_qname);
    $leaf = count($children) == 0;
    $nodes[] = array(
      'text' => $class_qname, 
      'id' => $class_qname, 
      'leaf' => $leaf, 
      'iconCls' => 'class-samevoc', 
      'children' => $children, 
      'checked' => false
    ); 
  }
  
  return $nodes;
}
*/

/**
 * 
 * @param object $node
 * @param object $reset [optional]
 * @return 
 */
function neologism_gateway_get_children2($node) {
  $nodes = array();
  
  $children = db_query('select prefix, reference from {evoc_rdf_superclasses} where superclass = "'.$node.'"');
    
  while ($child = db_fetch_object($children)) {
    $class = db_fetch_object(db_query('select * from evoc_rdf_classes where prefix = "'.$child->prefix.'" and id = "'.$child->reference.'" '));
    if ( $class ) {
      $class->prefix = trim($class->prefix);
      $class->id = trim($class->id); 
      $class_qname = $class->prefix.':'.$class->id;
      $children_nodes = neologism_gateway_get_children2($class_qname);  
      $leaf = count($children_nodes) == 0;
      $qtip = '<b>'.$class->label.'</b><br/>'.$class->comment;
      $nodes[] = array(
        'text' => $class_qname, 
        'id' => $class_qname, 
        'leaf' => $leaf, 
        'iconCls' => 'class-samevoc', 
        'children' => $children_nodes, 
        'checked' => false,
        'qtip' => $qtip
      );
    }
  }
  
  return $nodes;
}

// properties

/**
 * Construct the tree structure for a Tree using ExtJS Tree structure
 * @return json with the tree structure 
 */
function neologism_gateway_get_full_properties_tree() {

  $nodes = array();
  
  $node = $_REQUEST['node'];
  $nodes = array();
  
  if ( $node == 'root' ) {
    $properties = db_query(db_rewrite_sql("SELECT prefix, id, comment FROM {evoc_rdf_properties}"));

    $root_superproperties = array();
    while ($property = db_fetch_object($properties)) {
      $property->prefix = trim($property->prefix);
      $property->id = trim($property->id);
      $qname = $property->prefix.':'.$property->id;
      
      $count = db_result(db_query(db_rewrite_sql("select count(*)
        from {content_field_vocabulary} as c left join {node} as n on c.nid = n.nid
        left join {content_field_superproperty2} as s on n.nid = s.nid
        where c.field_vocabulary_nid = (select nid from {node} where type = 'neo_vocabulary' and title='%s')
        and n.type = 'neo_property' and n.title = '%s'
      "), $property->prefix, $property->id));
      
      if( !$count ) {
        $root_superproperties[] = Array('qname' => $property->prefix.':'.$property->id, 'comment' => $property->comment);
      }
      
    }
    
    //$property['qname'] == 'foaf:geekcode'
    
    foreach ($root_superproperties as $property) {
      $children = neologism_gateway_get_property_children2($property['qname'], TRUE);
      $leaf = count($children) == 0;
      $nodes[] = array(
        'text' => $property['qname'], 
        'id' => $property['qname'], 
        'leaf' => $leaf, 
        'iconCls' => 'class-samevoc', 
        'children' => $children, 
        'checked' => false,
        'qtip' => $property['comment'] 
      );        
    }
  }

  drupal_json($nodes);
}

/**
 * Search for all root properties in evoc_rdf_properties table
 * @param object $property
 * @return 
 */
/*
function neologism_gateway_get_root_superproperties($property){
  static $root_superproperties = array();
  
  $term_qname_parts = explode(':', $property);
  $prefix = $term_qname_parts[0];
  $id = $term_qname_parts[1];
  
  // TODO add the new tables field for evoc_rdf_properties, eg: superproperty field.
  // for the moment the evoc_rdf_properties table doesn't have the superproperty, so all the property
  // evaluated will be added as a root property
  $superproperty = db_query(db_rewrite_sql("SELECT superproperty FROM {evoc_rdf_properties} where prefix = '%s' and id = '%s'"), $prefix, $id);
  // to check if there is some super class because Drupal team remove db_num_rows function in version 6
  // and I need to now if in the query are some result and I can not do a sigle query with Count(*)
  $has_superproperty = false;
  while ( $term = db_fetch_object($superproperty) ) {
    $has_superproperty = true;
    $term->superproperty = trim($term->superproperty);
    if( $term->superproperty == '' ) {
      if( !_neologism_gateway_in_array($property, $root_superproperties) ) {
        $root_superproperties[] = $property;  
      }
    }
    else {
      $root_superproperties = neologism_gateway_get_root_superproperties($term->superproperty);  
    }
  }
  
  if( !$has_superproperty ) {
    if( !_neologism_gateway_in_array($property, $root_superproperties) ) {
        $root_superproperties[] = $property;  
      }
  }
  
  return $root_superproperties;
}
*/

/**
 * This recursive function return all the children from $node
 * @param object $node
 * @return 
 */
function neologism_gateway_get_property_children2($node, $reset = false) {
  $nodes = array();
  
  // search for the children in all the tables
  $children = db_query(db_rewrite_sql("select n.title, n.nid, c.field_comment_value from {content_field_superproperty2} as s 
    left join {node} as n on s.nid = n.nid 
    left join {content_field_comment} as c on c.nid = n.nid     
    where s.field_superproperty2_evoc_term = '%s'"), $node);
   
  // get children from vocabularies on Drupal content 
  $arr_children = array();  
  while ($child = db_fetch_object($children)) {
    
    // get the vocabulary for the current child
    $voc = db_fetch_object(db_query(db_rewrite_sql("SELECT n.nid, n.title FROM {content_field_vocabulary} c INNER JOIN {node} n ON c.field_vocabulary_nid = n.nid WHERE c.nid = %d"), $child->nid));    
    
    if( $voc ) {
      $arr = array('qname' => $voc->title.':'.$child->title, 'comment' => $child->field_comment_value);
      if( !in_array($arr, $arr_children) ) {
        $arr_children[] = $arr; 
      }
    }
  }
 
  // at this point we are finished to search all children of current node.
  // now  we need to expand the children
  
  
  // iterate through the children
  foreach( $arr_children as $child ) {
    $children = neologism_gateway_get_property_children2($child['qname']);
    $leaf = count($children) == 0;
    $nodes[] = array(
      'text' => $child['qname'], 
      'id' => $child['qname'], 
      'leaf' => $leaf, 
      'iconCls' => 'class-samevoc', 
      'children' => $children, 
      'checked' => false,
      'qtip' => $child['comment']
    ); 
  }
  
  return $nodes;
}

function _neologism_gateway_in_array($strproperty, array $strarray_values) {
  foreach ($strarray_values as $str) {
    if( $str == $strproperty ) {
      return true;
    }
  }
  
  return false; 
}
