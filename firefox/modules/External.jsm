EXPORTED_SYMBOLS = ['AnchoExternal'];

var AnchoExternal = {
  /* private: target context with `invoke` method and its setter */
  __context: null,
  __set: function(context) {
    if (context && !context.invoke) {
      throw 'AnchoExtenal::__set() : no invoke function in the provided context.';
    }
    this.__context = context;
  },
  /* public: invoking function, delegated to the `invoke` method of the context */
  invoke: function(/* arguments */) {
    if (!this.__context) {
      throw 'AnchoExternal::invoke() : target context for call delegation not set.';
    }
    return this.__context.invoke.apply(this.__context, arguments);
  }
};
