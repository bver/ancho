$(function() {
  // initialize RequireJS configuration
  requirejs.config(requirejsConfig);

  requirejs([],
  function(ConsoleReporter) {
    chrome.tabs.query({ windowId: chrome.windows.WINDOW_ID_CURRENT, 'active': true}, function(tabs) {
      var tab = tabs[0];

      $('#run-bg-test').on('click', function() {
        chrome.extension.sendMessage({cmd: 'runBgTest'});
      });

      $('#run-content-test').on('click', function() {
        chrome.tabs.sendMessage(tab.id, {cmd: 'runContentTest'});
      });

      $('#run-badge-test').on('click', function() {
        chrome.browserAction.setBadgeText({text: '!', tabId: tab.id});
        chrome.browserAction.setBadgeBackgroundColor({color: '#FFFF00', tabId: tab.id})
      });

    });
  });
});
