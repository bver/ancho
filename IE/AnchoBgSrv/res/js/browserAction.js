/******************************************************************************
 * browserAction.js
 * Part of Ancho browser extension framework
 * Implements chrome.browserAction
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("Event.js").Event;
var EventFactory = require("utils.js").EventFactory;

var EVENT_LIST = ['onClicked'];
var API_NAME = 'browserAction';
//******************************************************************************
//* main closure
var BrowserAction = function(instanceID) {
  //============================================================================
  // private variables


  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.browserAction.getBadgeBackgroundColor
  this.getBadgeBackgroundColor = function(details, callback) {
    console.debug("browserAction.getBadgeBackgroundColor(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.getBadgeText
  this.getBadgeText = function(details, callback) {
    console.debug("browserAction.getBadgeText(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.getPopup
  this.getPopup = function(details, callback) {
    console.debug("browserAction.getPopup(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.getTitle
  this.getTitle = function(details, callback) {
    console.debug("browserAction.getTitle(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.setBadgeBackgroundColor
  this.setBadgeBackgroundColor = function(details) {
    console.debug("browserAction.setBadgeBackgroundColor(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.setBadgeText
  this.setBadgeText = function(details) {
    console.debug("browserAction.setBadgeText(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.setIcon
  this.setIcon = function(details) {
    console.debug("browserAction.setIcon(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.setPopup
  this.setPopup = function(details) {
    console.debug("browserAction.setPopup(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.setTitle
  this.setTitle = function(details) {
    console.debug("browserAction.setTitle(..) called");
  };

  //============================================================================
  // events

  EventFactory.createEvents(this, instanceID, API_NAME, EVENT_LIST);

  //============================================================================
  //============================================================================
  // main initialization

}

exports.createAPI = function(instanceID) {
  return new BrowserAction(instanceID);
}

exports.releaseAPI = function(instanceID) {
  EventFactory.releaseEvents(instanceID, API_NAME, EVENT_LIST); ;
}

exports.initAPI = function(browserActionData) {
  var debugString = "browserAction.initAPI(..) called.";

  if (browserActionData) {
    if (browserActionData.default_icon) {
      debugString = debugString + " icon: " + browserActionData.default_icon + ";";
    }
    if (browserActionData.default_title) {
      debugString = debugString + " title: " + browserActionData.default_title + ";";
    }
    if (browserActionData.default_popup) {
      debugString = debugString + " popup: " + browserActionData.default_popup + ";";
    }
  }
  
  console.debug(debugString);
}