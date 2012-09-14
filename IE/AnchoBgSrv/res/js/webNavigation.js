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
(function(me){
  //============================================================================
  // private variables
  

  //============================================================================
  // public methods
    
  //----------------------------------------------------------------------------
  // aji.webNavigation.getAllFrames
  me.getAllFrames = function(details, callback) {
    console.debug("webNavigation.getAllFrames(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.webNavigation.getFrame
  me.getFrame = function(details, callback) {
    console.debug("webNavigation.getFrame(..) called");
  };

  //============================================================================
  // events
    
  me.onBeforeNavigate = new Event();
  me.onCommitted = new Event();
  me.onCompleted = new Event();
  me.onCreatedNavigationTarget = new Event();
  me.onDOMContentLoaded = new Event();
  me.onErrorOccurred = new Event();
  me.onReferenceFragmentUpdated = new Event();

  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);
