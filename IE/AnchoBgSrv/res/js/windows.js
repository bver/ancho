/******************************************************************************
 * windows.js
 * Part of Ancho browser extension framework
 * Implements aji.windows
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
  // public properties
    
  me.WINDOW_ID_NONE = null;
  me.WINDOW_ID_CURRENT = null;

  //============================================================================
  // public methods
    
  //----------------------------------------------------------------------------
  // aji.windows.create
  me.create = function(createData, callback) {
    console.debug("windows.create(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.windows.get
  me.get = function(windowId, getInfo, callback) {
    console.debug("windows.get(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.windows.getAll
  me.getAll = function(getInfo, callback) {
    console.debug("windows.getAll(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.windows.getCurrent
  me.getCurrent = function(getInfo, callback) {
    console.debug("windows.getCurrent(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.windows.getLastFocused
  me.getLastFocused = function(getInfo, callback) {
    console.debug("windows.getLastFocused(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.windows.remove
  me.remove = function(windowId, callback) {
    console.debug("windows.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.windows.update
  me.update = function(windowId, updateInfo, callback) {
    console.debug("windows.update(..) called");
  };

  //============================================================================
  // events
    
  me.onCreated = new Event();
  me.onFocusChanged = new Event();
  me.onRemoved = new Event();

  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);
