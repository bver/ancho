/******************************************************************************
 * browserAction.js
 * Part of Ancho browser extension framework
 * Implements aji.browserAction
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
  // aji.browserAction.getBadgeBackgroundColor
  this.getBadgeBackgroundColor = function(details, callback) {
    console.debug("browserAction.getBadgeBackgroundColor(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.browserAction.getBadgeText
  this.getBadgeText = function(details, callback) {
    console.debug("browserAction.getBadgeText(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.browserAction.getPopup
  this.getPopup = function(details, callback) {
    console.debug("browserAction.getPopup(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.browserAction.getTitle
  this.getTitle = function(details, callback) {
    console.debug("browserAction.getTitle(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.browserAction.setBadgeBackgroundColor
  this.setBadgeBackgroundColor = function(details) {
    console.debug("browserAction.setBadgeBackgroundColor(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.browserAction.setBadgeText
  this.setBadgeText = function(details) {
    console.debug("browserAction.setBadgeText(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.browserAction.setIcon
  this.setIcon = function(details) {
    console.debug("browserAction.setIcon(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.browserAction.setPopup
  this.setPopup = function(details) {
    console.debug("browserAction.setPopup(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.browserAction.setTitle
  this.setTitle = function(details) {
    console.debug("browserAction.setTitle(..) called");
  };

  //============================================================================
  // events

  this.onClicked = new Event('browserAction.onClicked', instanceID);

  //============================================================================
  //============================================================================
  // main initialization


})();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('browserAction.onClicked', instanceID);
}