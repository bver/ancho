/******************************************************************************
 * history.js
 * Part of Ancho browser extension framework
 * Implements chrome.history
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("events.js").Event;
var EventFactory = require("utils.js").EventFactory;

var EVENT_LIST = ['onVisitRemoved', 'onVisited'];
var API_NAME = 'history';
//******************************************************************************
//* main closure
var History = function(instanceID) {
  //============================================================================
  // private variables


  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.history.addUrl
  this.addUrl = function(details, callback) {
    console.debug("history.addUrl(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.history.deleteAll
  this.deleteAll = function(callback) {
    console.debug("history.deleteAll(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.history.deleteRange
  this.deleteRange = function(range, callback) {
    console.debug("history.deleteRange(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.history.deleteUrl
  this.deleteUrl = function(details, callback) {
    console.debug("history.deleteUrl(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.history.getVisits
  this.getVisits = function(details, callback) {
    console.debug("history.getVisits(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.history.search
  this.search = function(query, callback) {
    console.debug("history.search(..) called");
  };

  //============================================================================
  // events

  EventFactory.createEvents(this, instanceID, API_NAME, EVENT_LIST);

  //============================================================================
  //============================================================================
  // main initialization

}

exports.createAPI = function(instanceID) {
  return new History(instanceID);
}

exports.releaseAPI = function(instanceID) {
  EventFactory.releaseEvents(instanceID, API_NAME, EVENT_LIST);
}