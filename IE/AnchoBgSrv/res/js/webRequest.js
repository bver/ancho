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
(function(me){
  //============================================================================
  // private variables
  

  //============================================================================
  // public methods
    
  //----------------------------------------------------------------------------
  // aji.webRequest.handlerBehaviorChanged
  me.handlerBehaviorChanged = function(callback) {
    console.debug("webRequest.handlerBehaviorChanged(..) called");
  };

  //============================================================================
  // events
    
  me.onAuthRequired = new Event();
  me.onBeforeRedirect = new Event();
  me.onBeforeRequest = new Event();
  me.onBeforeSendHeaders = new Event();
  me.onCompleted = new Event();
  me.onErrorOccurred = new Event();
  me.onHeadersReceived = new Event();
  me.onResponseStarted = new Event();
  me.onSendHeaders = new Event();

  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);
