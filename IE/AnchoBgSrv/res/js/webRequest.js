/******************************************************************************
 * webRequest.js
 * Part of Ancho browser extension framework
 * Implements chrome.webRequest
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("events.js").Event;
var EventFactory = require("utils.js").EventFactory;

var EVENT_LIST = ['onAuthRequired',
                  'onBeforeRedirect',
                  'onBeforeRequest',
                  'onBeforeSendHeaders',
                  'onCompleted',
                  'onErrorOccurred',
                  'onHeadersReceived',
                  'onResponseStarted',
                  'onSendHeaders'];
var API_NAME = 'webRequest';
//******************************************************************************
//* main closure
exports.createAPI = function(instanceID) {
  return new (function() {
  //============================================================================
  // private variables


  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.webRequest.handlerBehaviorChanged
  this.handlerBehaviorChanged = function(callback) {
    console.debug("webRequest.handlerBehaviorChanged(..) called");
  };

  //============================================================================
  // events

  EventFactory.createEvents(this, instanceID, API_NAME, EVENT_LIST);

  //============================================================================
  //============================================================================
  // main initialization

})();
}

exports.releaseAPI = function(instanceID) {
  EventFactory.releaseEvents(instanceID, API_NAME, EVENT_LIST);
}