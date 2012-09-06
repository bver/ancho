/******************************************************************************
 * debugger.js
 * Part of Ancho browser extension framework
 * Implements aji.debugger
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
  // aji.debugger.attach
  me.attach = function(target, requiredVersion, callback) {
    console.debug("debugger.attach(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.debugger.detach
  me.detach = function(target, callback) {
    console.debug("debugger.detach(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.debugger.sendCommand
  me.sendCommand = function(target, method, params, callback) {
    console.debug("debugger.sendCommand(..) called");
  };

  //============================================================================
  // events
    
  me.onDetach = new Event();
  me.onEvent = new Event();

  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);
