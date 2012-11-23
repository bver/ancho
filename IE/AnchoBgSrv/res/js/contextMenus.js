/******************************************************************************
 * contextMenus.js
 * Part of Ancho browser extension framework
 * Implements chrome.contextMenus
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
  // chrome.contextMenus.create
  //   returns   integer
  me.create = function(createProperties, callback) {
    console.debug("contextMenus.create(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.contextMenus.remove
  me.remove = function(menuItemId, callback) {
    console.debug("contextMenus.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.contextMenus.removeAll
  me.removeAll = function(callback) {
    console.debug("contextMenus.removeAll(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.contextMenus.update
  me.update = function(id, updateProperties, callback) {
    console.debug("contextMenus.update(..) called");
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