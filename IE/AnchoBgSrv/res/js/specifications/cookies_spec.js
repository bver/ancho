var apiName = 'chrome.cookies';


//------------------------------------------------------------
//          Types extracted for chrome.cookies
//------------------------------------------------------------
var types = [
  {
    "id": "Cookie", 
    "properties": {
      "domain": {
        "id": "domain", 
        "required": true, 
        "type": "string"
      }, 
      "expirationDate": {
        "id": "expirationDate", 
        "required": false, 
        "type": "double"
      }, 
      "hostOnly": {
        "id": "hostOnly", 
        "required": true, 
        "type": "boolean"
      }, 
      "httpOnly": {
        "id": "httpOnly", 
        "required": true, 
        "type": "boolean"
      }, 
      "name": {
        "id": "name", 
        "required": true, 
        "type": "string"
      }, 
      "path": {
        "id": "path", 
        "required": true, 
        "type": "string"
      }, 
      "secure": {
        "id": "secure", 
        "required": true, 
        "type": "boolean"
      }, 
      "session": {
        "id": "session", 
        "required": true, 
        "type": "boolean"
      }, 
      "storeId": {
        "id": "storeId", 
        "required": true, 
        "type": "string"
      }, 
      "value": {
        "id": "value", 
        "required": true, 
        "type": "string"
      }
    }, 
    "type": "object"
  }, 
  {
    "id": "CookieStore", 
    "properties": {
      "id": {
        "id": "id", 
        "required": true, 
        "type": "string"
      }, 
      "tabIds": {
        "id": "tabIds", 
        "required": true, 
        "type": {
          "items": "integer", 
          "type": "array"
        }
      }
    }, 
    "type": "object"
  }
]

//------------------------------------------------------------
//          Methods extracted for chrome.cookies
//------------------------------------------------------------
var methods = [
  {
    "id": "getAll", 
    "items": [
      {
        "id": "details", 
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
    "id": "getAllCookieStores", 
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
    "id": "set", 
    "items": [
      {
        "id": "details", 
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
        "id": "details", 
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
    "id": "get", 
    "items": [
      {
        "id": "details", 
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
  }
]


var typeChecking = require("typeChecking.js");
var validatorManager = typeChecking.validatorManager;

for (var i = 0; i < types.length; ++i) {
  validatorManager.addSpecValidatorWrapper(apiName + '.' + types[i].id, types[i]);
}

for (var i = 0; i < methods.length; ++i) {
  validatorManager.addSpecValidatorWrapper(apiName + '.' + methods[i].id, methods[i]);
}
