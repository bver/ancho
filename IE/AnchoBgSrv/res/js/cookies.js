/******************************************************************************
 * cookies.js
 * Part of Ancho browser extension framework
 * Implements chrome.cookies
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("Event.js").Event;
var EventFactory = require("utils.js").EventFactory;

var EVENT_LIST = ['onChanged'];
var API_NAME = 'cookies';
//******************************************************************************
//* main closure
var Cookies = function(instanceID) {
  //============================================================================
  // private variables


  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.cookies.get
  this.get = function(details, callback) {
    console.debug("cookies.get(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.cookies.getAll
  this.getAll = function(details, callback) {
    console.debug("cookies.getAll(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.cookies.getAllCookieStores
  this.getAllCookieStores = function(callback) {
    console.debug("cookies.getAllCookieStores(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.cookies.remove
  this.remove = function(details, callback) {
    console.debug("cookies.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.cookies.set
  this.set = function(details, callback) {
    console.debug("cookies.set(..) called");
  };

  //============================================================================
  // events

  EventFactory.createEvents(this, instanceID, API_NAME, EVENT_LIST);

  //============================================================================
  //============================================================================
  // main initialization

}

exports.createAPI = function(instanceID) {
  return new Cookies(instanceID);
}

exports.releaseAPI = function(instanceID) {
  EventFactory.releaseEvents(instanceID, API_NAME, EVENT_LIST);
}