$(function() {
// initialize RequireJS configuration
requirejs.config(requirejsConfig);

requirejs(
[
  'common/consoleReporter',
  'content/jquery'
],
function(ConsoleReporter) {
  chrome.extension.onMessage.addListener(function(msg, sender, sendResponse) {
    if (msg.cmd === 'runContentTest') {
      // initialize jasmine
      var jasmineEnv = jasmine.getEnv();
      jasmineEnv.addReporter(new ConsoleReporter(chrome.console ? chrome.console : console));
      jasmineEnv.execute();
    }
  });
});

});
