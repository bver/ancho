/*chrome.extension.sendMessage(undefined, "from CONTENT: " + document.location, function(){
    chrome.console.log('Successfull response! But you should see this only once for every tab.')
});
try {
  chrome.console.log('Creating connection');
  port = chrome.extension.connect();
  chrome.console.log('Content script: Adding listener to port.onMessage');
  port.onMessage.addListener( function(msg) {
      chrome.console.log('Content script: Message from port recieved: ' + msg);
  });
  chrome.console.log('Content script: Sending message');
  port.postMessage('First message posted through port');
} catch (e) {
  chrome.console.error('Error: ' + e.message);
}*/
