const { classes: Cc, interfaces: Ci, utils: Cu } = Components;

Cu.import('resource://gre/modules/Services.jsm');
Cu.import('resource://ancho/modules/Require.jsm');
Cu.import('resource://ancho/modules/External.jsm');

var sandbox = Cu.Sandbox(window);
var baseURI = Services.io.newURI('resource://ancho/js/', '', null);
var require = Require.createRequireForWindow(sandbox, baseURI);

var ExtensionState = require('./state');
var applyContentScripts = require('./scripting').applyContentScripts;
var loadHtml = require('./scripting').loadHtml;
var BrowserEvents = require('./browserEvents');
var Toolbar = require('./toolbarSingleton');
var Config = require('./config');

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
  window.addEventListener('DOMContentLoaded', function(event) {
    window.removeEventListener('DOMContentLoaded', arguments.callee, false);
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
  ExtensionState.startSingletonAPIs(window);
  createWindowWatcher();
  var spec = Config.backgroundPage
        ? Config.hostExtensionRoot + Config.backgroundPage
        // Cannot use 'about:blank' here, because DOM for 'about:blank'
        // is inappropriate for script inserting: neither 'document.head'
        // nor 'document.body' are defined.
        : 'chrome://ancho/content/html/blank.html';
  var browser = document.getElementById('content');
  loadHtml(document, browser, spec, function(targetWindow) {
    // load background scripts, if any
    for (var i = 0; i < Config.backgroundScripts.length; i++) {
      var script = targetWindow.document.createElement('script');
      script.src = Config.hostExtensionRoot + Config.backgroundScripts[i];
      targetWindow.document.head.appendChild(script);
    }
    AnchoExternal.__set(targetWindow.ancho.external);
  });
}, false);

window.addEventListener('unload', function(event) {
  window.removeEventListener('unload', arguments.callee, false);
  AnchoExternal.__set(null);
  releaseWindowWatcher();
  ExtensionState.unloadAll();
}, false);
