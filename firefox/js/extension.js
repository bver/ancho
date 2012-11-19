(function() {

  var Cc = Components.classes;
  var Ci = Components.interfaces;
  var Cu = Components.utils;

  Cu.import('resource://gre/modules/Services.jsm');

  var Utils = require('./utils');
  var Event = require('./event');

  function ExtensionAPI(state, window) {
    this._state = state;
    this._tab = Utils.getWindowId(window);
    // Event handlers
    this.onRequest = new Event(window, this._tab, this._state, 'extension.request');
  }

  ExtensionAPI.prototype = {

    sendRequest: function(tabId, request, callback) {
      // tabId is optional
      if ('number' !== typeof(tabId)) {
        callback = request;
        request = tabId;
        tabId = null;
      }
      var sender = Utils.getSender(this._state['id'], this._tab);
      this.onRequest.fire([ request, sender, callback ], tabId);
    },

    getURL: function(path) {
      // RequireJS uses extension.getUrl() in require.load(),
      // we need to strip the /content/chrome-ext prefix here.
      var stripPrefix = '/content/chrome-ext/';
      if (0 === path.indexOf(stripPrefix)) {
        path = path.substr(stripPrefix.length);
      }
      // Treat all paths as relative so they are resolved from the root URI.
      if (path[0] === '/') {
        path = path.substr(1);
      }
      var baseURI = Services.io.newURI('chrome://ancho/content/chrome-ext/', '', null);
      var uri = Services.io.newURI(path, '', baseURI);
      return uri.spec;
    }
  };

  module.exports = ExtensionAPI;

}).call(this);
