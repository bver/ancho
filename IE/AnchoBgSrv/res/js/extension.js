/******************************************************************************
 * extension.js
 * Part of Ancho browser extension framework
 * Implements chrome.extension
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("Event.js").Event;

var EventFactory = require("utils.js").EventFactory;

var EVENT_LIST = ['onConnect',
                  'onConnectExternal',
                  'onMessage',
                  'onMessageExternal'];
var API_NAME = 'extension';

//******************************************************************************
//* main closure
var Extension = function(instanceID) {
  //============================================================================
  // private variables


  //============================================================================
  // public properties

  this.lastError = null;
  this.inIncognitoContext = null;

  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.extension.connect
  //   returns   Port
  this.connect = function(extensionId, connectInfo) {
    console.debug("extension.connect(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.extension.getBackgroundPage
  //   returns   global
  this.getBackgroundPage = function() {
    console.debug("extension.getBackgroundPage(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.extension.getURL
  //   returns   string
  this.getURL = function(path) {
    console.debug("extension.getURL(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.extension.getViews
  //   returns   array of global
  this.getViews = function(fetchProperties) {
    console.debug("extension.getViews(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.extension.isAllowedFileSchemeAccess
  this.isAllowedFileSchemeAccess = function(callback) {
    console.debug("extension.isAllowedFileSchemeAccess(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.extension.isAllowedIncognitoAccess
  this.isAllowedIncognitoAccess = function(callback) {
    console.debug("extension.isAllowedIncognitoAccess(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.extension.sendMessage
  this.sendMessage = function(extensionId, message, responseCallback) {
    console.debug("extension.sendMessage(..) called: " + message);
  };

  //----------------------------------------------------------------------------
  // chrome.extension.setUpdateUrlData
  this.setUpdateUrlData = function(data) {
    console.debug("extension.setUpdateUrlData(..) called");
  };

  //============================================================================
  // events
  EventFactory.createEvents(this, instanceID, API_NAME, EVENT_LIST);

  //============================================================================
  //============================================================================
  // main initialization

}

exports.createAPI = function(instanceID) {
  return new Extension(instanceID);
}

exports.releaseAPI = function(instanceID) {
  EventFactory.releaseEvents(instanceID, API_NAME, EVENT_LIST);
}