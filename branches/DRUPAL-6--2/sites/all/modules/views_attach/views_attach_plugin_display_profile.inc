<?php
// $Id: views_attach_plugin_display_profile.inc,v 1.1 2009/01/13 08:07:10 crell Exp $

/**
 * The plugin that handles a user profile.
 */
class views_attach_plugin_display_profile extends views_plugin_display {
  function option_definition () {
    $options = parent::option_definition();

    $options['weight'] = array('default' => 10);
    $options['category'] = array('default' => NULL);

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

    $categories['profile'] = array(
      'title' => t('Profile settings'),
    );

    $category = $this->get_option('category');
    if (empty($category)) {
      $category = t('Default');
    }

    $options['category'] = array(
      'category' => 'profile',
      'title' => t('Category'),
      'value' => $category,
    );

    $weight = $this->get_option('weight');
    if (empty($weight)) {
      $weight = 10;
    }

    $options['weight'] = array(
      'category' => 'profile',
      'title' => t('Weight'),
      'value' => $weight,
    );
  }

  /**
   * Provide the default form for setting options.
   */
  function options_form(&$form, &$form_state) {
    // It is very important to call the parent function here:
    parent::options_form($form, $form_state);

    switch ($form_state['section']) {
      case 'weight':
        $form['#title'] .= t('Weight');
        $form['weight'] = array(
          '#type' => 'weight',
          '#description' => t('The position of this view in relation to other profile elements.'),
          '#default_value' => $this->get_option('weight'),
        );
        break;
      case 'category':
        $form['#title'] .= t('Category');
        $form['category'] = array(
          '#type' => 'textfield',
          '#description' => t('The name of the profile tab this view should be listed in. If empty, it will be displayed on the main profile tab.'),
          '#default_value' => $this->get_option('category'),
        );
        break;
    }
  }

  function options_submit($form, &$form_state) {
    // It is very important to call the parent function here:
    parent::options_submit($form, $form_state);
    switch ($form_state['section']) {
      case 'weight':
        $this->set_option('weight', $form_state['values']['weight']);
        break;
      case 'category':
        $this->set_option('category', $form_state['values']['category']);
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
