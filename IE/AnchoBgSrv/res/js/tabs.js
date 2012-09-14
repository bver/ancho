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
(function(me){
  //============================================================================
  // private variables
  

  //============================================================================
  // public methods
    
  //----------------------------------------------------------------------------
  // aji.tabs.captureVisibleTab
  me.captureVisibleTab = function(windowId, options, callback) {
    console.debug("tabs.captureVisibleTab(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.connect
  //   returns   Port
  me.connect = function(tabId, connectInfo) {
    console.debug("tabs.connect(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.create
  me.create = function(createProperties, callback) {
    console.debug("tabs.create(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.detectLanguage
  me.detectLanguage = function(tabId, callback) {
    console.debug("tabs.detectLanguage(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.executeScript
  me.executeScript = function(tabId, details, callback) {
    console.debug("tabs.executeScript(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.get
  me.get = function(tabId, callback) {
    console.debug("tabs.get(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.getCurrent
  me.getCurrent = function(callback) {
    console.debug("tabs.getCurrent(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.highlight
  me.highlight = function(highlightInfo, callback) {
    console.debug("tabs.highlight(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.insertCSS
  me.insertCSS = function(tabId, details, callback) {
    console.debug("tabs.insertCSS(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.move
  me.move = function(tabIds, moveProperties, callback) {
    console.debug("tabs.move(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.query
  me.query = function(queryInfo, callback) {
    console.debug("tabs.query(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.reload
  me.reload = function(tabId, reloadProperties, callback) {
    console.debug("tabs.reload(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.remove
  me.remove = function(tabIds, callback) {
    console.debug("tabs.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.sendMessage
  me.sendMessage = function(tabId, message, responseCallback) {
    console.debug("tabs.sendMessage(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.tabs.update
  me.update = function(tabId, updateProperties, callback) {
    console.debug("tabs.update(..) called");
  };

  //============================================================================
  // events
    
  me.onActivated = new Event();
  me.onAttached = new Event();
  me.onCreated = new Event();
  me.onDetached = new Event();
  me.onHighlighted = new Event();
  me.onMoved = new Event();
  me.onRemoved = new Event();
  me.onUpdated = new Event();

  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);
