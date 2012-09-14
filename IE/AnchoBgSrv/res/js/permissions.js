/******************************************************************************
 * permissions.js
 * Part of Ancho browser extension framework
 * Implements aji.permissions
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/
  
//******************************************************************************
//* requires
var Event = require("Event.js").Event;
  
//******************************************************************************
//* main closure
(function(me){
  //============================================================================
  // private variables
  

  //============================================================================
  // public methods
    
  //----------------------------------------------------------------------------
  // aji.permissions.contains
  me.contains = function(permissions, callback) {
    console.debug("permissions.contains(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.permissions.getAll
  me.getAll = function(callback) {
    console.debug("permissions.getAll(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.permissions.remove
  me.remove = function(permissions, callback) {
    console.debug("permissions.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.permissions.request
  me.request = function(permissions, callback) {
    console.debug("permissions.request(..) called");
  };

  //============================================================================
  // events
    
  me.onAdded = new Event();
  me.onRemoved = new Event();

  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);
