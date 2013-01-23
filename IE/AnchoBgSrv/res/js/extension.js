/******************************************************************************
 * extension.js
 * Part of Ancho browser extension framework
 * Implements chrome.extension
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("events.js").Event;

var EventFactory = require("utils.js").EventFactory;

require("extension_spec.js");
var preprocessArguments = require("typeChecking.js").preprocessArguments;
var notImplemented = require("typeChecking.js").notImplemented;

var EVENT_LIST = ['onConnect',
                  'onConnectExternal',
                  'onMessage',
                  'onMessageExternal'];
var API_NAME = 'extension';

var portPairs = {}
var addPortPair = function (pair, instanceID) {
  if (!portPairs[instanceID]) {
    portPairs[instanceID] = [];
  }
  portPairs[instanceID].push(pair);
}
exports.addPortPair = addPortPair;

function releasePorts(instanceID) {
  if (portPairs[instanceID]) {
    for (var i = 0; i < portPairs[instanceID].length; ++i) {
      portPairs[instanceID][i].release();
    }
    delete portPairs[instanceID];
  }
}

var Port = function(aName, aSender) {
  var self = this;
  this.postMessage = function(msg) {
    if (self.otherPort) {
      self.otherPort.onMessage.fire(msg);
    }
  };
  this.otherPort = null;
  this.sender = aSender;
  this.onDisconnect = new Event('port.onDisconnect');
  this.onMessage = new Event('port.onMessage');
  this.name = aName;

  this.disconnect = function() {
    self.otherPort.onDisconnect.fire();
    self.otherPort = null;
  }
  this.release = function() {
    delete onDisconnect;
    delete onMessage;
  }
};

/*
Contains both ends (ports) of communication channel used by connect()
*/
var PortPair = function(aName, aMessageSender) {
  var self = this;
  this.near = new Port(aName, aMessageSender);
  this.far = new Port(aName, aMessageSender);

  this.near.otherPort = this.far;
  this.far.otherPort = this.near;

  this.release = function() {
    self.near.disconnect();
    self.far.disconnect();
    self.near.release();
    self.far.release();
  }
}
exports.PortPair = PortPair;

/*
To sendMessage is passed responseCallback, which can be invoked only once
for all listeners and if requested also asynchronously. This object wrapps the original
callback and prohibits multiple invocations.
*/
var CallbackWrapper = function(responseCallback) {
  var self = this;

  var responseCallback = responseCallback;

  this.shouldInvokeCallback = true;
  this.callback = function() {
    return function() {
    if (!self.shouldInvokeCallback) {
        return;
      }
      self.shouldInvokeCallback = false;
        //Solves the 'Different array constructors' problem:
        //apply cannot be called because if array was created in different script engine
        //it fails comparison with array constructor
        addonAPI.callFunction(responseCallback, arguments);
    }
  } ();
}
exports.CallbackWrapper = CallbackWrapper;

var MessageSender = function(aInstanceId) {
  this.id = addonAPI.id;

  if (aInstanceId > 0) { //we are in tab context
    try {
      this.tab = serviceAPI.getTabInfo(aInstanceId, Object);
    } catch (e) {
      console.error("Internal error: getTabInfo() failed! " + e.description + "; error code = " + e.number);
      this.tab = { "id": aInstanceId };
    }
  } else {
    this.tab = { //Dummy tab - reproducing chrome behavior
      "active": false,
      "highlighted": false,
      "id": -1,
      "incognito": false,
      "index": -1,
      "pinned": false,
      "selected": false,
      "status": "complete",
      "title": "", //TODO - get proper value for popup windows
      "url": "", //TODO - get proper value for popup windows
      "windowId": -1
    };
  }
};
exports.MessageSender = MessageSender;


var addonRootURL = 'chrome-extension://' + addonAPI.id + '/';
exports.addonRootURL = addonRootURL;


//******************************************************************************
//* main closure
var Extension = function(instanceID) {
  //============================================================================
  // private variables
  _instanceID = instanceID;
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
    var args = preprocessArguments('chrome.extension.connect', arguments);
    console.debug("extension.connect(..) called");
    var name = (args.connectInfo != undefined) ? connectInfo.name : undefined;
    var pair = new PortPair(name, new MessageSender(_instanceID));
    addPortPair(pair, _instanceID);
    if (args.extensionId != undefined && args.extensionId != addonAPI.id) {
      serviceAPI.invokeExternalEventObject(
              args.extensionId,
              'extension.onConnectExternal',
              [pair.far]
              );
    } else {
      addonAPI.invokeEventObject(
              'extension.onConnect',
              -1, //TODO: after tabs API finished prevent content scripts from notifications
              true,
              [pair.far]
              );
    }
    return pair.near;
  };

  //----------------------------------------------------------------------------
  // chrome.extension.getBackgroundPage
  //   returns   global
  this.getBackgroundPage = function() {
    var args = notImplemented('chrome.extension.getBackgroundPage', arguments);
  };

  //----------------------------------------------------------------------------
  // chrome.extension.getURL
  //   returns   string
  this.getURL = function(path) {
    return addonRootURL + path;
  };

  //----------------------------------------------------------------------------
  // chrome.extension.getViews
  //   returns   array of global
  this.getViews = function(fetchProperties) {
    var args = notImplemented('chrome.extension.getViews', arguments);
  };

  //----------------------------------------------------------------------------
  // chrome.extension.isAllowedFileSchemeAccess
  this.isAllowedFileSchemeAccess = function(callback) {
    var args = notImplemented('chrome.extension.isAllowedFileSchemeAccess', arguments);
  };

  //----------------------------------------------------------------------------
  // chrome.extension.isAllowedIncognitoAccess
  this.isAllowedIncognitoAccess = function(callback) {
    var args = notImplemented('chrome.extension.isAllowedIncognitoAccess', arguments);
  };

  //----------------------------------------------------------------------------
  // chrome.extension.sendMessage
  this.sendMessage = function(extensionId, message, responseCallback) {
    var args = preprocessArguments('chrome.extension.sendMessage', arguments);
    console.debug("extension.sendMessage(..) called: " + args.message);

    var sender = new MessageSender(_instanceID);
    var callback = undefined;
    var ret = undefined;
    if (args.responseCallback) {
      var callbackWrapper = new CallbackWrapper(args.responseCallback);
      callback = callbackWrapper.callback;
    }
    if (args.extensionId && args.extensionId != addonAPI.id) {
      ret = serviceAPI.invokeExternalEventObject(
            args.extensionId,
            'extension.onMessageExternal',
            [args.message, sender, callback]
            );
    } else {
      ret = addonAPI.invokeEventObject(
            'extension.onMessage',
            _instanceID,
            true, //we are skipping _instanceID
            [args.message, sender, callback]
            );
    }

    if (callbackWrapper === undefined) {
      return;
    }

    //if responseCallaback not yet called, check if some of the listeners
    //requests asynchronous responseCallback, otherwise disable responseCallback
    if (callbackWrapper.shouldInvokeCallback && ret != undefined) {
      var arr = new VBArray(ret).toArray();
      for (var i = 0; i < arr.length; ++i) {
        if (arr[i] === true) {
          console.debug("Asynchronous call to responseCallback requested!");
          return;
        }
      }
    }
    callbackWrapper.shouldInvokeCallback = false;
  };

  //----------------------------------------------------------------------------
  // chrome.extension.setUpdateUrlData
  this.setUpdateUrlData = function(data) {
    var args = notImplemented('chrome.extension.setUpdateUrlData', arguments);
  };

  //============================================================================
  // events

  EventFactory.createEvents(this, instanceID, API_NAME, EVENT_LIST);
  //============================================================================
  //============================================================================
  // main initialization


  this.testFunction = function() {
    return serviceAPI.testFunction();
  }


}

exports.createAPI = function(instanceID) {
  return new Extension(instanceID);
}

exports.releaseAPI = function(instanceID) {
  EventFactory.releaseEvents(instanceID, API_NAME, EVENT_LIST);

  releasePorts(instanceID);
}
