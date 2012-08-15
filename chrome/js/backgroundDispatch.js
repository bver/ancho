; (function () {
  // Map of popup ID to tab containing the popup.
  // Tab ID is null if the popup is in a window that doesn't have tabs.
  // This is used to dispatch the message to close the popup to the right
  // tab (or to all windows).
  // Used for 'borderless popups' which are actually divs containing an iframe.
  var popups = {};
  var callbacks = {};
  var requestId = 1;
  var onRequestListener = null;
  const BORDERLESS_BASE_ID = 50000;
  var currentBorderlessId = BORDERLESS_BASE_ID;

  if (typeof(chrome) === 'undefined') {
    return;
  }

  var dispatch = function(request, sender, callback) {
    if (!('type' in request)) {
      // Not a dispatch command so must be a listener set by the background
      // script.
      if (onRequestListener) {
        onRequestListener(request, sender, callback);
      }
      return;
    }
    if (request.type === 'IsPopup') {
      if (request.id >= BORDERLESS_BASE_ID) {
        callback(true);
      }
      else {
        callback(false);
      }
    }
    else if (request.type === 'RegisterPopup') {
      var borderlessId = currentBorderlessId++;
      if (('tab' in sender) && (sender.tab.id != -1)) {
        popups[borderlessId] = sender.tab.id;
      }
      callback(borderlessId);
    }
    else if (request.type === 'UnregisterPopup') {
      if (request.id >= BORDERLESS_BASE_ID) {
        var closeRequest = { type: 'ClosePopup', id: request.id };
        if (request.id in popups) {
          chrome.tabs.sendRequest(popups[request.id], closeRequest);
        }
        else {
          // Popup is in a window that doesn't have tabs, so dispatch the ClosePopup
          // command to everyone.
          chrome.extension.sendRequest(closeRequest);
        }
      }
      else {
        // TODO: Should be an error.
      }
    }
    else if (request.type === 'XMLHttpRequest') {
      var xhr = new XMLHttpRequest();
      xhr.open(request.method, request.url);
      for (key in request.extraHeaders) {
        xhr.setRequestHeader(key, request.extraHeaders[key]);
      }
      xhr.onreadystatechange = function() {
        var result = {
          readyState: xhr.readyState,
          status: xhr.status,
          statusText: xhr.statusText,
          responseHeaders: xhr.getAllResponseHeaders()
        };
        if (xhr.responseText) {
          result.responseText = xhr.responseText;
        }
        xhr.xml = !!xhr.responseXML;
        callback(result);
      }
      xhr.send(request.data);
    }
    else if (request.type === 'GetLocalStorageData') {
      var localStorage = {};
      for (var i=0; i<window.localStorage.length; i++) {
        var key = window.localStorage.key(i);
        localStorage[key] = btoa(window.localStorage.getItem(key));
      }
      callback(localStorage);
    }
    else if (request.type === 'LocalStorageSet') {
      window.localStorage.setItem(request.data.key, request.data.value);
    }
    else if (request.type === 'LocalStorageRemove') {
      window.localStorage.removeItem(request.data.key);
    }
    else if (request.type === 'APICallback') {
      if (request.callbackId in callbacks) {
        callbacks[request.callbackId].apply(window, request.args);
        delete callbacks[request.callbackId];
      }
    }

    if (request.type !== 'InvokeAPI') {
      return;
    }

    if ('name' in request.data) {
      if (request.data.name === 'extension.onRequest.addListener') {
        // Special handling since we can only have on message listener.
        onRequestListener = callback;
        return;
      }
      var nameSegments = request.data.name.split('.');

      // call directly chrome.extension....
      var api = chrome;
      var context; // this is the context used when invoking the API function
      for (var i = 0; i < nameSegments.length; i++) {
        context = api;
        api = api[nameSegments[i]];
      }

      var args = Array.prototype.slice.call(request.data.arguments);
      if (callback) {
        args.push(callback);
      }

      return api.apply(context, args);
    }
  };

  chrome.extension.onRequest.addListener(dispatch);

  window.addEventListener('message', function(event) {
    var request = event.data;
    var callback;
    if ('id' in request) {
      var callbackId;
      callback = function() {
        var args = Array.prototype.slice.call(arguments);
        if (args.length > 0) {
          var lastArg = args.pop();
          if (typeof(lastArg) === 'function') {
            // Special processing since the callback in this case also has a callback.
            // For example, this might be an onMessage listener whose last parameter
            // is the sendResponse callback.
            callbackId = requestId;
            requestId++;
            callbacks[callbackId] = lastArg;
          }
          else {
            args.push(lastArg);
          }
        }
        var response = { id: request.id, args: args };
        if (callbackId) {
          response.callbackId = callbackId;
        }
        event.source.postMessage(response, '*');
      }
    }

    dispatch(request, null, callback);
  }, false);

  if (typeof(exports) != 'undefined') {
    exports.dispatch = dispatch;
  }
}).call(this);