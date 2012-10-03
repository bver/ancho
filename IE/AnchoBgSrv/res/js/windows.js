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
exports.createAPI = function(instanceID) {
  return new (function() {
  //============================================================================
  // private variables
  

  //============================================================================
  // public properties

  this.WINDOW_ID_NONE = null;
  this.WINDOW_ID_CURRENT = null;

  //============================================================================
  // public methods
    
  //----------------------------------------------------------------------------
  // aji.windows.create
  this.create = function(createData, callback) {
    console.debug("windows.create(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.windows.get
  this.get = function(windowId, getInfo, callback) {
    console.debug("windows.get(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.windows.getAll
  this.getAll = function(getInfo, callback) {
    console.debug("windows.getAll(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.windows.getCurrent
  this.getCurrent = function(getInfo, callback) {
    console.debug("windows.getCurrent(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.windows.getLastFocused
  this.getLastFocused = function(getInfo, callback) {
    console.debug("windows.getLastFocused(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.windows.remove
  this.remove = function(windowId, callback) {
    console.debug("windows.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.windows.update
  this.update = function(windowId, updateInfo, callback) {
    console.debug("windows.update(..) called");
  };

  //============================================================================
  // events
    
  this.onCreated = new Event('windows.onCreated', instanceID);
  this.onFocusChanged = new Event('windows.onFocusChanged', instanceID);
  this.onRemoved = new Event('windows.onRemoved', instanceID);

  //============================================================================
  //============================================================================
  // main initialization
  
})();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('windows.onCreated', instanceID);
  addonAPI.removeEventObject('windows.onFocusChanged', instanceID);
  addonAPI.removeEventObject('windows.onRemoved', instanceID);
}