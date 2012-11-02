var apiName = "chrome.tabs";


//------------------------------------------------------------
//          Types extracted for chrome.tabs
//------------------------------------------------------------
var types = [
  {
    "id": "Tab",
    "properties": {
      "active": {
        "id": "active",
        "required": true,
        "type": "boolean"
      },
      "favIconUrl": {
        "id": "favIconUrl",
        "required": false,
        "type": "string"
      },
      "highlighted": {
        "id": "highlighted",
        "required": true,
        "type": "boolean"
      },
      "id": {
        "id": "id",
        "required": true,
        "type": "integer"
      },
      "incognito": {
        "id": "incognito",
        "required": true,
        "type": "boolean"
      },
      "index": {
        "id": "index",
        "required": true,
        "type": "integer"
      },
      "openerTabId": {
        "id": "openerTabId",
        "required": false,
        "type": "integer"
      },
      "pinned": {
        "id": "pinned",
        "required": true,
        "type": "boolean"
      },
      "status": {
        "id": "status",
        "required": false,
        "type": "string"
      },
      "title": {
        "id": "title",
        "required": false,
        "type": "string"
      },
      "url": {
        "id": "url",
        "required": true,
        "type": "string"
      },
      "windowId": {
        "id": "windowId",
        "required": true,
        "type": "integer"
      }
    },
    "type": "object"
  },
  {
    "id": "InjectDetails",
    "properties": {
      "allFrames": {
        "id": "allFrames",
        "required": false,
        "type": "boolean"
      },
      "code": {
        "id": "code",
        "required": false,
        "type": "string"
      },
      "file": {
        "id": "file",
        "required": false,
        "type": "string"
      },
      "runAt": {
        "id": "runAt",
        "required": false,
        "type": {
          "enum": [
            "document_start",
            "document_end",
            "document_idle"
          ],
          "type": "enumerated string"
        }
      }
    },
    "type": "object"
  }
];

//------------------------------------------------------------
//          Methods extracted for chrome.tabs
//------------------------------------------------------------
  var methods = [
  {
    "id": "executeScript",
    "items": [
      {
        "id": "tabId",
        "required": false,
        "type": "integer"
      },
      {
        "id": "details",
        "required": true,
        "type": "InjectDetails"
      },
      {
        "id": "callback",
        "required": false,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "get",
    "items": [
      {
        "id": "tabId",
        "required": true,
        "type": "integer"
      },
      {
        "id": "callback",
        "required": true,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "getCurrent",
    "items": [
      {
        "id": "callback",
        "required": true,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "create",
    "items": [
      {
        "id": "createProperties",
        "required": true,
        "type": "object"
      },
      {
        "id": "callback",
        "required": false,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "move",
    "items": [
      {
        "id": "tabIds",
        "required": true,
        "type": [
          {
            "items": "integer",
            "type": "array"
          },
          "integer"
        ]
      },
      {
        "id": "moveProperties",
        "required": true,
        "type": "object"
      },
      {
        "id": "callback",
        "required": false,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "update",
    "items": [
      {
        "id": "tabId",
        "required": false,
        "type": "integer"
      },
      {
        "id": "updateProperties",
        "required": true,
        "type": "object"
      },
      {
        "id": "callback",
        "required": false,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "remove",
    "items": [
      {
        "id": "tabIds",
        "required": true,
        "type": [
          {
            "items": "integer",
            "type": "array"
          },
          "integer"
        ]
      },
      {
        "id": "callback",
        "required": false,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "captureVisibleTab",
    "items": [
      {
        "id": "windowId",
        "required": false,
        "type": "integer"
      },
      {
        "id": "options",
        "required": false,
        "type": "object"
      },
      {
        "id": "callback",
        "required": true,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "reload",
    "items": [
      {
        "id": "tabId",
        "required": false,
        "type": "integer"
      },
      {
        "id": "reloadProperties",
        "required": false,
        "type": "object"
      },
      {
        "id": "callback",
        "required": false,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "sendMessage",
    "items": [
      {
        "id": "tabId",
        "required": true,
        "type": "integer"
      },
      {
        "id": "message",
        "required": true,
        "type": "any"
      },
      {
        "id": "responseCallback",
        "required": false,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "connect",
    "items": [
      {
        "id": "tabId",
        "required": true,
        "type": "integer"
      },
      {
        "id": "connectInfo",
        "required": false,
        "type": "object"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "insertCSS",
    "items": [
      {
        "id": "tabId",
        "required": false,
        "type": "integer"
      },
      {
        "id": "details",
        "required": true,
        "type": "InjectDetails"
      },
      {
        "id": "callback",
        "required": false,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "highlight",
    "items": [
      {
        "id": "highlightInfo",
        "required": true,
        "type": "object"
      },
      {
        "id": "callback",
        "required": true,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "query",
    "items": [
      {
        "id": "queryInfo",
        "required": true,
        "type": "object"
      },
      {
        "id": "callback",
        "required": true,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "detectLanguage",
    "items": [
      {
        "id": "tabId",
        "required": false,
        "type": "integer"
      },
      {
        "id": "callback",
        "required": true,
        "type": "function"
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

