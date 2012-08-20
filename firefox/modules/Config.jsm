(function() {
  // An array of objects describing the content scripts that should be applied
  // to webpages. Each content script has a `matches` property that decribes the
  // URLs that it should be applied to and a `js` property containing an array
  // of scripts that should be applied if the URLs match.
  exports.contentScripts = [];
}).call(this);