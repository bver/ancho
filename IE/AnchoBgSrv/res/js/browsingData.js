/******************************************************************************
 * browsingData.js
 * Part of Ancho browser extension framework
 * Implements chrome.browsingData
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* main closure
(function(me){
  //============================================================================
  // private variables


  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.browsingData.remove
  me.remove = function(options, dataToRemove, callback) {
    console.debug("browsingData.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browsingData.removeAppcache
  me.removeAppcache = function(options, callback) {
    console.debug("browsingData.removeAppcache(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browsingData.removeCache
  me.removeCache = function(options, callback) {
    console.debug("browsingData.removeCache(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browsingData.removeCookies
  me.removeCookies = function(options, callback) {
    console.debug("browsingData.removeCookies(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browsingData.removeDownloads
  me.removeDownloads = function(options, callback) {
    console.debug("browsingData.removeDownloads(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browsingData.removeFileSystems
  me.removeFileSystems = function(options, callback) {
    console.debug("browsingData.removeFileSystems(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browsingData.removeFormData
  me.removeFormData = function(options, callback) {
    console.debug("browsingData.removeFormData(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browsingData.removeHistory
  me.removeHistory = function(options, callback) {
    console.debug("browsingData.removeHistory(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browsingData.removeIndexedDB
  me.removeIndexedDB = function(options, callback) {
    console.debug("browsingData.removeIndexedDB(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browsingData.removeLocalStorage
  me.removeLocalStorage = function(options, callback) {
    console.debug("browsingData.removeLocalStorage(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browsingData.removePasswords
  me.removePasswords = function(options, callback) {
    console.debug("browsingData.removePasswords(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browsingData.removePluginData
  me.removePluginData = function(options, callback) {
    console.debug("browsingData.removePluginData(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.browsingData.removeWebSQL
  me.removeWebSQL = function(options, callback) {
    console.debug("browsingData.removeWebSQL(..) called");
  };


  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);

exports.createAPI = function(instanceID) {
  //We don't need special instances
  return exports;
}

exports.releaseAPI = function(instanceID) {
  //Nothing needs to be released
}