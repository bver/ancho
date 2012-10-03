/******************************************************************************
 * cookies.js
 * Part of Ancho browser extension framework
 * Implements aji.cookies
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
  // public methods
    
  //----------------------------------------------------------------------------
  // aji.cookies.get
  this.get = function(details, callback) {
    console.debug("cookies.get(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.cookies.getAll
  this.getAll = function(details, callback) {
    console.debug("cookies.getAll(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.cookies.getAllCookieStores
  this.getAllCookieStores = function(callback) {
    console.debug("cookies.getAllCookieStores(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.cookies.remove
  this.remove = function(details, callback) {
    console.debug("cookies.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.cookies.set
  this.set = function(details, callback) {
    console.debug("cookies.set(..) called");
  };

  //============================================================================
  // events
    
  this.onChanged = new Event('cookies.onChanged', instanceID);

  //============================================================================
  //============================================================================
  // main initialization

})();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('cookies.onChanged', instanceID);  
}