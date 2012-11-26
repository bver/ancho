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

var MessageSender = function(aTab) {
  this.id = addonAPI.id;
  if (aTab) {
    this.tab = aTab; //optional
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
    console.debug("extension.connect(..) called");
    var name = (connectInfo != undefined) ? connectInfo.name : undefined;
    var pair = new PortPair(name, new MessageSender());
    addPortPair(pair, _instanceID);
    if (extensionId != undefined && extensionId != addonAPI.id) {
      serviceAPI.invokeExternalEventObject(
              extensionId,
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
    console.debug("extension.getBackgroundPage(..) called");
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
    console.debug("extension.getViews(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.extension.isAllowedFileSchemeAccess
  this.isAllowedFileSchemeAccess = function(callback) {
    console.debug("extension.isAllowedFileSchemeAccess(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.extension.isAllowedIncognitoAccess
  this.isAllowedIncognitoAccess = function(callback) {
    console.debug("extension.isAllowedIncognitoAccess(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.extension.sendMessage
  this.sendMessage = function(extensionId, message, responseCallback) {
    console.debug("extension.sendMessage(..) called: " + message);
    var sender = new MessageSender();
    var callback = undefined;
    var ret = undefined;
    if (responseCallback) {
      callbackWrapper = new CallbackWrapper(responseCallback);
      callback = callbackWrapper.callback;
    }
    if (extensionId && extensionId != addonAPI.id) {
      ret = serviceAPI.invokeExternalEventObject(
            extensionId,
            'extension.onMessageExternal',
            [message, sender, callback]
            );
    } else {
      ret = addonAPI.invokeEventObject(
            'extension.onMessage',
            _instanceID,
            true, //we are skipping _instanceID
            [message, sender, callback]
            );
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
    console.debug("extension.setUpdateUrlData(..) called");
  };

  //============================================================================
  // events

  EventFactory.createEvents(this, instanceID, API_NAME, EVENT_LIST);
  //============================================================================
  //============================================================================
  // main initialization

}

exports.createAPI = function(instanceID) {
  return new Extension(instanceID);
}

exports.releaseAPI = function(instanceID) {
  EventFactory.releaseEvents(instanceID, API_NAME, EVENT_LIST);

  releasePorts(instanceID);
}
