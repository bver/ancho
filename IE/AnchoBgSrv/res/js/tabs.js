/******************************************************************************
 * tabs.js
 * Part of Ancho browser extension framework
 * Implements aji.tabs
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
  // aji.tabs.captureVisibleTab
  this.captureVisibleTab = function(windowId, options, callback) {
    console.debug("tabs.captureVisibleTab(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.connect
  //   returns   Port
  this.connect = function(tabId, connectInfo) {
    console.debug("tabs.connect(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.create
  this.create = function(createProperties, callback) {
    console.debug("tabs.create(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.detectLanguage
  this.detectLanguage = function(tabId, callback) {
    console.debug("tabs.detectLanguage(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.executeScript
  this.executeScript = function(tabId, details, callback) {
    console.debug("tabs.executeScript(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.get
  this.get = function(tabId, callback) {
    console.debug("tabs.get(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.getCurrent
  this.getCurrent = function(callback) {
    console.debug("tabs.getCurrent(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.highlight
  this.highlight = function(highlightInfo, callback) {
    console.debug("tabs.highlight(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.insertCSS
  this.insertCSS = function(tabId, details, callback) {
    console.debug("tabs.insertCSS(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.move
  this.move = function(tabIds, moveProperties, callback) {
    console.debug("tabs.move(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.query
  this.query = function(queryInfo, callback) {
    console.debug("tabs.query(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.reload
  this.reload = function(tabId, reloadProperties, callback) {
    console.debug("tabs.reload(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.remove
  this.remove = function(tabIds, callback) {
    console.debug("tabs.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.sendMessage
  this.sendMessage = function(tabId, message, responseCallback) {
    console.debug("tabs.sendMessage(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.update
  this.update = function(tabId, updateProperties, callback) {
    console.debug("tabs.update(..) called");
  };

  //============================================================================
  // events

  this.onActivated = new Event('tabs.onActivated', instanceID);
  this.onAttached = new Event('tabs.onAttached', instanceID);
  this.onCreated = new Event('tabs.onCreated', instanceID);
  this.onDetached = new Event('tabs.onDetached', instanceID);
  this.onHighlighted = new Event('tabs.onHighlighted', instanceID);
  this.onMoved = new Event('tabs.onMoved', instanceID);
  this.onRemoved = new Event('tabs.onRemoved', instanceID);
  this.onUpdated = new Event('tabs.onUpdated', instanceID);

  //============================================================================
  //============================================================================
  // main initialization

  })();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('tabs.onActivated', instanceID);
  addonAPI.removeEventObject('tabs.onAttached', instanceID);
  addonAPI.removeEventObject('tabs.onCreated', instanceID);
  addonAPI.removeEventObject('tabs.onDetached', instanceID);
  addonAPI.removeEventObject('tabs.onHighlighted', instanceID);
  addonAPI.removeEventObject('tabs.onMoved', instanceID);
  addonAPI.removeEventObject('tabs.onRemoved', instanceID);
  addonAPI.removeEventObject('tabs.onUpdated', instanceID);
}