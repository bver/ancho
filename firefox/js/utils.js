(function() {
  var Cc = Components.classes;
  var Ci = Components.interfaces;
  var Cu = Components.utils;

  Cu.import('resource://gre/modules/Services.jsm');

  exports.getWindowId = function(window) {
    return window.QueryInterface(Ci.nsIInterfaceRequestor).
      getInterface(Ci.nsIDOMWindowUtils).outerWindowID;
  };

  exports.getWindowForRequest = function(request) {
    // First get the load context
    var loadContext = null;
    try {
      // Try the notification callbacks
      loadContext = request.QueryInterface(Ci.nsIChannel).
        notificationCallbacks.getInterface(Ci.nsILoadContext);
    } catch (ex) {
      // Then try the load group
      try {
        if (request.loadGroup) {
          loadContext = request.loadGroup.notificationCallbacks.getInterface(Ci.nsILoadContext);
        }
      } catch(ex) {
      }
    }

    if (loadContext) {
      return loadContext.associatedWindow;
    }
    else {
      return null;
    }
  };

  exports.readStringFromUrl = function(url) {
    var channel = Services.io.newChannelFromURI(url);
    var channelInputStream = channel.open();

    // Get an intl-aware nsIConverterInputStream for the file
    var is = Cc['@mozilla.org/intl/converter-input-stream;1'].createInstance(Ci.nsIConverterInputStream);
    is.init(channelInputStream, 'UTF-8', 1024, Ci.nsIConverterInputStream.DEFAULT_REPLACEMENT_CHARACTER);

    // Read the file into string via buffer
    var data = '';
    var buffer = {};
    while (is.readString(4096, buffer) != 0) {
      data += buffer.value;
    }

    // Clean up
    is.close();
    channelInputStream.close();

    return data;
  };

  exports.getSender = function(extensionId, tabId) {
    return { id: extensionId, tab: { id: tabId } };
  };

  exports.getLoadContext = function(aRequest) {
    var loadContext = null;
    try {
      // first try the notification callbacks
      loadContext = aRequest
        .QueryInterface(Ci.nsIChannel)
        .notificationCallbacks
        .getInterface(Ci.nsILoadContext);
      return loadContext;
    } catch (ex) {
      // fail over to trying the load group
      try {
        if (!aRequest.loadGroup) {
          return null;
        }
        loadContext = aRequest
          .loadGroup
          .notificationCallbacks
          .getInterface(Ci.nsILoadContext);
        return loadContext;
      } catch(ex) {
        return null;
      }
    }
  };

  exports.removeFragment = function(str) {
    var pos = str.indexOf('#');
    if (pos != -1) {
      return str.substr(0, pos);
    } else {
      return str;
    }
  };

}).call(this);
