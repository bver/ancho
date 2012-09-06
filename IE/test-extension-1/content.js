//console.log("CONTENT SCRIPT");
chrome.extension.sendMessage(0, "from CONTENT: " + document.location);
