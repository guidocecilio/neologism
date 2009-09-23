/**
 * @author guicec
 */

/**
 * Override TreePanel onClick and onDblClick events
 * @param {Object} e
 */ 
Ext.override(Ext.tree.TreeNodeUI, {
    onClick : function(e){ //debugger;
        if ( this.dropping ){
            e.stopEvent();
            return;
        }
        if ( this.fireEvent("beforeclick", this.node, e) !== false ) {
            var a = e.getTarget('a');
            if ( !this.disabled && this.node.attributes.href && a ){
                this.fireEvent("click", this.node, e);
                return;
            }else if ( a && e.ctrlKey ){
                e.stopEvent();
            }
            e.preventDefault();
            if(this.disabled){
                return;
            }
            if( this.node.attributes.singleClickExpand && !this.animating && this.node.hasChildNodes() ){
                //this.node.expand(); 
                //this.node.toggle();
            }

            this.fireEvent("click", this.node, e);
        }else{
            e.stopEvent();
        }
    }
});

Ext.override(Ext.tree.TreeNodeUI, {
    onDblClick : function(e){ //debugger;
        e.preventDefault();
        if ( this.disabled ){
            return;
        }
        if ( this.checkbox ){
            return;
            // cancel the toggleCheck when dblclick
            //this.toggleCheck();
        }
        if ( this.animating && this.node.hasChildNodes() ){
            //this.node.toggle();
            //this.node.expand();
        }
        this.fireEvent("dblclick", this.node, e);
    }
});

/**
 * Create the class selection widget behaviour for filed_superclass2 
 * 
 * @param {Object} field_name
 */
Neologism.createClassSelecctionWidget = function( field_name ) {
  
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
        Neologism.disjointWithTreePanel.render(Neologism.objectToRender);
        Neologism.disjointWithTreePanel.getRootNode().expand(true, false);
              
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
  
  Neologism.superclassesTreePanel = new Ext.tree.TreePanel({
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
    singleClickExpand:true,
    
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
     
    // listeners for Neologism.superclassesTree TreePanel object           
    listeners: {
      dblclick: function(node, ev) {
        //alert(node);
        ev.preventDefault();
        ev.stopPropagation();
        ev.stopEvent();
      },
      
      // behaviour for on checkchange in Neologism.superclassesTree TreePanel object 
      checkchange: function(node, checked) {
        if ( checked && node.parentNode !== null ) {
          // if we're checking the box, check it all the way up
    			if ( node.parentNode.isRoot || !node.parentNode.getUI().isChecked() ) {
            
            //Ext.Msg.alert('Checkbox status', 'Checked: "' + node.attributes.text);
            //Neologism.classSelection.push(node.id);
            //alert(node.id);
            if ( baseParams.arrayOfValues.indexOf(node.id) == -1 ) {
              baseParams.arrayOfValues.push(node.id);
            }
            
            if (!node.parentNode.isRoot) {
              node.bubble( function(){
                if (node.id != this.id) {
                  this.getUI().checkbox.disabled = true;
                }
                //this.getUI().addClass('complete');
                // if this node is the root node then return false to stop the bubble process
                if ( this.parentNode.isRoot ) {
                  return false;
                }
              });
            }
            
            //alert(Neologism.disjointWithTree);
            // disable all the classes in disjoint tree
            Neologism.disjointWithTreePanel.expandPath(node.getPath());
            disjointWithNode = Neologism.disjointWithTreePanel.getNodeById(node.id);
            // when the Neologism.superclassesTree is expanding its nodes the
            // Neologism.disjointWithTree has not load its nodes yet, so we need to check it
            // to avoid errors 
            //alert(disjointWithNode);
            if( disjointWithNode !== undefined ) {
              disjointWithNode.bubble( function(){
                this.getUI().checkbox.disabled	= true;
                this.getUI().addClass('complete');
                // if this node is the root node then return false to stop the bubble process
                if ( this.parentNode.id == Neologism.disjointWithTreePanel.getRootNode().id ) {
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
              
              if (!node.parentNode.isRoot) {
                node.bubble( function(){
                  if (node.id != this.id) {
                    this.getUI().checkbox.disabled = false;
                  }
                  //this.getUI().addClass('complete');
                  // if this node is the root node then return false to stop the bubble process
                  if ( this.parentNode.isRoot ) {
                    return false;
                  }
                });
              }
              
              // enable all the classes in disjoint tree
              Neologism.disjointWithTreePanel.expandPath(node.getPath());
              Neologism.disjointWithTreePanel.getNodeById(node.id).bubble( function(){
                this.getUI().checkbox.disabled = false;
                this.getUI().removeClass('complete');
                
                // stop the bubble if the parent is the root node
                if ( this.parentNode.id == Neologism.disjointWithTreePanel.getRootNode().id ) {
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
          if( currentNode.getUI().nodeClass != 'complete'  ) {
            //alert(currentNode.id);
            currentNode.getUI().checkbox.checked = false;
            // if the parent is not checked the child is not checked as well
            currentNode.getUI().checkbox.disabled = currentNode.parentNode.getUI().isChecked();
            //currentNode.getUI().checkbox.disabled = checked;  
          }
        });
      } // checkchange
    }
  });
  
  //Neologism.superclassesTreePanel.on('dblclick', null);
  
  Neologism.superclassesTreePanel.update = function() {
    
    //alert(this.getRootNode());
    node = this.getRootNode();
    
    node.eachChild(function(currentNode){
      node.getOwnerTree().expandPath(currentNode.getPath());
      currentNode.cascade( function() {
        for (var j = 0, lenValues = baseParams.arrayOfValues.length; j < lenValues; j++) {
          if ( this.id == baseParams.arrayOfValues[j] ) {
            this.getUI().toggleCheck(true);
            //this.getUI().checkbox.checked = true;
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
Neologism.createDisjointWithSelecctionWidget = function( field_name ) {
  
  var objectToRender = Drupal.settings.neologism.field_id[field_name];
  Neologism.objectToRender = objectToRender; 
  
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
        Neologism.superclassesTreePanel.enable();
        
        // I need to update this TreePanel from superclassesTreePanel
        Neologism.superclassesTreePanel.update();
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
  
  Neologism.disjointWithTreePanel = new Ext.tree.TreePanel({
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
            Neologism.superclassesTreePanel.expandPath(node.getPath());
            superclassesTreeNode = Neologism.superclassesTreePanel.getNodeById(node.id);
            if (superclassesTreeNode !== undefined) {
              superclassesTreeNode.getUI().checkbox.disabled = true;
              superclassesTreeNode.getUI().nodeClass = 'complete';
              superclassesTreeNode.getUI().addClass('complete');
              
              superclassesTreeNode.eachChild(function(currentNode){
                Neologism.superclassesTreePanel.expandPath(currentNode.getPath());
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
              Neologism.superclassesTreePanel.expandPath(node.getPath());
              superclassesTreeNode = Neologism.superclassesTreePanel.getNodeById(node.id); 
              if ( !superclassesTreeNode.parentNode.getUI().isChecked() ) {
                superclassesTreeNode.getUI().checkbox.disabled = false;
              }
              superclassesTreeNode.getUI().nodeClass = '';
              superclassesTreeNode.getUI().removeClass('complete');
              
              superclassesTreeNode.eachChild(function(currentNode){
                Neologism.superclassesTreePanel.expandPath(currentNode.getPath());
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
Neologism.createStandardClassSelecctionWidget = function( field_name ) {
  
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
     
    // listeners for Neologism.superclassesTree TreePanel object           
    listeners: {
      // behaviour for on checkchange in Neologism.superclassesTree TreePanel object 
      checkchange: function(node, checked) {
        if ( checked && node.parentNode !== null ) {
          // if we're checking the box, check it all the way up
    			if ( node.parentNode.isRoot || !node.parentNode.getUI().isChecked() ) {
            //Ext.Msg.alert('Checkbox status', 'Checked: "' + node.attributes.text);
            //Neologism.classSelection.push(node.id);
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
Neologism.createSuperpropertySelecctionWidget = function( field_name ) {
  
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
     
    // listeners for Neologism.superclassesTree TreePanel object           
    listeners: {
      // behaviour for on checkchange in Neologism.superclassesTree TreePanel object 
      checkchange: function(node, checked) {
        if ( checked && node.parentNode !== null ) {
          // if we're checking the box, check it all the way up
    			if ( node.parentNode.isRoot || !node.parentNode.getUI().isChecked() ) {
            //Ext.Msg.alert('Checkbox status', 'Checked: "' + node.attributes.text);
            //Neologism.classSelection.push(node.id);
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