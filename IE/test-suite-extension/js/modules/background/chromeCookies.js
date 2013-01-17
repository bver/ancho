define(function() {

  describe('chrome.cookies API', function() {

    beforeEach(function() {
    });
/*
    it('can getAllCookieStores() cookies', function(){
        var cookieStores;

        runs( function() {
            chrome.cookies.getAllCookieStores(function(aCookieStores){ cookieStores = aCookieStores; });
        });

        waitsFor(function() {
          return cookieStores;
        }, "getAllCookieStores cookies failed", 3000);

        runs( function(){
            expect(cookieStores).not.toBe(null);
        });
    });
*/
    it('can getAll() cookies', function(){
      var cookies;

      runs( function() {
        chrome.cookies.getAll({}, function(aCookies){cookies = aCookies;});
      });

      waitsFor(function() {
        return cookies;
      }, "getAll cookies failed", 3000);

      runs( function(){
        expect(cookies).not.toBe(null);
      });
    });

    describe('set, get and remove cookies', function(){
      var cookie = null;
      var eventInvoked = false;
      var expirationDate = new Date();
      expirationDate.setHours( expirationDate.getHours()+1 );
      expirationDate = expirationDate.getTime()/1000;

      var testUrl = 'http://www.myweb.tst/';
      var testValue = 'TEST';
      var testName = 'testName'
      var eventInvoked = false;

      function onChanged(changeInfo) {
        eventInvoked = eventInvoked || (changeInfo.cookie.name.indexOf(testName) != -1 && changeInfo.cookie.value === testValue);
      }

      function callback(aCookie) {
        cookie = aCookie;
      }

      it('can set cookie, call the callback and catch an event', function(){
        runs( function() {
          chrome.cookies.onChanged.addListener(onChanged);
          chrome.cookies.set(
            {
              url: testUrl,
              value : testValue,
              name : testName,
              expirationDate: expirationDate
            },
            callback
          );
        });

        waitsFor(function() {
          return cookie && eventInvoked;
        }, "Callback for setting cookie was not called", 3000);

        runs(function(){
          expect(cookie).not.toBe(null);
          expect(cookie.value).toBe(testValue);
          expect(eventInvoked).toBe(true);
          chrome.cookies.onChanged.removeListener(onChanged)
        });
      });

      it('can get a cookie', function() {
        cookie = null;

        runs( function() {
          chrome.cookies.get(
            {
              url: testUrl, name : testName
            },
            function(aCookie){cookie = aCookie;}
          );
        });

        waitsFor(function() {
          return cookie;
        }, "Callback for getting cookie was not called", 3000);

        runs(function(){
          expect(cookie).not.toBeNull();
          expect(cookie.value).toEqual(testValue);
          expect(cookie.name).toEqual(testName);
        });
      });
/*
      it('can remove a cookie', function() {
        cookie = null;

        runs(function(){
          chrome.cookies.remove({url: testUrl, name : testName}, callback);
        });

        waitsFor(function() {
          return cookie;
        }, "Callback for removing cookie was not called", 3000);

        runs(function(){
          expect(cookie).not.toBeNull();
          expect(cookie.url).toEqual(testUrl);
          expect(cookie.name).toEqual(testName);

          chrome.cookies.get({url: testUrl, name : testName}, function(aCookie){
            cookie = aCookie;
          });
        });

        waitsFor(function() {
          return cookie ? false : true;
        }, "Callback for getting removed cookie was not called", 3000);

        runs(function() {
          expect(cookie).toBeNull();
        });
      });
*/
      it('doesn\'t fail for undefined cookie', function() {
        cookie = true;

        runs(function(){
          chrome.cookies.get({url: testUrl, name : "ANCHO__UNDEFINED"}, callback);
        });

        waitsFor(function() {
          return cookie ? false : true;
        }, "Callback for getting undefined cookie was not called", 3000);

        runs(function(){
          expect(cookie).toBeNull();
        });
      });
    });
  });
});
