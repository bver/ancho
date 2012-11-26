/******************************************************************************
 * i18n.js
 * Part of Ancho browser extension framework
 * Implements chrome.i18n
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
  // chrome.i18n.getAcceptLanguages
  me.getAcceptLanguages = function(callback) {
    console.debug("i18n.getAcceptLanguages(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.i18n.getMessage
  //   returns   string
  me.getMessage = function(messageName, substitutions) {
    console.debug("i18n.getMessage(..) called");
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