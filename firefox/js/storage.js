(function() {

  var Cc = Components.classes;
  var Ci = Components.interfaces;

  var StorageAPI = function(extensionState, contentWindow, prefix) {
    this.pref = Cc['@mozilla.org/preferences-service;1']
                .getService(Ci.nsIPrefService)
                .getBranch(prefix);
  };

  StorageAPI.prototype = {

    get: function(keys, callback) {
      if (keys) {
        var myKeys = [];
        var defaults = {};

        if (Array.isArray(keys)) {
          myKeys = keys;
        } else if (typeof keys == 'string') {
          myKeys = [keys];
        } else if (typeof keys == 'object') {
          defaults = keys;
          for (var key in keys) {
            myKeys.push(key);
          }
        }

        var key, result = {};
        for (var i=0; i<myKeys.length; i++) {
          key = myKeys[i];
          if (this.pref.prefHasUserValue(key)) {
            result[key] = JSON.parse(this.pref.getCharPref(key));
          } else if (defaults[key]) {
            result[key] = defaults[key];
          }
        }

        if (typeof callback === 'function') {
          callback(result);
        }
      }
    },

    set: function(items, callback) {
      if (typeof items == 'object') {
        for (var key in items) {
          this.pref.setCharPref(key, JSON.stringify(items[key]));
        }
        if (typeof callback === 'function') {
          callback();
        }
      }
    }
  }

  module.exports = StorageAPI;

}).call(this);

