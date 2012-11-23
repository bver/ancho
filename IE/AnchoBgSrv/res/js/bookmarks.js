/******************************************************************************
 * bookmarks.js
 * Part of Ancho browser extension framework
 * Implements chrome.bookmarks
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("events.js").Event;

var EventFactory = require("utils.js").EventFactory;

var EVENT_LIST = ['onChanged',
                    'onChildrenReordered',
                    'onCreated',
                    'onImportBegan',
                    'onImportEnded',
                    'onMoved',
                    'onRemoved'];
var API_NAME = 'bookmarks';

//******************************************************************************
//* main closure
var Bookmarks = function(instanceID) {
  //============================================================================
  // private variables


  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.bookmarks.create
  this.create = function(bookmark, callback) {
    console.debug("bookmarks.create(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.bookmarks.get
  this.get = function(idOrIdList, callback) {
    console.debug("bookmarks.get(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.bookmarks.getChildren
  this.getChildren = function(id, callback) {
    console.debug("bookmarks.getChildren(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.bookmarks.getRecent
  this.getRecent = function(numberOfItems, callback) {
    console.debug("bookmarks.getRecent(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.bookmarks.getSubTree
  this.getSubTree = function(id, callback) {
    console.debug("bookmarks.getSubTree(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.bookmarks.getTree
  this.getTree = function(callback) {
    console.debug("bookmarks.getTree(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.bookmarks.move
  this.move = function(id, destination, callback) {
    console.debug("bookmarks.move(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.bookmarks.remove
  this.remove = function(id, callback) {
    console.debug("bookmarks.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.bookmarks.removeTree
  this.removeTree = function(id, callback) {
    console.debug("bookmarks.removeTree(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.bookmarks.search
  this.search = function(query, callback) {
    console.debug("bookmarks.search(..) called");
  };

  //----------------------------------------------------------------------------
  // chrome.bookmarks.update
  this.update = function(id, changes, callback) {
    console.debug("bookmarks.update(..) called");
  };

  //============================================================================
  // events
  EventFactory.createEvents(this, instanceID, API_NAME, EVENT_LIST);

  //============================================================================
  //============================================================================
  // main initialization

}

exports.createAPI = function(instanceID) {
  return new Bookmarks(instanceID);
}

exports.releaseAPI = function(instanceID) {
  EventFactory.releaseEvents(instanceID, 'bookmarks', EVENT_LIST);
}