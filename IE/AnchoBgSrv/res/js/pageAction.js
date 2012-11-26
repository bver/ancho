/******************************************************************************
 * pageAction.js
 * Part of Ancho browser extension framework
 * Implements chrome.pageAction
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("events.js").Event;
var EventFactory = require("utils.js").EventFactory;

var EVENT_LIST = ['onClicked'];
var API_NAME = 'pageAction';
//******************************************************************************
//* main closure
var PageAction = function(instanceID) {
  //============================================================================
  // private variables


  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.pageAction.getPopup
  this.getPopup = function(details, callback) {
    console.debug("pageAction.getPopup(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.pageAction.getTitle
  this.getTitle = function(details, callback) {
    console.debug("pageAction.getTitle(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.pageAction.hide
  this.hide = function(tabId) {
    console.debug("pageAction.hide(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.pageAction.setIcon
  this.setIcon = function(details) {
    console.debug("pageAction.setIcon(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.pageAction.setPopup
  this.setPopup = function(details) {
    console.debug("pageAction.setPopup(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.pageAction.setTitle
  this.setTitle = function(details) {
    console.debug("pageAction.setTitle(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.pageAction.show
  this.show = function(tabId) {
    console.debug("pageAction.show(..) called");
  };

  //============================================================================
  // events

  EventFactory.createEvents(this, instanceID, API_NAME, EVENT_LIST);

  //============================================================================
  //============================================================================
  // main initialization

}

exports.createAPI = function(instanceID) {
  return new PageAction(instanceID);
}

exports.releaseAPI = function(instanceID) {
  EventFactory.releaseEvents(instanceID, API_NAME, EVENT_LIST);
}