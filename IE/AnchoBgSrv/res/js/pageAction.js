/******************************************************************************
 * pageAction.js
 * Part of Ancho browser extension framework
 * Implements aji.pageAction
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
  // aji.pageAction.getPopup
  this.getPopup = function(details, callback) {
    console.debug("pageAction.getPopup(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.pageAction.getTitle
  this.getTitle = function(details, callback) {
    console.debug("pageAction.getTitle(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.pageAction.hide
  this.hide = function(tabId) {
    console.debug("pageAction.hide(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.pageAction.setIcon
  this.setIcon = function(details) {
    console.debug("pageAction.setIcon(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.pageAction.setPopup
  this.setPopup = function(details) {
    console.debug("pageAction.setPopup(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.pageAction.setTitle
  this.setTitle = function(details) {
    console.debug("pageAction.setTitle(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.pageAction.show
  this.show = function(tabId) {
    console.debug("pageAction.show(..) called");
  };

  //============================================================================
  // events

  this.onClicked = new Event('pageAction.onClicked', instanceID);

  //============================================================================
  //============================================================================
  // main initialization

})();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('pageAction.onClicked', instanceID);
}