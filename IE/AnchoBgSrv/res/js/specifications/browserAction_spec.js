var apiName = 'chrome.browserAction';


//------------------------------------------------------------
//          Types extracted for chrome.browserAction
//------------------------------------------------------------
var types = [
  {
    "id": "ImageDataType",
    "type": "imagedata"
  },
  {
    "id": "ColorArray",
    "type": {
      "items": "integer",
      "type": "array"
    }
  }
]

//------------------------------------------------------------
//          Methods extracted for chrome.browserAction
//------------------------------------------------------------
  var methods = [
  {
    "id": "enable",
    "items": [
      {
        "id": "tabId",
        "required": false,
        "type": "integer"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "setBadgeBackgroundColor",
    "items": [
      {
        "id": "details",
        "properties": {
          "color": {
            "id": "color",
            "required": true,
            "type": [
              "ColorArray",
              "string"
            ]
          },
          "tabId": {
            "id": "tabId",
            "required": false,
            "type": "integer"
          }
        },
        "required": true,
        "type": "object"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "setBadgeText",
    "items": [
      {
        "id": "details",
        "properties": {
          "tabId": {
            "id": "tabId",
            "required": false,
            "type": "integer"
          },
          "text": {
            "id": "text",
            "required": true,
            "type": "string"
          }
        },
        "required": true,
        "type": "object"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "setTitle",
    "items": [
      {
        "id": "details",
        "properties": {
          "tabId": {
            "id": "tabId",
            "required": false,
            "type": "integer"
          },
          "title": {
            "id": "title",
            "required": true,
            "type": "string"
          }
        },
        "required": true,
        "type": "object"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "getBadgeText",
    "items": [
      {
        "id": "details",
        "properties": {
          "tabId": {
            "id": "tabId",
            "required": false,
            "type": "integer"
          }
        },
        "required": true,
        "type": "object"
      },
      {
        "id": "callback",
        "properties": {
          "result": {
            "id": "result",
            "required": true,
            "type": "string"
          }
        },
        "required": true,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "setPopup",
    "items": [
      {
        "id": "details",
        "properties": {
          "popup": {
            "id": "popup",
            "required": true,
            "type": "string"
          },
          "tabId": {
            "id": "tabId",
            "required": false,
            "type": "integer"
          }
        },
        "required": true,
        "type": "object"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "disable",
    "items": [
      {
        "id": "tabId",
        "required": false,
        "type": "integer"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "getTitle",
    "items": [
      {
        "id": "details",
        "properties": {
          "tabId": {
            "id": "tabId",
            "required": false,
            "type": "integer"
          }
        },
        "required": true,
        "type": "object"
      },
      {
        "id": "callback",
        "properties": {
          "result": {
            "id": "result",
            "required": true,
            "type": "string"
          }
        },
        "required": true,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "getBadgeBackgroundColor",
    "items": [
      {
        "id": "details",
        "properties": {
          "tabId": {
            "id": "tabId",
            "required": false,
            "type": "integer"
          }
        },
        "required": true,
        "type": "object"
      },
      {
        "id": "callback",
        "properties": {
          "result": {
            "id": "result",
            "required": true,
            "type": "ColorArray"
          }
        },
        "required": true,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "getPopup",
    "items": [
      {
        "id": "details",
        "properties": {
          "tabId": {
            "id": "tabId",
            "required": false,
            "type": "integer"
          }
        },
        "required": true,
        "type": "object"
      },
      {
        "id": "callback",
        "properties": {
          "result": {
            "id": "result",
            "required": true,
            "type": "string"
          }
        },
        "required": true,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "setIcon",
    "items": [
      {
        "id": "details",
        "properties": {
          "imageData": {
            "id": "imageData",
            "required": false,
            "type": [
              "ImageDataType",
              "object"
            ]
          },
          "path": {
            "id": "path",
            "required": false,
            "type": [
              "string",
              "object"
            ]
          },
          "tabId": {
            "id": "tabId",
            "required": false,
            "type": "integer"
          }
        },
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

