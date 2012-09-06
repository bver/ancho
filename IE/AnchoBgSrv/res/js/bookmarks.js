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
(function(me){
  //============================================================================
  // private variables
  

  //============================================================================
  // public methods
    
  //----------------------------------------------------------------------------
  // aji.bookmarks.create
  me.create = function(bookmark, callback) {
    console.debug("bookmarks.create(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.get
  me.get = function(idOrIdList, callback) {
    console.debug("bookmarks.get(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.getChildren
  me.getChildren = function(id, callback) {
    console.debug("bookmarks.getChildren(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.getRecent
  me.getRecent = function(numberOfItems, callback) {
    console.debug("bookmarks.getRecent(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.getSubTree
  me.getSubTree = function(id, callback) {
    console.debug("bookmarks.getSubTree(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.getTree
  me.getTree = function(callback) {
    console.debug("bookmarks.getTree(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.move
  me.move = function(id, destination, callback) {
    console.debug("bookmarks.move(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.remove
  me.remove = function(id, callback) {
    console.debug("bookmarks.remove(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.removeTree
  me.removeTree = function(id, callback) {
    console.debug("bookmarks.removeTree(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.search
  me.search = function(query, callback) {
    console.debug("bookmarks.search(..) called");
  };

  //----------------------------------------------------------------------------
  // aji.bookmarks.update
  me.update = function(id, changes, callback) {
    console.debug("bookmarks.update(..) called");
  };

  //============================================================================
  // events
    
  me.onChanged = new Event();
  me.onChildrenReordered = new Event();
  me.onCreated = new Event();
  me.onImportBegan = new Event();
  me.onImportEnded = new Event();
  me.onMoved = new Event();
  me.onRemoved = new Event();

  //============================================================================
  //============================================================================
  // main initialization


}).call(this, exports);
