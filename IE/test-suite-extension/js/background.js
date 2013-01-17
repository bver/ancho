// initialize RequireJS configuration
requirejs.config(requirejsConfig);

requirejs(
[
  'common/consoleReporter',
  'background/chromeExtension',
  'background/chromeWindows',
  'background/chromeTabs',
  'background/chromeCookies',
  //'background/chromeEvents',
  'background/document'
],
function(ConsoleReporter) {
  chrome.extension.onMessage.addListener(function(msg, sender, sendResponse) {
    if (msg.cmd === 'runBgTest') {
      // initialize jasmine
      var jasmineEnv = jasmine.getEnv();
      jasmineEnv.addReporter(new ConsoleReporter(console));
      jasmineEnv.execute();
    }
  });
});
