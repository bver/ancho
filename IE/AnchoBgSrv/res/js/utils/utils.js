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