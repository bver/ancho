; (function () {
  var delegate;
  if (document.location.hash.indexOf('__sandboxed__') != -1) {
    // Our parent is sandboxed so delegate to top-level window.
    delegate = top;
  }
  else {
    delegate = parent;
  }

  var popups = [];
  var callbacks = {};
  var requestId = 0;
  var localStorage = null;

  function DispatchedXMLHttpRequest() {
    this._extraHeaders = {};
    this._responseHeaderMap = {};
  }

  DispatchedXMLHttpRequest.prototype = {
    get responseXML() { return this._responseXML; },
    get responseText() { return this._responseText; },
    get status() { return this._status; },
    get statusText() { return this._statusText; },
    getAllResponseHeaders: function() { return this._responseHeaders; },
    getResponseHeader: function(header) { return this._responseHeaderMap[header]; },
    open: function(method, url, async) {
      // Only async is supported.
      // TODO: Report error if async is false.
      this._method = method;
      this._url = url;
    },
    send: function(body) {
      var self = this;
      callbacks[requestId] = function(response) {
        self._readyState = response.readyState;
        self._status = response.status;
        self._statusText = response.statusText;
        self._responseText = response.responseText;
        if (response.xml) {
          var parser = new DOMParser();
          self._responseXML = parse.parseFromString(self._responseText, 'application/xml');
        }
        self._responseHeaders = response.responseHeaders;
        var headers = self._responseHeaders.split('\n');
        for (var i=0; i<headers.length; i++) {
          if (!headers[i]) {
            // Skip blank lines.
            continue;
          }
          var header = headers[i].split(':');
          var key = header[0].replace(/^\s\s*/, '').replace(/\s\s*$/, '');
          var value = header[1].replace(/^\s\s*/, '').replace(/\s\s*$/, '');
          self._responseHeaderMap[key] = value;
        }
        if (self._readyStateCallback) {
          self._readyStateCallback.apply(self);
        }
        if (self._readyState != 4) {
          // Not done yet so don't remove the callback.
          return true;
        }
      };
      var request = {
        type: 'XMLHttpRequest',
        id: requestId,
        method: this._method,
        url: this._url,
        extraHeaders: this._extraHeaders,
        data: body
      };
      delegate.postMessage(request, '*');
      requestId++;
    },
    setRequestHeader: function(header, value) {
      this._extraHeaders[header] = value;
    },
    get readyState() { return this._readyState; },
    set onreadystatechange(callback) { this._readyStateCallback = callback; }
  };

  if (!('extension' in chrome) && jQuery) {
    jQuery.ajaxSettings.xhr = function() { return new DispatchedXMLHttpRequest(); }
  }

  var HelperFunctions = {
    _invokeApi: function (name, input_args, keepCallback) {
      var nameSegments = name.split('.');
      if (nameSegments[0] in chrome) {
        var api = chrome;
        var context; // this is the context used when invoking the API function
        for (var i = 0; i < nameSegments.length; i++) {
          context = api;
          api = api[nameSegments[i]];
        }

        return api.apply(context, input_args);
      }
      else {
        this._invokeBackgroundApi(name, input_args, keepCallback);
      }
    },

    _invokeBackgroundApi: function (name, input_args, keepCallback) {
      if ('tabs' in chrome) { // Non-hacky way to do this?
        return this._invokeApi.apply(this, arguments);
      }

      // arguments serializes to an associative array in JSON, so we convert to a normal array first
      var args = Array.prototype.slice.call(input_args);

      var request = { name: name, arguments: args };
      if (typeof (args[args.length - 1]) === 'function') {
        var callback = args[args.length - 1];
        args.splice(args.length - 1);
        request = { name: name, arguments: args };
      }
      else {
        callback = null;
      }
      if ('extension' in chrome) {
        if (callback) {
          chrome.extension.sendRequest({ type: 'InvokeAPI', data: request }, callback);
        }
        else {
          chrome.extension.sendRequest({ type: 'InvokeAPI', data: request });
        }
      }
      else {
        var payload = { type: 'InvokeAPI', data: request };
        if (callback) {
          callbacks[requestId] = function() {
            callback.apply(window, arguments);
            // TODO: If we keep the callback we should keep track of when it is removed and remove it
            // from the callback map as well.
            return keepCallback;
          }
          payload.id = requestId;
        }
        delegate.postMessage(payload, '*');
        requestId++;
      }
    }
  }

  aji = {
    extension: {

      sendRequest: function () {
        HelperFunctions._invokeApi('extension.sendRequest', arguments);
      },

      onRequest: {
        addListener: function () {
          HelperFunctions._invokeApi('extension.onRequest.addListener', arguments, true);
        },

        removeListener: function() {
          HelperFunctions._invokeApi('extension.onRequest.removeListener', arguments);
        }
      },

      connect: function () {
        HelperFunctions._invokeApi('extension.connect', arguments);
      },

      onConnect: {
        addListener: function () {
          HelperFunctions._invokeApi('extension.onConnect.addListener', arguments, true);
        }
      },

      getURL: function (path) {
        if (!('extension' in chrome)) {
          // We're sandboxed so simulate this call.
          if (path.indexOf('chrome-extension://') === 0) {
            return path;
          }
          else {
            if (path[0] === '/') {
              path = path.substr(1);
            }
            path = 'chrome-extension://' + document.location.hash.substr(1) + '/' + path;
            return path;
          }
        }
        var dot = path.lastIndexOf('.');
        if (dot != -1 && path.substr(dot+1) === 'html') {
          // Load HTML pages in a sandboxed frame to get around CSP restrictions.
          path = 'html/chrome_wrapper.html#' + path;
        }
        return HelperFunctions._invokeApi('extension.getURL', [path]);
      }
    },

    tabs: {
      getCurrent: function() {
        HelperFunctions._invokeBackgroundApi('tabs.getCurrent', arguments);
      },

      getSelected: function() {
        HelperFunctions._invokeBackgroundApi('tabs.getSelected', arguments);
      },

      create: function () {
        HelperFunctions._invokeBackgroundApi('tabs.create', arguments);
      },

      update: function () {
        HelperFunctions._invokeBackgroundApi('tabs.update', arguments);
      },

      sendRequest: function () {
        HelperFunctions._invokeBackgroundApi('tabs.sendRequest', arguments);
      },

      executeScript: function () {
        HelperFunctions._invokeBackgroundApi('tabs.executeScript', arguments);
      },

      onActivated: {
        addListener: function () {
          HelperFunctions._invokeBackgroundApi('tabs.onActivated.addListener', arguments, true);
        }
      },

      onRemoved: {
        addListener: function () {
          HelperFunctions._invokeBackgroundApi('tabs.onRemoved.addListener', arguments);
        }
      }
    },

    windows: {
      _screenToViewportX: function(x) {
        return Math.floor(x - window.screenLeft + $(window).scrollLeft());
      },

      _screenToViewportY: function(y) {
        return Math.floor(y - window.screenTop + $(window).scrollTop());
      },

      getCurrent: function(getInfo, callback) {
        var match = document.location.href.match(/[^#]*#__borderless__(\d+)/);
        if (match) {
          callback({ id: match[1] });
        }
        else {
          HelperFunctions._invokeBackgroundApi('windows.getCurrent', arguments);
        }
      },

      create: function(createData, callback) {
        var self = this;
        if (createData.borderless) {
          // Tell the background window about the new popup and get the next free ID.
          aji.extension.sendRequest({ type: 'RegisterPopup' }, function(borderlessId) {
            // Can't create borderless window using the chrome.windows.create API, so
            // we simulate using a floating div containing an iframe.
            var div = document.createElement('div');
            div.id = borderlessId;
            div.style.position = 'absolute';
            div.style.left = self._screenToViewportX(createData.left) + 'px';
            div.style.top = self._screenToViewportY(createData.top) + 'px';
            div.style.zIndex = '3000';
            div.style.backgroundColor = 'white';
            div.style.border = 'none';
            div.style.display = 'inline-block';
            if ('width' in createData) {
              div.style.width = createData.width;
            }
            else {
              div.style.width = 100;
            }
            if ('height' in createData) {
              div.style.height = createData.height;
            }
            else {
              div.style.height = 100;
            }
            var iframe = document.createElement('iframe');
            iframe.style.border = 'none';
            // TODO: What happens if the URL already has a fragment identifier?
            iframe.src = createData.url + '#__borderless__' + div.id;
            if (!('extension' in chrome)) {
              // Let the iframe know that the parent is sandboxed.
              iframe.src = iframe.src + '__sandboxed__';
            }
            iframe.style.height = '100%';
            iframe.style.width = '100%';

            window.addEventListener('message', function(event) {
              var data = event.data;
              if ('updateInfo' in data) {
                var updateInfo = data.updateInfo;
                var winId = data.id;
                if (updateInfo.height) {
                  iframe.style.height = div.style.height = updateInfo.height + 'px';
                }
                if (updateInfo.width) {
                  iframe.style.width = div.style.width = updateInfo.width + 'px';
                }
                if (updateInfo.left) {
                  div.style.left = self._screenToViewportX(updateInfo.left) + 'px';
                }
                if (updateInfo.top) {
                  div.style.top = self._screenToViewportY(updateInfo.top) + 'px';
                }
                // See comment in onUpdated. This belongs in the background window.
                for (var i=0; i<aji.windows.onUpdated._listeners.length; i++) {
                  listener = aji.windows.onUpdated._listeners[i];
                  listener(winId, updateInfo);
                }
              }
            });

            div.appendChild(iframe);

            document.body.appendChild(div);
            document.body.addEventListener('click', function(event) {
              document.body.removeEventListener('click', arguments.callee, false);
              // Tell the background window to unregister the popup.
              // This will cause it to be closed.
              aji.extension.sendRequest({ type: 'UnregisterPopup', id: div.id });
            }, false);

            // Remember the popup so we can unregister it when we unload.
            popups.push(div);

            if (callback) {
              callback({ id: div.id });
            }
          });
        }
        else {
          HelperFunctions._invokeBackgroundApi('windows.create', arguments);
        }
      },

      remove: function(id) {
        aji.extension.sendRequest({ type: 'IsPopup', id: id}, function(isPopup) {
          if (isPopup) {
            aji.extension.sendRequest({ type: 'UnregisterPopup', id: id });
          }
          else {
            HelperFunctions._invokeBackgroundApi('window.remove', arguments);
          }
        });
      },

      update: function(winId, updateInfo, callback) {
        aji.extension.sendRequest({ type: 'IsPopup', id: winId }, function(isPopup) {
          if (isPopup) {
            // TODO: Implement winId parameter.
            // Right now it only works for the current window.
            parent.postMessage({ type: 'UpdateWindow', id: winId, updateInfo: updateInfo }, '*');
          }
          else {
            HelperFunctions._invokeBackgroundApi('window.update', arguments);
          }
        });
      },

      onRemoved: {
        // We maintain our own list of listeners as well for borderless popups.
        _listeners: [],

        addListener: function (listener) {
          this._listeners.push(listener);
          HelperFunctions._invokeBackgroundApi('windows.onRemoved.addListener', arguments, true);
        },

        removeListener: function(listener) {
          var index = this._listeners.indexOf(listener);
          if (index != -1) {
            this._listeners.splice(index, 1);
          }
          HelperFunctions._invokeBackgroundApi('windows.onRemoved.removeListener', arguments);
        }
      },

      onUpdated: {
        // We maintain our own list of listeners since Chrome doesn't support this event
        // currently.
        // Note that this will only alert the window that makes the windows.update() call
        // since we are maintaining a separate list of listeners for each instance of the aji
        // object (i.e. each window).
        // The right solution is to have the background dispatcher maintain the list of listeners
        // and notify them when appropriate. This isn't entirely trivial since you can't pass
        // functions (like listeners) in requests between windows. I imagine we can fix this by
        // having each aji object register with the background and receive a request whenever
        // there is a message it might be interested in.
        _listeners: [],

        addListener: function (listener) {
          this._listeners.push(listener);
        },

        removeListener: function(listener) {
          var index = this._listeners.indexOf(listener);
          if (index != -1) {
            this._listeners.splice(index, 1);
          }
        }
      }
    },

    localStorage: {
      getItem : function(key) {
        if (localStorage) {
          return localStorage[key];
        }
        if (this.hasValue(key)) {
          return window.localStorage.getItem(key);
        }
      },
      setItem : function(key, value) {
        if (localStorage) {
          localStorage[key] = value;
          return delegate.postMessage({ type: 'LocalStorageSet', data: { key: key, value: value }}, '*');
        }
        window.localStorage.setItem(key, value);
      },
      hasValue : function(key) {
        if (localStorage) {
          return 'key' in localStorage;
        }
        return key in window.localStorage;
      },
      removeItem : function(key) {
        if (localStorage) {
          delete localStorage[key];
          return delegate.postMessage({ type: 'LocalStorageRemove', data: { key: key }}, '*');
        }
        window.localStorage.removeItem(key);
      },
      getKeys : function() {
        if (localStorage) {
          return localStorage;
        }
        return window.localStorage;
      },
      addLocalStorageReadyFn : function(callback) {
        if (!('extension' in chrome)) {
          callbacks[requestId] = function(data) {
            localStorage = {};
            localStorage.getItem = function(key) { return this[key] };
            localStorage.setItem = function(key, value) { this[key] = value; };
            for (key in data) {
              localStorage[key] = atob(data[key]);
            }
            callback(localStorage);
          }
          delegate.postMessage({ type: 'GetLocalStorageData', id: requestId }, '*');
          requestId++;
        }
        else {
          // IE implementation requires this waiting callback for background.js code.
          // TODO: make it unnecessary.
          callback(this); // We have window.localStorage always ready on Chrome.
        }
      }
    },

    browserAction: {
      setPopup: function() {
        HelperFunctions._invokeBackgroundApi('browserAction.setPopup', arguments);
      },
      setIcon: function() {
        HelperFunctions._invokeBackgroundApi('browserAction.setIcon', arguments);
      },
      setBadgeText: function() {
        HelperFunctions._invokeBackgroundApi('browserAction.setBadgeText', arguments);
      },
      setTitle: function() {
        HelperFunctions._invokeBackgroundApi('browserAction.setTitle', arguments);
      }
    },

    toolbar: {
      show: function(options) {
        aji.browserAction.setPopup({ popup: 'html/chrome_wrapper.html#' + options.html });

        if (options.icon) {
          aji.browserAction.setIcon({ path: options.icon });
        }

        aji.browserAction.setBadgeText({ text: '' });

        if (options.title) {
          aji.browserAction.setTitle({ title: options.title });
        }
      },

      hide: function() {
        aji.browserAction.setPopup({ popup: '' });

        var iconPath = '../images/action_none.png';
        aji.browserAction.setIcon({ path: iconPath });

        aji.browserAction.setBadgeText({ text: '' });

        aji.browserAction.setTitle({ title: ' ' }); // workaround for bug #31811409
      }
    },

    console: {
      error: function(args) {
        console.error(args);
      },

      warn: function(args) {
        console.warn(args);
      },

      log: function(args) {
        console.log(args);
      }
    }
  };

  if (typeof(exports) !== 'undefined') {
    exports.aji = aji;
  }
  else if (typeof(window) != 'undefined') {
    window.aji = aji;
  }

  aji.extension.onRequest.addListener(function(request, sender) {
    if (('type' in request) && (request.type === 'ClosePopup')) {
      var element = document.getElementById(request.id);
      var index = popups.indexOf(element);
      if (index != -1) {
        popups.splice(index, 1);
      }
      if (element) {
        document.body.removeChild(element);
        // Alert listeners that the window has been removed.
        for (var i=0; i<aji.windows.onRemoved._listeners.length; i++) {
          listener = aji.windows.onRemoved._listeners[i];
          listener(request.id);
        }
      }
    }
  });

  window.addEventListener('unload', function(event) {
    // Unregister all the popups so the background window can free the associated
    // metadata.
    for (var index=0; index<popups.length; index++) {
      aji.extension.sendRequest({ type: 'UnregisterPopup', id: popups[index].id });
    }

    popups = null;
  }, false);

  if (!('extension' in chrome)) {
    window.addEventListener('message', function(event) {
      if ('args' in event.data) {
        var args = event.data.args;
        if ('callbackId' in event.data) {
          args.push(function() {
            delegate.postMessage({ type: 'APICallback', callbackId: event.data.callbackId,
              args: Array.prototype.slice.call(arguments) }, '*');
          });
        }
        if (event.data.id in callbacks) {
          if (!callbacks[event.data.id].apply(window, args)) {
            // Unless the callback returns true, we are done with it.
            delete callbacks[event.data.id];
          }
        }
      }
    }, false);
  }

  window.addEventListener('load', function(event) {
    parent.postMessage({ resize: { height: document.body.scrollHeight, width: document.body.scrollWidth }}, '*');
  }, false);
}).call(this);
