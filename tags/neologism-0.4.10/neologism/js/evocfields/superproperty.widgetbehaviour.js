/**
 * 
 * @param {Object} field_name
 */
Neologism.createSuperpropertySelecctionWidget = function(field_name) {

  var objectToRender = Drupal.settings.evocwidget.field_id[field_name];
  var dataUrl = Drupal.settings.evocwidget.json_url[field_name];
  var editingValue = Drupal.settings.evocwidget.editing_value[field_name];
  
  // we need to past the baseParams as and object, that is why we creat the baseParams object
  // and add the arrayOfValues array 
  var baseParams = {};
  //Drupal.settings.neologism.field_values[field_name] = Drupal.parseJson(Drupal.settings.neologism.field_values[field_name]);
  Drupal.settings.evocwidget.field_values[field_name] = Ext.util.JSON.decode(Drupal.settings.evocwidget.field_values[field_name]);
  baseParams.arrayOfValues = Drupal.settings.evocwidget.field_values[field_name];
  
  Neologism.superpropertyTermsTree = new Neologism.TermsTree({
    //renderTo: objectToRender,
    title: Drupal.t('Subproperty of'),
    disabled: false,
    
    loader: new Ext.tree.TreeLoader({
      dataUrl: dataUrl,
      baseParams: baseParams,
      
	  listeners: {
		    // load : ( Object This, Object node, Object response )
	    	// Fires when the node has been successfuly loaded.
	    	// added event to refresh the checkbox from its parent 
	    	load: function(loader, node, response){
			}
    	}
    }),
    
    root: new Ext.tree.AsyncTreeNode({
      text: Drupal.t('Thing / Superclass'),
      id: 'root', // this IS the id of the startnode
      iconCls: 'class-samevoc',
      disabled: true,
      expanded: false
    }),
    
    listeners: {
      // behaviour for on checkchange in Neologism.superclassesTree TreePanel object 
      checkchange: function(node, checked){

	  	var tree = node.getOwnerTree();
	  	var rootNode = tree.getRootNode();
	  	if ( rootNode.childNodes[0].attributes.references != undefined ) {
	  		var references = rootNode.childNodes[0].attributes.references;
	  		if (references[node.text] != undefined) {
	  			var reference = references[node.text];
	  			for ( var p = 0; p < reference.paths.length; p++ ) {
	  				var rnode = tree.expandPath(reference.paths[p])
	  				if ( rnode != undefined ) {
	  					if (rnode.attributes.checked != checked) {
	  						rnode.getUI().toggleCheck(checked);
	  					}
	  				}
	  			}
	  		}
		}
	  
        if ( checked /*&& node.parentNode !== null*/ ) {
	        // add selection to array of values
    		if ( baseParams.arrayOfValues.indexOf(node.text) == -1 ) {
            	baseParams.arrayOfValues.push(node.text);
            	
            	//check for dependences 
            }
        }
        else {
        	for ( var i = 0, len = baseParams.arrayOfValues.length; i < len; i++ ) {
        		if ( baseParams.arrayOfValues[i] == node.text ) {
        			baseParams.arrayOfValues.splice(i, 1);
        		}
        	}
        }
      } // checkchange
  
		,expandnode: function( node ) {
			node.eachChild(function(currentNode){
				if ( currentNode !== undefined ) {
		            if (currentNode.attributes.text == editingValue) {
		            	currentNode.remove();
		            }
		              
		          	for (var j = 0, lenValues = baseParams.arrayOfValues.length; j < lenValues; j++) {
		          		if ( currentNode.attributes.text == baseParams.arrayOfValues[j] ) {
		          			currentNode.getUI().toggleCheck(true);
		          		}
		          	}
				}
		});
		}
    }
  });
  
  Neologism.superpropertyTermsTree.objectToRender = objectToRender;
};
    