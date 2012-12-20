(function() {

  function ExternalAPI(state, contentWindow) {
    this.__handlers = {};
  }

  ExternalAPI.prototype = {

    register: function(name, callback) {
      try {
        this.__handlers[name] = callback;
      } catch(e) {
        // pass
      }
      return (name in this.__handlers);
    },

    invoke : function(/* name, args */) {
      var name = arguments[0];
      var args = Array.prototype.slice.call(arguments, 1);
      if ('undefined' === typeof(name)) {
        throw 'ancho.external.invoke() - handler name missing';
      }
      if (!(name in this.__handlers)) {
        throw 'ancho.external.invoke() - handler "' + name + '" not registered';
      }
      return this.__handlers[name].apply(null, args);
    }

  };

  module.exports = ExternalAPI;

}).call(this);
