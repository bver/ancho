/******************************************************************************
 * Event.js
 * Part of Aji browser extension framework
 * Implements aji.windows
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* class Event
(function(me) {

  me.Event = function() {
    var _listeners = [];
    function _findListener(listener) {
      for (var i = 0; i < _listeners.length; i++) {
        if (_listeners[i] === listener) {
          return i;
        }
      }
      return -1;
    }

    me.addListener = function(callback) {
      var i = _findListener(callback);
      if (-1 != i) {
        return;
      }
      _listeners.push(callback);
    };

    me.removeListener = function(callback) {
      var i = _findListener(callback);
      if (-1 != i) {
        _listeners.splice(i, 1);
      }
    };

    me.hasListener = function(callback) {
      return _findListener(callback) != -1;
    }

    me.hasListeners = function() {
      return _listeners.length > 0;
    }
  };

}).call(this, exports);