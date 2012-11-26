/******************************************************************************
 * contentSettings.js
 * Part of Ancho browser extension framework
 * Implements chrome.contentSettings
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* main closure
(function(me){
  //============================================================================
  // private variables


  //============================================================================
  // public properties

  me.cookies = null;
  me.images = null;
  me.javascript = null;
  me.plugins = null;
  me.popups = null;
  me.notifications = null;

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