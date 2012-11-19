(function() {

/*
   Client portion of WebRequest API that provides access to events and methods.
   The actual implementation runs in the background window of the extension and
   fires the events. For the implementation, please see webRequestSingleton.js.
*/


  var Cc = Components.classes;
  var Ci = Components.interfaces;
  var Cu = Components.utils;

  Cu.import('resource://gre/modules/Services.jsm');

  var Event = require('./event');
  var Utils = require('./utils');

  var WebRequestAPI = function(state, window) {
    this._state = state;
    this._tab = Utils.getWindowId(window);

    this.onCompleted = new Event(window, this._tab, this._state, 'webRequest.completed');
    this.onHeadersReceived = new Event(window, this._tab, this._state, 'webRequest.headersReceived');
    this.onBeforeRedirect = new Event(window, this._tab, this._state, 'webRequest.beforeRedirect');
    this.onAuthRequired = new Event(window, this._tab, this._state, 'webRequest.authRequired');
    this.onBeforeSendHeaders = new Event(window, this._tab, this._state, 'webRequest.beforeSendHeaders');
    this.onErrorOccurred = new Event(window, this._tab, this._state, 'webRequest.errorOccurred');
    this.onResponseStarted = new Event(window, this._tab, this._state, 'webRequest.responseStarted');
    this.onSendHeaders = new Event(window, this._tab, this._state, 'webRequest.sendHeaders');
    this.onBeforeRequest = new Event(window, this._tab, this._state, 'webRequest.beforeRequest');
  };

  WebRequestAPI.prototype.handlerBehaviorChanged = function(callback) {
    // noop
    if (callback) {
      callback();
    }
  };

  module.exports = WebRequestAPI;

}).call(this);
