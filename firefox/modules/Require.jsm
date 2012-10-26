EXPORTED_SYMBOLS = ['Require'];

const { classes: Cc, interfaces: Ci, utils: Cu } = Components;
Cu.import('resource://gre/modules/Services.jsm');

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

var Require = {
  _moduleCache: {},

  _resProtocolHandler: Services.io.getProtocolHandler("resource"),

  moduleSearchPath: [],

  createWrappedXMLHttpRequest: function() { return new WrappedXMLHttpRequest(); },

  findModuleInPath: function(id) {
    for (var i=0; i<this.moduleSearchPath.length; i++) {
      var url = Services.io.newURI(id + '.js', '', this.moduleSearchPath[i]);
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
  },

  createRequireForWindow: function(sandbox, baseUrl) {
    var self = this;
    return function require(id, scriptUrl) {
      if (baseUrl && !scriptUrl) {
        scriptUrl = baseUrl;
      }
      if (!scriptUrl) {
        // No base URL available so we need to get it from the stacktrace.
        try {
          // To get a stacktrace we have to thrown an exception.
          throw new Error();
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
        url = self.findModuleInPath(id);
      }
      else {
        url = Services.io.newURI(id + '.js', '', scriptUrl);
      }
      if (!url) {
        // TODO: Logging.
        return;
      }

      var spec = url.spec;
      if (spec in self._moduleCache) {
        return self._moduleCache[spec];
      }

      var scriptLoader = Cc['@mozilla.org/moz/jssubscript-loader;1'].
        getService(Ci.mozIJSSubScriptLoader);

      var context = {};
      context.require = function(id) { return require(id, url); };

      if ('window' in sandbox) {
        context.window = sandbox.window;
        context.document = sandbox.window.document;
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
          context.jQuery.ajaxSettings.xhr = self.createWrappedXMLHttpRequest;
        }
      }

      if ('chrome' in sandbox) {
        context.chrome = sandbox.chrome;
      }
      if ('ancho' in sandbox) {
        context.ancho = sandbox.ancho;
      }
      if ('console' in sandbox) {
        context.console = sandbox.console;
      }

      // Need to add to the cache here to avoid stack overflow in case of require() cycles
      // (e.g. A requires B which requires A).
      self._moduleCache[spec] = context.exports = {};
      // Support for 'module.exports' (overrides 'exports' if 'module.exports' is used).
      context.module = {};

      try {
        scriptLoader.loadSubScript(spec, context);
      } catch (reason) {
        dump('\nERROR: Loading of subscript "' + spec + '" failed:\n');
        dump(reason + '\n\n');
      }
      if (context.module.exports) {
        context.exports = context.module.exports;
      }
      self._moduleCache[spec] = context.exports;
      return context.exports;
    }
  }
};
