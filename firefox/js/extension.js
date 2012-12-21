(function() {

  var Cc = Components.classes;
  var Ci = Components.interfaces;
  var Cu = Components.utils;

  Cu.import('resource://gre/modules/Services.jsm');

  var Utils = require('./utils');
  var Event = require('./event');
  var Config = require('./config');

  function sendHelper(self, tabId, request, callback, event) {
    // tabId is optional
    if ('number' !== typeof(tabId)) {
      callback = request;
      request = tabId;
      tabId = null;
    }
    var sender = Utils.getSender(self._state['id'], self._tab);
    event.fire([ request, sender, callback ], tabId);
  }

  function ExtensionAPI(state, window) {
    this._state = state;
    this._tab = Utils.getWindowId(window);
    // Event handlers
    this.onRequest = new Event(window, this._tab, this._state, 'extension.request');
    this.onMessage = new Event(window, this._tab, this._state, 'extension.message');
  }

  ExtensionAPI.prototype = {

    sendRequest: function(tabId, request, callback) {
      sendHelper(this, tabId, request, callback, this.onRequest);
    },

    sendMessage: function(tabId, request, callback) {
      sendHelper(this, tabId, request, callback, this.onMessage);
    },

    getURL: function(path) {
      // RequireJS uses extension.getUrl() in require.load(),
      // we need to strip the /content/chrome-ext prefix here.
      if (0 === path.indexOf(Config.hostExtensionPath)) {
        path = path.substr(Config.hostExtensionPath.length);
      }
      // Treat all paths as relative so they are resolved from the root URI.
      if (path[0] === '/') {
        path = path.substr(1);
      }
      var baseURI = Services.io.newURI(Config.hostExtensionRoot, '', null);
      var uri = Services.io.newURI(path, '', baseURI);
      return uri.spec;
    }
  };

  module.exports = ExtensionAPI;

}).call(this);
