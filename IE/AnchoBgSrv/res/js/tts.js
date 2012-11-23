/******************************************************************************
 * tts.js
 * Part of Ancho browser extension framework
 * Implements chrome.tts
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* main closure
(function(me){
  //============================================================================
  // private variables


  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.tts.getVoices
  me.getVoices = function(callback) {
    console.debug("tts.getVoices(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tts.isSpeaking
  me.isSpeaking = function(callback) {
    console.debug("tts.isSpeaking(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tts.speak
  me.speak = function(utterance, options, callback) {
    console.debug("tts.speak(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tts.stop
  me.stop = function() {
    console.debug("tts.stop(..) called");
  };


  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);


exports.createAPI = function(instanceID) {
  //We don't need special instances
  return exports;
}

exports.releaseAPI = function(instanceID) {
  //Nothing needs to be released
}
