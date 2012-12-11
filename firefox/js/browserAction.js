(function() {

  var Cu = Components.utils;

  Cu.import('resource://gre/modules/Services.jsm');

  var Event = require('./event');
  var Utils = require('./utils');
  var WindowWatcher = require('./windowWatcher');
  var Config = require('./config');


  var BrowserActionAPI = function() {
    // TODO: this.onClicked = new Event();
    // TODO : get default button icon url
    var urlPrefix = Config.hostExtensionRoot;
    // FIXME: get default extension icon URL from manifest.json
    // FIXME: Config undefined here...
    this.iconEnabled = false && Config.browser_action
        && Config.browser_action.default_icon ? true : false;
    this.currentIcon = this.iconEnabled ? urlPrefix
        + Config.browser_action.default_icon : '';

    this.buttonId = '__ANCHO_CUSTOM_BUTTON__';
    var self = this;
    if (this.iconEnabled) {
      WindowWatcher.register(function(window) {
        self.startup(window);
      }, function(window) {
        self.shutdown(window);
      });
    }
  };

  BrowserActionAPI.prototype._installIcon = function(window) {
    var toolbarId = "nav-bar";
    var id = this.buttonId;
    var document = window.document;
    var toolbar = document.getElementById(toolbarId);
    if (toolbar && !document.getElementById(id)) {
      var toolbarButton = document.createElement("toolbarbutton");
      toolbarButton.setAttribute("id", id);
      toolbarButton.setAttribute("type", "button");
      toolbarButton.setAttribute("image", "");
      toolbarButton
          .setAttribute("class", "toolbarbutton-1 chromeclass-toolbar-additional");
      toolbarButton.setAttribute("label", "button");

      toolbar.appendChild(toolbarButton);
      toolbar.setAttribute("currentset", toolbar.currentSet);
      document.persist(toolbar.id, "currentset");

      var panel = document.createElement("panel");
      var iframe = document.createElement("iframe");
      iframe.flex = 1;
      toolbarButton.appendChild(panel);
      panel.appendChild(iframe);
      toolbarButton
        .addEventListener('click', function() {
          iframe.setAttribute('src', 'about:blank');
          panel.openPopup(toolbarButton, "after_start", 0, 0, false, false);
          // FIXME: get popup URL from manifest.json
          loadHtml(document, iframe, Config.hostExtensionRoot + 'html/popup.html',
            function() {
              var body = iframe.contentDocument.body;
              panel.height = body.scrollHeight + PANEL_MARGIN_SIZE;
              panel.width = body.scrollWidth + PANEL_MARGIN_SIZE;
              iframe.height = body.scrollHeight;
              iframe.width = body.scrollWidth;
              iframe.contentWindow.close = function() {
                panel.hidePopup();
              };
            }
          );
        });
    }
    this._setIcon(window, this.currentIcon, true);
  };

  BrowserActionAPI.prototype.shutdown = function(window) {
    if (this.iconEnabled) {
      var id = this.buttonId;
      var element = window.document.getElementById(id);
      if (element) {
        element.parentNode.removeChild(element);
      }
    }
  };

  BrowserActionAPI.prototype.startup = function(window) {
    if (this.iconEnabled) {
      this._installIcon(window);
    }
  };

  BrowserActionAPI.prototype.getBadgeBackgroundColor = function() {
    throw new Error('Unsupported method');
  };

  BrowserActionAPI.prototype.getBadgeText = function() {
    throw new Error('Unsupported method');
  };

  BrowserActionAPI.prototype.getPopup = function() {
    throw new Error('Unsupported method');
  };

  BrowserActionAPI.prototype.getTitle = function() {
    throw new Error('Unsupported method');
  };

  BrowserActionAPI.prototype.setBadgeBackgroundColor = function() {
    throw new Error('Unsupported method');
  };

  BrowserActionAPI.prototype.setBadgeText = function() {
    throw new Error('Unsupported method');
  };

  BrowserActionAPI.prototype.setPopup = function() {
    throw new Error('Unsupported method');
  };

  BrowserActionAPI.prototype.setTitle = function() {
    throw new Error('Unsupported method');
  };

  BrowserActionAPI.prototype.setIcon = function(details) {
    if (details && details.path) {
      this.currentIcon = details.path;
    } else {
      throw new Error('Unsupported details when setting icon - '
          + JSON.stringify(details));
    }
    this._updateIcon();
  };

  BrowserActionAPI.prototype._setIcon = function(window, iconUrl, notAttach) {
    var id = this.buttonId;
    var element = window.document.getElementById(id);
    // if the button is available set new icon
    if (element) {
      element.style.listStyleImage = 'url("' + iconUrl + '")';
    }
  };

  BrowserActionAPI.prototype._updateIcon = function() {
    var self = this;
    WindowWatcher.forAllWindows(function(window) {
      self._setIcon(window, self.currentIcon);
    });
  };

  module.exports = BrowserActionAPI;

}).call(this);
