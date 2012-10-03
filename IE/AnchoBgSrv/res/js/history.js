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
exports.createAPI = function(instanceID) {
  return new (function() {
  //============================================================================
  // private variables
  

  //============================================================================
  // public methods
    
  //----------------------------------------------------------------------------
  // aji.history.addUrl
  this.addUrl = function(details, callback) {
    console.debug("history.addUrl(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.history.deleteAll
  this.deleteAll = function(callback) {
    console.debug("history.deleteAll(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.history.deleteRange
  this.deleteRange = function(range, callback) {
    console.debug("history.deleteRange(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.history.deleteUrl
  this.deleteUrl = function(details, callback) {
    console.debug("history.deleteUrl(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.history.getVisits
  this.getVisits = function(details, callback) {
    console.debug("history.getVisits(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.history.search
  this.search = function(query, callback) {
    console.debug("history.search(..) called");
  };

  //============================================================================
  // events

  this.onVisitRemoved = new Event('history.onVisitRemoved', instanceID);
  this.onVisited = new Event('history.onVisited', instanceID);

  //============================================================================
  //============================================================================
  // main initialization

})();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('history.onVisitRemoved', instanceID);
  addonAPI.removeEventObject('history.onVisited', instanceID);
}