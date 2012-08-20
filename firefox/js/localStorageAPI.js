(function() {
  var Cc = Components.classes;
  var Ci = Components.interfaces;
  var Cu = Components.utils;

  var LocalStorageAPI = function(extensionState, contentWindow) {
    var branch = 'extensions.trusted-ads.';
    var prefs = Cc['@mozilla.org/preferences-service;1']
        .getService(Ci.nsIPrefService).getBranch(branch);
    this.prefs = prefs;
  };

  LocalStorageAPI.prototype.getItem = function(key) {
    if (this.hasValue(key)) {
      return this.prefs.getCharPref(key);
    }
  };
  LocalStorageAPI.prototype.setItem = function(key, value) {
    this.prefs.setCharPref(key, value);
  };
  LocalStorageAPI.prototype.hasValue = function(key) {
    return this.prefs.prefHasUserValue(key);
  };
  LocalStorageAPI.prototype.removeItem = function(key) {
    this.prefs.clearUserPref(key);
  };
  LocalStorageAPI.prototype.getKeys = function() {
    return this.prefs.getChildList('', {});
  };
  LocalStorageAPI.prototype.addLocalStorageReadyFn = function(callback) {
    // IE implementation requires this waiting callback for background.js code.
    // TODO: make it unnecessary.
    callback(this); // We have this.prefs always ready on Firefox.
  };
}).call(this);