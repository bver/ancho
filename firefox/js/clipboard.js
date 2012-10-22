(function() {

  var Cc = Components.classes;

  function ClipboardAPI(state, contentWindow) {
  }

  ClipboardAPI.prototype = {

    copy : function(data) {
      var gClipboardHelper = Cc["@mozilla.org/widget/clipboardhelper;1"].
      getService(Ci.nsIClipboardHelper);
      gClipboardHelper.copyString(data);
    }

  };

  mnodule.exports = ClipboardAPI;

}).call(this);
