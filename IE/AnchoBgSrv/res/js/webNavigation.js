/******************************************************************************
 * webNavigation.js
 * Part of Ancho browser extension framework
 * Implements aji.webNavigation
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
  // aji.webNavigation.getAllFrames
  this.getAllFrames = function(details, callback) {
    console.debug("webNavigation.getAllFrames(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.webNavigation.getFrame
  this.getFrame = function(details, callback) {
    console.debug("webNavigation.getFrame(..) called");
  };

  //============================================================================
  // events

  this.onBeforeNavigate = new Event('webNavigation.onBeforeNavigate', instanceID);
  this.onCommitted = new Event('webNavigation.onCommitted', instanceID);
  this.onCompleted = new Event('webNavigation.onCompleted', instanceID);
  this.onCreatedNavigationTarget = new Event('webNavigation.onCreatedNavigationTarget', instanceID);
  this.onDOMContentLoaded = new Event('webNavigation.onDOMContentLoaded', instanceID);
  this.onErrorOccurred = new Event('webNavigation.onErrorOccurred', instanceID);
  this.onReferenceFragmentUpdated = new Event('webNavigation.onReferenceFragmentUpdated', instanceID);

  //============================================================================
  //============================================================================
  // main initialization

  })();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('webNavigation.onBeforeNavigate', instanceID);
  addonAPI.removeEventObject('webNavigation.onCommitted', instanceID);
  addonAPI.removeEventObject('webNavigation.onCompleted', instanceID);
  addonAPI.removeEventObject('webNavigation.onCreatedNavigationTarget', instanceID);
  addonAPI.removeEventObject('webNavigation.onDOMContentLoaded', instanceID);
  addonAPI.removeEventObject('webNavigation.onErrorOccurred', instanceID);
  addonAPI.removeEventObject('webNavigation.onReferenceFragmentUpdated', instanceID);
}