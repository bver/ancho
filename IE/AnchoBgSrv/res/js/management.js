/******************************************************************************
 * management.js
 * Part of Ancho browser extension framework
 * Implements chrome.management
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("events.js").Event;
var EventFactory = require("utils.js").EventFactory;

var EVENT_LIST = ['onDisabled',
                  'onEnabled',
                  'onInstalled',
                  'onUninstalled'];
var API_NAME = 'management';
//******************************************************************************
//* main closure
var Management = function(instanceID) {
  //============================================================================
  // private variables


  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.management.get
  this.get = function(id, callback) {
    console.debug("management.get(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.management.getAll
  this.getAll = function(callback) {
    console.debug("management.getAll(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.management.getPermissionWarningsById
  this.getPermissionWarningsById = function(id, callback) {
    console.debug("management.getPermissionWarningsById(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.management.getPermissionWarningsByManifest
  this.getPermissionWarningsByManifest = function(manifestStr, callback) {
    console.debug("management.getPermissionWarningsByManifest(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.management.launchApp
  this.launchApp = function(id, callback) {
    console.debug("management.launchApp(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.management.setEnabled
  this.setEnabled = function(id, enabled, callback) {
    console.debug("management.setEnabled(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.management.uninstall
  this.uninstall = function(id, callback) {
    console.debug("management.uninstall(..) called");
  };

  //============================================================================
  // events

  EventFactory.createEvents(this, instanceID, API_NAME, EVENT_LIST);

  //============================================================================
  //============================================================================
  // main initialization

}

exports.createAPI = function(instanceID) {
  return new Management(instanceID);
}

exports.releaseAPI = function(instanceID) {
  EventFactory.releaseEvents(instanceID, API_NAME, EVENT_LIST);
}