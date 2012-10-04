chrome.extension.sendMessage(undefined, "from CONTENT: " + document.location, function(){
    chrome.console.log('Successfull response!')
});

