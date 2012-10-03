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
exports.createAPI = function(instanceID) {
  return new (function() {
  //============================================================================
  // private variables
  
  //============================================================================
  // events
    
  this.onSpeak = new Event('ttsEngine.onSpeak', instanceID);
  this.onStop = new Event('ttsEngine.onStop', instanceID);

  //============================================================================
  //============================================================================
  // main initialization


})();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('ttsEngine.onSpeak', instanceID);
  addonAPI.removeEventObject('ttsEngine.onStop', instanceID);
}
