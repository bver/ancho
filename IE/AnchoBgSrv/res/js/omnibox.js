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
(function(me){
  //============================================================================
  // private variables
  

  //============================================================================
  // public methods
    
  //----------------------------------------------------------------------------
  // aji.omnibox.setDefaultSuggestion
  me.setDefaultSuggestion = function(suggestion) {
    console.debug("omnibox.setDefaultSuggestion(..) called");
  };

  //============================================================================
  // events
    
  me.onInputCancelled = new Event();
  me.onInputChanged = new Event();
  me.onInputEntered = new Event();
  me.onInputStarted = new Event();

  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);
