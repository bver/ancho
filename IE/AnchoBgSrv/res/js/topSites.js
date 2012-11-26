/******************************************************************************
 * topSites.js
 * Part of Ancho browser extension framework
 * Implements chrome.topSites
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
  // chrome.topSites.get
  me.get = function(callback) {
    console.debug("topSites.get(..) called");
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
