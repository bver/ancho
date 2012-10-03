/******************************************************************************
 * bookmarks.js
 * Part of Ancho browser extension framework
 * Implements aji.bookmarks
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
  // aji.bookmarks.create
  this.create = function(bookmark, callback) {
    console.debug("bookmarks.create(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.get
  this.get = function(idOrIdList, callback) {
    console.debug("bookmarks.get(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.getChildren
  this.getChildren = function(id, callback) {
    console.debug("bookmarks.getChildren(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.getRecent
  this.getRecent = function(numberOfItems, callback) {
    console.debug("bookmarks.getRecent(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.getSubTree
  this.getSubTree = function(id, callback) {
    console.debug("bookmarks.getSubTree(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.getTree
  this.getTree = function(callback) {
    console.debug("bookmarks.getTree(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.move
  this.move = function(id, destination, callback) {
    console.debug("bookmarks.move(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.remove
  this.remove = function(id, callback) {
    console.debug("bookmarks.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.removeTree
  this.removeTree = function(id, callback) {
    console.debug("bookmarks.removeTree(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.search
  this.search = function(query, callback) {
    console.debug("bookmarks.search(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.update
  this.update = function(id, changes, callback) {
    console.debug("bookmarks.update(..) called");
  };

  //============================================================================
  // events

  this.onChanged = new Event('bookmarks.onChanged', instanceID);
  this.onChildrenReordered = new Event('bookmarks.onChildrenReordered', instanceID);
  this.onCreated = new Event('bookmarks.onCreated', instanceID);
  this.onImportBegan = new Event('bookmarks.onImportBegan', instanceID);
  this.onImportEnded = new Event('bookmarks.onImportEnded', instanceID);
  this.onMoved = new Event('bookmarks.onMoved', instanceID);
  this.onRemoved = new Event('bookmarks.onRemoved', instanceID);

  //============================================================================
  //============================================================================
  // main initialization


})();
}

exports.releaseAPI = function(instanceID) {
  addonAPI.removeEventObject('bookmarks.onChanged', instanceID);
  addonAPI.removeEventObject('bookmarks.onChildrenReordered', instanceID);
  addonAPI.removeEventObject('bookmarks.onCreated', instanceID);
  addonAPI.removeEventObject('bookmarks.onImportBegan', instanceID);
  addonAPI.removeEventObject('bookmarks.onImportEnded', instanceID);
  addonAPI.removeEventObject('bookmarks.onMoved', instanceID);
  addonAPI.removeEventObject('bookmarks.onRemoved', instanceID);
}