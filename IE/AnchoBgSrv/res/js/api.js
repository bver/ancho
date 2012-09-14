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
// This name is defined in anchocommons/strings.cpp
exports.chrome = {};

// create and initialize the background API
(function(chrome){
  chrome.bookmarks = require("bookmarks.js");
  chrome.browserAction = require("browserAction.js");
  chrome.browsingData = require("browsingData.js");
  chrome.contentSettings = require("contentSettings.js");
  chrome.contextMenus = require("contextMenus.js");
  chrome.cookies = require("cookies.js");
  chrome.extension = require("extension.js");
  chrome.fileBrowserHandler = require("fileBrowserHandler.js");
  chrome.history = require("history.js");
  chrome.i18n = require("i18n.js");
  chrome.idle = require("idle.js");
  chrome.management = require("management.js");
  chrome.omnibox = require("omnibox.js");
  chrome.pageAction = require("pageAction.js");
  chrome.pageCapture = require("pageCapture.js");
  chrome.permissions = require("permissions.js");
  chrome.privacy = require("privacy.js");
  chrome.proxy = require("proxy.js");
  chrome.storage = require("storage.js");
  chrome.tabs = require("tabs.js");
  chrome.topSites = require("topSites.js");
  chrome.tts = require("tts.js");
  chrome.ttsEngine = require("ttsEngine.js");
  chrome.webNavigation = require("webNavigation.js");
  chrome.webRequest = require("webRequest.js");
  chrome.webstore = require("webstore.js");
  chrome.windows = require("windows.js");
})(exports.chrome)

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
contentAPI.prototype.extension = exports.chrome.extension;

//------------------------------------------------------------------------------
// INTERNAL API
//------------------------------------------------------------------------------

// Creates a new instance of the content API for a certain tab.
// The name of this method must match the one defined in anchocommons/strings.cpp
// Called from the addon when a new browser window or tab opens
exports.getContentAPI = function(instanceID) {
  console.debug("getContentAPI for: [" + instanceID + "]");
  return (contentInstances[instanceID])
    ? contentInstances[instanceID]
    : contentInstances[instanceID] = new contentAPI(instanceID);
};

// Releases an instance of the content API for a certain tab.
// The name of this method must match the one defined in anchocommons/strings.cpp
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
