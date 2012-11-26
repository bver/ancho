/******************************************************************************
 * pageCapture.js
 * Part of Ancho browser extension framework
 * Implements chrome.pageCapture
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
  // chrome.pageCapture.saveAsMHTML
  me.saveAsMHTML = function(details, callback) {
    console.debug("pageCapture.saveAsMHTML(..) called");
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