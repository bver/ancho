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

require("browserAction_spec.js");
var preprocessArguments = require("typeChecking.js").preprocessArguments;
var notImplemented = require("typeChecking.js").notImplemented;



var browserActionInfo;

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
    var args = notImplemented('chrome.browserAction.getBadgeBackgroundColor', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.getBadgeText
  this.getBadgeText = function(details, callback) {
    var args = notImplemented('chrome.browserAction.getBadgeText', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.getPopup
  this.getPopup = function(details, callback) {
    var args = notImplemented('chrome.browserAction.getPopup', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.getTitle
  this.getTitle = function(details, callback) {
    var args = notImplemented('chrome.browserAction.getTitle', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.setBadgeBackgroundColor
  this.setBadgeBackgroundColor = function(details) {
    var args = notImplemented('chrome.browserAction.setBadgeBackgroundColor', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.setBadgeText
  this.setBadgeText = function(details) {
    var args = notImplemented('chrome.browserAction.setBadgeText', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.setIcon
  this.setIcon = function(details) {
    var args = notImplemented('chrome.browserAction.setIcon', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.setPopup
  this.setPopup = function(details) {
    var args = notImplemented('chrome.browserAction.setPopup', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.setTitle
  this.setTitle = function(details) {
    var args = notImplemented('chrome.browserAction.setTitle', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
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
    browserActionInfo = {
      id: addonAPI.id,
      onClick: function() { console.log('CLICKED'); },
      backgroundColor: '#FFFFFF',
      badgeBackgroundColor: '#FFFFFF'
    };

    if (browserActionData.default_icon) {
      browserActionInfo.icon = 'chrome-extension://' + addonAPI.id + '/' + browserActionData.default_icon;
      debugString = debugString + " icon: " + browserActionData.default_icon + ";";
    }
    if (browserActionData.default_title) {
      browserActionInfo.title = browserActionData.default_title;
      debugString = debugString + " title: " + browserActionData.default_title + ";";
    }
    if (browserActionData.default_popup) {
      debugString = debugString + " popup: " + browserActionData.default_popup + ";";
    }
    serviceAPI.addBrowserActionInfo(browserActionInfo);
    serviceAPI.browserActionNotification();
  }

  console.debug(debugString);
}