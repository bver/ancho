var apiName = 'chrome.windows';


//------------------------------------------------------------
//          Types extracted for chrome.windows
//------------------------------------------------------------
var types = [
  {
    "id": "Window",
    "properties": {
      "alwaysOnTop": {
        "id": "alwaysOnTop",
        "required": true,
        "type": "boolean"
      },
      "focused": {
        "id": "focused",
        "required": true,
        "type": "boolean"
      },
      "height": {
        "id": "height",
        "required": true,
        "type": "integer"
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
      "left": {
        "id": "left",
        "required": true,
        "type": "integer"
      },
      "state": {
        "id": "state",
        "required": true,
        "type": {
          "enum": [
            "normal",
            "minimized",
            "maximized",
            "fullscreen"
          ],
          "type": "enumerated string"
        }
      },
      "tabs": {
        "id": "tabs",
        "required": false,
        "type": {
          "items": "tabs.Tab",
          "type": "array"
        }
      },
      "top": {
        "id": "top",
        "required": true,
        "type": "integer"
      },
      "type": {
        "id": "type",
        "required": true,
        "type": {
          "enum": [
            "normal",
            "popup",
            "panel",
            "app"
          ],
          "type": "enumerated string"
        }
      },
      "width": {
        "id": "width",
        "required": true,
        "type": "integer"
      }
    },
    "type": "object"
  }
];

//------------------------------------------------------------
//          Methods extracted for chrome.windows
//------------------------------------------------------------
  var methods = [
  {
    "id": "get",
    "items": [
      {
        "id": "windowId",
        "required": true,
        "type": "integer"
      },
      {
        "id": "getInfo",
        "required": false,
        "type": "object"
      },
      {
        "id": "callback",
        "properties": {
          "window": {
            "id": "window",
            "required": true,
            "type": "Window"
          }
        },
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
        "id": "getInfo",
        "required": false,
        "type": "object"
      },
      {
        "id": "callback",
        "properties": {
          "window": {
            "id": "window",
            "required": true,
            "type": "Window"
          }
        },
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
        "id": "createData",
        "properties": {
          "focused": {
            "id": "focused",
            "required": false,
            "type": "boolean"
          },
          "height": {
            "id": "height",
            "required": false,
            "type": "integer"
          },
          "incognito": {
            "id": "incognito",
            "required": false,
            "type": "boolean"
          },
          "left": {
            "id": "left",
            "required": false,
            "type": "integer"
          },
          "tabId": {
            "id": "tabId",
            "required": false,
            "type": "integer"
          },
          "top": {
            "id": "top",
            "required": false,
            "type": "integer"
          },
          "type": {
            "id": "type",
            "required": false,
            "type": {
              "enum": [
                "normal",
                "popup",
                "panel",
                "detached_panel"
              ],
              "type": "enumerated string"
            }
          },
          "url": {
            "id": "url",
            "required": false,
            "type": "string"
          },
          "width": {
            "id": "width",
            "required": false,
            "type": "integer"
          }
        },
        "required": false,
        "type": "object"
      },
      {
        "id": "callback",
        "properties": {
          "window": {
            "id": "window",
            "required": false,
            "type": "Window"
          }
        },
        "required": false,
        "type": "function"
      }
    ],
    "type": "functionArguments"
  },
  {
    "id": "getAll",
    "items": [
      {
        "id": "getInfo",
        "required": false,
        "type": "object"
      },
      {
        "id": "callback",
        "properties": {
          "windows": {
            "id": "windows",
            "required": true,
            "type": {
              "items": "Window",
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
    "id": "update",
    "items": [
      {
        "id": "windowId",
        "required": true,
        "type": "integer"
      },
      {
        "id": "updateInfo",
        "properties": {
          "drawAttention": {
            "id": "drawAttention",
            "required": false,
            "type": "boolean"
          },
          "focused": {
            "id": "focused",
            "required": false,
            "type": "boolean"
          },
          "height": {
            "id": "height",
            "required": false,
            "type": "integer"
          },
          "left": {
            "id": "left",
            "required": false,
            "type": "integer"
          },
          "state": {
            "id": "state",
            "required": false,
            "type": {
              "enum": [
                "normal",
                "minimized",
                "maximized",
                "fullscreen"
              ],
              "type": "enumerated string"
            }
          },
          "top": {
            "id": "top",
            "required": false,
            "type": "integer"
          },
          "width": {
            "id": "width",
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
          "window": {
            "id": "window",
            "required": true,
            "type": "Window"
          }
        },
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
        "id": "windowId",
        "required": true,
        "type": "integer"
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
    "id": "getLastFocused",
    "items": [
      {
        "id": "getInfo",
        "required": false,
        "type": "object"
      },
      {
        "id": "callback",
        "properties": {
          "window": {
            "id": "window",
            "required": true,
            "type": "Window"
          }
        },
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
