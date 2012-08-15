; (function () {
  var requestId = 1;
  var callbacks = {};
  var listeners = [];

  function dispatch(request, sender, callback) {
    if (request.type === 'XMLHttpRequest') {
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
    else if (request.type === 'UpdateWindow') {
      // Delegate to our parent since they are responsible for
      // dealing with borderless popups they created.
      parent.postMessage(request, '*');
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
      var nameSegments = request.data.name.split('.');

      var api = chrome;
      var context; // this is the context used when invoking the API function
      var lastSegment = null;
      for (var i = 0; i < nameSegments.length; i++) {
        if (!(nameSegments[i] in api)) {
          // We can't use this API here (we're probably in a content script)
          // so delegate to the background window.
          if (callback) {
            return chrome.extension.sendRequest(request, callback);
          }
          else {
            return chrome.extension.sendRequest(request);
          }
        }
        context = api;
        api = api[nameSegments[i]];
      }

      var args = Array.prototype.slice.call(request.data.arguments);

      if (request.data.name === 'extension.onRequest.addListener') {
        // We manage the message listeners here as well so that we can dispatch
        // messages between frames of the same window.
        listeners.push(callback);
      }
      else if (request.data.name === 'extension.onRequest.removeListener') {
        var index = listeners.indexOf(callback);
        if (index != -1) {
          listeners.splice(index, 1);
        }
      }
      else if (request.data.name === 'extension.sendRequest') {
        for (var i=0; i<listeners.length; i++) {
          var requestArgs = [].concat(args);
          requestArgs.push(null); // sender
          requestArgs.push(callback); // sendResponse
          if (listeners[i].apply(context, requestArgs)) {
            // The listener has asked us to remove it.
            listeners.splice(i, 1);
          }
        }
      }

      if (callback) {
        args.push(callback);
      }
      return api.apply(context, args);
    }
  }

  window.addEventListener('message', function(event) {
    var request = event.data;
    var callback;
    if ('id' in request) {
      var callbackId;
      callback = function() {
        if (!event.source) {
          // The window who registered us is gone.
          return true;
        }
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
}).call(this);