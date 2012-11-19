(function() {
  var Cc = Components.classes;
  var Ci = Components.interfaces;
  var Cu = Components.utils;

  Cu.import('resource://gre/modules/Services.jsm');

  var Utils = require('./utils');
  var Event = require('./event');
  var loadHtml = require('./scripting').loadHtml;

  // Map of popups to the window that contains them.
  // Global for the whole extension.
  var globalPopups = {};

  function WindowsAPI(state, contentWindow) {
    this._contentWindow = contentWindow;
    this._state = state;

    this.onRemoved = new Event(contentWindow, null, state, 'windows.removed');
    this.onUpdated = new Event(contentWindow, null, state, 'windows.updated');
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

      var panel = document.createElement('panel');
      // I'm getting spurious mouseleave events unless I set the level to 'top'.
      // This shouldn't matter for our current use cases, not sure if it will matter
      // in the future.
      // I will inquire about why this is.
      panel.setAttribute('level', 'top');
      panel.setAttribute('style', 'border: 1px solid #aaa');
      document.getElementById('mainPopupSet').appendChild(panel);

      if (createData.height) {
        panel.height = createData.height;
      }
      if (createData.width) {
        panel.width = createData.width;
      }

      var iframe = document.createElement('iframe');
      iframe.setAttribute('flex', '1');
      iframe.setAttribute('src', 'about:blank');
      iframe.setAttribute('style', 'overflow: hidden');

      var baseSpec = Services.io.newURI(this._contentWindow.document.location.href, '', null);
      iframe.addEventListener('DOMContentLoaded', function() {
        iframe.removeEventListener('DOMContentLoaded', arguments.callee, false);
        panel.openPopupAtScreen(createData.left, createData.top, false);
      });

      panel.appendChild(iframe);

      var windowId = null;
      panel.addEventListener('popupshowing', function() {
        panel.removeEventListener('popupshowing', arguments.callee, false);

        if ('url' in createData) {
          var uri = Services.io.newURI(createData.url, '', baseSpec);
          loadHtml(document, iframe, uri.spec);
        }
        windowId = Utils.getWindowId(iframe.contentWindow);
        panel.id = 'panel_' + windowId;

        // Remember where we added the panel so we can remove it later.
        globalPopups[windowId] = chromeWindow;

        callback({ id: windowId });
      }, false);

      panel.addEventListener('popuphiding', function() {
        if (windowId) {
          // Notify listeners that we are closing.
          self.onRemoved.fire([ windowId ]);
        }

        panel.removeEventListener('popuphiding', arguments.callee, false);
      });

      panel.addEventListener('popuphidden', function() {
        panel.removeEventListener('popuphidden', arguments.callee, false);
        document.getElementById('mainPopupSet').removeChild(panel);
      }, false);
    },

    remove: function(windowId, callback) {
      if (!(windowId in globalPopups)) {
        // Don't know about this popup.
        return;
      }
      var chromeWindow = globalPopups[windowId];
      var document  = chromeWindow.document;
      var panel = document.getElementById('panel_' + windowId);
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
        var panel = chromeWindow.document.getElementById('panel_' + windowId);
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
        this.onUpdated.fire([ windowId, updateInfo ]);
      }
    }
  };

  module.exports = WindowsAPI;

}).call(this);
