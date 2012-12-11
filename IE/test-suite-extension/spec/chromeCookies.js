
define(function() {


  describe('chrome.cookies API', function() {
    var callback = function(aTab){tab = aTab;}
    var address = 'http://www.google.cz';

    beforeEach(function() {

    });

    xit('can getAllCookieStores() cookies', function(){
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
        var expirationDate = new Date;
        expirationDate.setHours( expirationDate.getHours()+1 );
        expirationDate = expirationDate.getTime()/1000;
        var cookie = null;
        var cookieWasSet = false;
        var callback = function(aCookie){cookie = aCookie; cookieWasSet = true;}
        var cookieRemoveCalled = false;
        var removedCookieDetails = null;
        var removeCallback = function(aCookieDetails){removedCookieDetails = aCookieDetails; cookieRemoveCalled = true;}
        var testUrl = 'http://www.myweb.tst';
        var testValue = 'TEST';
        var testName = 'testName'
        var eventInvoked = false;
				
        describe('setting cookie', function(){
          it('can set cookie, call the callback and catch an event', function(){
            chrome.cookies.onChanged.addListener(function(aChangeInfo){
                  eventInvoked = eventInvoked || (aChangeInfo.cookie.name.indexOf(testName) != -1 && aChangeInfo.cookie.value === testValue);
                });

            runs( function() {
                chrome.cookies.set({url: testUrl, value : testValue, name : testName, expirationDate: expirationDate}, callback);
            });
            waitsFor(function() {
              return cookieWasSet && eventInvoked;
            }, "Callback for setting cookie was not called", 3000);

            runs(function(){
              expect(cookie).not.toBe(null);
              expect(cookie.value).toBe(testValue);
              expect(eventInvoked).toBe(true);
              cookie = null;
              cookieWasSet = false;
            });
          });
        });
        describe('getting a cookie', function(){
          it('can get a cookie', function() {
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
          });
        });
        describe('getting a cookie', function(){
          it('can get a cookie', function() {

              runs(function(){
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
          });
        });

        describe('getting an undefined cookie', function(){
          it('doesn\'t fail for undefined cookie', function() {

            runs(function(){
              chrome.cookies.get({url: testUrl, name : "ANCHO__UNDEFINED"}, callback);
            });
            waitsFor(function() {
              return cookieWasSet;
            }, "Callback for getting undefined cookie was not called", 3000);

            runs(function(){
              expect(cookie).toBe(null);
            });
          });
        });
    });
  });

});

