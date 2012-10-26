(function() {

  module.exports = {
    // An array of objects describing the content scripts that should be applied
    // to webpages. Each content script has a `matches` property that decribes the
    // URLs that it should be applied to and a `js` property containing an array
    // of scripts that should be applied if the URLs match.
    contentScripts: [],
    // An array of script filenames to load into background page.
    backgroundScripts: [],
    // Filename of HTML to load into background window.
    backgroundPage: null
  };

}).call(this);
