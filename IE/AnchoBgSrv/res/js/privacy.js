/******************************************************************************
 * privacy.js
 * Part of Ancho browser extension framework
 * Implements chrome.privacy
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* main closure
(function(me){
  //============================================================================
  // private variables


  //============================================================================
  // public properties

  me.network = null;
  me.services = null;
  me.websites = null;

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
