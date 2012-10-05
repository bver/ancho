﻿
define(function() {
 
    
  describe('chrome.events API', function() {
    var event;
    var listener = function(){};
    beforeEach(function() {
      event = new chrome.events.Event('test.event', 0);
    });

    it('has whole event listener API available', function() {
      expect(event.addListener).toBeDefined();
      expect(event.hasListener).toBeDefined();
      expect(event.removeListener).toBeDefined();
      expect(event.hasListeners).toBeDefined();
    });

    it('can add listeners to Event object', function() {

      expect(event.hasListener(listener)).toBe(false);
      event.addListener(listener);
      expect(event.hasListener(listener)).toBe(true);
    });

    it('can remove listeners from Event object', function() {

      event.addListener(listener);
      expect(event.hasListener(listener)).toBe(true);
      event.removeListener(listener);
      expect(event.hasListener(listener)).toBe(false);
    });

    it('has proper hasListeners implementation', function() {
      var listener2 = function(){};
      expect(event.hasListeners()).toBe(false);
      event.addListener(listener);
      event.addListener(listener2);
      expect(event.hasListener(listener)).toBe(true);
      expect(event.hasListener(listener2)).toBe(true);
      expect(event.hasListeners()).toBe(true);
    });
  });


});


