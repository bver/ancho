/******************************************************************************
 * ttsEngine.js
 * Part of Ancho browser extension framework
 * Implements aji.ttsEngine
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
  // events
    
  me.onSpeak = new Event();
  me.onStop = new Event();

  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);
