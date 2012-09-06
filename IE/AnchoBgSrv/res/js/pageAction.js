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
(function(me){
  //============================================================================
  // private variables
  

  //============================================================================
  // public methods
    
  //----------------------------------------------------------------------------
  // aji.pageAction.getPopup
  me.getPopup = function(details, callback) {
    console.debug("pageAction.getPopup(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.pageAction.getTitle
  me.getTitle = function(details, callback) {
    console.debug("pageAction.getTitle(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.pageAction.hide
  me.hide = function(tabId) {
    console.debug("pageAction.hide(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.pageAction.setIcon
  me.setIcon = function(details) {
    console.debug("pageAction.setIcon(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.pageAction.setPopup
  me.setPopup = function(details) {
    console.debug("pageAction.setPopup(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.pageAction.setTitle
  me.setTitle = function(details) {
    console.debug("pageAction.setTitle(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.pageAction.show
  me.show = function(tabId) {
    console.debug("pageAction.show(..) called");
  };

  //============================================================================
  // events
    
  me.onClicked = new Event();

  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);
