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
(function(me){
  //============================================================================
  // private variables
  

  //============================================================================
  // public methods
    
  //----------------------------------------------------------------------------
  // aji.browserAction.getBadgeBackgroundColor
  me.getBadgeBackgroundColor = function(details, callback) {
    console.debug("browserAction.getBadgeBackgroundColor(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.browserAction.getBadgeText
  me.getBadgeText = function(details, callback) {
    console.debug("browserAction.getBadgeText(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.browserAction.getPopup
  me.getPopup = function(details, callback) {
    console.debug("browserAction.getPopup(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.browserAction.getTitle
  me.getTitle = function(details, callback) {
    console.debug("browserAction.getTitle(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.browserAction.setBadgeBackgroundColor
  me.setBadgeBackgroundColor = function(details) {
    console.debug("browserAction.setBadgeBackgroundColor(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.browserAction.setBadgeText
  me.setBadgeText = function(details) {
    console.debug("browserAction.setBadgeText(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.browserAction.setIcon
  me.setIcon = function(details) {
    console.debug("browserAction.setIcon(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.browserAction.setPopup
  me.setPopup = function(details) {
    console.debug("browserAction.setPopup(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.browserAction.setTitle
  me.setTitle = function(details) {
    console.debug("browserAction.setTitle(..) called");
  };

  //============================================================================
  // events
    
  me.onClicked = new Event();

  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);
