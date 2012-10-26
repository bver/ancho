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
    console.debug("tabs.captureVisibleTab(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.connect
  //   returns   Port
  this.connect = function(tabId, connectInfo) {
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
    addonAPI.createTab(createProperties, Object, callback);
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.detectLanguage
  this.detectLanguage = function(tabId, callback) {
    console.debug("tabs.detectLanguage(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.executeScript
  this.executeScript = function(tabId, details, callback) {
    if ((tabId && !(typeof tabId === 'number'))
      || (!details || (!details.code && !details.file))
      || (callback && !(typeof callback === 'function'))
      ) {
      throw new Error('Invocation of chrome.tabs.executeScript doesn\'t match definition.');
    }
    if (details.code && details.file) {
      throw new Error('Code and file should not be specified at the same time in the second argument.');
    }
    var str = details.code;
    var isInFile = false;
    var allFrames = !!details.allFrames;
    if (details.code) {
      str = details.code;
      isInFile = false;
    } else {
      str = details.file;
      isInFile = true;
    }
    var ret = addonAPI.executeScript(tabId, str, isInFile, allFrames);
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
      return retVal;
    }

    var ret = addonAPI.queryTabs(queryInfo, Object);
    var tabs = new VBArray(ret).toArray();

    var filteredTabs = [];
    for (var i = 0; i < tabs.length; ++i) {
      if (checkTabForQueryInfo(tabs[i], queryInfo)) {
        filteredTabs.push(tabs[i]);
      }
    }
    callback(filteredTabs);
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.reload
  this.reload = function(tabId, reloadProperties, callback) {
    addonAPI.reloadTab(tabId);
    if (callback) {
      callback();
    }
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.remove
  this.remove = function(tabIds, callback) {
    var tabs;
    if (typeof tabIds === 'number') {
      tabs = [tabIds];
    } else {
      tabs = tabIds;
    }
    var callbackWrapper = new removeCallbackWrapper(tabs, callback);
    try {
      addonAPI.removeTabs(tabs, callbackWrapper.singleTabRemoveCallback);
    } catch (e) {
      console.error("Error while removing tabs [" + tabs + "] " + typeof(callbackWrapper.singleTabRemoveCallback) + " : " + e.message);
      throw e;
    }
  };

  //----------------------------------------------------------------------------
  // chrome.tabs.sendMessage
  this.sendMessage = function(tabId, message, responseCallback) {
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
    addonAPI.updateTab(tabId, updateProperties);

    if (callback) {
      this.get(tabId, callback);
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