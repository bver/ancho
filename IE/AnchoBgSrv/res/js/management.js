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
(function(me){
  //============================================================================
  // private variables
  

  //============================================================================
  // public methods
    
  //----------------------------------------------------------------------------
  // aji.management.get
  me.get = function(id, callback) {
    console.debug("management.get(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.management.getAll
  me.getAll = function(callback) {
    console.debug("management.getAll(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.management.getPermissionWarningsById
  me.getPermissionWarningsById = function(id, callback) {
    console.debug("management.getPermissionWarningsById(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.management.getPermissionWarningsByManifest
  me.getPermissionWarningsByManifest = function(manifestStr, callback) {
    console.debug("management.getPermissionWarningsByManifest(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.management.launchApp
  me.launchApp = function(id, callback) {
    console.debug("management.launchApp(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.management.setEnabled
  me.setEnabled = function(id, enabled, callback) {
    console.debug("management.setEnabled(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.management.uninstall
  me.uninstall = function(id, callback) {
    console.debug("management.uninstall(..) called");
  };

  //============================================================================
  // events
    
  me.onDisabled = new Event();
  me.onEnabled = new Event();
  me.onInstalled = new Event();
  me.onUninstalled = new Event();

  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);
