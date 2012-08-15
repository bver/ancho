window.addEventListener('load', function(event) {
  var extensionId = chrome.i18n.getMessage('@@extension_id');
  var hash = document.location.hash.substr(1);
  var path;
  if (hash.indexOf('chrome-extension://') === 0) {
    path = hash;
  }
  else {
    path = document.location.origin;
    if (hash[0] !== '/') {
      path = path + '/';
    }
    path = path + hash;
  }
  var iframe = document.createElement('iframe');
  // Pass the extension ID to the frame so it can simulate chrome.extension.getURL().
  iframe.src = path + '#' + extensionId;
  iframe.frameborder = 0;

  window.addEventListener('message', function(event) {
    if ('resize' in event.data) {
      window.removeEventListener('message', arguments.callee, false);
      var html = document.getElementsByTagName('html')[0];
      html.style.height = document.body.style.height = event.data.resize.height + 'px';
      html.style.width = document.body.style.width = event.data.resize.width + 'px';
    }
  }, false);

  document.body.appendChild(iframe);
}, false);