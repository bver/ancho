/******************************************************************************
 * storage.js
 * Part of Ancho browser extension framework
 * Implements aji.storage
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

  this.sync = null;
  this.local = null;
  //============================================================================
  // events

  this.onChanged = new Event('storage.onChanged', instanceID);

  //============================================================================
  //============================================================================
  // main initialization


})();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('storage.onChanged', instanceID);
}