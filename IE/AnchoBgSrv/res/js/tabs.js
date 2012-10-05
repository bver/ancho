/******************************************************************************
 * tabs.js
 * Part of Ancho browser extension framework
 * Implements chrome.tabs
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("Event.js").Event;
var EventFactory = require("utils.js").EventFactory;

var EVENT_LIST = ['onActivated',
                  'onAttached',
                  'onCreated',
                  'onDetached',
                  'onMoved',
                  'onRemoved',
                  'onUpdated'];
var API_NAME = 'tabs';
//******************************************************************************
//* main closure
var Tabs = function(instanceID) {
  //============================================================================
  // private variables


  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.tabs.captureVisibleTab
  this.captureVisibleTab = function(windowId, options, callback) {
    console.debug("tabs.captureVisibleTab(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.connect
  //   returns   Port
  this.connect = function(tabId, connectInfo) {
    console.debug("tabs.connect(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.create
  this.create = function(createProperties, callback) {
    console.debug("tabs.create(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.detectLanguage
  this.detectLanguage = function(tabId, callback) {
    console.debug("tabs.detectLanguage(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.executeScript
  this.executeScript = function(tabId, details, callback) {
    console.debug("tabs.executeScript(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.get
  this.get = function(tabId, callback) {
    console.debug("tabs.get(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.getCurrent
  this.getCurrent = function(callback) {
    console.debug("tabs.getCurrent(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.highlight
  this.highlight = function(highlightInfo, callback) {
    console.debug("tabs.highlight(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.insertCSS
  this.insertCSS = function(tabId, details, callback) {
    console.debug("tabs.insertCSS(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.move
  this.move = function(tabIds, moveProperties, callback) {
    console.debug("tabs.move(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.query
  this.query = function(queryInfo, callback) {
    console.debug("tabs.query(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.reload
  this.reload = function(tabId, reloadProperties, callback) {
    console.debug("tabs.reload(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.remove
  this.remove = function(tabIds, callback) {
    console.debug("tabs.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.sendMessage
  this.sendMessage = function(tabId, message, responseCallback) {
    console.debug("tabs.sendMessage(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.update
  this.update = function(tabId, updateProperties, callback) {
    console.debug("tabs.update(..) called");
  };

  //============================================================================
  // events

  EventFactory.createEvents(this, instanceID, API_NAME, EVENT_LIST);

  //============================================================================
  //============================================================================
  // main initialization

}

exports.createAPI = function(instanceID) {
  return new Tabs(instanceID);
}

exports.releaseAPI = function(instanceID) {
  EventFactory.releaseEvents(instanceID, API_NAME, EVENT_LIST);
}