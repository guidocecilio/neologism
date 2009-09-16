<?php

// $Id: ext.theme.inc,v 1.1.2.14 2009/08/07 15:44:40 apanag Exp $

/*
 * @file
 * ext.theme.inc
 */

/*
 * Theme ext_store
 */
function theme_ext_store($rows, $options, $id) {
  return $rows;
}

/*
 * Theme the grid
 */
function theme_ext_js($rows, $options, $id, $columns, $data, $view_name, $display_id) {

  //User configuration
  $options["autoHeight"] = ($options["autoHeight"] == 1) ? 'true' : 'false';
  $options["autoShow"] = ($options["autoShow"] == 1) ? 'true' : 'false';
  $options["border"] = ($options["border"] == 1) ? 'true' : 'false';
  $options["collapseFirst"] = ($options["collapseFirst"] == 1) ? 'true' : 'false';
  $options["collapsed"] = ($options["collapsed"] == 1) ? 'true' : 'false';
  $options["collapsible"] = ($options["collapsible"] == 1) ? 'true' : 'false';
  $options["deferRowRender"] = ($options["deferRowRender "] == 1) ? 'true' : 'false';
  $options["disableSelection"] = ($options["disableSelection"] == 1) ? 'true' : 'false';
  $options["disabled"] = ($options["disabled"] == 1) ? 'true' : 'false';
  $options["enableColumnHide"] = ($options["enableColumnHide"] == 1) ? 'true' : 'false';
  $options["enableColumnMove"] = ($options["enableColumnMove"] == 1) ? 'true' : 'false';
  $options["enableColumnResize"] = ($options["enableColumnResize"] == 1) ? 'true' : 'false';
  $options["enableHdMenu"] = ($options["enableHdMenu"] == 1) ? 'true' : 'false';
  $options["floating"] = ($options["floating"] == 1) ? 'true' : 'false';
  $options["frame"] = ($options["frame"] == 1) ? 'true' : 'false';
  $options["header"] = ($options["header"] == 1) ? 'true' : 'false';
  $options["headerAsText"] = ($options["headerAsText"] == 1) ? 'true' : 'false';
  $options["hidden"] = ($options["hidden"] == 1) ? 'true' : 'false';
  $options["hideCollapseTool"] = ($options["hideCollapseTool"] == 1) ? 'true' : 'false';
  $options["hideHeaders"] = ($options["hideHeaders"] == 1) ? 'true' : 'false';
  $options["hideParent"] = ($options["hideParent"] == 1) ? 'true' : 'false';
  $options["maskDisabled"] = ($options["maskDisabled"] == 1) ? 'true' : 'false';
  $options["shim"] = ($options["shim"] == 1) ? 'true' : 'false';
  $options["stripeRows"] = ($options["stripeRows"] == 1) ? 'true' : 'false';
  $options["trackMouseOver"] = ($options["trackMouseOver"] == 1) ? 'true' : 'false';
  $options["titleCollapse"] = ($options["titleCollapse"] == 1) ? 'true' : 'false';

  foreach ($columns as $column => $dataindex) {
    //create the fields for columns
    $fields .= '{name: "'. $column .'"},';

    // true/false for the custom options
    $fixed = ($options[$column]['fixed'] == 1) ? TRUE : FALSE;
    $sortable = ($options[$column]['sortable'] == 1) ? TRUE : FALSE;
    $hidden = ($options[$column]['hidden'] == 1) ? TRUE : FALSE;
    $menudisabled = ($options[$column]['menuDisabled'] == 1) ? TRUE : FALSE;
    $resizable = ($options[$column]['resizable'] == 1) ? FALSE : TRUE;
    $label = ($options[$column]['header'] == '') ? $dataindex : $options[$column]['header'];
    $hideable = ($options[$column]['hideable'] == 1) ? TRUE : FALSE;
    // Creating the header
    $header .= '{';
    //we only show the methods that have values
    if ($options[$column]['align'] != "") {
      $header .= 'align: "'. $options[$column]['align'] .'",';
    }
    if ($options[$column]['css'] != '') {
      $header .= 'css: "'. $options[$column]['css'] .'",';
    }
    if ($fixed != '') {
      $header .= 'fixed: "'. $fixed .'",';
    }
    if ($label != '') {
      $header .= 'header: "'. $label .'",';
    }
    if ($hidden != '') {
      $header .= 'hidden: "'. $hidden .'",';
    }
    if ($hideable != '') {
      $header .= 'hideable: "'. $hideable .'",';
    }
    if ($options[$column]['id'] != '') {
      $header .= 'id: "'. $options[$column]['id'] .'",';
    }
    if ($menudisabled != '') {
      $header .= 'menudisabled: "'. $menudisabled .'",';
    }
    if ($column != '') {
      $header .= 'dataIndex: "'. $column .'",';
    }
    if ($resizable != '') {
      $header .= 'resizable: "'. $resizable .'",';
    }
    if ($sortable != '') {
      $header .= 'sortable: "'. $sortable .'",';
    }
    if ($options[$column]['tooltip'] != '') {
      $header .= 'tooltip: "'. $options[$column]['tooltip'] .'",';
    }
    if ($options[$column]['width'] != '') {
      $header .= 'width: "'. $options[$column]['width'] .'"';
    }
    $header .= '},';
  }

  //determine the store
  $store = (variable_get('ext_store', 'SimpleStore') == 'JsonStore') ? 'JsonStore' : 'SimpleStore';

  //We checked for autoExpandMax/Min
  //if autoExpandMax/Min are greater than zero, only then we use them
  //otherwise we need at least one column with ID
  if (is_int($options['autoExpandMax']) && $options['autoExpandMax'] >= 0) {
    $autoexpand  = 'autoExpandMax: \''. $options['autoExpandMax'] .'\',';
  }

  if (is_int($options['autoExpandMax']) && $options['autoExpandMin'] >= 0) {
    $autoexpand .= 'autoExpandMin: \''. $options['autoExpandMin'] .'\',';
  }

  if ($store == 'JsonStore') {
    $store = '
               var data = Ext.data.Record.create([
                 '. $fields .'
               ]);

               var myReader = new Ext.data.JsonReader({
                 root: "rows",
                 id: "nid"
               }, data);

               var store = new Ext.data.JsonStore({
                 autoLoad: true,
				 totalProperty: "totalRows",
			     reader: myReader,
				 url: "?q=ext/json/'. $view_name .'/'. $display_id .'",
				 root: "rows",
				 fields: [ '. $fields .' ]
			   });';
  }
  else {
    $store = '
              var mydata = ['. $data .'];

              var store = new Ext.data.SimpleStore({
                 root: "rows",
				 fields: [ '. $fields .' ]
			   });

			   store.loadData(mydata);
			   ';
  }

  $js = '

  Ext.onReady(function(){

    Ext.QuickTips.init();

    '. $store .'
    var grid = new Ext.grid.GridPanel({
	    store: store,
	    viewConfig: {
	        forceFit: true
	    },
	    columns: [
	        '. $header .'
	    ],
	    sm: new Ext.grid.RowSelectionModel({singleSelect:true}),
		'. $autoexpand .'
	    autoHeight:          '. $options['autoHeight'] .',
	    autoShow:            '. $options['autoShow'] .',
	    baseCls:             \''. $options['baseCls'] .'\',
	    buttonAlign:         \''. $options['buttonAlign'] .'\',
	    border:              '. $options['border'] .',
	    cls:                 \''. $options['cls'] .'\',
	    collapseFirst:       '. $options['collapseFirst'] .',
	    collapsed:           '. $options['collapsed'] .',
	    collapsedCls:        \''. $options['collapsedCls'] .'\',
	    collapsible:         '. $options['collapsible'] .',
	    ctCls:               \''. $options['ctCls'] .'\',
	    deferRowRender:      '. $options['deferRowRender'] .',
	    disableSelection:    '. $options['disableSelection'] .',
	    disabled:            '. $options['disabled'] .',
	    enableColumnHide:    '. $options['enableColumnHide'] .',
	    enableColumnMove:    '. $options['enableColumnMove'] .',
	    enableColumnResize:  '. $options['enableColumnResize'] .',
	    enableHdMenu:        '. $options['enableHdMenu'] .',
	    floating:            '. $options['floating'] .',
	    frame:               '. $options['frame'] .',
	    headerAsText:        '. $options['headerAsText'] .',
	    height:              '. $options['grid_height'] .',
	    hidden:              '. $options['hidden'] .',
	    hideCollapseTool:    '. $options['hideCollapseTool'] .',
	    hideHeaders:         '. $options['hideHeaders'] .',
	    hideMode:            \''. $options['hideMode'] .'\',
	    hideParent:          '. $options['hideParent'] .',
	    iconCls:             \''. $options['iconCls'] .'\',
	    id:                  \''. $options['id'] .'\',
	    maskDisabled:        '. $options['maskDisabled'] .',
	    maxHeight:           \''. $options['maxHeight'] .'\',
	    minButtonWidth:      \''. $options['minButtonWidth'] .'\',
	    minColumnWidth:      \''. $options['minColumnWidth'] .'\',
	    overCls:             \''. $options['overCls'] .'\',
	    pageX:               \''. $options['pageX'] .'\',
	    pageY:               \''. $options['pageY'] .'\',
	    shadowOffset:        \''. $options['shadowOffset'] .'\',
	    shim:                '. $options['shim'] .',
	    stripeRows:          '. $options['stripeRows'] .',
	    title:               "'. $options['grid_title'] .'",
	    titleCollapse:       '. $options['titleCollapse'] .',
	    trackMouseOver:      '. $options['trackMouseOver'] .',
	    width :              '. $options['grid_width'] .',
	    x:                   \''. $otpions['x'] .'\',
	    y:                   \''. $options['y'] .'\',
	    renderTo:            "extjs-grid"
	});
  });';
  return $js;
}