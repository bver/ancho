/******************************************************************************
 * webRequest.js
 * Part of Ancho browser extension framework
 * Implements aji.webRequest
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
  // aji.webRequest.handlerBehaviorChanged
  this.handlerBehaviorChanged = function(callback) {
    console.debug("webRequest.handlerBehaviorChanged(..) called");
  };

  //============================================================================
  // events

  this.onAuthRequired = new Event('webRequest.onAuthRequired', instanceID);
  this.onBeforeRedirect = new Event('webRequest.onBeforeRedirect', instanceID);
  this.onBeforeRequest = new Event('webRequest.onBeforeRequest', instanceID);
  this.onBeforeSendHeaders = new Event('webRequest.onBeforeSendHeaders', instanceID);
  this.onCompleted = new Event('webRequest.onCompleted', instanceID);
  this.onErrorOccurred = new Event('webRequest.onErrorOccurred', instanceID);
  this.onHeadersReceived = new Event('webRequest.onHeadersReceived', instanceID);
  this.onResponseStarted = new Event('webRequest.onResponseStarted', instanceID);
  this.onSendHeaders = new Event('webRequest.onSendHeaders', instanceID);

  //============================================================================
  //============================================================================
  // main initialization

})();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('webRequest.onAuthRequired', instanceID);
  addonAPI.removeEventObject('webRequest.onBeforeRedirect', instanceID);
  addonAPI.removeEventObject('webRequest.onBeforeRequest', instanceID);
  addonAPI.removeEventObject('webRequest.onBeforeSendHeaders', instanceID);
  addonAPI.removeEventObject('webRequest.onCompleted', instanceID);
  addonAPI.removeEventObject('webRequest.onErrorOccurred', instanceID);
  addonAPI.removeEventObject('webRequest.onHeadersReceived', instanceID);
  addonAPI.removeEventObject('webRequest.onResponseStarted', instanceID);
  addonAPI.removeEventObject('webRequest.onSendHeaders', instanceID);
}
