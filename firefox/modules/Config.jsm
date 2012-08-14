EXPORTED_SYMBOLS = ["Config"];

// When I tried to declare the Config properties inside the curly braces, Firefox told me:
// *** Compartment mismatch 0x119390000 vs. 0x1178db000
// It then froze completely.
// When I have time I'll create a simple test case and file a bug with Mozilla.

var Config = {};

// Paths that should be searched for modules that are required using a
// relative path that does not being with . or ..
// Stored as an array of nsIURI.
Config.MODULE_SEARCH_PATH = [];

// An array of objects describing the content scripts that should be applied
// to webpages. Each content script has a `matches` property that decribes the
// URLs that it should be applied to and a `js` property containing an array
// of scripts that should be applied if the URLs match.
Config.contentScripts = [];
