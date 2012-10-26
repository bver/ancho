(function() {

  function Event(window, tabId, state, type) {
    var listeners = [];

    window.addEventListener('unload', function(event) {
      window.removeEventListener('unload', arguments.callee, false);
      state.eventDispatcher.removeListener(type, notifyListeners);
    }, false);

    function notifyListeners(targetTab, params) {
      if (!targetTab || (targetTab == tabId)) {
        for (var i = 0; i < listeners.length; i++) {
          listeners[i].apply(this, params);
        }
      }
    }

    state.eventDispatcher.addListener(type, notifyListeners);

    this.addListener = function(listener) {
      listeners.push(listener);
    };

    this.removeListener = function(listener) {
      var index = listeners.indexOf(listener);
      if (index != -1) {
        listeners.splice(index);
      }
    };

  }

  module.exports = Event;

}).call(this);
