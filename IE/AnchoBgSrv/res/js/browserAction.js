/******************************************************************************
* browserAction.js
* Part of Ancho browser extension framework
* Implements chrome.browserAction
* Copyright 2012 Salsita software (http://www.salsitasoft.com).
******************************************************************************/

//******************************************************************************
//* requires
var utils = require("utils.js");
var Event = require("events.js").Event;
var EventFactory = utils.EventFactory;

var EVENT_LIST = ['onClicked'];
var API_NAME = 'browserAction';

require("browserAction_spec.js");
var preprocessArguments = require("typeChecking.js").preprocessArguments;
var notImplemented = require("typeChecking.js").notImplemented;

var addonRootURL = require("extension.js").addonRootURL;

var browserActionInfo;


//******************************************************************************
//* main closure
var BrowserAction = function(instanceID) {
  //============================================================================
  // private variables

  var _instanceID = instanceID;

  function setBrowserActionProperty(aTabId, aPropertyName, aValue) {
    if (aTabId) {
      if (!browserActionInfo.tabSpecificInfo[aTabId]) {
        browserActionInfo.tabSpecificInfo[aTabId] = {};
      }
      browserActionInfo.tabSpecificInfo[aTabId][aPropertyName] = aValue;
    } else {
      browserActionInfo.globalInfo[aPropertyName] = aValue;
    }
  }

  function getBrowserActionProperty(aTabId, aPropertyName) {
    if (aTabId
      && browserActionInfo.tabSpecificInfo[aTabId]
      && browserActionInfo.tabSpecificInfo[aTabId][aPropertyName] != undefined)
    {
      return browserActionInfo.tabSpecificInfo[aTabId][aPropertyName];
    } else {
      return browserActionInfo.globalInfo[aPropertyName];
    }
  }
  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.browserAction.getBadgeBackgroundColor
  this.getBadgeBackgroundColor = function(details, callback) {
    var args = preprocessArguments('chrome.browserAction.getBadgeBackgroundColor', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
    args.callback(getBrowserActionProperty(args.details.tabId, 'badgeBackgroundColor'));
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.getBadgeText
  this.getBadgeText = function(details, callback) {
    var args = preprocessArguments('chrome.browserAction.getBadgeText', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
    args.callback(getBrowserActionProperty(args.details.tabId, 'badge'));
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.getPopup
  this.getPopup = function(details, callback) {
    var args = preprocessArguments('chrome.browserAction.getPopup', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
    args.callback(getBrowserActionProperty(args.details.tabId, 'popup'));
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.getTitle
  this.getTitle = function(details, callback) {
    var args = preprocessArguments('chrome.browserAction.getTitle', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
    args.callback(getBrowserActionProperty(args.details.tabId, 'title'));
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.setBadgeBackgroundColor
  this.setBadgeBackgroundColor = function(details) {
    var args = preprocessArguments('chrome.browserAction.setBadgeBackgroundColor', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
    setBrowserActionProperty(args.details.tabId, 'badgeBackgroundColor', utils.stringColorRepresentation(args.details.color));
    serviceAPI.browserActionNotification();
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.setBadgeText
  this.setBadgeText = function(details) {
    var args = preprocessArguments('chrome.browserAction.setBadgeText', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
    setBrowserActionProperty(args.details.tabId, 'badge', args.details.text);
    serviceAPI.browserActionNotification();
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.setIcon
  this.setIcon = function(details) {
    var args = preprocessArguments('chrome.browserAction.setIcon', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
    //TODO - handle other possible icon specifications
    setBrowserActionProperty(args.details.tabId, 'icon', args.details.path);
    serviceAPI.browserActionNotification();
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.setPopup
  this.setPopup = function(details) {
    var args = preprocessArguments('chrome.browserAction.setPopup', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
    setBrowserActionProperty(args.details.tabId, 'popup', args.details.popup);
    serviceAPI.browserActionNotification();
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.setTitle
  this.setTitle = function(details) {
    var args = preprocessArguments('chrome.browserAction.setTitle', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
    setBrowserActionProperty(args.details.tabId, 'title', args.details.title);
    serviceAPI.browserActionNotification();
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.enable
  this.enable = function(tabId) {
    var args = preprocessArguments('chrome.browserAction.enable', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
    setBrowserActionProperty(args.details.tabId, 'enabled', true);
    serviceAPI.browserActionNotification();
  };

  //----------------------------------------------------------------------------
  // chrome.browserAction.enable
  this.disable = function(tabId) {
    var args = preprocessArguments('chrome.browserAction.disable', arguments);
    if (!browserActionInfo) {
      throw new Error('This extension has no action specified.');
    }
    setBrowserActionProperty(args.details.tabId, 'enabled', false);
    serviceAPI.browserActionNotification();
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
  if (browserActionInfo && browserActionInfo.tabSpecificInfo[instanceID]) {
    delete browserActionInfo.tabSpecificInfo[instanceID];
  }

  EventFactory.releaseEvents(instanceID, API_NAME, EVENT_LIST);
}

function createPopup(aX, aY, aPopup) {
  var api = require("api.js");
  var apiId = api.reserveFullAPIInstanceID();
  var data = {
    chrome: api.createFullAPI(apiId),
    console: console
  };
  var cleanUpProcedure = function() {
    console.debug("Cleaning after popup window");
    api.releaseFullAPI(apiId);
  }
  console.debug("Creating popup window");
  serviceAPI.createPopupWindow(aPopup, aX, aY, data, cleanUpProcedure);
}



exports.initBrowserAction = function(browserActionData) {
  var debugString = "browserAction.initAPI(..) called.";

  if (browserActionData) {
    browserActionInfo = {
      id: addonAPI.id,
      onClick: function(aX, aY, aTabId) {
        var popup = browserActionInfo.tabSpecificInfo[aTabId] ? browserActionInfo.tabSpecificInfo[aTabId].popup : undefined;
        popup = popup || browserActionInfo.globalInfo.popup;
        if (popup) {
          createPopup(aX, aY, popup);
        }
        serviceAPI.invokeExternalEventObject(
              addonAPI.id,
              'browserAction.onClicked',
              [{}]//TODO - send tab info
              );
      },
      globalInfo: {
        enabled: true,
        badgeBackgroundColor: '#FFFFFF',
        badge: '',
        title: undefined,
        popup: undefined
      },
      tabSpecificInfo: {}
    };

    if (browserActionData.default_icon) {
      browserActionInfo.globalInfo.icon = 'chrome-extension://' + addonAPI.id + '/' + browserActionData.default_icon;
      debugString = debugString + " icon: " + browserActionData.default_icon + ";";
    }
    if (browserActionData.default_title) {
      browserActionInfo.globalInfo.title = browserActionData.default_title;
      debugString = debugString + " title: " + browserActionData.default_title + ";";
    }
    if (browserActionData.default_popup) {
      browserActionInfo.globalInfo.popup = addonRootURL + browserActionData.default_popup;
      debugString = debugString + " popup: " + browserActionData.default_popup + ";";
    }
    if (browserActionData.default_badge) {
      browserActionInfo.globalInfo.badge = browserActionData.default_badge;
      debugString = debugString + " popup: " + browserActionData.default_badge + ";";
    }
    if (browserActionData.default_badgeBackgroundColor) {
      browserActionInfo.globalInfo.badgeBackgroundColor = utils.stringColorRepresentation(browserActionData.default_badgeBackgroundColor);
      debugString = debugString + " badgeBackgroundColor: " + browserActionData.default_badgeBackgroundColor + ";";
    }
    serviceAPI.addBrowserActionInfo(browserActionInfo);
    serviceAPI.browserActionNotification();
  }

  console.debug(debugString);
}