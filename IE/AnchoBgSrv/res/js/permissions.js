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
exports.createAPI = function(instanceID) {
  return new (function() {
  //============================================================================
  // private variables


  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // aji.permissions.contains
  this.contains = function(permissions, callback) {
    console.debug("permissions.contains(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.permissions.getAll
  this.getAll = function(callback) {
    console.debug("permissions.getAll(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.permissions.remove
  this.remove = function(permissions, callback) {
    console.debug("permissions.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.permissions.request
  this.request = function(permissions, callback) {
    console.debug("permissions.request(..) called");
  };

  //============================================================================
  // events

  this.onAdded = new Event('permissions.onAdded', instanceID);
  this.onRemoved = new Event('permissions.onRemoved', instanceID);

  //============================================================================
  //============================================================================
  // main initialization


})();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('permissions.onAdded', instanceID);
  addonAPI.removeEventObject('permissions.onRemoved', instanceID);
}