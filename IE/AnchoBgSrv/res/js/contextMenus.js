/******************************************************************************
 * contextMenus.js
 * Part of Ancho browser extension framework
 * Implements aji.contextMenus
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/
  
//******************************************************************************
//* main closure
(function(me){
  //============================================================================
  // private variables
  

  //============================================================================
  // public methods
    
  //----------------------------------------------------------------------------
  // aji.contextMenus.create
  //   returns   integer
  me.create = function(createProperties, callback) {
    console.debug("contextMenus.create(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.contextMenus.remove
  me.remove = function(menuItemId, callback) {
    console.debug("contextMenus.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.contextMenus.removeAll
  me.removeAll = function(callback) {
    console.debug("contextMenus.removeAll(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.contextMenus.update
  me.update = function(id, updateProperties, callback) {
    console.debug("contextMenus.update(..) called");
  };


  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);
