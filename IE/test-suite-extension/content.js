chrome.extension.sendMessage(undefined, "from CONTENT: " + document.location, function(){
    chrome.console.log('Successfull response! But you should see this only once.')
});

