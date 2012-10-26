(function() {

  var Cc = Components.classes;
  var Ci = Components.interfaces;
  var Cu = Components.utils;

  Cu.import('resource://gre/modules/Services.jsm');

  var Event = require('./event');
  var Utils = require('./utils');


  var HTTP_ON_EXAMINE_RESPONSE = "http-on-examine-response";
  var HTTP_ON_MODIFY_REQUEST = "http-on-modify-request";

  var WebRequestAPI = function(state, window) {
    this._state = state;
    this._tab = Utils.getWindowId(window);

    this.onHeadersReceived = new Event(window, this._tab, this._state, 'wrHeadersReceived');
    this.onBeforeRequest = new Event(window, this._tab, this._state, 'wrBeforeRequest');

    Services.obs.addObserver(this, HTTP_ON_EXAMINE_RESPONSE, false);
    Services.obs.addObserver(this, HTTP_ON_MODIFY_REQUEST, false);

    var self = this;
    window.addEventListener('unload', function() {
      Services.obs.removeObserver(self, HTTP_ON_EXAMINE_RESPONSE);
      Services.obs.removeObserver(self, HTTP_ON_MODIFY_REQUEST);
    });
  };

  WebRequestAPI.prototype.observe = function(aSubject, aTopic, aData) {
    if (aTopic == HTTP_ON_EXAMINE_RESPONSE) {
      var httpChannel = aSubject.QueryInterface(Ci.nsIHttpChannel);
      this.processHeaders(httpChannel);
    } else if (aTopic == HTTP_ON_MODIFY_REQUEST) {
      var httpChannel = aSubject.QueryInterface(Ci.nsIHttpChannel);
      this.processRequest(httpChannel);
    }
  };

  WebRequestAPI.prototype.processRequest = function(httpChannel) {
    // FIXME: this function needs rewriting...
    return;

    if (!this.onBeforeRequest.hasListeners()) {
      return;
    }
    var url = httpChannel.URI.spec;
    var browser = Utils.getWindowForRequest(httpChannel);
    var tabId = browser ? Utils.getWindowId(browser) : -1;
    var loadFlags = httpChannel.loadFlags;
    var type = 'other';
    // is it main request of the tab?
    if (loadFlags & Ci.nsIChannel.LOAD_DOCUMENT_URI) {
      type = 'main_frame';
    }
    // is it request for an image? there is no direct way, from debugging
    // experience image requests usualy have loadFlags == 0 or contain
    // LOAD_INITIAL_DOCUMENT_URI flag. But sometimes it also contains other flags.
    // Therefore there is also check for Accept header, which should ensure
    // that browser wants an image.
    // Ci.nsIRequest.LOAD_FROM_CACHE => after refresh button is clicked
    if (loadFlags == 0 || loadFlags & Ci.nsIChannel.LOAD_INITIAL_DOCUMENT_URI
        || loadFlags == Ci.nsIRequest.LOAD_FROM_CACHE) {
      var accept = httpChannel.getRequestHeader('Accept');
      if (accept.indexOf('image/') == 0) {
        type = 'image';
      }
    }
    // TODO: recognize other types - css, ajax, ...
    // http://code.google.com/chrome/extensions/webRequest.html#event-onBeforeRequest
    var param = {
      url : url,
      tabId : tabId,
      type : type
    };
    // FIXME: collect return values from notifyListeners...
    this._state.eventDispatcher.notifyListeners(
      'wrBeforeRequest', null, param
    );
    var results = []; // FIXME
    for (var i = 0; i < results.length; i++) {
      if (results[i] && results[i].cancel) {
        httpChannel.cancel(Cr.NS_BINDING_ABORTED);
      }
    }
  };

  WebRequestAPI.prototype.processHeaders = function(httpChannel) {
    // FIXME: this function needs rewriting...
    return;

    if (!this.onHeadersReceived.hasListeners()) {
      return;
    }
    var visitor = new HttpHeaderVisitor();
    httpChannel.visitResponseHeaders(visitor);
    var url = httpChannel.URI.spec;
    var browser = Utils.getWindowForRequest(httpChannel);
    var tabId = browser ? Utils.getWindowId(browser) : -1;
    var param = {
      url : url,
      // if no tab related request => -1
      tabId : tabId,
      responseHeaders : visitor.headers
    };
    // fire event and get list of results from each handler
    this._state.eventDispatcher.notifyListeners('wrHeadersReceived', null, param);
    // FIXME: result = ... notifyListeners();
    var results = [];
    for (var i = 0; i < results.length; i++) {
      if (results[i] && results[i].responseHeaders) {
        // clear removed original headers
        var newHeaders = results[i].responseHeaders;
        for ( var j = 0; j < visitor.originalHeaders.length; j++) {
          var remove = true;
          for ( var k = 0; k < newHeaders.length; k++) {
            if (visitor.originalHeaders[j].name === newHeaders[k].name) {
              if (visitor.originalHeaders[j].value === newHeaders[k].value) {
                newHeaders[k].modified = false;
              } else {
                newHeaders[k].modified = true;
              }
              remove = false;
              break;
            }
          }
          if (remove) {
            httpChannel
                .setResponseHeader(visitor.originalHeaders[j].name, null, false);
          }
        }
        // sets modified headers
        for ( var j = 0; j < newHeaders.length; j++) {
          if (newHeaders[j].modified) {
            httpChannel
                .setResponseHeader(newHeaders[j].name, newHeaders[j].value, false);
          }
        }

        break; // set only headers from first handler that returns something
      }
    }
  };

  var HttpHeaderVisitor = function() {
    this.headers = [];
    this.originalHeaders = [];
  };

  HttpHeaderVisitor.prototype.visitHeader = function(aHeader, aValue) {
    this.headers.push({
      name : aHeader,
      value : aValue
    });
    this.originalHeaders.push({
      name : aHeader,
      value : aValue
    });
  };

  module.exports = WebRequestAPI;

}).call(this);
