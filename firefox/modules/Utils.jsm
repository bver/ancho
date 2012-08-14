EXPORTED_SYMBOLS = ["Utils"];

const { classes: Cc, interfaces: Ci, utils: Cu } = Components;

Cu.import('resource://gre/modules/Services.jsm');

// Get the unique ID of the provided window.
var Utils = {};

Utils.getWindowId = function(window) {
  return window.QueryInterface(Ci.nsIInterfaceRequestor).
    getInterface(Ci.nsIDOMWindowUtils).outerWindowID;
};

Utils.getWindowForRequest = function(request) {
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

Utils.readStringFromUrl = function(url) {
  var channel = Services.io.newChannelFromURI(url);
  var channelInputStream = channel.open();

  // Get an intl-aware nsIConverterInputStream for the file
  var is = Cc["@mozilla.org/intl/converter-input-stream;1"].createInstance(Ci.nsIConverterInputStream);
  is.init(channelInputStream, "UTF-8", 1024, Ci.nsIConverterInputStream.DEFAULT_REPLACEMENT_CHARACTER);

  // Read the file into string via buffer
  var data = "";
  var buffer = {};
  while (is.readString(4096, buffer) != 0) {
    data += buffer.value;
  }

  // Clean up
  is.close();
  channelInputStream.close();

  return data;
};
