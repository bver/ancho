/******************************************************************************
 * history.js
 * Part of Ancho browser extension framework
 * Implements aji.history
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
  // aji.history.addUrl
  me.addUrl = function(details, callback) {
    console.debug("history.addUrl(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.history.deleteAll
  me.deleteAll = function(callback) {
    console.debug("history.deleteAll(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.history.deleteRange
  me.deleteRange = function(range, callback) {
    console.debug("history.deleteRange(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.history.deleteUrl
  me.deleteUrl = function(details, callback) {
    console.debug("history.deleteUrl(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.history.getVisits
  me.getVisits = function(details, callback) {
    console.debug("history.getVisits(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.history.search
  me.search = function(query, callback) {
    console.debug("history.search(..) called");
  };

  //============================================================================
  // events
    
  me.onVisitRemoved = new Event();
  me.onVisited = new Event();

  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);
