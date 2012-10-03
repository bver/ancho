/******************************************************************************
 * management.js
 * Part of Ancho browser extension framework
 * Implements aji.management
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/
  
//******************************************************************************
//* requires
var Event = require("Event.js").Event;
  
//******************************************************************************
//* main closure
exports.createAPI = function(instanceID) {
  return new (function() {
  //============================================================================
  // private variables
  

  //============================================================================
  // public methods
    
  //----------------------------------------------------------------------------
  // aji.management.get
  this.get = function(id, callback) {
    console.debug("management.get(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.management.getAll
  this.getAll = function(callback) {
    console.debug("management.getAll(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.management.getPermissionWarningsById
  this.getPermissionWarningsById = function(id, callback) {
    console.debug("management.getPermissionWarningsById(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.management.getPermissionWarningsByManifest
  this.getPermissionWarningsByManifest = function(manifestStr, callback) {
    console.debug("management.getPermissionWarningsByManifest(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.management.launchApp
  this.launchApp = function(id, callback) {
    console.debug("management.launchApp(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.management.setEnabled
  this.setEnabled = function(id, enabled, callback) {
    console.debug("management.setEnabled(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.management.uninstall
  this.uninstall = function(id, callback) {
    console.debug("management.uninstall(..) called");
  };

  //============================================================================
  // events

  this.onDisabled = new Event('management.onDisabled', instanceID);
  this.onEnabled = new Event('management.onEnabled', instanceID);
  this.onInstalled = new Event('management.onInstalled', instanceID);
  this.onUninstalled = new Event('management.onUninstalled', instanceID);

  //============================================================================
  //============================================================================
  // main initialization
  })();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('management.onDisabled', instanceID);
  addonAPI.removeEventObject('management.onEnabled', instanceID);
  addonAPI.removeEventObject('management.onInstalled', instanceID);
  addonAPI.removeEventObject('management.onUninstalled', instanceID);
}