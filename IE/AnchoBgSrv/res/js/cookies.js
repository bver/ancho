/******************************************************************************
 * cookies.js
 * Part of Ancho browser extension framework
 * Implements chrome.cookies
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("Event.js").Event;
var EventFactory = require("utils.js").EventFactory;

require("cookies_spec.js");
var preprocessArguments = require("typeChecking.js").preprocessArguments;
var notImplemented = require("typeChecking.js").notImplemented;

var EVENT_LIST = ['onChanged'];
var API_NAME = 'cookies';



// this function just copies IECookie objects to plain Javascript objects,
// so the caller can modificate it
function createCookieCopy(cookie) {
  return {
    domain: cookie.domain,
    path: cookie.path,
    name: cookie.name,
    value: cookie.value,
    expirationDate: cookie.expirationDate
  };
}


exports.invokeEventWithIDispatch = function(aEventName, aIDispatchData) {
  if (aEventName != 'cookies.onChanged') {
    console.error('Event ' + aEventName + ' unsupported!');
    return;
  }
  var cookie = createCookieCopy(aIDispatchData);
  var changeInfo = {
    cause: 'explicit',
    removed: 'false',
    cookie: cookie
  };
  console.debug('Invoking event');
  addonAPI.invokeEventObject(
            'cookies.onChanged',
            -1,
            true, //we are skipping _instanceID
            [changeInfo]
            );
}

//******************************************************************************
//* main closure
var Cookies = function(instanceID) {
  //============================================================================
  // private variables

  //============================================================================
  // public methods

  //----------------------------------------------------------------------------
  // chrome.cookies.get
  this.get = function(details, callback) {
    var args = preprocessArguments('chrome.cookies.get', arguments);

  };

  //----------------------------------------------------------------------------
  // chrome.cookies.getAll
  this.getAll = function(details, callback) {
    var args = preprocessArguments('chrome.cookies.getAll', arguments);
    var cookies = [];
    /*serviceAPI.cookieManager.enumCookies(function(cookie) {
      cookies.push(createCookieCopy(cookie));
    });
    callback(cookies);*/
  };

  //----------------------------------------------------------------------------
  // chrome.cookies.getAllCookieStores
  this.getAllCookieStores = function(callback) {
    var args = notImplemented('chrome.cookies.getAllCookieStores', arguments);
  };

  //----------------------------------------------------------------------------
  // chrome.cookies.remove
  this.remove = function(details, callback) {
    var args = preprocessArguments('chrome.cookies.remove', arguments);
    serviceAPI.cookieManager.removeCookie(args['details'].url, args['details'].name);
    //TODO callback
  };

  //----------------------------------------------------------------------------
  // chrome.cookies.set
  this.set = function(details, callback) {
    var args = preprocessArguments('chrome.cookies.set', arguments);

    var data = "";
    if (args['details'].value) {
      data += args['details'].value;
    }
    data += '; ';

    if (args['details'].domain) {
      data += 'domain=' + args['details'].domain + '; ';
    }

    if (args['details'].expirationDate) {
      var date = new Date(args['details'].expirationDate * 1000);
      data += 'Expires=' + date.toUTCString() + '; ';
    }
    if (args['details'].path) {
      data += 'Path=' + args['details'].path + '; ';
    }
    if (args['details'].secure) {
      data += 'Secure; ';
    }
    if (args['details'].httpOnly) {
      data += 'HttpOnly; ';
    }

    //if (args['details'].storeId) {
    //TODO cookieStores
    //}
    console.debug('Stored cookie : ' + args['details'].url + ' : ' + data);
    serviceAPI.cookieManager.setCookie(args['details'].url, args['details'].name, data);

    //TODO callback
  };

  //============================================================================
  // events

  EventFactory.createEvents(this, instanceID, API_NAME, EVENT_LIST);

  //============================================================================
  //============================================================================
  // main initialization
  /*try {
  serviceAPI.cookieManager.cookiesChangedCallback = function() { console.info("COOOOOOKIES"); };
  } catch (e) {
  console.log(e.description + e.message);
  }*/
}

exports.createAPI = function(instanceID) {
  return new Cookies(instanceID);
}

exports.releaseAPI = function(instanceID) {
  EventFactory.releaseEvents(instanceID, API_NAME, EVENT_LIST);
}