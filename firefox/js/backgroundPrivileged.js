const { classes: Cc, interfaces: Ci, utils: Cu } = Components;

Cu.import('resource://gre/modules/Services.jsm');
Cu.import('resource://trusted-ads/modules/State.jsm');
Cu.import('resource://trusted-ads/modules/Utils.jsm');
Cu.import('resource://trusted-ads/modules/Scripting.jsm');
Cu.import('resource://trusted-ads/modules/BrowserEvents.jsm');
Cu.import('resource://trusted-ads/modules/Toolbar.jsm');

function createWindowUnloader(win) {
  return function(event) {
    ExtensionState.unloadWindow(window);
  }
}

function windowLoaded(window) {
  var browser = window.document.getElementById('content');
  if (browser) {
    var browserEvents = new BrowserEvents(browser, ExtensionState);
    ExtensionState.registerUnloader(window,
      browserEvents.init(applyContentScripts));
  }

  var unloader = Toolbar.createToolbarIfActive(window);
  if (unloader) {
    ExtensionState.registerUnloader(window, unloader);
  }

  var windowUnloader = createWindowUnloader(window);
  window.addEventListener('unload', windowUnloader, false);

  // Make sure we remove the window unloader if the extension is unloaded
  // (disabled or removed) before the window is.
  ExtensionState.registerUnloader(window, function() {
    window.removeEventListener('unload', windowUnloader, false);
  });
}

function watchOnWindowLoad(window) {
  window.addEventListener('load', function(event) {
    window.removeEventListener('load', arguments.callee, false);
    windowLoaded(window);
  }, false);
}

function watchWindow(subject, topic) {
  if (topic === 'domwindowopened') {
    watchOnWindowLoad(subject);
  }
}

function createWindowWatcher() {
  // Watch the existing windows
  var browserWindows = Services.wm.getEnumerator('navigator:browser');

  while (browserWindows.hasMoreElements()) {
    var browserWindow = browserWindows.getNext();
    if ('complete' === browserWindow.document.readyState) {
      // Document is fully loaded so we can watch immediately.
      windowLoaded(browserWindow);
    }
    else {
      // Wait for the window to load before watching.
      watchOnWindowLoad(browserWindow);
    }
  }

  // Register to be notified when new windows are created.
  Services.ww.registerNotification(watchWindow);
}

function releaseWindowWatcher() {
  Services.ww.unregisterNotification(watchWindow);
}

window.addEventListener('load', function(event) {
  window.removeEventListener('load', arguments.callee, false);

  ExtensionState.backgroundWindow = window;

  createWindowWatcher();

  var browser = document.getElementById('content');
  loadHtml(document, browser, 'chrome://trusted-ads/content/html/background.html');
}, false);

window.addEventListener('unload', function(event) {
  window.removeEventListener('unload', arguments.callee, false);

  releaseWindowWatcher();

  ExtensionState.unloadAll();
}, false);
