/******************************************************************************
 * idle.js
 * Part of Ancho browser extension framework
 * Implements aji.idle
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
  // aji.idle.queryState
  this.queryState = function(thresholdSeconds, callback) {
    console.debug("idle.queryState(..) called");
  };

  //============================================================================
  // events

  this.onStateChanged = new Event('idle.onStateChanged', instanceID);

  //============================================================================
  //============================================================================
  // main initialization


  })();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('idle.onStateChanged', instanceID);
}