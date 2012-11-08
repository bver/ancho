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

require("cookies_spec.js");
var preprocessArguments = require("typeChecking.js").preprocessArguments;
var notImplemented = require("typeChecking.js").notImplemented;

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
    var args = preprocessArguments('chrome.cookies.get', arguments);

  };

  //----------------------------------------------------------------------------
  // chrome.cookies.getAll
  this.getAll = function(details, callback) {
    var args = preprocessArguments('chrome.cookies.getAll', arguments);
  };

  //----------------------------------------------------------------------------
  // chrome.cookies.getAllCookieStores
  this.getAllCookieStores = function(callback) {
    var args = preprocessArguments('chrome.cookies.getAllCookieStores', arguments);
  };

  //----------------------------------------------------------------------------
  // chrome.cookies.remove
  this.remove = function(details, callback) {
    var args = preprocessArguments('chrome.cookies.remove', arguments);
  };

  //----------------------------------------------------------------------------
  // chrome.cookies.set
  this.set = function(details, callback) {
    var args = preprocessArguments('chrome.cookies.set', arguments);
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