(function() {

  var Cu = Components.utils;
  var Cc = Components.classes;

  Cu.import('resource://gre/modules/Services.jsm');

  var Event = require('./event');
  var Utils = require('./utils');


  var COOKIE_CHANGED = 'cookie-changed';
  var COOKIE_CHANGED_DATA_ADDED = 'added';
  var COOKIE_CHANGED_DATA_CHANGED = 'changed';

  var CookiesAPI = function(state, window) {
    this._state = state;
    this._tab = Utils.getWindowId(window);
    this.cookieService = Cc["@mozilla.org/cookieService;1"]
        .getService(Ci.nsICookieService);
    this.onChanged = new Event(window, this._tab, this._state, 'cookie.changed');
    Services.obs.addObserver(this, COOKIE_CHANGED, false);
  };

  CookiesAPI.prototype.observe = function(subject, topic, data) {
    try {
      if (topic == COOKIE_CHANGED) {
        var changed = data == COOKIE_CHANGED_DATA_CHANGED
            || data == COOKIE_CHANGED_DATA_ADDED;
        // TODO: handle other events not only added/changed
        // https://developer.mozilla.org/en-US/docs/XPCOM_Interface_Reference/nsICookieService
        if (changed) {
          var cookie = this.toCookie(subject);
          this.onChanged.fire([ {
            cookie : cookie,
            removed : false
          } ]);
        }

      }
    } catch (e) {
      dump('error:  ' + e + '\n');
    }
  };

  CookiesAPI.prototype.parseUrl = function(url) {
    return Services.io.newURI(url, null, null);
  };

  CookiesAPI.prototype.toCookie = function(cookieItem) {
    var nsCookie = cookieItem.QueryInterface(Ci.nsICookie);
    return {
      name : nsCookie.name,
      value : nsCookie.value,
      domain : nsCookie.host,
      hostOnly : nsCookie.isDomain,
      path : nsCookie.path,
      secure : nsCookie.isSecure,
      // httpOnly is not supported in FireFox
      session : nsCookie.expires == 0 ? true : false,
      expirationDate : nsCookie.expires
    };
  };

  /**
   * @param details
   *          {url, name, domain, path, secure, session}
   * @param callback
   *          function([cookies])
   */
  CookiesAPI.prototype.getAll = function(details, callback) {
    var urlRegexp = 'url' in details ? new RegExp('^' + details.url) : null;
    var domainRegexp = 'domain' in details ? new RegExp('([^.]*\\.)?'
        + details.domain) : null;
    var pathRegexp = 'path' in details ? new RegExp('^' + details.path + '$')
        : null;
    var enumerator = Services.cookies.enumerator;
    var result = [];
    while (enumerator.hasMoreElements()) {
      var next = enumerator.getNext();
      var cookie = this.toCookie(next);
      var condition = true;
      if (urlRegexp) {
        condition &= urlRegexp.test(cookie.domain + cookie.path);
      }
      if (domainRegexp) {
        condition &= domainRegexp.test(cookie.domain);
      }
      if (pathRegexp) {
        condition &= pathRegexp.test(cookie.path);
      }
      if ('name' in details) {
        condition &= details.name == cookie.name;
      }
      if ('secure' in details) {
        condition &= details.secure == cookie.secure;
      }
      if ('session' in details) {
        condition &= details.session == cookie.secure;
      }
      if (condition) {
        result.push(cookie);
      }
    }
    callback(result);
  };

  CookiesAPI.prototype.get = function(details, callback) {
    var url = this.parseUrl(details.url);
    var enumerator = Services.cookies.getCookiesFromHost(url.domain);
    while (enumerator.hasMoreElements()) {
      var next = enumerator.getNext();
      var cookie = this.toCookie(next);
      var condition = true;
      condition &= url.domain == cookie.domain;
      condition &= url.path.indexOf(cookie.path) == 0;
      condition &= details.name == cookie.name;
      if (condition) {
        callback(cookie);
        return;
      }
    }
    // not found
    callback(null);
  };

  /**
   * @param details
   *          {url, name}
   * @param callback
   *          function({url, name})
   */
  CookiesAPI.prototype.remove = function(details, callback) {
    var url = this.parseUrl(details.url);
    Services.cookies.remove(url.host, details.name, url.path, false);
    if (callback) {
      callback({
        url : url.protocol + '://' + url.host + url.path,
        name : name
      });
    }
  };

  module.exports = CookiesAPI;

}).call(this);
