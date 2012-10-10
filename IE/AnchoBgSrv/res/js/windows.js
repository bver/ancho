/******************************************************************************
 * windows.js
 * Part of Ancho browser extension framework
 * Implements chrome.windows
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("Event.js").Event;
var EventFactory = require("utils.js").EventFactory;

var EVENT_LIST = ['onCreated',
                  'onFocusChanged',
                  'onRemoved'];
var API_NAME = 'windows';
//******************************************************************************
//* main closure
var Windows = function(instanceID) {
  //============================================================================
  // private variables


  //============================================================================
  // public properties

  this.WINDOW_ID_NONE = null;
  this.WINDOW_ID_CURRENT = null;

  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.windows.create
  this.create = function(createData, callback) {
    console.debug("windows.create(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.windows.get
  this.get = function(windowId, getInfo, callback) {
    console.debug("windows.get(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.windows.getAll
  this.getAll = function(getInfo, callback) {
    console.debug("windows.getAll(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.windows.getCurrent
  this.getCurrent = function(getInfo, callback) {
    console.debug("windows.getCurrent(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.windows.getLastFocused
  this.getLastFocused = function(getInfo, callback) {
    console.debug("windows.getLastFocused(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.windows.remove
  this.remove = function(windowId, callback) {
    console.debug("windows.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.windows.update
  this.update = function(windowId, updateInfo, callback) {
    console.debug("windows.update(..) called");
  };

  //============================================================================
  // events

  EventFactory.createEvents(this, instanceID, API_NAME, EVENT_LIST);
  //============================================================================
  //============================================================================
  // main initialization

}

exports.createAPI = function(instanceID) {
  return new Windows(instanceID);
}

exports.releaseAPI = function(instanceID) {
  EventFactory.releaseEvents(instanceID, API_NAME, EVENT_LIST);
}