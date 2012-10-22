(function() {

  function ConsoleAPI(state, contentWindow) {
  }

  ConsoleAPI.prototype = {
    error: function(args) {
      dump('ERROR: ');
      dump(args);
      dump('\n');
    },

    warn: function(args) {
      dump('Warning: ');
      dump(args);
      dump('\n');
    },

    log: function(args) {
      dump('Log: ');
      dump(args);
      dump('\n');
    }
  };

  mnodule.exports = ConsoleAPI;

}).call(this);
