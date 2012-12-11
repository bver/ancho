(function() {
  var Cc = Components.classes;
  var Ci = Components.interfaces;
  var Cu = Components.utils;

  Cu.import('resource://gre/modules/Services.jsm');

  var State = require('./state');
  var loadHtml = require('./scripting').loadHtml;
  var Config = require('./config');

  var Toolbar = {
    TOOLBAR_ID: '__ANCHO_TOOLBAR__',
    EXTENSION_ROOT_DIR: Config.hostExtensionRoot,

    // Remember the current options of the toolbar.
    // null means there is no active toolbar present.
    currentOptions: null,

    remove: function(win) {
      this.currentOptions = null; // remember there is no Toolbar

      var toolbox = win.document.getElementById('navigator-toolbox');
      var toolbar = win.document.getElementById(Toolbar.TOOLBAR_ID);
      if (toolbox && toolbar) {
        toolbox.removeChild(toolbar);
      }
    },

    create: function(win, options) {
      this.currentOptions = options; // remember the active state of Toolbar

      var toolbox = win.document.getElementById('navigator-toolbox');
      if (!toolbox) {
        return;
      }
      var toolbar = win.document.createElement('toolbar');
      toolbar.id = Toolbar.TOOLBAR_ID;
      if (options.height) {
        toolbar.setAttribute('height', options.height);
      }
      toolbar.setAttribute('style', 'padding: 0px');
      toolbar.setAttribute('class', 'toolbar-primary');
      if (options.title) {
        toolbar.setAttribute('toolbarname', options.title);
      }

      // Create an iframe inside the toolbar to hold the HTML content.
      var iframe = win.document.createElement('iframe');
      iframe.setAttribute('flex', '1');

      // Attach the elements to the document.
      toolbar.appendChild(iframe);
      toolbox.appendChild(toolbar);

      iframe.addEventListener('DOMContentLoaded', function(event) {
        iframe.removeEventListener('DOMContentLoaded', arguments.callee, false);
        loadHtml(win.document, iframe, Toolbar.EXTENSION_ROOT_DIR + options.html);
      }, false);

      // Return the unloader
      return function() {
        Toolbar.remove(win);
      }
    },

    createToolbarIfActive: function(win) {
      var toolbar = win.document.getElementById(Toolbar.TOOLBAR_ID);
      if (this.currentOptions && !toolbar) {
        return this.create(win, this.currentOptions);
      }
      return null;
    }
  };


  function ToolbarAPI(state, contentWindow) {
  }

  ToolbarAPI.prototype = {
    show: function(options) {

      // Ensure toolbar reflects the embedded features state.
      // It fixes the bug when enabling/disabling toolbar parts like
      // dropdown_menu, current_site_icons or search_box.
      this.hide();

      var browserWindows = Services.wm.getEnumerator('navigator:browser');

      while (browserWindows.hasMoreElements()) {
        var win = browserWindows.getNext();

        var toolbar = win.document.getElementById(Toolbar.TOOLBAR_ID);
        if (!toolbar) { // do not create toolbar twice
          State.registerUnloader(win, Toolbar.create(win,options));
        }
      }
    },

    hide: function() {
      var browserWindows = Services.wm.getEnumerator('navigator:browser');

      while (browserWindows.hasMoreElements()) {
        var win = browserWindows.getNext();
        Toolbar.remove(win);
      }
    }
  };

  module.exports = ToolbarAPI;

}).call(this);
