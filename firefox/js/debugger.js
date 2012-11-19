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
  }

  DebuggerAPI.prototype = {
  };

  module.exports = DebuggerAPI;

}).call(this);
