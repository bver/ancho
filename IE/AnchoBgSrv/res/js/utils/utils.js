/******************************************************************************
 * tabs.js
 * Part of Ancho browser extension framework
 * Implements chrome.tabs
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 ******************************************************************************/

//******************************************************************************
//* requires
var Event = require("Event.js").Event;

exports.EventFactory = {

  createEvents : function(targetAPI, instanceID, apiName, eventNames) {
    for (i = 0; i < eventNames.length; ++i) {
      targetAPI[eventNames[i]] = new Event(apiName + '.' + eventNames[i], instanceID);
    }
  },

  releaseEvents : function(instanceID, apiName, eventNames) {
    for (i = 0; i < eventNames.length; ++i) {
      addonAPI.removeEventObject(apiName + '.' + eventNames[i], instanceID);
    }
  }
};


//Type checking utilities - instanceof and typeof are not working well when used 
//on objects from different script dispach instances
exports.isArray = function(aArg) {
  return Object.prototype.toString.call(aArg) === '[object Array]';
}

exports.isFunction = function(aArg) {
  return Object.prototype.toString.call(aArg) === '[object Function]'
    || aArg.constructor.toString().match('function Function');
}

exports.isObject = function(aArg) {
  return typeof (aArg) == 'object' && !exports.isFunction(aArg);
}

exports.isString = function(aArg) {
  return Object.prototype.toString.call(aArg) === '[object String]';
}

exports.isInteger = function(aArg) {
  return (typeof(aArg) === 'number') && (Math.ceil(aArg) === aArg);
}

exports.isNumber = function(aArg) {
  return (typeof (aArg) === 'number');
}

exports.typeName = function(aArg) {
  if (aArg === undefined) {
    return undefined;
  }

  var typeCheckers = {
    'function': exports.isFunction,
    'number': exports.isNumber,
    'array': exports.isArray
  };

  for (var checker in typeCheckers) {
    if (typeCheckers[checker](aArg)) {
      return checker;
    }
  }
  return typeof (aArg);
}


exports.cleanWhiteSpace = function(aString) {
  if (typeof (aString) != 'string') {
    throw new Error('Expected string instead of :' + exports.typeName(aString));
  }
  return aString.replace(/^\s+|\s+$/g, '');
}