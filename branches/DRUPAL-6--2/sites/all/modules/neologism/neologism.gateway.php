<?php

/**
 * Contruct a tree structure
 * @return json with the tree structure 
 */
function neologism_get_classes_tree() {
  $voc['id'] = $_POST['voc_id'];
  $voc['title'] = $_POST['voc_title'];
  
  $node = $_POST['node'];
  $nodes = array();
  if ( $node == 'super' ) {
    $classes = db_query(db_rewrite_sql("SELECT n.nid, n.title FROM {content_field_vocabulary} c INNER JOIN {node} n ON c.nid = n.nid 
      WHERE c.field_vocabulary_nid = %d AND n.type = '%s'"), $voc['id'], NEOLOGISM_CT_CLASS);
    
    $result = db_query(db_rewrite_sql("SELECT n.nid, n.title FROM {node} n WHERE n.type = '%s'"), NEOLOGISM_CT_VOCABULARY);
    while ($v = db_fetch_object($result)) {
      $vocs[] = $v->title;
    }
    
    $root_superclasses = array();
    while ($class = db_fetch_object($classes)) {
      $root_superclasses = _get_superclass($class->nid, $voc, $vocs);
    }
    
    foreach ($root_superclasses as $snode) {
      $leaf = (count($children_of_children = neologism_gateway_get_onedepth_children($snode->field_superclass2_evoc_term, $voc['title'])) == 0 );
      $term_qname_parts = explode(':', $snode->field_superclass2_evoc_term);      
      $nodes[] = array(
        'text' => $snode->field_superclass2_evoc_term, 
        'id' => $snode->field_superclass2_evoc_term, 
        'leaf' => $leaf, 
        'iconCls' => 'class-samevoc',
        'cls' => ($term_qname_parts[0] == $voc['title']) ? 'currentvoc' : '',
        'qtip' => $snode->comment  
      );
    }
  }
  else {
    $children = neologism_gateway_get_onedepth_children($node, $voc['title']);
    foreach( $children as $child ) {
      //if( $child['prefix'] == $voc['title'] ) {
        $class_qname = $child['prefix'].':'.$child['id'];
        $leaf = (count($children_of_children = neologism_gateway_get_onedepth_children($class_qname, $voc['title'])) == 0 );
        $nodes[] = array(
          'text' => $class_qname, 
          'id' => $class_qname, 
          'leaf' => $leaf, 
          'iconCls' => 'class-samevoc',
          'cls' => ($child['prefix'] == $voc['title']) ? 'currentvoc' : '',
          'qtip' => $child['comment']  
        );
    }
  }

  drupal_json($nodes);
}

function _get_superclass($class_id, $voc, array $vocs) {
  static $superclasses_array = array();
  
  $superclasses = db_query(db_rewrite_sql("SELECT s.field_superclass2_evoc_term, n.title, n.nid 
    FROM {content_field_vocabulary} as c left JOIN {node} as n ON c.nid = n.nid
    left join {content_field_superclass2} as s on n.nid = s.nid
    WHERE c.field_vocabulary_nid = %d /*voc*/ AND n.type = '%s' and n.nid = %d /*class*/"), $voc['id'], NEOLOGISM_CT_CLASS, $class_id);
    
  while ($superclass = db_fetch_object($superclasses)) {
    if ( $superclass->field_superclass2_evoc_term != NULL ) {
      // clear the field, sometimes come with spaces
      $superclass->field_superclass2_evoc_term = trim($superclass->field_superclass2_evoc_term); 
      $term_qname_parts = explode(':', $superclass->field_superclass2_evoc_term);
      $term_prefix = $term_qname_parts[0];

      if( !in_array($term_prefix, $vocs) ) { 
        if( !_in_array($superclass, $superclasses_array) ) {
          $comment = db_result(db_query(db_rewrite_sql("SELECT comment FROM {evoc_rdf_classes} where prefix = '%s' and id = '%s'"), $term_prefix, $term_qname_parts[1]));
          $superclass->comment = $comment; 
          $superclasses_array[] = $superclass;  
        }
      }
      else {
        // check that you are using a $vocs variable that hold the vocabularies' title and id 
        $class = db_fetch_object(db_query(db_rewrite_sql("SELECT n.nid, n.title, c.field_vocabulary_nid FROM 
          {content_field_vocabulary} c INNER JOIN {node} n ON c.nid = n.nid 
          where c.field_vocabulary_nid = (select nv.nid from {node} nv where nv.title = '%s' and nv.type = 'neo_vocabulary') and n.title = '%s'"), $term_prefix, $term_qname_parts[1])); 
        $voc['id'] = $class->field_vocabulary_nid;
        $superclasses_array = _get_superclass($class->nid, $voc, $vocs); 
      }   
    }
  } 

  return $superclasses_array; 
}

function neologism_gateway_get_onedepth_children($node, $vocabulary = "") {
  $children = db_query(db_rewrite_sql("select n.title, n.nid from {content_field_superclass2} as c inner join {node} as n on c.nid = n.nid 
    where c.field_superclass2_evoc_term = '%s'"), $node);
  
  /*
  if ( $same_vocabulary ) {
    $term_qname_parts = explode(':', $node);
    $futher_constrain = "and e.prefix = '".$term_qname_parts[0]."'";
  }
  */
  
  // TODO check this more relaxed
  $arr_children = array();  
  while ($child = db_fetch_object($children)) {
    $classes = db_query(db_rewrite_sql("select e.prefix, e.comment from {evoc_rdf_classes} as e where e.id = '%s'"), $child->title);
    // may there is more than one class with the same title but belong to a different vocabulary
    while ($class = db_fetch_object($classes)) {
      if ( $vocabulary != "" && $class->prefix == $vocabulary ) {
        $arr = array('prefix' => $class->prefix, 'id' => $child->title, 'comment' => $class->comment);
        if( !in_array($arr, $arr_children) ) {
          $arr_children[] = $arr; 
        }
      } else if( $vocabulary == "" ) {
        $arr = array('prefix' => $class->prefix, 'id' => $child->title, 'comment' => $class->comment);
        if( !in_array($arr, $arr_children) ) {
          $arr_children[] = $arr; 
        }
      } 
    }
  }
  
  return $arr_children;
}

/**
 * Check if the superclass is in superclasses array usign the evoc term field (field_superclass2_evoc_term)
 * @param object $superclass
 * @param object $superclasses [optional]
 * @return 
 */
function _in_array($superclass, array $superclasses ) {
  foreach ($superclasses as $superc) {
    if( $superc->field_superclass2_evoc_term == $superclass->field_superclass2_evoc_term ) {
      return true;
    }
  }
  
  return false;
}

//-----------------------------------------------------------------------------------------------------------------
// functions for objectproperty_tree

/**
 * 
 * @return 
 */
function neologism_get_objectproperty_tree() {
  
  $voc['id'] = $_POST['voc_id'];
  $voc['title'] = $_POST['voc_title'];
  
  $node = $_POST['node'];
  $nodes = array();
  if( $node == 'super' ) {
    $properties = db_query(db_rewrite_sql("SELECT n.nid, n.title FROM {content_field_vocabulary} c INNER JOIN {node} n ON c.nid = n.nid 
      WHERE c.field_vocabulary_nid = %d AND n.type = '%s'"), $voc['id'], NEOLOGISM_CT_PROPERTY);
    
    $result = db_query(db_rewrite_sql("SELECT n.nid, n.title FROM {node} n WHERE n.type = '%s'"), NEOLOGISM_CT_VOCABULARY);
    while ($v = db_fetch_object($result)) {
      $vocs[] = $v->title;
    }
    
    $root_superproperties = array();
    while ($property = db_fetch_object($properties)) {
      $root_superproperties = _get_superproperties($property->nid, $voc, $vocs);
    }
    
    foreach ($root_superproperties as $snode) {
      $leaf = (count($children_of_children = neologism_gateway_get_property_children($snode->field_superproperty2_evoc_term, $voc['title'])) == 0 );
      $term_qname_parts = explode(':', $snode->field_superclass2_evoc_term);   
      $nodes[] = array(
        'text' => $snode->field_superproperty2_evoc_term, 
        'id' => $snode->field_superproperty2_evoc_term, 
        'leaf' => $leaf, 
        'iconCls' => 'property-samevoc',
        'cls' => ($term_qname_parts[0] == $voc['title']) ? 'currentvoc' : '',
        'qtip' => $snode->comment  
      );
    }
  }
  else {
    $children = neologism_gateway_get_property_children($node, $voc['title']);
    foreach( $children as $child ) {
      $property_qname = $child['prefix'].':'.$child['id'];
      $leaf = (count($children_of_children = neologism_gateway_get_property_children($property_qname, $voc['title'])) == 0 );
      $nodes[] = array(
        'text' => $property_qname, 
        'id' => $property_qname, 
        'leaf' => $leaf, 
        'iconCls' => 'property-samevoc',
        'cls' => ($child['prefix'] == $voc['title']) ? 'currentvoc' : '',
        'qtip' => $child['comment']  
      );  
    }
  }

  drupal_json($nodes);
}

function _get_superproperties($property_id, $voc, array $vocs) {
  //static $superproperties_array = array();
  $superproperties_array = array();
  
  //vid, nid, delta, field_superproperty2_evoc_term
  $superproperties = db_query(db_rewrite_sql("SELECT s.field_superproperty2_evoc_term, n.title, n.nid 
    FROM {content_field_vocabulary} as c left JOIN {node} as n ON c.nid = n.nid
    left join {content_field_superproperty2} as s on n.nid = s.nid
    WHERE c.field_vocabulary_nid = %d /*voc*/ AND n.type = '%s' and n.nid = %d /*property*/"), $voc['id'], NEOLOGISM_CT_PROPERTY, $property_id);
  
  while ($superproperty = db_fetch_object($superproperties)) {
    if ( $superproperty->field_superproperty2_evoc_term != NULL ) {
      $term_qname_parts = explode(':', $superproperty->field_superproperty2_evoc_term);
      $term_prefix = $term_qname_parts[0];

      if( !in_array($term_prefix, $vocs) ) { 
        if( !_in_array_property($superproperty, $superproperties_array) ) {
          // get comment from evoc_rdf_classes because there is where all classes has its cooment stored
          $comment = db_result(db_query(db_rewrite_sql("SELECT comment FROM {evoc_rdf_classes} where prefix = '%s' and id = '%s'"), $term_prefix, $term_qname_parts[1]));
          $superproperty->comment = $comment; 
          $superproperties_array[] = $superproperty;  
        }
      }
      else {
        // check that you are using a $vocs variable that hold the vocabularies' title and id 
        $property = db_fetch_object(db_query(db_rewrite_sql("SELECT n.nid, n.title, c.field_vocabulary_nid 
          FROM {content_field_vocabulary} c INNER JOIN {node} n ON c.nid = n.nid 
          where c.field_vocabulary_nid = (select nv.nid from {node} nv where nv.title = '%s' 
          and nv.type = 'neo_vocabulary') and n.title = '%s'"), $term_prefix, $term_qname_parts[1])); 
        $voc['id'] = $property->field_vocabulary_nid;
        $superproperties_array = _get_superproperties($property->nid, $voc, $vocs); 
      }   
    }
    else {
      if( !_in_array_property($superproperty, $superproperties_array) ) {
          // ajust the evoc term with the same prefix          
          $superproperty->field_superproperty2_evoc_term = $voc['title'].':'.$superproperty->title;
          $superproperties_array[] = $superproperty;  
        }
    }
  } 

  return $superproperties_array; 
}

function neologism_gateway_get_property_children($node, $vocabulary = "") {
  $children = db_query(db_rewrite_sql("select n.title from content_field_superproperty2 as c inner join node as n on c.nid = n.nid 
    where c.field_superproperty2_evoc_term = '%s'"), $node);
  
  $arr_children = array();  
  while ($child = db_fetch_object($children)) {
    $properties = db_query(db_rewrite_sql("select e.prefix, e.comment from {evoc_rdf_properties} as e where e.id = '%s'"), $child->title);
    while ($property = db_fetch_object($properties)) {
      if ( $vocabulary != "" && $property->prefix == $vocabulary ) {
        $arr = array('prefix' => $property->prefix, 'id' => $child->title, 'comment' => $property->comment);
        if( !in_array($arr, $arr_children) ) {
          $arr_children[] = $arr; 
        }
      } else if( $vocabulary == "" ) {
        $arr = array('prefix' => $property->prefix, 'id' => $child->title, 'comment' => $property->comment);
        if( !in_array($arr, $arr_children) ) {
          $arr_children[] = $arr; 
        }
      }
    }
  }

  return $arr_children;
}

/**
 * Check if the superproperty is in superclasses array usign the evoc term field (field_superclass2_evoc_term)
 * @param object $superclass
 * @param object $superclasses [optional]
 * @return 
 */
function _in_array_property($superproperty, array $superproperties ) {
  foreach ($superproperties as $superp) {
    if( $superp->field_superproperty2_evoc_term == $superproperty->field_superproperty2_evoc_term ) {
      return true;
    }
  }
  
  return false;
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
  
  $object = db_fetch_object(db_query(db_rewrite_sql("select has_superclass from {evoc_rdf_classes} where prefix = '%s' and id = '%s'"), $prefix, $id));
  $has_superclass = ($object->has_superclass == '1');
  if ( $has_superclass ) {
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
