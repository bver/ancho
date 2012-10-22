(function() {
  var Cc = Components.classes;
  var Ci = Components.interfaces;
  var Cu = Components.utils;

  var getWindowId = require('./utils').getWindowId;

  const EXTENSION_ID = 'ancho@salsitasoft.com';

  function EventDispatcher() {
    this._listeners = {};
  }

  EventDispatcher.prototype = {
    addListener: function(type, callback) {
      if (!(type in this._listeners)) {
        this._listeners[type] = [];
      }
      this._listeners[type].push(callback);
    },

    removeListener: function(type, callback) {
      if (type in this._listeners) {
        var index = this._listeners[type].indexOf(callback);
        if (index != -1) {
          this._listeners[type].splice(index, 1);
        }
      }
    },

    notifyListeners: function(type, targetTab, params) {
      if (type in this._listeners) {
        for (var i=0; i<this._listeners[type].length; i++) {
          this._listeners[type][i](targetTab, params);
        }
      }
    }
  };

  var ExtensionState = {
    id: EXTENSION_ID,
    backgroundWindow: null,
    eventDispatcher: new EventDispatcher(),
    _unloaders: {},

    registerUnloader: function(win, unloader) {
      var windowId = Utils.getWindowId(win);
      if (!(windowId in this._unloaders)) {
        this._unloaders[windowId] = [];
      }
      var unloaders = this._unloaders[windowId];
      unloaders.push(unloader);
    },

    unloadWindow: function(win) {
      var windowId = Utils.getWindowId(win);
      if (windowId in this._unloaders) {
        this._unloadWindowId(windowId);
      }
    },

    unloadAll: function() {
      for (var windowId in this._unloaders) {
        this._unloadWindowId(windowId);
      }
    },

    _unloadWindowId: function(windowId) {
      var unloaders = this._unloaders[windowId];
      for (var i=0; i<unloaders.length; i++) {
        unloaders[i]();
      }
      delete this._unloaders[windowId];
    }
  };

  module.exports = ExtensionState;
}).call(this);
