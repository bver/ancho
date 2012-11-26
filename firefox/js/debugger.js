(function() {

  // TODO: implement me!

  var Cc = Components.classes;
  var Ci = Components.interfaces;
  var Cu = Components.utils;

  Cu.import('resource://gre/modules/Services.jsm');

  var Utils = require('./utils');
  var Event = require('./event');

  function DebuggerAPI(state, window) {
    this._state = state;
    this._tab = Utils.getWindowId(window);
    // Event handlers
    this.onEvent = new Event(window, this._tab, this._state, 'debugger.event');
    this.onDetach = new Event(window, this._tab, this._state, 'debugger.detach');
  }

  DebuggerAPI.prototype = {
    attach: function(target, requiredVersion, /* optional */ callback) {
    },

    detach: function(target, /* optional */ callback) {
    },

    sendCommand: function(target, method, /* optional */ commandParams,
      /* optional */ callback) {
    }
  };

  module.exports = DebuggerAPI;

}).call(this);
