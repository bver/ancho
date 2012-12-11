var apiName = 'chrome.extension';


//------------------------------------------------------------
//          Types extracted for chrome.extension
//------------------------------------------------------------
var types = [
  {
    "id": "MessageSender",
    "properties": {
      "id": {
        "id": "id",
        "required": true,
        "type": "string"
      },
      "tab": {
        "id": "tab",
        "required": false,
        "type": "tabs.Tab"
      }
    },
    "type": "object"
  },
  {
    "id": "Port",
    "properties": {
      "name": {
        "id": "name",
        "required": true,
        "type": "string"
      },
      "onDisconnect": {
        "id": "onDisconnect",
        "required": true,
        "type": "events.Event"
      },
      "onMessage": {
        "id": "onMessage",
        "required": true,
        "type": "events.Event"
      },
      "postMessage": {
        "id": "postMessage",
        "required": true,
        "type": "function"
      },
      "sender": {
        "id": "sender",
        "required": false,
        "type": "MessageSender"
      }
    },
    "type": "object"
  }
];

//------------------------------------------------------------
//          Methods extracted for chrome.extension
//------------------------------------------------------------
  var methods = [
  {
    "id": "connect",
    "items": [
      {
        "id": "extensionId",
        "required": false,
        "type": "string"
      },
      {
        "id": "connectInfo",
        "properties": {
          "name": {
            "id": "name",
            "required": false,
            "type": "string"
          }
        },
        "required": false,
        "type": "object"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "sendMessage",
    "items": [
      {
        "id": "extensionId",
        "required": false,
        "type": "string"
      },
      {
        "id": "message",
        "required": true,
        "type": "any"
      },
      {
        "id": "responseCallback",
        "properties": {
          "response": {
            "id": "response",
            "required": true,
            "type": "any"
          }
        },
        "required": false,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "getURL",
    "items": [
      {
        "id": "path",
        "required": true,
        "type": "string"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "getViews",
    "items": [
      {
        "id": "fetchProperties",
        "properties": {
          "type": {
            "id": "type",
            "required": false,
            "type": {
              "enum": [
                "tab",
                "infobar",
                "notification",
                "popup"
              ],
              "type": "enumerated string"
            }
          },
          "windowId": {
            "id": "windowId",
            "required": false,
            "type": "integer"
          }
        },
        "required": false,
        "type": "object"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "getBackgroundPage",
    "items": [],
    "type": "functionArguments"
  },
  {
    "id": "isAllowedIncognitoAccess",
    "items": [
      {
        "id": "callback",
        "properties": {
          "isAllowedAccess": {
            "id": "isAllowedAccess",
            "required": true,
            "type": "boolean"
          }
        },
        "required": true,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "isAllowedFileSchemeAccess",
    "items": [
      {
        "id": "callback",
        "properties": {
          "isAllowedAccess": {
            "id": "isAllowedAccess",
            "required": true,
            "type": "boolean"
          }
        },
        "required": true,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "setUpdateUrlData",
    "items": [
      {
        "id": "data",
        "required": true,
        "type": "string"
      }
    ],
    "type": "functionArguments"
  }
];


var typeChecking = require("typeChecking.js");
var validatorManager = typeChecking.validatorManager;

for (var i = 0; i < types.length; ++i) {
  validatorManager.addSpecValidatorWrapper(apiName + '.' + types[i].id, types[i]);
}

for (var i = 0; i < methods.length; ++i) {
  validatorManager.addSpecValidatorWrapper(apiName + '.' + methods[i].id, methods[i]);
}