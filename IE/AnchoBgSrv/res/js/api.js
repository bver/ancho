/******************************************************************************
 * Main API script.
 *  This script creates the Ancho API for both the background and the content
 *  scripts.
 *
 *  Before this script gets executed the extension's manifest gets loaded as
 *  module 'manifest'.
 *
 * Global objects available:
 *  addonAPI      :   The addon object (IAnchoBackgroundAPI)
 ******************************************************************************/

// get manifest
var manifest = require("manifest").manifest;

console.info("Loading extension [" + addonAPI.id + "] [" + addonAPI.guid + "]");

//------------------------------------------------------------------------------
// BACKGROUND API
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// exports.chrome will be available to background pages as 'chrome'.
// This name is defined in staticstrings.h / .cpp
exports.chrome = {extension: {}};

// create and initialize the background API
// probably all the certain parts of the API should go to modules
(function(ext){

  // SAMPLE: onMessage implementation
  ext.onMessage = require("onMessageAPI.js");

})(exports.chrome.extension)

//------------------------------------------------------------------------------
// CONTENT API
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Content API instances
var contentInstances = {};

// Content API constructor
function contentAPI(instanceID) {
  console.debug("Content API created: [" + instanceID + "]");
}
// the content API gets composed here. Decide which methods and objects should
// be part of the content API.
contentAPI.prototype.doSomethingUseful = function(msg) {
  console.log("Something useful: " + msg);
}

//------------------------------------------------------------------------------
// INTERNAL API
//------------------------------------------------------------------------------

// Creates a new instance of the content API for a certain tab.
// The name of this method must match the one defined in staticstrings.h / .cpp
// Called from the addon when a new browser window or tab opens
exports.getContentAPI = function(instanceID) {
  console.debug("getContentAPI for: [" + instanceID + "]");
  return (contentInstances[instanceID])
    ? contentInstances[instanceID]
    : contentInstances[instanceID] = new contentAPI(instanceID);
};

// Releases an instance of the content API for a certain tab.
// The name of this method must match the one defined in staticstrings.h / .cpp
// Called from the addon when a browser window or tab closes
exports.releaseContentAPI = function(instanceID) {
  console.debug("Content API release requested for [" + instanceID + "]");
  if (contentInstances[instanceID]) {
    delete contentInstances[instanceID];
    console.debug("Content API FOUND and released: [" + instanceID + "]");
  }
};

//------------------------------------------------------------------------------
// MAIN
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// do internal initialization here

// look for a background window and open it
if (manifest.background) {
  if (manifest.background.page) {
    addonAPI.startBackgroundWindow(manifest.background.page);
  }
  else {
    // TODO: implement non-HTML background window (scripts only)
  }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// just stuff we might want to use below here, no actual functionality
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------







//------------------------------------------------------------------------------
// ancho.extension methods
function sendMessage(
    /*string, optional*/    extensionId,
    /*any*/                 message,
    /*function, optional*/  responseCallback) {
  // TODO: implement
  console.log("sendMessage called");
}

function connect(
    /*string, optional*/  extensionId,
    /*object, optional*/  connectInfo) {
  // TODO: implement
  console.log("connect called");
}

function getURL(
    /*string*/  path) {
  // TODO: implement
  console.log("getURL called");
}

// event sources
var onMessage = {};
onMessage.addListener = function(
    /*function*/  callback) {
  // TODO: implement
  console.log("onMessage.addListener called");
}

var onConnect = {};
onConnect.addListener = function(
    /*function*/  callback) {
  // TODO: implement
  console.log("onConnect.addListener called");
}

//------------------------------------------------------------------------------
// compose the APIs
// content part of ancho.extension
var anchoContentAPI = {};
anchoContentAPI.extension = {
  sendMessage: sendMessage,
  connect: connect,
  getURL: getURL,
  onMessage: onMessage,
  onConnect: onConnect
};

// background part of ancho.extension
anchoContentAPI.extension = {
  sendMessage: sendMessage,
  connect: connect,
  getURL: getURL,
  onMessage: onMessage,
  onConnect: onConnect
};
