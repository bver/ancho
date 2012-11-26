/******************************************************************************
 * events.js
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
    //console.debug('Created new event: ' + eventName + ' [' + instanceID + ']');
    var self = this;

    function _findListener(listener) {
      for (var i = 0; i < _listeners.length; ++i) {
        if (_listeners[i] === listener) {
          return i;
        }
      }
      return -1;
    }

    this.fire = function() {
      //console.debug('Fire event ' + _eventName + ' - ' + _listeners.length + ' instance:' + _instanceID);
      var results = [];
      for (var i = 0; i < _listeners.length; ++i) {
        //We cannot use apply - doesn't work for arrays from different script engines
        //http://stackoverflow.com/questions/7688070/why-is-comparing-the-constructor-property-of-two-windows-unreliable
        var ret = addonAPI.callFunction(_listeners[i], arguments);
        if (ret != undefined) {
          results.push(ret);
        }
      }
      if (results.length > 0) {
        return results;
      }
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

    if (instanceID != undefined) { //register only events with assigned instanceID
      addonAPI.addEventObject(eventName, instanceID, function() {
        return self.fire.apply(self, arguments);
      });
    }
  };

}).call(this, exports);

exports.createAPI = function(instanceID) {
  //We don't need special instances
  return exports;
}

exports.releaseAPI = function(instanceID) {
  //Nothing needs to be released
}