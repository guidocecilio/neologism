<?php
// $Id: views_attach_plugin_display_node_content.inc,v 1.1 2009/01/13 08:07:10 crell Exp $

/**
 * The plugin that handles node-attached views.
 */
class views_attach_plugin_display_node_content extends views_plugin_display {
  function option_definition () {
    $options = parent::option_definition();

    $options['types'] = array('default' => array());
    $options['teaser'] = array('default' => FALSE);
    $options['full'] = array('default' => TRUE);

    return $options;
  }

  /**
   * Provide the summary for page options in the views UI.
   *
   * This output is returned as an array.
   */
  function options_summary(&$categories, &$options) {
    // It is very important to call the parent function here:
    parent::options_summary($categories, $options);

    $categories['node_content'] = array(
      'title' => t('Node content settings'),
    );

    $types = $this->get_option('types');
    if (empty($types)) {
      $types = array('story');
    }

    $options['types'] = array(
      'category' => 'node_content',
      'title' => t('Node types'),
      'value' => implode(', ', $types),
    );

    $teaser = $this->get_option('teaser');
    if (empty($teaser)) {
      $teaser = FALSE;
    }

    $options['teaser'] = array(
      'category' => 'node_content',
      'title' => t('Show on teaser'),
      'value' => $teaser ? t('True') : t('False'),
    );

    $full = $this->get_option('full');
    if (empty($full)) {
      $full = TRUE;
    }

    $options['full'] = array(
      'category' => 'node_content',
      'title' => t('Show on full view'),
      'value' => $full ? t('True') : t('False'),
    );

    $weight = $this->get_option('weight');
    if (empty($weight)) {
      $weight = 10;
    }
  }

  /**
   * Provide the default form for setting options.
   */
  function options_form(&$form, &$form_state) {
    // It is very important to call the parent function here:
    parent::options_form($form, $form_state);

    switch ($form_state['section']) {
      case 'types':
        $form['#title'] .= t('Node types');
        $form['types'] = array(
          '#type' => 'select',
          '#multiple' => TRUE,
          '#required' => TRUE,
          '#title' => t("Embed this display in the following node types"),
          '#options' => node_get_types('names'),
          '#default_value' => $this->get_option('types'),
        );
        break;

      case 'teaser':
        $form['#title'] .= t('Teaser view');
        $form['teaser'] = array(
          '#type' => 'checkbox',
          '#title' => t("Show this view on the node's teaser"),
          '#default_value' => $this->get_option('teaser'),
        );
        break;

      case 'full':
        $form['#title'] .= t('Full view');
        $form['full'] = array(
          '#type' => 'checkbox',
          '#title' => t("Show this view on the node's full view"),
          '#default_value' => $this->get_option('full'),
        );
        break;
      }
  }

  function options_submit($form, &$form_state) {
    // It is very important to call the parent function here:
    parent::options_submit($form, $form_state);
    switch ($form_state['section']) {
      case 'types':
        $this->set_option('types', $form_state['values']['types']);
        break;
      case 'teaser':
        $this->set_option('teaser', $form_state['values']['teaser']);
        break;
      case 'full':
        $this->set_option('full', $form_state['values']['full']);
        break;
    }
  }



  /**
   * The display block handler returns the structure necessary for a block.
   */
  function execute() {
    // Prior to this being called, the $view should already be set to this
    // display, and arguments should be set on the view.
    $data = $this->view->render();
    if (!empty($this->view->result) || $this->get_option('empty') || !empty($this->view->style_plugin->definition['even empty'])) {
      return $data;
    }
  }

  /**
   * Block views do not use exposed widgets.
   */
  function uses_exposed() { return FALSE; }
}
