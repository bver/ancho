/******************************************************************************
 * permissions.js
 * Part of Ancho browser extension framework
 * Implements chrome.permissions
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("events.js").Event;
var EventFactory = require("utils.js").EventFactory;

var EVENT_LIST = ['onAdded', 'onRemoved'];
var API_NAME = 'permissions';
//******************************************************************************
//* main closure
var Permissions = function(instanceID) {
  //============================================================================
  // private variables


  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.permissions.contains
  this.contains = function(permissions, callback) {
    console.debug("permissions.contains(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.permissions.getAll
  this.getAll = function(callback) {
    console.debug("permissions.getAll(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.permissions.remove
  this.remove = function(permissions, callback) {
    console.debug("permissions.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.permissions.request
  this.request = function(permissions, callback) {
    console.debug("permissions.request(..) called");
  };

  //============================================================================
  // events

  EventFactory.createEvents(this, instanceID, API_NAME, EVENT_LIST);

  //============================================================================
  //============================================================================
  // main initialization

}

exports.createAPI = function(instanceID) {
  return new Permissions(instanceID);
}

exports.releaseAPI = function(instanceID) {
  EventFactory.releaseEvents(instanceID, API_NAME, EVENT_LIST);
}