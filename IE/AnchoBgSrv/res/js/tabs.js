/******************************************************************************
 * tabs.js
 * Part of Ancho browser extension framework
 * Implements chrome.tabs
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("Event.js").Event;
var EventFactory = require("utils.js").EventFactory;

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
var addPortPair = require("extension.js").addPortPair;
//******************************************************************************
//* main closure
var Tabs = function(instanceID) {
  //============================================================================
  // private variables
  API = this;
  //============================================================================
  // public properties

  API.Tab = function() {
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
  API.InjectDetails = new function() {
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
    console.debug("tabs.captureVisibleTab(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.connect
  //   returns   Port
  this.connect = function(tabId, connectInfo) {
    console.debug("tabs.connect(..) called");
    var name = (connectInfo != undefined) ? connectInfo.name : undefined;
    var pair = new PortPair(name, new MessageSender());
    addPortPair(pair, _instanceID);
    addonAPI.invokeEventObject(
              'extension.onConnect',
              tabId,
              false,
              [pair.far]
              );
    return pair.near;
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.create
  this.create = function(createProperties, callback) {
    console.debug("tabs.create(..) called");
    try {
      var tab = addonAPI.createTab(createProperties, Object, callback);
    } catch(e) {
      console.debug("Error: " + e.message);
    }
    /*if (callback) {
      callback(tab);
    }*/
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.detectLanguage
  this.detectLanguage = function(tabId, callback) {
    console.debug("tabs.detectLanguage(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.executeScript
  this.executeScript = function(tabId, details, callback) {
    console.debug("tabs.executeScript(..) called");
    if ((tabId && !(typeof tabId === 'number'))
      || (!details || (!details.code && !details.file))
      || (callback && !(typeof callback === 'function'))
      ) {
      throw new Error('Invocation of chrome.tabs.executeScript doesn\'t match definition.');
    }
    if (details.code && details.file) {
      throw new Error('Code and file should not be specified at the same time in the second argument.');
    }
    var ret = undefined;
    if (details.code) {
      ret = addonAPI.executeScript(tabId, details.code, false);
    } else {
      ret = addonAPI.executeScript(tabId, details.file, true);
    }
    if (callback) {
      callback(ret);
    }
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.get
  this.get = function(tabId, callback) {
    console.debug("tabs.get(..) called");
    if (!callback) {
      return;
    }
    var tab = addonAPI.getTabInfo(tabId, Object); //Pass reference to Object - used to create tab info

    callback(tab);
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.getCurrent
  this.getCurrent = function(callback) {
    console.debug("tabs.getCurrent(..) called");
    //if we are not running in tab context - return undefined
    if (instanceID <= 0) {
      if (callback) {
        callback(undefined);
      }
      return;
    }
    get(instanceID, callback);
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.highlight
  this.highlight = function(highlightInfo, callback) {
    console.debug("tabs.highlight(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.insertCSS
  this.insertCSS = function(tabId, details, callback) {
    console.debug("tabs.insertCSS(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.move
  this.move = function(tabIds, moveProperties, callback) {
    console.debug("tabs.move(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.query
  this.query = function(queryInfo, callback) {
    var tabs = addonAPI.queryTabs(queryInfo, Object);
    callback(tabs);
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.reload
  this.reload = function(tabId, reloadProperties, callback) {
    addonAPI.reloadTab(tabId);
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.remove
  this.remove = function(tabIds, callback) {
    if (typeof tabIds === 'number') {
      tabIds = [tabIds];
    }
    addonAPI.removeTabs(tabIds);
    callback();
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.sendMessage
  this.sendMessage = function(tabId, message, responseCallback) {
    console.debug("tabs.sendMessage(..) called");
    sender = new MessageSender();
    callback = undefined;
    ret = undefined;
    if (responseCallback) {
      callbackWrapper = new CallbackWrapper(responseCallback);
      callback = callbackWrapper.callback;
    }
    ret = addonAPI.invokeEventObject(
            'extension.onMessage',
            tabId,
            false, //we are selecting tab with tabId
            [message, sender, callback]
            ); //TODO: fill MessageSender


    //if responseCallaback not yet called, check if some of the listeners
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
    console.debug("tabs.update(..) called");
    addonAPI.updateTab(tabId, updateProperties);

    if (typeof callback === 'function') {
      API.get(tabId, callback);
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