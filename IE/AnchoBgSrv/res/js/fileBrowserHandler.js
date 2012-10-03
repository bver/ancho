/******************************************************************************
 * fileBrowserHandler.js
 * Part of Ancho browser extension framework
 * Implements aji.fileBrowserHandler
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
  // events

  this.onExecute = new Event('fileBrowserHandler.onExecute', instanceID);

  //============================================================================
  //============================================================================
  // main initialization

})();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('fileBrowserHandler.onExecute', instanceID);
}