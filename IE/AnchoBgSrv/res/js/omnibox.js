/******************************************************************************
 * omnibox.js
 * Part of Ancho browser extension framework
 * Implements chrome.omnibox
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("events.js").Event;
var EventFactory = require("utils.js").EventFactory;

var EVENT_LIST = ['onInputCancelled',
                  'onInputChanged',
                  'onInputEntered',
                  'onInputStarted'];
var API_NAME = 'omnibox';
//******************************************************************************
//* main closure
exports.createAPI = function(instanceID) {
  return new (function() {
  //============================================================================
  // private variables


  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.omnibox.setDefaultSuggestion
  this.setDefaultSuggestion = function(suggestion) {
    console.debug("omnibox.setDefaultSuggestion(..) called");
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