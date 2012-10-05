/******************************************************************************
 * Event.js
 * Part of Ancho browser extension framework
 * Implements chrome.events
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* class Event
(function(me) {

  me.Event = function(eventName, instanceID) {
    var _listeners = [];
    var _eventName = eventName;
    var _instanceID = instanceID;
    console.debug('Created new event: ' + eventName + ' [' + instanceID + ']');

    var self = this;
    addonAPI.addEventObject(eventName, instanceID, function() {
      _fire.apply(self, arguments);
    });

    function _fire() {
      for (var i = 0; i < _listeners.length; ++i) {
        _listeners[i].apply(_listeners[i], arguments);
      }
    }

    function _findListener(listener) {
      for (var i = 0; i < _listeners.length; ++i) {
        if (_listeners[i] === listener) {
          return i;
        }
      }
      return -1;
    }

    this.addListener = function(callback) {
      var i = _findListener(callback);
      if (-1 != i) {
        return;
      }
      _listeners.push(callback);
    };

    this.removeListener = function(callback) {
      var i = _findListener(callback);
      if (-1 != i) {
        _listeners.splice(i, 1);
      }
    };

    this.hasListener = function(callback) {
      return _findListener(callback) != -1;
    }

    this.hasListeners = function() {
      return _listeners.length > 0;
    }
  };

}).call(this, exports);