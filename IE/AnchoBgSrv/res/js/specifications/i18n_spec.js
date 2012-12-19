var apiName = 'chrome.i18n';


//------------------------------------------------------------
//          Methods extracted for chrome.i18n
//------------------------------------------------------------
var methods = [
  {
    "id": "getAcceptLanguages",
    "items": [
      {
        "id": "callback",
        "properties": {
          "languages": {
            "id": "languages",
            "required": true,
            "type": {
              "items": "string",
              "type": "array"
            }
          }
        },
        "required": true,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "getMessage",
    "items": [
      {
        "id": "messageName",
        "required": true,
        "type": "string"
      },
      {
        "id": "substitutions",
        "required": false,
        "type": "any"
      }
    ],
    "type": "functionArguments"
  }
];

var typeChecking = require("typeChecking.js");
var validatorManager = typeChecking.validatorManager;

for (var i = 0; i < methods.length; ++i) {
  validatorManager.addSpecValidatorWrapper(apiName + '.' + methods[i].id, methods[i]);
}
