/******************************************************************************
 * omnibox.js
 * Part of Ancho browser extension framework
 * Implements aji.omnibox
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
  // aji.omnibox.setDefaultSuggestion
  this.setDefaultSuggestion = function(suggestion) {
    console.debug("omnibox.setDefaultSuggestion(..) called");
  };

  //============================================================================
  // events

  this.onInputCancelled = new Event('omnibox.onInputCancelled', instanceID);
  this.onInputChanged = new Event('omnibox.onInputChanged', instanceID);
  this.onInputEntered = new Event('omnibox.onInputEntered', instanceID);
  this.onInputStarted = new Event('omnibox.onInputStarted', instanceID);

  //============================================================================
  //============================================================================
  // main initialization

  })();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('omnibox.onInputCancelled', instanceID);
  addonAPI.removeEventObject('omnibox.onInputChanged', instanceID);
  addonAPI.removeEventObject('omnibox.onInputEntered', instanceID);
  addonAPI.removeEventObject('omnibox.onInputStarted', instanceID);
}