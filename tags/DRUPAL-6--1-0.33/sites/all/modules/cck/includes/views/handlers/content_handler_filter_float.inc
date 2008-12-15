<?php
// $Id: content_handler_filter_float.inc,v 1.1.2.1 2008/09/04 18:12:27 yched Exp $

/**
 * The subclass simply adds properties,
 * for field-specific subclasses to use if they need to.
 */
class content_handler_filter_float extends views_handler_filter_float {
  var $content_field;

  function construct() {
    parent::construct();
    $this->content_field = content_fields($this->definition['content_field_name']);
    $this->additional_fields = $this->definition['additional fields'];
  }
}