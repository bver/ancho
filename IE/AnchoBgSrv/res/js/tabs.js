/******************************************************************************
 * tabs.js
 * Part of Ancho browser extension framework
 * Implements chrome.tabs
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("events.js").Event;
var EventFactory = require("utils.js").EventFactory;

var windows = require("windows.js").windows;
require("tabs_spec.js");
var preprocessArguments = require("typeChecking.js").preprocessArguments;
var notImplemented = require("typeChecking.js").notImplemented;

var EVENT_LIST = ['onActivated',
                  'onAttached',
                  'onCreated',
                  'onDetached',
                  'onMoved',
                  'onRemoved',
                  'onUpdated'];
var API_NAME = 'tabs';

var MessageSender = require("extension.js").MessageSender;
var CallbackWrapper = require("extension.js").CallbackWrapper;
var PortPair = require("extension.js").PortPair;
var addPortPair = require("extension.js").addPortPair;
var windows = require("windows.js");
//Used for gathering callbacks from removeTabs
//singleTabRemoveCallback is called from even if the tab was already removed
var removeCallbackWrapper = function(aTabs, aCallback) {
  var callback = aCallback;
  var tabs = aTabs;
  var count = aTabs.length;
  this.singleTabRemoveCallback = function(aTabID) {
    console.debug("removeSingleTab callback for tab: " + aTabID);
    --count;
    if (count == 0) {
      callback();
    }
  }
}

//******************************************************************************
//* main closure
var Tabs = function(instanceID) {
  //============================================================================
  // private variables

  //============================================================================
  // public properties

  this.Tab = function() {
    //Either loading or complete.
    this.status = null;
    //The zero-based index of the tab within its window.
    this.index = -1;
    //The ID of the tab that opened this tab, if any. This will only be present if the opener tab still exists.
    this.openerTabId = null;
    //The title of the tab. This may not be available if the tab is loading.
    this.title = null;
    //The URL the tab is displaying.
    this.url = "";
    //Whether the tab is pinned.
    this.pinned = false;
    //Whether the tab is highlighted.
    this.highlighted = false;
    //The ID of the window the tab is contained within.
    this.windowId = -1;
    //Whether the tab is active in its window.
    this.active = false;
    //The URL of the tab's favicon. This may not be available if the tab is loading.
    this.favIconUrl = null;
    //The ID of the tab. Tab IDs are unique within a browser session.
    this.id = -1;
    //Whether the tab is in an incognito window.
    this.incognito = false;
  }

  //Details of the script or CSS to inject.
  //Either the code or the file property must be set, but both may not be set at the same time.
  this.InjectDetails = new function() {
    //If allFrames is true, implies that the JavaScript or CSS should be injected into all frames of current page. By default, it's false and will only be injected into the top frame.
    this.allFrames = false;
    //JavaScript or CSS code to inject.
    this.code = null;
    //The soonest that the JavaScript or CSS will be injected into the tab. Defaults to "document_idle".
    this.runAt = "document_idle"; //enumerated string ["document_start", "document_end", "document_idle"]
    //JavaScript or CSS file to inject.
    this.file = null;
  }

  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.tabs.captureVisibleTab
  this.captureVisibleTab = function(windowId, options, callback) {
    var args = notImplemented('chrome.tabs.captureVisibleTab', arguments);
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.connect
  //   returns   Port
  this.connect = function(tabId, connectInfo) {
    var args = preprocessArguments('chrome.tabs.connect', arguments);

    var name = (args['connectInfo'] != undefined) ? args['connectInfo'].name : undefined;
    var pair = new PortPair(name, new MessageSender());
    addPortPair(pair, _instanceID);
    addonAPI.invokeEventObject(
              'extension.onConnect',
              args['tabId'],
              false,
              [pair.far]
              );
    return pair.near;
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.create
  this.create = function(createProperties, callback) {
    var args = preprocessArguments('chrome.tabs.create', arguments);
    serviceAPI.createTab(args['createProperties'], Object, args['callback']);
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.detectLanguage
  this.detectLanguage = function(tabId, callback) {
    var args = notImplemented('chrome.tabs.detectLanguage', arguments);
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.executeScript
  this.executeScript = function(tabId, details, callback) {
    var args = preprocessArguments('chrome.tabs.executeScript', arguments);
    if (!args['details'].code && !args['details'].file) {
      throw new Error('Invocation of chrome.tabs.executeScript doesn\'t match definition.');
    }
    if (args['details'].code && args['details'].file) {
      throw new Error('Code and file should not be specified at the same time in the second argument.');
    }
    var str = args['details'].code;
    var isInFile = false;
    var allFrames = !!args['details'].allFrames;
    if (args['details'].code) {
      str = args['details'].code;
      isInFile = false;
    } else {
      str = args['details'].file;
      isInFile = true;
    }
    var ret = serviceAPI.executeScript(addonAPI.id, args['tabId'], str, isInFile, allFrames);
    if (args['callback']) {
      args['callback'](ret);
    }
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.get
  this.get = function(tabId, callback) {
    var args = preprocessArguments('chrome.tabs.get', arguments);
    console.debug("tabs.get(..) called");
    var tab = serviceAPI.getTabInfo(args['tabId'], Object); //Pass reference to Object - used to create tab info
    args['callback'](tab);
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.getCurrent
  this.getCurrent = function(callback) {
    var args = preprocessArguments('chrome.tabs.getCurrent', arguments);
    //if we are not running in tab context - return undefined
    if (instanceID <= 0) {
      if (args['callback']) {
        args['callback'](undefined);
      }
      return;
    }
    get(instanceID, args['callback']);
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.highlight
  this.highlight = function(highlightInfo, callback) {
    var args = notImplemented('chrome.tabs.highlight', arguments);
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.insertCSS
  this.insertCSS = function(tabId, details, callback) {
    var args = notImplemented('chrome.tabs.insertCSS', arguments);
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.move
  this.move = function(tabIds, moveProperties, callback) {
    var args = notImplemented('chrome.tabs.move', arguments);
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.query
  this.query = function(queryInfo, callback) {
    var args = preprocessArguments('chrome.tabs.query', arguments);

    function checkTabForQueryInfo(aTab, aQueryInfo) {
      var retVal = true;
      if (aQueryInfo.url) {
        retVal = retVal && (aTab.url == aQueryInfo.url);
      }
      if (aQueryInfo.id) {
        retVal = retVal && (aTab.id == aQueryInfo.id);
      }
      if (aQueryInfo.active) {
        retVal = retVal && (aTab.active == aQueryInfo.active);
      }
      if (aQueryInfo.windowId) {
        var winId = aQueryInfo.windowId;
        if (windows.WINDOW_ID_CURRENT == winId) {
          winId = serviceAPI.getCurrentWindowId();
        }
        retVal = retVal && (aTab.windowId == winId);
      }
      return retVal;
    }

    var ret = serviceAPI.queryTabs(args['queryInfo'], Object);
    var tabs = new VBArray(ret).toArray();

    var filteredTabs = [];
    for (var i = 0; i < tabs.length; ++i) {
      if (checkTabForQueryInfo(tabs[i], queryInfo)) {
        filteredTabs.push(tabs[i]);
      }
    }
    args['callback'](filteredTabs);
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.reload
  this.reload = function(tabId, reloadProperties, callback) {
    var args = preprocessArguments('chrome.tabs.reload', arguments);
    serviceAPI.reloadTab(args['tabId']);
    if (args['callback']) {
      args['callback']();
    }
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.remove
  this.remove = function(tabIds, callback) {
    var args = preprocessArguments('chrome.tabs.remove', arguments);
    var tabs;
    if (typeof (args['tabIds']) === 'number') {
      tabs = [args['tabIds']];
    } else {
      tabs = args['tabIds'];
    }
    var callbackWrapper = new removeCallbackWrapper(tabs, args['callback']);
    try {
      serviceAPI.removeTabs(tabs, callbackWrapper.singleTabRemoveCallback);
    } catch (e) {
      console.error("Error while removing tabs [" + tabs + "] " + typeof (callbackWrapper.singleTabRemoveCallback) + " : " + e.message);
      throw e;
    }
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.sendMessage
  this.sendMessage = function(tabId, message, responseCallback) {
    var args = preprocessArguments('chrome.tabs.sendMessage', arguments);

    sender = new MessageSender();
    callback = undefined;
    ret = undefined;
    if (responseCallback) {
      callbackWrapper = new CallbackWrapper(args['responseCallback']);
      callback = callbackWrapper.callback;
    }
    ret = addonAPI.invokeEventObject(
            'extension.onMessage',
            args['tabId'],
            false, //we are selecting tab with tabId
            [message, sender, callback]
            ); //TODO: fill MessageSender


    //if responseCallback not yet called, check if some of the listeners
    //requests asynchronous responseCallback, otherwise disable responseCallback
    if (callbackWrapper.callable && ret != undefined) {
      var arr = new VBArray(ret).toArray();
      for (var i = 0; i < arr.length; ++i) {
        if (arr[i] === true) {
          console.debug("Asynchronous call to responseCallback requested!");
          return;
        }
      }
    }
    callbackWrapper.callable = false;

  };

  //----------------------------------------------------------------------------
  // chrome.tabs.update
  this.update = function(tabId, updateProperties, callback) {
    var args = preprocessArguments('chrome.tabs.update', arguments);
    serviceAPI.updateTab(args['tabId'], args['updateProperties']);

    if (args['callback']) {
      this.get(args['tabId'], args['callback']);
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
  return new Tabs(instanceID);
}

exports.releaseAPI = function(instanceID) {
  EventFactory.releaseEvents(instanceID, API_NAME, EVENT_LIST);
}