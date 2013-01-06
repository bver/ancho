(function() {
  var Cc = Components.classes;
  var Ci = Components.interfaces;
  var Cu = Components.utils;

  var getWindowId = require('./utils').getWindowId;
  var WebRequestSingleton = require('./webRequestSingleton');

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

    hasListeners: function(type) {
      return (type in this._listeners);
    },

    notifyListeners: function(type, targetTab, params) {
      var res, results = [];
      if (type in this._listeners) {
        for (var i = 0; i < this._listeners[type].length; i++) {
          res = this._listeners[type][i](targetTab, params);
          results = results.concat(res);
        }
      }
      return results;
    }
  };

  function GlobalId() {
    this._id = 1;
  }

  GlobalId.prototype.getNext = function() {
    return this._id++;
  }

  var ExtensionState = {
    id: EXTENSION_ID,
    backgroundWindow: null,
    eventDispatcher: new EventDispatcher(),
    _unloaders: {},
    _globalIds: {},
    _tabIds: [],

    registerUnloader: function(win, unloader) {
      var windowId = getWindowId(win);
      if (!(windowId in this._unloaders)) {
        this._unloaders[windowId] = [];
      }
      var unloaders = this._unloaders[windowId];
      unloaders.push(unloader);
    },

    unloadWindow: function(win) {
      var windowId = getWindowId(win);
      if (windowId in this._unloaders) {
        this._unloadWindowId(windowId);
      }
    },

    unloadAll: function() {
      for (var windowId in this._unloaders) {
        this._unloadWindowId(windowId);
      }
    },

    getGlobalId: function(name) {
      if (!this._globalIds[name]) {
        this._globalIds[name] = new GlobalId();
      }
      return this._globalIds[name].getNext();
    },

    _unloadWindowId: function(windowId) {
      var unloaders = this._unloaders[windowId];
      for (var i=0; i<unloaders.length; i++) {
        unloaders[i]();
      }
      delete this._unloaders[windowId];
    },

    startSingletonAPIs: function(window) {
      this.backgroundWindow = window;
      new WebRequestSingleton(this, window);
    },

    registerTab: function(tabId) {
      this._tabIds.push(tabId);
    },

    tabIds: function() {
      return this._tabIds.slice(0);
    }

  };

  module.exports = ExtensionState;

}).call(this);
