/******************************************************************************
 * windows.js
 * Part of Ancho browser extension framework
 * Implements chrome.windows
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("events.js").Event;
var EventFactory = require("utils.js").EventFactory;

require("windows_spec.js");
var preprocessArguments = require("typeChecking.js").preprocessArguments;
var notImplemented = require("typeChecking.js").notImplemented;
var addonRootURL = require("extension.js").addonRootURL;


var EVENT_LIST = ['onCreated',
                  'onFocusChanged',
                  'onRemoved'];
var API_NAME = 'windows';

exports.WINDOW_ID_NONE = -1;
exports.WINDOW_ID_CURRENT = -2;
//******************************************************************************
//* main closure
var Windows = function(instanceID) {
  //============================================================================
  // private variables


  //============================================================================
  // public properties

  this.WINDOW_ID_NONE = exports.WINDOW_ID_NONE;
  this.WINDOW_ID_CURRENT = exports.WINDOW_ID_CURRENT;

  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.windows.create
  this.create = function(createData, callback) {
    var args = preprocessArguments('chrome.windows.create', arguments);
    serviceAPI.createWindow(args['createData'], Object, args['callback']);
  };

  //----------------------------------------------------------------------------
  // chrome.windows.get
  this.get = function(windowId, getInfo, callback) {
    var args = preprocessArguments('chrome.windows.get', arguments);
    var winId = args['windowId'];
    if (winId === this.WINDOW_ID_CURRENT) {
      winId = serviceAPI.getCurrentWindowId();
    }
    var win = serviceAPI.getWindow(winId, Object, (args['getInfo'] && args['getInfo'].populate));
    args['callback'](win);
  };

  //----------------------------------------------------------------------------
  // chrome.windows.getAll
  this.getAll = function(getInfo, callback) {
    var args = preprocessArguments('chrome.windows.getAll', arguments);
    var windowsSafeArray = serviceAPI.getAllWindows(Object, (args['getInfo'] && args['getInfo'].populate));
    var windows = new VBArray(windowsSafeArray).toArray();
    args['callback'](windows);
  };

  //----------------------------------------------------------------------------
  // chrome.windows.getCurrent
  this.getCurrent = function(getInfo, callback) {
    var args = preprocessArguments('chrome.windows.getCurrent', arguments);
    this.get(this.WINDOW_ID_CURRENT, getInfo, callback);
  };

  //----------------------------------------------------------------------------
  // chrome.windows.getLastFocused
  this.getLastFocused = function(getInfo, callback) {
    var args = notImplemented('chrome.windows.getLastFocused', arguments);
  };

  //----------------------------------------------------------------------------
  // chrome.windows.remove
  this.remove = function(windowId, callback) {
    var args = preprocessArguments('chrome.windows.remove', arguments);
    serviceAPI.closeWindow(args['windowId']);
    if (args['callback']) {
      args['callback']();
    }
  };

  //----------------------------------------------------------------------------
  // chrome.windows.update
  this.update = function(windowId, updateInfo, callback) {
    var args = preprocessArguments('chrome.windows.update', arguments);
    serviceAPI.updateWindow(args.windowId, args.updateInfo);
    if (args.callback) {
      this.get(args.windowId, { populate: false }, args.callback);
    }
  };

  this.test = function() {
    serviceAPI.createPopupWindow(addonRootURL + "popup.html");
  }
  //============================================================================
  // events

  EventFactory.createEvents(this, instanceID, API_NAME, EVENT_LIST);
  //============================================================================
  //============================================================================
  // main initialization

}

exports.createAPI = function(instanceID) {
  return new Windows(instanceID);
}

exports.releaseAPI = function(instanceID) {
  EventFactory.releaseEvents(instanceID, API_NAME, EVENT_LIST);
}