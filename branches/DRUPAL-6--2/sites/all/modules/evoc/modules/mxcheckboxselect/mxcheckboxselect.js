/**
 * @author guicec
 */

var EvocWidget = {};
//EvocWidget.classSelection = new Array();

/**
* appends a javascript array to a form
*
* @param array array - the array
* @param string name - name of the array 
* @param mixed form - the form
*/
EvocWidget.convertJsArrayToPhpArray = function( array, name, form ) {
  
  if ( typeof( form ) == 'string' ) {
    form = document.getElementById( form );
  }
  
  var hidden = null;
  for( index = 0; index < array.length; index++ ) {
    hidden = document.createElement( 'input' );
    hidden.setAttribute( 'type', 'hidden' );
    hidden.setAttribute( 'name', name + '[' + index +']' );
    hidden.setAttribute( 'value', array[index]);
    form.appendChild( hidden );
  }
  
  return true;
}

/**
 * This function create input field for all the Drupal.settings.neologism.field_values
 * 
 * @param {Object} formId
 */
EvocWidget.onsubmitCreateInputFields = function(formId){

  //alert('onsubmitCreateInputFields(' + formId.toString() + ')');
 
  for (field in Drupal.settings.neologism.field_values ) {
    //alert(field);
    // it's very important know that we are using field.toString() + "_values" to hold the values.
    // so, we need to access it from the server side (PHP) as $field_name."_values"
    //alert(Drupal.settings.neologism.field_values[field]);
    EvocWidget.convertJsArrayToPhpArray(
      Drupal.settings.neologism.field_values[field], 
      field.toString() + "_values", 
      formId
    );    
  }
  
  //alert('before return: ' + formId.toString());
  return true;
}

/**
 * Create the class selection widget behaviour for filed_superclass2 
 * 
 * @param {Object} field_name
 */
EvocWidget.createClassSelecctionWidget = function( field_name ) {
  
  var objectToRender = Drupal.settings.neologism.field_id[field_name];
  var dataUrl = Drupal.settings.neologism.json_url[field_name];
  //var baseParams = Drupal.settings.neologism.field_selected_values;
   
  // we need to past the baseParams as and object, that is why we creat the baseParams object
  // and add the arrayOfValues array 
  var baseParams = {};
  //Drupal.settings.neologism.field_values[field_name] = Drupal.parseJson(Drupal.settings.neologism.field_values[field_name]);
  Drupal.settings.neologism.field_values[field_name] = Ext.util.JSON.decode(Drupal.settings.neologism.field_values[field_name]);
  baseParams.arrayOfValues = Drupal.settings.neologism.field_values[field_name];
 
  var treeLoader = new Ext.tree.TreeLoader({
    dataUrl: dataUrl,
    baseParams: baseParams,
    
    listeners: {
      // load : ( Object This, Object node, Object response )
      // Fires when the node has been successfuly loaded.
      // added event to refresh the checkbox from its parent 
      load: function(loader, node, response){
        node.eachChild(function(currentNode){
          node.getOwnerTree().expandPath(currentNode.getPath());
          currentNode.cascade( function() {
            for (var j = 0, lenValues = baseParams.arrayOfValues.length; j < lenValues; j++) {
              if (this.id == baseParams.arrayOfValues[j]) {
                this.getUI().toggleCheck(true);
              }
            }
          }, null);
        });
        
        //node.expand(false);
        EvocWidget.disjointWithTreePanel.render(EvocWidget.objectToRender);
        EvocWidget.disjointWithTreePanel.getRootNode().expand(true, false);
              
        //node.getOwnerTree().enable();
      }
    }
  });
    
    // SET the root node.
  var rootNode = new Ext.tree.AsyncTreeNode({
    text	: Drupal.t('Thing / Superclass'),
    id		: 'root',                  // this IS the id of the startnode
    iconCls: 'class-samevoc',
    disabled: true,
    expanded: false,
  });
  
  EvocWidget.superclassesTreePanel = new Ext.tree.TreePanel({
    renderTo         : objectToRender,
    title            : Drupal.t('Defined classes'),
    useArrows        : true,  
    collapsible      : true,
    animCollapse     : true,
    border           : true,
    autoScroll       : true,
    animate          : true,
    enableDD         : false,
    containerScroll  : true,
    height           : 400,
    width            : '100%',
    disabled         : true,
    loader           : treeLoader,
    rootVisible      : false,
    root             : rootNode,
    
    tbar: {
      cls:'top-toolbar',
      items:[' ',
        {
          xtype: 'tbbutton',
          iconCls: 'icon-expand-all', 
          tooltip: Drupal.t('Expand all'),
          handler: function(){ 
            rootNode.expand(true); 
          }
        }, {
          xtype: 'tbseparator' // equivalent to '-'
        }, {
          iconCls: 'icon-collapse-all',
          tooltip: Drupal.t('Collapse all'),
          handler: function(){ 
            rootNode.collapse(true); 
          }
        }
      ]
    },
     
    // listeners for EvocWidget.superclassesTree TreePanel object           
    listeners: {
      // behaviour for on checkchange in EvocWidget.superclassesTree TreePanel object 
      checkchange: function(node, checked) {
        if ( checked && node.parentNode !== null ) {
          // if we're checking the box, check it all the way up
    			if ( node.parentNode.isRoot || !node.parentNode.getUI().isChecked() ) {
            //Ext.Msg.alert('Checkbox status', 'Checked: "' + node.attributes.text);
            //EvocWidget.classSelection.push(node.id);
            //alert(node.id);
            if ( baseParams.arrayOfValues.indexOf(node.id) == -1 ) {
              baseParams.arrayOfValues.push(node.id);
            }
            
            //alert(EvocWidget.disjointWithTree);
            // disable all the classes in disjoint tree
            EvocWidget.disjointWithTreePanel.expandPath(node.getPath());
            disjointWithNode = EvocWidget.disjointWithTreePanel.getNodeById(node.id);
            // when the EvocWidget.superclassesTree is expanding its nodes the
            // EvocWidget.disjointWithTree has not load its nodes yet, so we need to check it
            // to avoid errors 
            //alert(disjointWithNode);
            if( disjointWithNode !== undefined ) {
              disjointWithNode.bubble( function(){
                this.getUI().checkbox.disabled	= true;
                this.getUI().addClass('complete');
                // if this node is the root node then return false to stop the bubble process
                if ( this.parentNode.id == EvocWidget.disjointWithTreePanel.getRootNode().id ) {
                  return false;
                }
      				});
            }
            //else {
              //Ext.Msg.alert('Synchronization error!', 'There is not synchronization between disjointWith TreePanel and superclasses TreePanel');
            //}
            
          }
    		} else {
          for (var i = 0, len = baseParams.arrayOfValues.length; i < len; i++) {
            if ( baseParams.arrayOfValues[i] == node.attributes.id ) {
              //alert(node.getPath());
              baseParams.arrayOfValues.splice(i, 1);
              
              // enable all the classes in disjoint tree
              EvocWidget.disjointWithTreePanel.expandPath(node.getPath());
              EvocWidget.disjointWithTreePanel.getNodeById(node.id).bubble( function(){
                this.getUI().checkbox.disabled	= false;
                this.getUI().removeClass('complete');
                
                // stop the bubble if the parent is the root node
                if ( this.parentNode.id == EvocWidget.disjointWithTreePanel.getRootNode().id ) {
                  return false;
                }
                
                // Loop through its childen
                for (var i = 0, len = this.parentNode.childNodes.length; i < len; i++) {
                  var currentChild = this.parentNode.childNodes[i];
                              
                  // if this child is disable so we need to keep its parent disable, return false to stop
                  // bubble process
                  if ( currentChild.getUI().checkbox.disabled == true ) {
                    return false;
                  }
                }
    				  });
             
            }
          }    
        }

        this.expandPath(node.getPath());
        node.eachChild( function(currentNode){
          currentNode.getUI().toggleCheck(checked);
          if( currentNode.getUI().nodeClass != 'complete' ) {
            currentNode.getUI().checkbox.disabled = checked;  
          }
        });
      } // checkchange
    }
  });
  
  
  EvocWidget.superclassesTreePanel.update = function() {
    
    //alert(this.getRootNode());
    node = this.getRootNode();
    
    node.eachChild(function(currentNode){
      node.getOwnerTree().expandPath(currentNode.getPath());
      currentNode.cascade( function() {
        for (var j = 0, lenValues = baseParams.arrayOfValues.length; j < lenValues; j++) {
          if ( this.id == baseParams.arrayOfValues[j] ) {
            this.getUI().toggleCheck(true);
          }
        }
      }, null);
    });
    
  }
  
}


/**
 * widget behaviour for field_disjointwith2 field
 * 
 * @param {Object} field_name
 */
EvocWidget.createDisjointWithSelecctionWidget = function( field_name ) {
  
  var objectToRender = Drupal.settings.neologism.field_id[field_name];
  EvocWidget.objectToRender = objectToRender; 
  
  var dataUrl = Drupal.settings.neologism.json_url[field_name];
   
  // we need to past the baseParams as and object, that is why we creat the baseParams object
  // and add the arrayOfValues array 
  var baseParams = {};
  //Drupal.settings.neologism.field_values[field_name] = Drupal.parseJson(Drupal.settings.neologism.field_values[field_name]);
  Drupal.settings.neologism.field_values[field_name] = Ext.util.JSON.decode(Drupal.settings.neologism.field_values[field_name]);
  baseParams.arrayOfValues = Drupal.settings.neologism.field_values[field_name];
 
  var treeLoader = new Ext.tree.TreeLoader({
    dataUrl: dataUrl,
    baseParams: baseParams,//baseParams,
    
    listeners: {
      load: function(loader, node, response){
        node.eachChild(function(currentNode){
          node.getOwnerTree().expandPath(currentNode.getPath());
          currentNode.cascade( function() {
            for (var j = 0, lenValues = baseParams.arrayOfValues.length; j < lenValues; j++) {
              if (this.id == baseParams.arrayOfValues[j]) {
                this.getUI().toggleCheck(true);
              }
            }
          }, null);
        });
        
        node.getOwnerTree().enable();
        EvocWidget.superclassesTreePanel.enable();
        
        // I need to update this TreePanel from superclassesTreePanel
        EvocWidget.superclassesTreePanel.update();
      }
    }
  });
    
    // SET the root node.
  var rootNode = new Ext.tree.AsyncTreeNode({
    text	: Drupal.t('Thing / Superclass'),
    id		: 'root',                  // this IS the id of the startnode
    iconCls: 'class-samevoc',
    disabled: true,
    expanded: false, 
  });
  
  EvocWidget.disjointWithTreePanel = new Ext.tree.TreePanel({
    //renderTo: objectToRender,
    title            : Drupal.t('Select disnoint with classes'),
    useArrows        : true,  
    collapsible      : true,
    animCollapse     : true,
    border           : true,
    autoScroll       : true,
    animate          : true,
    enableDD         : false,
    containerScroll  : true,
    height           : 400,
    width            : '100%',
    rootVisible      : false,
    //autoHeight       : true,
    disabled         : true,
    loader           : treeLoader,
    root             : rootNode,
    tbar: {
      cls:'top-toolbar',
      items:[' ',
        {
          xtype: 'tbbutton',
          iconCls: 'icon-expand-all', 
          tooltip: Drupal.t('Expand all'),
          handler: function(){ 
            rootNode.expand(true); 
          }
        }, {
          xtype: 'tbseparator' // equivalent to '-'
        }, {
          iconCls: 'icon-collapse-all',
          tooltip: Drupal.t('Collapse all'),
          handler: function(){ 
            rootNode.collapse(true); 
          }
        }
      ]
    },
               
    // listeners for disjointWithTree TreePanel object
    listeners: {
      /*
      load: function(node) {
        alert('TreePanel onload fired');
      },
      */
      
      checkchange: function(node, checked) {
        if ( checked && node.parentNode !== null ) {
    			// if we're checking the box, check it all the way up
    			if ( node.parentNode.isRoot || !node.parentNode.getUI().isChecked() ) {
            if ( baseParams.arrayOfValues.indexOf(node.id) == -1 ) {
              baseParams.arrayOfValues.push(node.id);
            }
                        
            // disable all the classes in disjoint tree
            EvocWidget.superclassesTreePanel.expandPath(node.getPath());
            superclassesTreeNode = EvocWidget.superclassesTreePanel.getNodeById(node.id);
            if (superclassesTreeNode !== undefined) {
              superclassesTreeNode.getUI().checkbox.disabled = true;
              superclassesTreeNode.getUI().nodeClass = 'complete';
              superclassesTreeNode.getUI().addClass('complete');
              
              superclassesTreeNode.eachChild(function(currentNode){
                EvocWidget.superclassesTreePanel.expandPath(currentNode.getPath());
                currentNode.cascade( function() {
                  //alert(this);
                  this.getUI().checkbox.disabled = true;
                  this.getUI().nodeClass = 'complete';
                  this.getUI().addClass('complete');
                }, null);
              });
            }

          }
    		} else {
          for (i in baseParams.arrayOfValues ) {
            if ( baseParams.arrayOfValues[i] == node.attributes.id ) {
              baseParams.arrayOfValues.splice(i, 1);
          
              // enable all the classes in disjoint tree
              EvocWidget.superclassesTreePanel.expandPath(node.getPath());
              superclassesTreeNode = EvocWidget.superclassesTreePanel.getNodeById(node.id); 
              if ( !superclassesTreeNode.parentNode.getUI().isChecked() ) {
                superclassesTreeNode.getUI().checkbox.disabled = false;
              }
              superclassesTreeNode.getUI().nodeClass = '';
              superclassesTreeNode.getUI().removeClass('complete');
              
              superclassesTreeNode.eachChild(function(currentNode){
                EvocWidget.superclassesTreePanel.expandPath(currentNode.getPath());
                currentNode.cascade( function() {
                  //alert(this);
                  this.getUI().checkbox.disabled = false;
                  this.getUI().nodeClass = '';
                  this.getUI().removeClass('complete');
                }, null);
              });

            }
          }    
        }

        node.getOwnerTree().expandPath(node.getPath());
        node.eachChild( function(currentNode){
          currentNode.getUI().toggleCheck(checked);  
          currentNode.getUI().checkbox.disabled = checked;
        });
       
      }
    } // listeners
  });
  
}

/**
 * 
 * @param {Object} field_name
 */ 
EvocWidget.createStandardClassSelecctionWidget = function( field_name ) {
  
  var objectToRender = Drupal.settings.neologism.field_id[field_name];
  var dataUrl = Drupal.settings.neologism.json_url[field_name];
  //var baseParams = Drupal.settings.neologism.field_selected_values;
   
  // we need to past the baseParams as and object, that is why we creat the baseParams object
  // and add the arrayOfValues array 
  var baseParams = {};
  //Drupal.settings.neologism.field_values[field_name] = Drupal.parseJson(Drupal.settings.neologism.field_values[field_name]);
  Drupal.settings.neologism.field_values[field_name] = Ext.util.JSON.decode(Drupal.settings.neologism.field_values[field_name]);
  baseParams.arrayOfValues = Drupal.settings.neologism.field_values[field_name];
 
  var treeLoader = new Ext.tree.TreeLoader({
    dataUrl: dataUrl,
    baseParams: baseParams,
    
    listeners: {
      // load : ( Object This, Object node, Object response )
      // Fires when the node has been successfuly loaded.
      // added event to refresh the checkbox from its parent 
      load: function(loader, node, response){
        node.eachChild(function(currentNode){
          node.getOwnerTree().expandPath(currentNode.getPath());
          currentNode.cascade( function() {
            for (var j = 0, lenValues = baseParams.arrayOfValues.length; j < lenValues; j++) {
              if (this.id == baseParams.arrayOfValues[j]) {
                this.getUI().toggleCheck(true);
              }
            }
          }, null);
        });
        
        node.getOwnerTree().enable();
      }
    }
  });
    
    // SET the root node.
  var rootNode = new Ext.tree.AsyncTreeNode({
    text	: Drupal.t('Thing / Superclass'),
    id		: 'root',                  // this IS the id of the startnode
    iconCls: 'class-samevoc',
    disabled: true,
    expanded: false,
  });
  
  var tree = new Ext.tree.TreePanel({
    renderTo         : objectToRender,
    title            : Drupal.t('Defined classes'),
    useArrows        : true,  
    collapsible      : true,
    animCollapse     : true,
    border           : true,
    autoScroll       : true,
    animate          : true,
    enableDD         : false,
    containerScroll  : true,
    height           : 400,
    width            : '100%',
    disabled         : true,
    loader           : treeLoader,
    rootVisible      : false,
    root             : rootNode,
    
    tbar: {
      cls:'top-toolbar',
      items:[' ',
        {
          xtype: 'tbbutton',
          iconCls: 'icon-expand-all', 
          tooltip: Drupal.t('Expand all'),
          handler: function(){ 
            rootNode.expand(true); 
          }
        }, {
          xtype: 'tbseparator' // equivalent to '-'
        }, {
          iconCls: 'icon-collapse-all',
          tooltip: Drupal.t('Collapse all'),
          handler: function(){ 
            rootNode.collapse(true); 
          }
        }
      ]
    },
     
    // listeners for EvocWidget.superclassesTree TreePanel object           
    listeners: {
      // behaviour for on checkchange in EvocWidget.superclassesTree TreePanel object 
      checkchange: function(node, checked) {
        if ( checked && node.parentNode !== null ) {
          // if we're checking the box, check it all the way up
    			if ( node.parentNode.isRoot || !node.parentNode.getUI().isChecked() ) {
            //Ext.Msg.alert('Checkbox status', 'Checked: "' + node.attributes.text);
            //EvocWidget.classSelection.push(node.id);
            //alert(node.id);
            if ( baseParams.arrayOfValues.indexOf(node.id) == -1 ) {
              baseParams.arrayOfValues.push(node.id);
            }
          }
    		} else {
          for (var i = 0, len = baseParams.arrayOfValues.length; i < len; i++) {
            if ( baseParams.arrayOfValues[i] == node.attributes.id ) {
              //alert(node.getPath());
              baseParams.arrayOfValues.splice(i, 1);
            }
          }    
        }

        this.expandPath(node.getPath());
        node.eachChild( function(currentNode){
          currentNode.getUI().toggleCheck(checked);
          if( currentNode.getUI().nodeClass != 'complete' ) {
            currentNode.getUI().checkbox.disabled = checked;  
          }
        });
      } // checkchange
    }
  });
  
}

/**
 * 
 * @param {Object} field_name
 */
EvocWidget.createSuperpropertySelecctionWidget = function( field_name ) {
  
  var objectToRender = Drupal.settings.neologism.field_id[field_name];
  var dataUrl = Drupal.settings.neologism.json_url[field_name];
  //var baseParams = Drupal.settings.neologism.field_selected_values;
   
  // we need to past the baseParams as and object, that is why we creat the baseParams object
  // and add the arrayOfValues array 
  var baseParams = {};
  //Drupal.settings.neologism.field_values[field_name] = Drupal.parseJson(Drupal.settings.neologism.field_values[field_name]);
  Drupal.settings.neologism.field_values[field_name] = Ext.util.JSON.decode(Drupal.settings.neologism.field_values[field_name]);
  baseParams.arrayOfValues = Drupal.settings.neologism.field_values[field_name];
 
  var treeLoader = new Ext.tree.TreeLoader({
    dataUrl: dataUrl,
    baseParams: baseParams,
    
    listeners: {
      // load : ( Object This, Object node, Object response )
      // Fires when the node has been successfuly loaded.
      // added event to refresh the checkbox from its parent 
      load: function(loader, node, response){
        node.eachChild(function(currentNode){
          node.getOwnerTree().expandPath(currentNode.getPath());
          currentNode.cascade( function() {
            for (var j = 0, lenValues = baseParams.arrayOfValues.length; j < lenValues; j++) {
              if (this.id == baseParams.arrayOfValues[j]) {
                this.getUI().toggleCheck(true);
              }
            }
          }, null);
        });
        
        node.getOwnerTree().enable();
      }
    }
  });
    
    // SET the root node.
  var rootNode = new Ext.tree.AsyncTreeNode({
    text	: Drupal.t('Thing / Superclass'),
    id		: 'root',                  // this IS the id of the startnode
    iconCls: 'class-samevoc',
    disabled: true,
    expanded: false,
  });
  
  var tree = new Ext.tree.TreePanel({
    renderTo         : objectToRender,
    title            : Drupal.t('Properties'),
    useArrows        : true,  
    collapsible      : true,
    animCollapse     : true,
    border           : true,
    autoScroll       : true,
    animate          : true,
    enableDD         : false,
    containerScroll  : true,
    height           : 400,
    width            : '100%',
    disabled         : true,
    loader           : treeLoader,
    rootVisible      : false,
    root             : rootNode,
    
    tbar: {
      cls:'top-toolbar',
      items:[' ',
        {
          xtype: 'tbbutton',
          iconCls: 'icon-expand-all', 
          tooltip: Drupal.t('Expand all'),
          handler: function(){ 
            rootNode.expand(true); 
          }
        }, {
          xtype: 'tbseparator' // equivalent to '-'
        }, {
          iconCls: 'icon-collapse-all',
          tooltip: Drupal.t('Collapse all'),
          handler: function(){ 
            rootNode.collapse(true); 
          }
        }
      ]
    },
     
    // listeners for EvocWidget.superclassesTree TreePanel object           
    listeners: {
      // behaviour for on checkchange in EvocWidget.superclassesTree TreePanel object 
      checkchange: function(node, checked) {
        if ( checked && node.parentNode !== null ) {
          // if we're checking the box, check it all the way up
    			if ( node.parentNode.isRoot || !node.parentNode.getUI().isChecked() ) {
            //Ext.Msg.alert('Checkbox status', 'Checked: "' + node.attributes.text);
            //EvocWidget.classSelection.push(node.id);
            //alert(node.id);
            if ( baseParams.arrayOfValues.indexOf(node.id) == -1 ) {
              baseParams.arrayOfValues.push(node.id);
            }
          }
    		} else {
          for (var i = 0, len = baseParams.arrayOfValues.length; i < len; i++) {
            if ( baseParams.arrayOfValues[i] == node.attributes.id ) {
              //alert(node.getPath());
              baseParams.arrayOfValues.splice(i, 1);
            }
          }    
        }

        this.expandPath(node.getPath());
        node.eachChild( function(currentNode){
          currentNode.getUI().toggleCheck(checked);
          if( currentNode.getUI().nodeClass != 'complete' ) {
            currentNode.getUI().checkbox.disabled = checked;  
          }
        });
      } // checkchange
    }
  });
  
}