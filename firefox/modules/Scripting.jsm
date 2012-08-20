(function() {
  var Cc = Components.classes;
  var Ci = Components.interfaces;
  var Cu = Components.utils;

  Cu.import('resource://gre/modules/Services.jsm');
  Cu.import('resource://gre/modules/Require.jsm');
  
  var ExtensionState = require('./state');
  var API = require('./ajiAPI');
  var readStringFromUrl = require('./utils').readStringFromUrl;
  
  function prepareWindow(window) {
    if (!('require' in window)) {
      var sandbox = Cu.Sandbox(window, { sandboxPrototype: window });
      window.aji = sandbox.aji = new API(window, ExtensionState);
      window.require = sandbox.require = Require.createRequireForWindow(sandbox);

      window.addEventListener('unload', function(event) {
        window.removeEventListener('unload', arguments.callee, false);
        delete window.require;
        delete window.aji;
      });
    }
  }

  exports.applyContentScripts = function(win, spec) {
    var baseUrl = Services.io.newURI(spec, '', null);
    var contentScripts = require('./config').contentScripts;
    var readStringFromUrl = require('./utils').readStringFromUrl;
    for (var i=0; i<contentScripts.length; i++) {
      var scriptInfo = contentScripts[i];
      var matches = scriptInfo.matches;
      var principal = ExtensionState.backgroundWindow;
      var sandbox = Cu.Sandbox(principal, { sandboxPrototype: win });
      sandbox.aji = new API(win, ExtensionState);
      for (var j=0; j<matches.length; j++) {
        if (spec.match(matches[j])) {
          for (var k=0; k<scriptInfo.js.length; k++) {
            if (sandbox.jQuery) {
              sandbox.jQuery.ajaxSettings.xhr = Require.createWrappedXMLHttpRequest;
            }
            var scriptUri = Services.io.newURI('chrome://trusted-ads/content/' + scriptInfo.js[k], '', null);
            sandbox.require = Require.createRequireForWindow(sandbox, scriptUri);
            var script = readStringFromUrl(scriptUri);
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
  exports.loadHtml = function(document, iframe, htmlSpec, callback) {
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
}).call(this);
