<?php
// $Id: ext_style_plugin.inc,v 1.1.2.18 2009/08/07 15:44:40 apanag Exp $
/**
 * @file
 *  Provide the views carousel plugin object with default options and form.
 */

/**
  * Implementation of views_plugin_style().
  */
class ext_style_plugin extends views_plugin_style {
  function option_definition() {
    $options = parent::option_definition();

    //1 => checked
    //0 => unchecked
    $options['autoHeight'] = array('default' => '0');
    $options['autoExpandMin'] = array('default' => '');
    $options['autoExpandMax'] = array('default' => '');
    $options['autoShow'] = array('default' => '0');
    $options['baseCls'] = array('default' => 'x-panel');
    $options['border'] = array('default' => '1');
    $options['buttonAlign'] = array('default' => 'right');
    $options['cls'] = array('default' => '');
    $options['collapseFirst'] = array('default' => '1');
    $options['collapsed'] = array('default' => '0');
    $options['collapsedCls'] = array('default' => 'x-panel-collapsed');
    $options['collapsible'] = array('default' => '0');
    $options['ctCls'] = array('default' => '');
    $options['deferRowRender'] = array('default' => '1');
    $options['disableSelection'] = array('default' => '0');
    $options['disabled'] = array('default' => '0');
    $options['enableColumnMove'] = array('default' => '0');
    $options['enableColumnResize'] = array('default' => '0');
    $options['enableColumnHide'] = array('default' => '1');
    $options['enableHdMenu'] = array('default' => '0');
    $options['floating'] = array('default' => '0');
    $options['frame'] = array('default' => '1');
    $options['header'] = array('default' => '');
    $options['headerAsText'] = array('default' => '1');
    $options['hidden'] = array('default' => '0');
    $options['hideCollapseTool'] = array('default' => '0');
    $options['hideHeaders'] = array('default' => '0');
    $options['hideMode'] = array('default' => 'display');
    $options['hideParent'] = array('default' => '0');
    $options['iconCls'] = array('default' => '');
    $options['id'] = array('default' => '');
    $options['maskDisabled'] = array('default' => '1');
    $options['maxHeight'] = array('default' => '');
    $options['minButtonWidth'] = array('default' => 75);
    $options['minColumnWidth'] = array('default' => 25);
    $options['overCls'] = array('default' => '');
    $options['pageX'] = array('default' => '');
    $options['pageY'] = array('default' => '');
    $options['shadowOffset'] = array('default' => '4');
    $options['shim'] = array('default' => '1');
    $options['stripeRows'] = array('default' => '0');
    $options['titleCollapse'] = array('default' => '0');
    $options['trackMouseOver'] = array('default' => '1');
    $options['settings']['grid_title'] = array('default' => '');
    $options['grid_width'] = array('default' => '800');
    $options['grid_height'] = array('default' => '600');
    $options['x'] = array('default' => '');
    $options['y'] = array('default' => '');
    return $options;
  }

  function create_columns() {
    $fields = $this->display->handler->get_option('fields');

    foreach ($fields as $field => $header) {
      $columns[$field] = $field;
    }

    return $columns;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $columns = $this->create_columns();

    foreach ($columns as $column) {
      $form[$column] = array(
        '#title' => t('Settings for '. $column .' column'),
        '#type'  => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );

      //Align
      $form[$column]['align'] = array(
        '#type' => 'radios',
        '#title' => t('Align'),
        '#options' => array( 'left' => 'left' , 'right' => 'right', 'center' => 'center', 'justify' => 'justify', 'inherit' => 'inherit'),
        '#description' => t('Set the CSS text-align property of the column. Defaults to undefined.'),
        '#default_value' => $this->options[$column]['align'],
      );

      //CSS
      $form[$column]['css'] = array(
        '#type' => 'textfield',
        '#title' => t('CSS'),
        '#description' => t('Set custom CSS for all table cells in the column (excluding headers). Defaults to undefined.'),
        '#size' => 25,
        '#default_value' => $this->options[$column]['css'],
      );

      //Fixed
      $form[$column]['fixed'] = array(
        '#type' => 'checkbox',
        '#title' => t('Fixed'),
        '#default_value' => $this->options[$column]['fixed'],
        '#description' => 'Checked if the column width cannot be changed. Unchecked to false.',
      );

      //header
      $form[$column]['header'] = array(
        '#type' => 'textfield',
        '#title' => t('Header'),
        '#default_value' => $this->options[$column]['header'],
        '#description' => 'The header text to display in the Grid view.',
      );

      //hideable
      $form[$column]['hideable'] = array(
        '#type' => 'checkbox',
        '#title' => t('Hideable'),
        '#default_value' => $this->options[$column]['hideable'],
        '#description' => 'Uncheck to prevent the user from hiding this column (defaults to checked). To disallow column hiding globally for all columns in the grid, use EnableColumnHide from the config options instead.',
      );

      //id
      $form[$column]['id'] = array(
        '#type' => 'textfield',
        '#title' => t('Id'),
        '#default_value' => $this->options[$column]['id'],
        '#description' => 'Defaults to the column\'s initial ordinal position. A name which identifies this column. The id is used to create a CSS class name which is applied to all table cells (including headers) in that column. The class name takes the form of x-grid3-td-<b>id</b>. Header cells will also recieve this class name, but will also have the class x-grid3-hd, so to target header cells, use CSS selectors such as: .x-grid3-hd.x-grid3-td-<b>id</b>.The Ext.grid.GridPanel.autoExpandColumn grid config option references the column via this identifier.',
      );

      //Hidden
      $form[$column]['hidden'] = array(
        '#type' => 'checkbox',
        '#title' => t('Hidden'),
        '#default_value' => $this->options[$column]['hidden'],
        '#description' => 'Checked to hide the column. Defaults to unchecked.',
      );

      //menuDisabled
      $form[$column]['menuDisabled'] = array(
        '#type' => 'checkbox',
        '#title' => t('MenuDisabled'),
        '#default_value' => $this->options[$column]['menuDisabled'],
        '#description' => 'Checked to disable the column menu. Defaults to unchecked.',
      );

      //Resizable
      $form[$column]['resizable'] = array(
        '#type' => 'checkbox',
        '#title' => t('Resizable'),
        '#default_value' => $this->options[$column]['resizable'],
        '#description' => 'Check to disable column resizing. Defaults to unchecked.',
      );

      //Sorting
      $form[$column]['sortable'] = array(
        '#type' => 'checkbox',
        '#title' => t('Sortable'),
        '#default_value' => $this->options[$column]['sortable'],
        '#description' => 'True if sorting is to be allowed on this column. Defaults to the value of the defaultSortable property. Whether local/remote sorting is used is specified in Ext.data.Store.remoteSort.',
      );

      //Tooltip
      $form[$column]['tooltip'] = array(
        '#type' => 'textfield',
        '#title' => t('Tooltip'),
        '#description' => t('A text string to use as the column header\'s tooltip. If Quicktips are enabled, this value will be used as the text of the quick tip, otherwise it will be set as the header\'s HTML title attribute. Defaults to \'\'.'),
        '#size' => 25,
        '#default_value' => $this->options[$column]['tooltip'],
      );

      //Width
      $form[$column]['width'] = array(
        '#type' => 'textfield',
        '#title' => t('Width'),
        '#description' => t('The initial width in pixels of the column.'),
        '#size' => 25,
        '#default_value' => $this->options[$column]['width'],
      );
    }

    //Title of the grid
    $form['grid_title'] = array(
      '#type' => 'textfield',
      '#title' => t('Title'),
      '#description' => t('The title of the grid.'),
      '#size' => 25,
      '#default_value' => $this->options['grid_title'],
    );

    //Default width
    $form['grid_width'] = array(
      '#type' => 'textfield',
      '#title' => t('Width'),
      '#description' => t('A grid requires a width in which to scroll its columns.'),
      '#size' => 6,
      '#maxlength' => 6,
      '#required' => TRUE,
      '#default_value' => $this->options['grid_width'],
    );

    //Default height
    $form['grid_height'] = array(
      '#type' => 'textfield',
      '#title' => t('Height'),
      '#description' => t('A grid requires a height in which to scroll its rows.'),
      '#size' => 6,
      '#maxlength' => 6,
      '#required' => TRUE,
      '#default_value' => $this->options['grid_height'],
    );

    //AutoExpandMax
    $form['autoExpandMax'] = array(
      '#type' => 'textfield',
      '#title' => t('AutoExpandMax'),
      '#description' => t('The maximum width the autoExpandColumn can have (if enabled). Defaults to 1000.'),
      '#size' => 6,
      '#maxlength' => 6,
      '#default_value' => $this->options['autoExpandMax'],
    );

    //AutoExpandMin
    $form['autoExpandMin'] = array(
      '#type' => 'textfield',
      '#title' => t('AutoExpandMin'),
      '#description' => t('The minimum width the autoExpandColumn can have (if enabled). defaults to 50.'),
      '#size' => 6,
      '#maxlength' => 6,
      '#default_value' => $this->options['autoExpandMin'],
    );

    //AutoHeight
    $form['autoHeight'] = array(
      '#type' => 'checkbox',
      '#title' => t('AutoHeight'),
      '#default_value' => $this->options['autoHeight'],
      '#description' => 'Check to use height:\'auto\', uncheck to use fixed height. Note: although many components inherit this config option, not all will function as expected with a height of \'auto\'(defaults to unchecked).',
    );

    //AutoShow
    $form['autoShow'] = array(
      '#type' => 'checkbox',
      '#title' => t('AutoShow'),
      '#default_value' => $this->options['autoShow'],
      '#description' => 'Check if the component should check for hidden classes (e.g. \'x-hidden\' or \'x-hide-display\') and remove them on render(defaults to unchecked).',
    );

    //BaseCls
    $form['baseCls'] = array(
      '#type' => 'textfield',
      '#title' => t('BaseCls'),
      '#description' => t('The base CSS class to apply to this panel\'s element (defaults to \'x-panel\').'),
      '#size' => 15,
      '#maxlength' => 60,
      '#default_value' => $this->options['baseCls'],
    );

    //Border
    $form['border'] = array(
      '#type' => 'checkbox',
      '#title' => t('Border'),
      '#default_value' => $this->options['border'],
      '#description' => 'Check to display the borders of the panel\'s body element, uncheck to hide them (defaults to true). By default, the border is a 2px wide inset border, but this can be further altered by setting bodyBorder to false.',
    );

    //ButtonAlign
    $form['buttonAlign'] = array(
      '#type' => 'radios',
      '#title' => t('ButtonAlign'),
      '#description' => t('The alignment of any buttons added to this panel. Valid values are \'right,\' \'left\' and \'center\' (defaults to \'right\').'),
      '#size' => 6,
      '#maxlength' => 6,
      '#options' => array('right', 'left', 'center'),
      '#default_value' => $this->options['buttonAlign'],
    );

    //Cls
    $form['cls'] = array(
      '#type' => 'textfield',
      '#title' => t('Cls'),
      '#description' => t('An optional extra CSS class that will be added to this component\'s Element (defaults to \'\'). This can be useful for adding customized styles to the component or any of its children using standard CSS rules.'),
      '#size' => 15,
      '#maxlength' => 60,
      '#default_value' => $this->options['cls'],
    );

    //CollapseFirst
    $form['collapseFirst'] = array(
      '#type' => 'checkbox',
      '#title' => t('CollapseFirst'),
      '#default_value' => $this->options['collapseFirst'],
      '#description' => 'Check to make sure the collapse/expand toggle button always renders first (to the left of) any other tools in the panel\'s title bar, uncheck to render it last(defaults to checked).',
    );

    //Collapsed
    $form['collapsed'] = array(
      '#type' => 'checkbox',
      '#title' => t('Collapsed'),
      '#default_value' => $this->options['collapsed'],
      '#description' => 'Checked to make the panel collapsible and have the expand/collapse toggle button automatically rendered into the header tool button area, uncheck to keep the panel statically sized with no button(defaults to unchecked).'
    );

    //CollapsedCls
    $form['collapsedCls'] = array(
      '#type' => 'textfield',
      '#title' => t('CollapsedCls'),
      '#description' => t('A CSS class to add to the panel\'s element after it has been collapsed (defaults to \'x-panel-collapsed\').'),
      '#size' => 15,
      '#maxlength' => 60,
      '#default_value' => $this->options['collapsedCls'],
    );

    //Collapsible
    $form['collapsible'] = array(
      '#type' => 'checkbox',
      '#title' => t('Collapsible'),
      '#default_value' => $this->options['collapsible'],
      '#description' => 'Check to render the panel collapsed, uncheck to render it expanded(defaults to unchecked).'
    );

    //CtCls
    $form['ctCls'] = array(
      '#type' => 'textfield',
      '#title' => t('CtCls'),
      '#description' => t('An optional extra CSS class that will be added to this component\'s container (defaults to \'\'). This can be useful for adding customized styles to the container or any of its children using standard CSS rules.'),
      '#size' => 15,
      '#maxlength' => 60,
      '#default_value' => $this->options['ctCls'],
    );

    //DeferRowRender
    $form['deferRowRender'] = array(
      '#type' => 'checkbox',
      '#title' => t('DeferRowRender'),
      '#default_value' => $this->options['deferRowRender'],
      '#description' => 'Check to enable deferred row rendering(defaults to check).'
    );

    //DisableSelection
    $form['disableSelection'] = array(
      '#type' => 'checkbox',
      '#title' => t('DisableSelection'),
      '#default_value' => $this->options['disableSelection'],
      '#description' => 'Check to disable selections in the grid (defaults to unchecked). - ignored if a SelectionModel is specified.'
    );

    //Disabled
    $form['disabled'] = array(
      '#type' => 'checkbox',
      '#title' => t('Disabled'),
      '#default_value' => $this->options['disabled'],
      '#description' => 'Render this component disabled(defaults to unchecked).'
    );

    //EnableColumnHide
    $form['enableColumnHide'] = array(
      '#type' => 'checkbox',
      '#title' => t('EnableColumnHide'),
      '#default_value' => $this->options['enableColumnHide'],
      '#description' => 'Check to enable hiding of columns with the header context menu(defaults to unchecked).'
    );

    //EnableColumnMove
    $form['enableColumnMove'] = array(
      '#type' => 'checkbox',
      '#title' => t('EnableColumnMove'),
      '#default_value' => $this->options['enableColumnMove'],
      '#description' => 'Check to enable drag and drop reorder of columns(defaults to unchecked).'
    );

    //EnableColumnResize
    $form['enableColumnResize'] = array(
      '#type' => 'checkbox',
      '#title' => t('EnableColumnResize'),
      '#default_value' => $this->options['enableColumnResize'],
      '#description' => 'Check to turn off column resizing for the whole grid(defaults to checked).'
    );

    //EnableHdMenu
    $form['enableHdMenu'] = array(
      '#type' => 'checkbox',
      '#title' => t('EnableHdMenu'),
      '#default_value' => $this->options['enableHdMenu'],
      '#description' => 'Check to enable the drop down button for menu in the headers(defaults to unchecked).'
    );

    //Floating
    $form['floating'] = array(
      '#type' => 'checkbox',
      '#title' => t('Floating'),
      '#default_value' => $this->options['floating'],
      '#description' => 'Checked to float the panel (absolute position it with automatic shimming and shadow), false to display it inline where it is rendered (defaults to unchecked). Note that by default, setting floating to true will cause the panel to display at negative offsets so that it is hidden -- because the panel is absolute positioned, the position must be set explicitly after render (e.g., myPanel.setPosition(100,100);). Also, when floating a panel you should always assign a fixed width, otherwise it will be auto width and will expand to fill to the right edge of the viewport.'
    );

    //Frame
    $form['frame'] = array(
      '#type' => 'checkbox',
      '#title' => t('Frame'),
      '#default_value' => $this->options['frame'],
      '#description' => 'Check to render the panel with custom rounded borders, uncheck to render with plain 1px square borders(defaults to unchecked).'
    );

    //Header
    $form['header'] = array(
      '#type' => 'checkbox',
      '#title' => t('Header'),
      '#default_value' => $this->options['header'],
      '#description' => 'Check to create the header element explicitly, uncheck to skip creating it. By default, when header is not specified, if a title is set the header will be created automatically, otherwise it will not. If a title is set but header is explicitly set to false, the header will not be rendered.'
    );

    //HeaderAsText
    $form['headerAsText'] = array(
      '#type' => 'checkbox',
      '#title' => t('HeaderAsText'),
      '#default_value' => $this->options['headerAsText'],
      '#description' => 'Check to display the panel title in the header, uncheck to hide it(defaults to checked).'
    );

    //HideCollapseTool
    $form['hideCollapseTool'] = array(
      '#type' => 'checkbox',
      '#title' => t('HideCollapseTool'),
      '#default_value' => $this->options['hideCollapseTool'],
      '#description' => 'Check to hide the expand/collapse toggle button when collapsible = true, uncheck to display it (defaults to unchecked).'
    );

    //Hidden
    $form['hidden'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hidden'),
      '#default_value' => $this->options['hidden'],
      '#description' => 'Check to render this component hidden(defaults to unchecked).'
    );

    //HideHeaders
    $form['hideHeaders'] = array(
      '#type' => 'checkbox',
      '#title' => t('HideHeaders'),
      '#default_value' => $this->options['hideHeaders'],
      '#description' => 'Check to hide the grid\'s header (defaults to unchecked).'
    );

    //hideMode
    $form['hideMode'] = array(
      '#type' => 'radios',
      '#title' => t('HideMode'),
      '#description' => t('How this component should hidden. Supported values are "visibility" (css visibility), "offsets" (negative offset position) and "display" (css display) - defaults to "display".'),
      '#options' => array('visibility' => 'visibility', 'offsets' => 'offsets', 'display' => 'display'),
      '#default_value' => $this->options['hideMode'],
    );

    //HideParent
    $form['hideParent'] = array(
      '#type' => 'checkbox',
      '#title' => t('HideParent'),
      '#default_value' => $this->options['hideParent'],
      '#description' => 'Check to hide and show the component\'s container when hide/show is called on the component, uncheck to hide and show the component itself (defaults to unchecked). For example, this can be used as a shortcut for a hide button on a window by setting hide:true on the button when adding it to its parent container.'
    );

    //iconCls
    $form['iconCls'] = array(
      '#type' => 'textfield',
      '#title' => t('IconCls'),
      '#description' => t('A CSS class that will provide a background image to be used as the header icon (defaults to \'\'). An example custom icon class would be something like: .my-icon { background: url(../images/my-icon.gif) 0 6px no-repeat !important;}'),
      '#size' => 15,
      '#maxlength' => 60,
      '#default_value' => $this->options['iconCls'],
    );

    //id
    $form['id'] = array(
      '#type' => 'textfield',
      '#title' => t('Id'),
      '#description' => t('The unique id of this component (defaults to an auto-assigned id).'),
      '#size' => 15,
      '#maxlength' => 60,
      '#default_value' => $this->options['id'],
    );

    //MaskDisabled
    $form['maskDisabled'] = array(
      '#type' => 'checkbox',
      '#title' => t('MaskDisabled'),
      '#default_value' => $this->options['maskDisabled'],
      '#description' => 'Check to mask the panel when it is disabled, uncheck to not mask it (defaults to true). Either way, the panel will always tell its contained elements to disable themselves when it is disabled, but masking the panel can provide an additional visual cue that the panel is disabled.'
    );

    //maxHeight
    $form['maxHeight'] = array(
      '#type' => 'textfield',
      '#title' => t('MaxHeight'),
      '#description' => t('Sets the maximum height of the grid - ignored if autoHeight is not on.'),
      '#size' => 15,
      '#maxlength' => 60,
      '#default_value' => $this->options['maxHeight'],
    );

    //minButtonWidth
    $form['minButtonWidth'] = array(
      '#type' => 'textfield',
      '#title' => t('MinButtonWidth'),
      '#description' => t('Minimum width in pixels of all buttons in this panel (defaults to 75)'),
      '#size' => 15,
      '#maxlength' => 60,
      '#default_value' => $this->options['minButtonWidth'],
    );

    //minColumnWidth
    $form['minColumnWidth'] = array(
      '#type' => 'textfield',
      '#title' => t('MinColumnWidth'),
      '#description' => t('The minimum width a column can be resized to. Defaults to 25.'),
      '#size' => 15,
      '#maxlength' => 60,
      '#default_value' => $this->options['minColumnWidth'],
    );

    //OverCls
    $form['overCls'] = array(
      '#type' => 'textfield',
      '#title' => t('overCls'),
      '#description' => t('An optional extra CSS class that will be added to this component\'s Element when the mouse moves over the Element, and removed when the mouse moves out. (defaults to \'\'). This can be useful for adding customized "active" or "hover" styles to the component or any of its children using standard CSS rules.'),
      '#size' => 15,
      '#maxlength' => 60,
      '#default_value' => $this->options['overCls'],
    );

    //PageX
    $form['pageX'] = array(
      '#type' => 'textfield',
      '#title' => t('PageX'),
      '#description' => t('The page level x coordinate for this component if contained within a positioning container.'),
      '#size' => 15,
      '#maxlength' => 60,
      '#default_value' => $this->options['pageX'],
    );

    //PageY
    $form['pageY'] = array(
      '#type' => 'textfield',
      '#title' => t('pageY'),
      '#description' => t('The page level y coordinate for this component if contained within a positioning container.'),
      '#size' => 15,
      '#maxlength' => 60,
      '#default_value' => $this->options['pageY'],
    );

    //shadowOffset
    $form['shadowOffset'] = array(
      '#type' => 'textfield',
      '#title' => t('ShadowOffset'),
      '#description' => t('The number of pixels to offset the shadow if displayed (defaults to 4). Note that this option only applies when floating = true.'),
      '#size' => 15,
      '#maxlength' => 60,
      '#default_value' => $this->options['shadowOffset'],
    );

    //Shim
    $form['shim'] = array(
      '#type' => 'checkbox',
      '#title' => t('Shim'),
      '#default_value' => $this->options['shim'],
      '#description' => 'Uncheck to disable the iframe shim in browsers which need one (defaults to checked). Note that this option only applies when floating = true.'
    );

    //StripeRows
    $form['stripeRows'] = array(
      '#type' => 'checkbox',
      '#title' => t('StripeRows'),
      '#default_value' => $this->options['stripeRows'],
      '#description' => 'Check to stripe the rows(defaults to unchecked).'
    );

    //TitleCollapse
    $form['titleCollapse'] = array(
      '#type' => 'checkbox',
      '#title' => t('TitleCollapse'),
      '#default_value' => $this->options['titleCollapse'],
      '#description' => 'Checked to allow expanding and collapsing the panel (when collapsible = checked) by clicking anywhere in the header bar, unchecked to allow it only by clicking to tool button (defaults to unchecked).'
    );

    //TrackMouseOver
    $form['trackMouseOver'] = array(
      '#type' => 'checkbox',
      '#title' => t('TrackMouseOver'),
      '#default_value' => $this->options['trackMouseOver'],
      '#description' => 'Check to highlight rows when the mouse is over(defaults to checked).'
    );

    //x
    $form['x'] = array(
      '#type' => 'textfield',
      '#title' => t('X'),
      '#description' => t('The local x (left) coordinate for this component if contained within a positioning container.'),
      '#size' => 15,
      '#maxlength' => 60,
      '#default_value' => $this->options['x'],
    );

    //y
    $form['y'] = array(
      '#type' => 'textfield',
      '#title' => t('Y'),
      '#description' => t('The local y (top) coordinate for this component if contained within a positioning container.'),
      '#size' => 15,
      '#maxlength' => 60,
      '#default_value' => $this->options['y'],
    );
  }
}