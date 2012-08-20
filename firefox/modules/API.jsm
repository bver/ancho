EXPORTED_SYMBOLS = ["AjiAPI"];

const { classes: Cc, interfaces: Ci, utils: Cu} = Components;

Cu.import("resource://gre/modules/Services.jsm");
Cu.import("resource://trusted-ads/modules/Utils.jsm");
Cu.import("resource://trusted-ads/modules/Scripting.jsm");
Cu.import("resource://trusted-ads/modules/LocalStorageAPI.jsm");
Cu.import('resource://trusted-ads/modules/Toolbar.jsm');
Cu.import("resource://trusted-ads/modules/State.jsm");

// Utility functions
function getSender(extensionId, tabId) {
  return { id: extensionId, tab: { id: tabId } };
}

// Google Chrome-compatible extension API

// Event that keeps track of the target tab.
// Most events are dispatched to all tabs, but this gives us the
// option of dispatching them only to a specific tab
// (e.g. aji.tabs.sendRequest).
function Event(window, tabId, state, type) {
  var listeners = [];

  window.addEventListener("unload", function(event) {
    window.removeEventListener("unload", arguments.callee, false);

    state.eventDispatcher.removeListener(type, notifyListeners);
  }, false);

  state.eventDispatcher.addListener(type, notifyListeners);

  function notifyListeners(targetTab, params) {
    if (!targetTab || (targetTab == tabId)) {
      for (var i=0; i<listeners.length; i++) {
        listeners[i].apply(this, params);
      }
    }
  }

  this.addListener = function(listener) {
    listeners.push(listener);
  };

  this.removeListener = function(listener) {
    var index = listeners.indexOf(listener);
    if (index != -1) {
      listeners.splice(index);
    }
  };
}

function TabsAPI(state, window) {
  this._state = state;
  this._tab = Utils.getWindowId(window);

  // Event handlers
  this.onCreated = new Event(window, this._tab, this._state, "tabCreated");
  this.onActivated = new Event(window, this._tab, this._state, "tabActivated");
  this.onRemoved = new Event(window, this._tab, this._state, "tabRemoved");
}

TabsAPI.prototype = {
  getCurrent: function(callback) {
    // TODO: Check whether we are really a tab
    callback({ id: this._tab});
  },

  sendRequest: function(tabId, request, callback) {
    var sender = getSender(this._state["id"], this._tab);
    this._state.eventDispatcher.notifyListeners("request", tabId,
      [ request, sender, callback ]);
  },

  create: function(createProperties, callback) {
    if (!("active" in createProperties)) {
      createProperties.active = true;
    }
    var tabbrowser = this._getBrowserForWindowId(createProperties.windowId);
    var tab = tabbrowser.addTab(createProperties.url || "about:blank");
    if (createProperties.active) {
      tabbrowser.selectedTab = tab;
    }
  },

  update: function(tabId, updateProperties, callback) {
    if (typeof(tabId) != "number") {
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
      browserWindow = Services.wm.getMostRecentWindow("navigator:browser");
    }
    else {
      var browserWindows = Services.wm.getEnumerator("navigator:browser");

      while (browserWindows.hasMoreElements()) {
        browserWindow = browserWindows.getNext();
        if (Utils.getWindowId(browserWindow) === windowId) {
          break;
        }
      }
    }
    return browserWindow.document.getElementById("content");
  }
};

function ExtensionAPI(state, window) {
  this._state = state;
  this._tab = Utils.getWindowId(window);

  // Event handlers
  this.onRequest = new Event(window, this._tab, this._state, "request");
}

ExtensionAPI.prototype = {
  sendRequest: function(request, callback) {
    var sender = getSender(this._state["id"], this._tab);
    this._state.eventDispatcher.notifyListeners("request", null,
      [ request, sender, callback ]);
  },

  getURL: function(path) {
    // TODO: Get the extension ID from somewhere.
    // Treat all paths as relative so they are resolved from the root URI.
    if (path[0] === '/') {
      path = path.substr(1);
    }
    var baseURI = Services.io.newURI("chrome://trusted-ads/content", "", null);
    var uri = Services.io.newURI(path, "", baseURI);
    return uri.spec;
  }
};

// Map of popups to the window that contains them.
// Global for the whole extension.
var globalPopups = {};

function WindowsAPI(state, contentWindow) {
  this._contentWindow = contentWindow;
  this._state = state;

  this.onRemoved = new Event(contentWindow, null, state, "windowRemoved");
  this.onUpdated = new Event(contentWindow, null, state, "windowUpdated");
}

WindowsAPI.prototype = {
  getCurrent: function(getInfo, callback) {
    callback({ id: Utils.getWindowId(this._contentWindow) });
  },

  create: function(createData, callback) {
    var self = this;
    var xulWindow = this._contentWindow.QueryInterface(Ci.nsIInterfaceRequestor)
      .getInterface(Ci.nsIWebNavigation)
      .QueryInterface(Ci.nsIDocShellTreeItem)
      .treeOwner
      .QueryInterface(Ci.nsIInterfaceRequestor)
      .getInterface(Ci.nsIXULWindow);
    var chromeWindow = xulWindow.QueryInterface(Ci.nsIInterfaceRequestor)
      .getInterface(Ci.nsIDOMWindow);

    var document = chromeWindow.document;

    var panel = document.createElement("panel");
    // I'm getting spurious mouseleave events unless I set the level to "top".
    // This shouldn't matter for our current use cases, not sure if it will matter
    // in the future.
    // I will inquire about why this is.
    panel.setAttribute("level", "top");
    panel.setAttribute("style", "border: 1px solid #aaa");
    document.getElementById("mainPopupSet").appendChild(panel);

    if (createData.height) {
      panel.height = createData.height;
    }
    if (createData.width) {
      panel.width = createData.width;
    }

    var iframe = document.createElement("iframe");
    iframe.setAttribute("flex", "1");
    iframe.setAttribute("src", "about:blank");
    iframe.setAttribute("style", "overflow: hidden");

    var baseSpec = Services.io.newURI(this._contentWindow.document.location.href, "", null);
    iframe.addEventListener("DOMContentLoaded", function() {
      iframe.removeEventListener("DOMContentLoaded", arguments.callee, false);
      panel.openPopupAtScreen(createData.left, createData.top, false);
    });

    panel.appendChild(iframe);

    var windowId = null;
    panel.addEventListener("popupshowing", function() {
      panel.removeEventListener("popupshowing", arguments.callee, false);

      if ("url" in createData) {
        var uri = Services.io.newURI(createData.url, "", baseSpec);
        loadHtml(document, iframe, uri.spec);
      }
      windowId = Utils.getWindowId(iframe.contentWindow);
      panel.id = "panel_" + windowId;

      // Remember where we added the panel so we can remove it later.
      globalPopups[windowId] = chromeWindow;

      callback({ id: windowId });
    }, false);

    panel.addEventListener("popuphiding", function() {
      if (windowId) {
        // Notify listeners that we are closing.
        self._state.eventDispatcher.notifyListeners("windowRemoved", null, [ windowId ]);
      }

      panel.removeEventListener("popuphiding", arguments.callee, false);
    });

    panel.addEventListener("popuphidden", function() {
      panel.removeEventListener("popuphidden", arguments.callee, false);
      document.getElementById("mainPopupSet").removeChild(panel);
    }, false);
  },

  remove: function(windowId, callback) {
    if (!(windowId in globalPopups)) {
      // Don't know about this popup.
      return;
    }
    var chromeWindow = globalPopups[windowId];
    var document  = chromeWindow.document;
    var panel = document.getElementById("panel_" + windowId);
    if (panel) {
      panel.hidePopup();
    }
    else {
      // TODO: Logging
    }
  },

  update: function(windowId, updateInfo) {
    // TODO: Support for windows other than popups.
    if (windowId in globalPopups) {
      var chromeWindow = globalPopups[windowId];
      var panel = chromeWindow.document.getElementById("panel_" + windowId);
      if (updateInfo.height) {
        panel.height = updateInfo.height;
      }
      if (updateInfo.width) {
        panel.width = updateInfo.width;
      }
      if (updateInfo.left && updateInfo.top) {
        // TODO: We should really be able to use left and top independently of each other per the spec.
        panel.moveTo(updateInfo.left, updateInfo.top);
      }
      this._state.eventDispatcher.notifyListeners("windowUpdated", null, [ windowId, updateInfo ]);
    }
  }
};

function ToolbarAPI(state, contentWindow) {
}
ToolbarAPI.prototype = {
  show: function(options) {

    // Ensure toolbar reflects the embedded features state.
    // It fixes the bug when enabling/disabling toolbar parts like
    // dropdown_menu, current_site_icons or search_box.
    this.hide();

    var browserWindows = Services.wm.getEnumerator("navigator:browser");

    while (browserWindows.hasMoreElements()) {
      var win = browserWindows.getNext();

      var toolbar = win.document.getElementById(Toolbar.TOOLBAR_ID);
      if (!toolbar) { // do not create toolbar twice
        ExtensionState.registerUnloader(win, Toolbar.create(win,options));
      }
    }
  },

  hide: function() {
    var browserWindows = Services.wm.getEnumerator("navigator:browser");

    while (browserWindows.hasMoreElements()) {
      var win = browserWindows.getNext();
      Toolbar.remove(win);
    }
  }
};

function ConsoleAPI(state, contentWindow) {
}
ConsoleAPI.prototype = {
  error: function(args) {
    dump("ERROR: ");
    dump(args);
    dump("\n");
  },

  warn: function(args) {
    dump("Warning: ");
    dump(args);
    dump("\n");
  },

  log: function(args) {
    dump("Log: ");
    dump(args);
    dump("\n");
  }
};


function AjiAPI(contentWindow, extensionState) {
  this.extension = new ExtensionAPI(extensionState, contentWindow);
  this.tabs = new TabsAPI(extensionState, contentWindow);
  this.windows = new WindowsAPI(extensionState, contentWindow);
  this.localStorage = new LocalStorageAPI(extensionState, contentWindow);
  this.toolbar = new ToolbarAPI(extensionState, contentWindow);
  this.console = new ConsoleAPI(extensionState, contentWindow);
}
