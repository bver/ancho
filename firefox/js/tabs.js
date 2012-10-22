(function() {

  var Cc = Components.classes;
  var Ci = Components.interfaces;
  var Cu = Components.utils;

  Cu.import('resource://gre/modules/Services.jsm');

  var Event = require('./event');
  var Utils = require('./utils');


  function TabsAPI(state, window) {
    this._state = state;
    this._tab = Utils.getWindowId(window);

    // Event handlers
    this.onCreated = new Event(window, this._tab, this._state, 'tabCreated');
    this.onActivated = new Event(window, this._tab, this._state, 'tabActivated');
    this.onRemoved = new Event(window, this._tab, this._state, 'tabRemoved');
  }

  TabsAPI.prototype = {
    getCurrent: function(callback) {
      // TODO: Check whether we are really a tab
      callback({ id: this._tab});
    },

    sendRequest: function(tabId, request, callback) {
      var sender = Utils.getSender(this._state['id'], this._tab);
      this._state.eventDispatcher.notifyListeners('request', tabId,
        [ request, sender, callback ]);
    },

    create: function(createProperties, callback) {
      if (!('active' in createProperties)) {
        createProperties.active = true;
      }
      var tabbrowser = this._getBrowserForWindowId(createProperties.windowId);
      var tab = tabbrowser.addTab(createProperties.url || 'about:blank');
      if (createProperties.active) {
        tabbrowser.selectedTab = tab;
      }
    },

    update: function(tabId, updateProperties, callback) {
      if (typeof(tabId) != 'number') {
        // No tabId specified so shift the arguments.
        callback = updateProperties;
        updateProperties = tabId;
        tabId = null;
      }
      var tabbrowser = this._getBrowserForWindowId(null);
      var browser = this._getBrowserForTabId(tabbrowser, tabId);
      browser.loadURI(updateProperties.url);
      if (updateProperties.active) {
        tabbrowser.selectedBrowser = browser;
      }
    },

    executeScript: function(tabId, executeScriptProperties, callback) {
      var tabbrowser = this._getBrowserForWindowId(null);
      var browser = this._getBrowserForTabId(tabbrowser, tabId);
      var doc = browser.contentDocument;
      if (doc && doc.body) {
        var scriptElement = doc.createElement('script');
        scriptElement.setAttribute('type', 'text/javascript');
        var tt = doc.createTextNode(executeScriptProperties.code);
        scriptElement.appendChild(tt);
        doc.body.appendChild(scriptElement);
      }
    },

    _getBrowserForTabId: function(tabbrowser, tabId) {
      if (!tabId) {
        return tabbrowser.selectedBrowser;
      }
      for (var i=0; i<tabbrowser.browsers.length; i++) {
        var browser = tabbrowser.browsers[i];
        if (Utils.getWindowId(browser.contentWindow) == tabId) {
          return tabbrowser.getBrowserAtIndex(i);
        }
      }
      return null;
    },

    _getBrowserForWindowId: function(windowId) {
      var browserWindow;
      if (!windowId) {
        browserWindow = Services.wm.getMostRecentWindow('navigator:browser');
      }
      else {
        var browserWindows = Services.wm.getEnumerator('navigator:browser');

        while (browserWindows.hasMoreElements()) {
          browserWindow = browserWindows.getNext();
          if (Utils.getWindowId(browserWindow) === windowId) {
            break;
          }
        }
      }
      return browserWindow.document.getElementById('content');
    }
  };

  module.exports = TabsAPI;

}).call(this);
