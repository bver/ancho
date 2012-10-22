(function() {

  var Cu = Components.utils;
  Cu.import('resource://gre/modules/Services.jsm');

  var Event = require('./event');

  var WindowWatcherImpl = function() {
    this.loaders = new Event();
    this.unloaders = new Event();
    this.notificationListener = null;
  };

  WindowWatcherImpl.prototype.unload = function() {
    if (this.notificationListener != null) {
      Services.ww.unregisterNotification(this.notificationListener);
      this.notificationListener = null;
    }
    this.forAllWindows(function(browserWindow) {
      this.unloaders.fire(browserWindow);
    });
    this.loaders = new Event();
    this.unloaders = new Event();
  };

  WindowWatcherImpl.prototype.load = function() {
    if (this.notificationListener == null) {
      var self = this;
      this.notificationListener = function(browserWindow, topic) {
        if (topic === "domwindowopened") {
          if ('complete' === browserWindow.document.readyState) {
            self.loaders.fire(browserWindow);
          } else {
            browserWindow.addEventListener('load', function() {
              browserWindow.removeEventListener('load', arguments.callee, false);
              self.loaders.fire(browserWindow);
            });
          }
        }
        if (topic === "domwindowclosed") {
          self.unloaders.fire(browserWindow);
        }
      };
      Services.ww.registerNotification(this.notificationListener);
    }
  };

  WindowWatcherImpl.prototype.register = function(loader, unloader) {
    this.loaders.addListener(loader);
    this.unloaders.addListener(unloader);

    // start listening of browser window open/close events
    this.load();

    // go through open windows and call loader there
    this.forAllWindows(function(browserWindow) {
      if ('complete' === browserWindow.document.readyState) {
        // Document is fully loaded so we can watch immediately.
        loader(browserWindow);
      } else {
        // Wait for the window to load before watching.
        browserWindow.addEventListener('load', function() {
          browserWindow.removeEventListener('load', arguments.callee, false);
          loader(browserWindow);
        });
      }
    });
  };

  WindowWatcherImpl.prototype.forAllWindows = function(callback) {
    var browserWindows = Services.wm.getEnumerator("navigator:browser");

    while (browserWindows.hasMoreElements()) {
      var browserWindow = browserWindows.getNext();
      callback.call(this, browserWindow);
    }
  };

  WindowWatcherImpl.prototype.isActiveBrowserWindow = function(browserWindow) {
    return browserWindow === Services.wm.getMostRecentWindow("navigator:browser");
  };

  WindowWatcherImpl.prototype.isActiveTab = function(browserWindow, tab) {
    return browserWindow.gBrowser.selectedTab === tab;
  };

  module.exports = new WindowWatcherImpl();

}).call(this);
