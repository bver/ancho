/******************************************************************************
 * webNavigation.js
 * Part of Ancho browser extension framework
 * Implements chrome.webNavigation
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("events.js").Event;
var EventFactory = require("utils.js").EventFactory;

var EVENT_LIST = ['onBeforeNavigate',
                  'onCommitted',
                  'onCompleted',
                  'onCreatedNavigationTarget',
                  'onDOMContentLoaded',
                  'onErrorOccurred',
                  'onReferenceFragmentUpdated'];
var API_NAME = 'webNavigation';
//******************************************************************************
//* main closure
exports.createAPI = function(instanceID) {
  return new (function() {
  //============================================================================
  // private variables


  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.webNavigation.getAllFrames
  this.getAllFrames = function(details, callback) {
    console.debug("webNavigation.getAllFrames(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.webNavigation.getFrame
  this.getFrame = function(details, callback) {
    console.debug("webNavigation.getFrame(..) called");
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