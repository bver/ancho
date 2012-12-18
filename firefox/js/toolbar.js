(function() {

  Components.utils.import('resource://gre/modules/Services.jsm');

  var State = require('./state');
  var Toolbar = require('./toolbarSingleton');

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
