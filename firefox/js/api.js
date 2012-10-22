(function() {

  // Google Chrome APIs
  var ExtensionAPI = require('./extension');
  var TabsAPI = require('./tabs');
  var WindowsAPI = require('./windows');
  var WebRequestAPI = require('./webRequest');
  var BrowserActionAPI = require('./browserAction');
  var CookiesAPI = require('./cookies');
  var HistoryAPI = require('./history');

  // Ancho APIs
  var ToolbarAPI = require('./toolbar');

  // System APIs
  var ConsoleAPI = require('./console');
  var LocalStorageAPI = require('./localStorage');
  var ClipboardAPI = require('./clipboard');

  // export
  function API(contentWindow, extensionState) {

    this.chrome = {
      extension: new ExtensionAPI(extensionState, contentWindow),
      tabs: new TabsAPI(extensionState, contentWindow),
      windows: new WindowsAPI(extensionState, contentWindow),
      webRequest: new WebRequestAPI(extensionState, contentWindow),
      browserAction: new BrowserActionAPI(extensionState, contentWindow),
      cookies: new CookiesAPI(extensionState, contentWindow),
      history: new HistoryAPI(extensionState, contentWindow)
    };

    this.ancho = {
      toolbar: new ToolbarAPI(extensionState, contentWindow)
    };

    this.console = new ConsoleAPI(extensionState, contentWindow);
    this.localStorage = new LocalStorageAPI(extensionState, contentWindow);
    this.clipboard = new ClipboardAPI(extensionState, contentWindow);
  }

  module.exports = API;

}).call(this);
