(function() {

  function Event(window, tabId, state, type) {
    var listeners = [];

    window.addEventListener('unload', function(event) {
      window.removeEventListener('unload', arguments.callee, false);
      state.eventDispatcher.removeListener(type, notifyListeners);
    }, false);

    function notifyListeners(targetTab, params) {
      var results = [];
      if (!targetTab || (targetTab === tabId)) {
        for (var i = 0; i < listeners.length; i++) {
          results.push(listeners[i].apply(this, params));
        }
      }
      return results;
    }

    this.addListener = function(listener) {
      listeners.push(listener);
      // register local notification function only if we have listeners
      if (1 === listeners.length) {
        state.eventDispatcher.addListener(type, notifyListeners);
      }
    };

    this.removeListener = function(listener) {
      var index = listeners.indexOf(listener);
      if (index != -1) {
        listeners.splice(index);
      }
      // last local listener was removed:
      // no need for central dispatcher to notify us
      if (0 === listeners.length) {
        state.eventDispatcher.removeListener(type, notifyListeners);
      }
    };

    this.hasListeners = function() {
      return state.eventDispatcher.hasListeners(type);
    };

    // convenience function:
    // if `id` is provided, make sure `params` is provided, too
    this.fire = function(params, id) {
      params = params || [];
      return state.eventDispatcher.notifyListeners(type, id, params);
    };

  }

  module.exports = Event;

}).call(this);
