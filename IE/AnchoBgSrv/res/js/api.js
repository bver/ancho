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
 *  serviceAPI    :   The central service managing all addons (IAnchoServiceApi)
 ******************************************************************************/

// get manifest
var manifest = require("manifest").manifest;

console.info("Loading extension [" + addonAPI.id + "] [" + addonAPI.guid + "]");

//------------------------------------------------------------------------------
// BACKGROUND API
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
exports.chrome = {};

var API_NAMES = ["bookmarks",
    "browserAction",
    "browsingData",
    "contentSettings",
    "contextMenus",
    "cookies",
    "events",
    "extension",
    "fileBrowserHandler",
    "history",
    "i18n",
    "idle",
    "management",
    "omnibox",
    "pageAction",
    "pageCapture",
    "permissions",
    "privacy",
    "proxy",
    "storage",
    "tabs",
    "topSites",
    "tts",
    "ttsEngine",
    "webNavigation",
    "webRequest",
    "webstore",
    "windows"];

var CONTENT_API_NAMES = [
    "extension",
    "i18n"];


function getChromeAPISubset(chrome, aInstanceID, aAPINames) {
  for (var i = 0; i < aAPINames.length; ++i) {
    console.debug("Creating chrome." + aAPINames[i] + " API instance n. " + aInstanceID);
    chrome[aAPINames[i]] = require(aAPINames[i] + ".js").createAPI(aInstanceID);
  }
}

function releaseAPISubsetByName(aInstanceID, aAPINames) {
  for (var i = 0; i < aAPINames.length; ++i) {
    console.debug("Releasing chrome." + aAPINames[i] + " API instance n. " + aInstanceID);
    require(aAPINames[i] + ".js").releaseAPI(aInstanceID);
  }
}

function releaseAPISubset(aInstanceID, aAPIInstance) {
  for (var i in aAPIInstance) {
    try {
      console.debug("Releasing chrome." + i + " API instance n. " + aInstanceID);
      require(i + ".js").releaseAPI(aInstanceID);
    } catch (e) {
      console.error("Releasing of chrome." + i + " API instance n. " + aInstanceID + " failed! " + e.description);
    }
  }
}

// create and initialize the background API
function getFullAPI(chrome, aInstanceID) {
  getChromeAPISubset(chrome, aInstanceID, API_NAMES);
}

function releaseFullAPIInstance(aInstanceID) {
  releaseAPISubsetByName(aInstanceID, API_NAMES);
}

function fullAPI(aInstanceID) {
  getChromeAPISubset(this, aInstanceID, API_NAMES);
  this.console = console;// TODO: remove
  console.debug("Full API created: [" + aInstanceID + "]");
}

// exports.chrome will be available to background pages as 'chrome'.
// This name is defined in anchocommons/strings.cpp
getFullAPI(exports.chrome, 0);
exports.console = console;

//***************************************************
//*********** MANAGING CHROME API INSTANCES ********

// ------------- INSTANCE IDs ----------------------
//    0         - background script
//    >0        - content scripts - equals tabId
//    [-20..-1] - reserved for errors
//    <-20      - other full APIs (popups)
// ------------- INSTANCE IDs ----------------------

var fullAPINextID = -21;


var fullAPIInstances = {};

exports.reserveFullAPIInstanceID = function() {
  return fullAPINextID--;
}

exports.createFullAPI = function(aInstanceID) {
  var chromeAPI = {};
  getFullAPI(chromeAPI, aInstanceID);
  fullAPIInstances[aInstanceID] = chromeAPI;
  return chromeAPI;
};

exports.releaseFullAPI = function(aInstanceID) {
  if (fullAPIInstances[aInstanceID]) {
    releaseFullAPIInstance(aInstanceID);
    delete fullAPIInstances[aInstanceID]
  }
};

var browserAction = require("browserAction.js");
browserAction.initBrowserAction(manifest.browser_action);


//------------------------------------------------------------------------------
// CONTENT API
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Content API instances
var contentInstances = {};

// Content API constructor
// the content API gets composed here. Decide which methods and objects should
// be part of the content API.
function restrictedAPI(aInstanceID) {
  getChromeAPISubset(this, aInstanceID, CONTENT_API_NAMES);
  this.console = console;// TODO: remove
  console.debug("Restricted API created: [" + aInstanceID + "]");
}

//------------------------------------------------------------------------------
// INTERNAL API
//------------------------------------------------------------------------------

function isExtensionPage(aUrl) {
  return typeof (aUrl) == 'string' && aUrl.match('chrome-extension://');
}

// Creates a new instance of the content API for a certain tab.
// The name of this method must match the one defined in anchocommons/strings.cpp
// Called from the addon when a new browser window or tab opens
exports.getContentInfo = function(aInstanceID, aUrl) {
  console.debug("getContentAPI for: [" + aInstanceID + "]");

  if (!contentInstances[aInstanceID]) {
    if (aUrl && isExtensionPage(aUrl)) {
      contentInstances[aInstanceID] = new fullAPI(aInstanceID);
    } else {
      contentInstances[aInstanceID] = new restrictedAPI(aInstanceID);
    }
  }
  var api = contentInstances[aInstanceID];
  var scripts = (
    manifest.content_scripts instanceof Array
    && manifest.content_scripts.length > 0
    && manifest.content_scripts[0].js instanceof Array
    ? manifest.content_scripts[0].js
    : []
  );
  return { api: api, scripts: scripts, console: console };
};

// Releases an instance of the content API for a certain tab.
// The name of this method must match the one defined in anchocommons/strings.cpp
// Called from the addon when a browser window or tab closes
exports.releaseContentInfo = function(instanceID) {
  console.debug("Content API release requested for [" + instanceID + "]");
  if (contentInstances[instanceID]) {
    releaseAPISubset(instanceID, contentInstances[instanceID]);
    //require("extension.js").releaseAPI(instanceID);
    //require("i18n.js").releaseAPI(instanceID);
    delete contentInstances[instanceID];
    console.debug("Content API FOUND and released: [" + instanceID + "]");
  }
};

function invokeEvent(aEventName, aIDispatchData) {
  require("cookies.js").invokeEventWithIDispatch(aEventName, aIDispatchData);
}
addonAPI.setIDispatchEventInvocationHandler(invokeEvent);

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
