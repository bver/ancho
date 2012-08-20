EXPORTED_SYMBOLS = ['applyContentScripts', 'loadHtml'];

const { classes: Cc, interfaces: Ci, utils: Cu } = Components;

Cu.import('resource://gre/modules/Services.jsm');
Cu.import('resource://gre/modules/XPCOMUtils.jsm');
Cu.import('resource://trusted-ads/modules/Config.jsm');
Cu.import('resource://trusted-ads/modules/State.jsm');
Cu.import('resource://trusted-ads/modules/API.jsm');
Cu.import('resource://trusted-ads/modules/Utils.jsm');

// XHR implementation with no XSS restrictions
function WrappedXMLHttpRequest() {
  this._inner = Cc['@mozilla.org/xmlextras/xmlhttprequest;1'].createInstance();
}

WrappedXMLHttpRequest.prototype = {
  get responseXML() { return this._inner.responseXML; },
  get responseText() { return this._inner.responseText; },
  get status() { return this._inner.status; },
  get statusText() { return this._inner.statusText; },
  getAllResponseHeaders: function() { return this._inner.getAllResponseHeaders(); },
  getResponseHeader: function(header) { return this._inner.getResponseHeader(header); },
  open: function(method, url, async) { return this._inner.open(method, url, async); },
  send: function(body) { this._inner.send(body); },
  setRequestHeader: function(header, value) { this._inner.setRequestHeader(header, value); },
  get readyState() { return this._inner.readyState; },
  set onreadystatechange(callback) { this._inner.onreadystatechange = callback; }
};

function createWrappedHttpRequest() {
  return new WrappedXMLHttpRequest;
}

// require() functionality for XPCOM
function findModuleInPath(id) {
  for (var i=0; i<Config.MODULE_SEARCH_PATH.length; i++) {
    var url = Services.io.newURI(id + '.js', '', Config.MODULE_SEARCH_PATH[i]);
    var channel = Services.io.newChannelFromURI(url);
    try {
      var inputStream = channel.open();
      return url;
    }
    catch(e) {
      // No stream so the module doesn't exist
    }
  }
  return null;
}

function createRequireForWindow(sandbox, baseUrl) {
  var moduleCache = {};

  return function require(id, scriptUrl) {
    if (baseUrl && !scriptUrl) {
      scriptUrl = baseUrl;
    }
    if (!scriptUrl) {
      // No base URL available so we need to get it from the stacktrace.
      try {
        this.undef();
      }
      catch (e) {
        var frames = e.stack.split('\n');
        var baseSpec = frames[1].match(/@(.*):\d+$/)[1];
        scriptUrl = Services.io.newURI(baseSpec, '', null);
      }
    }

    var url;
    if (id[0] != '.' && id[0] != '/') {
      // Try to find the module in the search path
      url = findModuleInPath(id);
    }
    else {
      url = Services.io.newURI(id + '.js', '', scriptUrl);
    }
    if (!url) {
      return;
    }

    var spec = url.spec;
    if (spec in moduleCache) {
      return moduleCache[spec];
    }

    var scriptLoader = Cc['@mozilla.org/moz/jssubscript-loader;1'].
      getService(Ci.mozIJSSubScriptLoader);

    var context = {};
    context.window = sandbox.window;
    context.document = sandbox.window.document;
    context.aji = sandbox.aji;
    context.require = function(id) { return require(id, url); };
    var jQuery = null;
    if ('jQuery' in sandbox) {
      jQuery = sandbox.jQuery;
    }
    else if ('jQuery' in sandbox.window) {
      // Get the jQuery object from the window (was presumably loaded via <script> tag).
      jQuery = sandbox.window.jQuery;
    }
    if (jQuery) {
      // Use the jQuery from the window.
      context.jQuery = context.$ = jQuery;
      context.jQuery.ajaxSettings.xhr = function() { return new WrappedXMLHttpRequest(); };
    }

    // Need to add to the cache here to avoid stack overflow in case of require() cycles
    // (e.g. A requires B which requires A).
    moduleCache[spec] = context.exports = {};

    scriptLoader.loadSubScript(spec, context);

    return context.exports;
  }
};

function prepareWindow(window) {
  if (!('require' in window)) {
    var sandbox = Cu.Sandbox(window, { sandboxPrototype: window });
    window.aji = sandbox.aji = new AjiAPI(window, ExtensionState);
    window.require = sandbox.require = createRequireForWindow(sandbox);

    window.addEventListener('unload', function(event) {
      window.removeEventListener('unload', arguments.callee, false);
      delete window.require;
      delete window.aji;
    });
  }
}

function applyContentScripts(win, spec) {
  var baseUrl = Services.io.newURI(spec, '', null);
  for (var i=0; i<Config.contentScripts.length; i++) {
    var scriptInfo = Config.contentScripts[i];
    var matches = scriptInfo.matches;
    var principal = ExtensionState.backgroundWindow;
    var sandbox = Cu.Sandbox(principal, { sandboxPrototype: win });
    sandbox.aji = new AjiAPI(win, ExtensionState);
    for (var j=0; j<matches.length; j++) {
      if (spec.match(matches[j])) {
        for (var k=0; k<scriptInfo.js.length; k++) {
          if (sandbox.jQuery) {
            sandbox.jQuery.ajaxSettings.xhr = createWrappedHttpRequest;
          }
          var scriptUri = Services.io.newURI('chrome://trusted-ads/content/' + scriptInfo.js[k], '', null);
          sandbox.require = createRequireForWindow(sandbox, scriptUri);
          var script = Utils.readStringFromUrl(scriptUri);
          Cu.evalInSandbox(script, sandbox);
        }
        break;
      }
    }
  }
}

// When we load a privileged HTML page we want all scripts to load as content
// scripts so that they have access to the require function and aji.* APIs.
// So we strip the <script> tags out of the document and load them separately
// as content scripts.
function loadHtml(document, iframe, htmlSpec, callback) {
  var targetWindow = iframe.contentWindow;
  document.addEventListener('DOMWindowCreated', function(event) {
    var window = event.target.defaultView.wrappedJSObject;
    if (window != targetWindow) {
      return;
    }
    document.removeEventListener('DOMWindowCreated', arguments.callee, false);

    prepareWindow(window);
  }, false);

  if (callback) {
    iframe.addEventListener('DOMContentLoaded', function(event) {
      iframe.removeEventListener('DOMContentLoaded', arguments.callee, false);
      callback();
    }, false);
  }

  iframe.webNavigation.loadURI(htmlSpec, Ci.nsIWebNavigation.LOAD_FLAGS_NONE, null, null, null);
}
