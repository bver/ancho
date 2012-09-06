//console.log("CONTENT SCRIPT");
chrome.extension.sendMessage(0, "from CONTENT: " + document.location);
require("jquery.js");

$(document).ready(function() {
  $("a").each(function(){
    $(this).css({"background-color": "#ff0000"});
  });
});