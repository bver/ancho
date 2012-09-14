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
(function(me){
  //============================================================================
  // private variables
  

  //============================================================================
  // public methods
    
  //----------------------------------------------------------------------------
  // aji.cookies.get
  me.get = function(details, callback) {
    console.debug("cookies.get(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.cookies.getAll
  me.getAll = function(details, callback) {
    console.debug("cookies.getAll(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.cookies.getAllCookieStores
  me.getAllCookieStores = function(callback) {
    console.debug("cookies.getAllCookieStores(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.cookies.remove
  me.remove = function(details, callback) {
    console.debug("cookies.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.cookies.set
  me.set = function(details, callback) {
    console.debug("cookies.set(..) called");
  };

  //============================================================================
  // events
    
  me.onChanged = new Event();

  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);
