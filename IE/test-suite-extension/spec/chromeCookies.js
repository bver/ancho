
define(function() {


  describe('chrome.cookies API', function() {
    var callback = function(aTab){tab = aTab;}
    var address = 'http://www.google.cz';

    beforeEach(function() {

    });

    it('can getAllCookieStores() cookies', function(){
        var cookieStores = null;
        var callback = function(aCookieStores){cookieStores = aCookieStores;}
        runs( function() {
            chrome.cookies.getAllCookieStores(callback);
        });

        waitsFor(function() {
          return cookieStores != null;
        }, "getAllCookieStores cookies failed", 3000);

        runs( function(){
            expect(cookieStores).not.toBe(null);
        });
    });
    
    it('can getAll() cookies', function(){
        var cookies = null;
        var callback = function(aCookies){cookies = aCookies;}
        runs( function() {
            chrome.cookies.getAll({}, callback);
        });

        waitsFor(function() {
          return cookies != null;
        }, "getAll cookies failed", 3000);

        runs( function(){
            expect(cookies).not.toBe(null);
        });
    });
    
    it('Can set, get and remove cookies', function(){
      
        var cookie = null;
        var cookieWasSet = false;
        var callback = function(aCookie){cookie = aCookie; cookieWasSet = true;}
        var cookieRemoveCalled = false;
        var removedCookieDetails = null;
        var removeCallback = function(aCookieDetails){removedCookieDetails = aCookieDetails; cookieRemoveCalled = true;}
        var testUrl = 'www.myweb.tst';
        var testValue = 'TEST';
        var testName = 'testName'
        runs( function() {
            chrome.cookies.set({url: testUrl, value : testValue, name : testName}, callback);
        });
        waitsFor(function() {
          return cookieWasSet;
        }, "Callback for getting cookie was not called", 3000);

        runs(function(){
          expect(cookie).not.toBe(null);
          expect(cookie.value).toBe(testValue);
          cookie = null;
          cookieWasSet = false;
        });
        
        runs( function() {
            chrome.cookies.get({url: testUrl, name : testName}, callback);
        });
        
        waitsFor(function() {
          return cookieWasSet;
        }, "Callback for getting cookie was not called", 3000);
        
        runs(function(){
          expect(cookie).not.toBe(null);
          expect(cookie.value).toBe(testValue);
          expect(cookie.name).toBe(testName);
          cookie = null;
          cookieWasSet = false;
          chrome.cookies.remove({url: testUrl, name : testName}, removeCallback);
        });
        
        waitsFor(function() {
          return cookieRemoveCalled;
        }, "Callback for removing cookie was not called", 3000);
        
        runs(function(){
          expect(removedCookieDetails).not.toBe(null);
          expect(removedCookieDetails.url).toBe(testUrl);
          expect(removedCookieDetails.name).toBe(testName);
          chrome.cookies.get({url: testUrl, name : testName}, callback);
        });
        
        waitsFor(function() {
          return cookieWasSet;
        }, "Callback for getting cookie was not called", 3000);
        
        runs(function(){
          expect(cookie).toBe(null);
          expect(cookie.value).toBe(testValue);
          expect(cookie.name).toBe(testName);
          cookie = null;
          cookieWasSet = false;
          chrome.cookies.remove({url: testUrl, name : testName}, removeCallback);
        });
    });
    
  });

});



