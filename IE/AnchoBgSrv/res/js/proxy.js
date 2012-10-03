/******************************************************************************
 * proxy.js
 * Part of Ancho browser extension framework
 * Implements aji.proxy
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

  this.settings = null;
  //============================================================================
  // events

  this.onProxyError = new Event('proxy.onProxyError', instanceID);

  //============================================================================
  //============================================================================
  // main initialization


})();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('proxy.onProxyError', instanceID);
}