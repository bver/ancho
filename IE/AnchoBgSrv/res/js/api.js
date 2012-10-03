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
(function(chrome, instanceID) {
  chrome.bookmarks = require("bookmarks.js").createAPI(instanceID);
  chrome.browserAction = require("browserAction.js").createAPI(instanceID);
  chrome.browsingData = require("browsingData.js");
  chrome.contentSettings = require("contentSettings.js");
  chrome.contextMenus = require("contextMenus.js");
  chrome.cookies = require("cookies.js").createAPI(instanceID);
  chrome.events = require("event.js");
  chrome.extension = require("extension.js").createAPI(instanceID);
  chrome.fileBrowserHandler = require("fileBrowserHandler.js").createAPI(instanceID);
  chrome.history = require("history.js").createAPI(instanceID);
  chrome.i18n = require("i18n.js");
  chrome.idle = require("idle.js").createAPI(instanceID);
  chrome.management = require("management.js").createAPI(instanceID);
  chrome.omnibox = require("omnibox.js").createAPI(instanceID);
  chrome.pageAction = require("pageAction.js").createAPI(instanceID);
  chrome.pageCapture = require("pageCapture.js");
  chrome.permissions = require("permissions.js").createAPI(instanceID);
  chrome.privacy = require("privacy.js");
  chrome.proxy = require("proxy.js").createAPI(instanceID);
  chrome.storage = require("storage.js").createAPI(instanceID);
  chrome.tabs = require("tabs.js").createAPI(instanceID);
  chrome.topSites = require("topSites.js");
  chrome.tts = require("tts.js");
  chrome.ttsEngine = require("ttsEngine.js").createAPI(instanceID);
  chrome.webNavigation = require("webNavigation.js").createAPI(instanceID);
  chrome.webRequest = require("webRequest.js").createAPI(instanceID);
  chrome.webstore = require("webstore.js");
  chrome.windows = require("windows.js").createAPI(instanceID); ;
})(exports.chrome,0)

exports.console = console;
//------------------------------------------------------------------------------
// CONTENT API
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Content API instances
var contentInstances = {};

// Content API constructor
// the content API gets composed here. Decide which methods and objects should
// be part of the content API.
function contentAPI(instanceID) {
  this.extension = require("extension.js").createAPI(instanceID);

  console.debug("Content API created: [" + instanceID + "]");
}

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
    require("extension.js").releaseAPI(instanceID)
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
