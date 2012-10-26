
define(function() {


  describe('chrome.tabs API', function() {
    var callback = function(aTab){tab = aTab;}
    var address = 'http://www.google.cz';

    beforeEach(function() {

    });

    it('Has important methods defined', function(){
        expect(chrome.tabs.create).toBeDefined();
    });

    it('Can create tabs - callback invoked', function(){
        var callbackSpy = jasmine.createSpy("tab create callback");
        runs( function() {
           chrome.tabs.create({'url': address}, callbackSpy);
        });

        waitsFor(function() {
          return callbackSpy.calls.length > 0;
        }, "Callback fot createTab was not called", 3000);

        runs( function(){
            expect(callbackSpy).toHaveBeenCalled();
            expect(callbackSpy.mostRecentCall.args.length).toBe(1);
        });
    });

    it('Can create tabs - correct tab info passed to callback ', function(){
        var tab;
        var created = false;
        var callback = function(aTab){tab = aTab; created = true;}
        runs( function() {
            chrome.tabs.create({'url': address}, callback);
        });
        waitsFor(function() {
          return created;
        }, "Callback fot createTab was not called", 3000);

        runs( function() {
          expect(tab).toBeDefined();
          expect(tab.id).toBeDefined();
          //expect(tab.url).toMatch(address);
        });
    });
    it('Can remove tabs', function(){
        var tab;
        var tab2;
        var created = false;
        var tabId;
        var removeCallbackCalled = false;
        var getCallbackCalled = false;
        var callback = function(aTab){tab = aTab; created = true;}
        var callbackFromGet = function(aTab){tab2 = aTab; getCallbackCalled = true;}
        runs( function() {
            chrome.tabs.create({'url': address}, callback);
        });
        waitsFor(function() {
          return created;
        }, "Callback fot createTab was not called", 3000);

        runs(function(){
          tabId = tab.id;
          expect(tab).toBeDefined();
          expect(tab.id).toBeDefined();

          chrome.tabs.remove(tabId, function(){ removeCallbackCalled = true; });
        });

        waitsFor(function() {
          return removeCallbackCalled;
        }, "Callback for remove was not called", 3000);

        runs(function(){
          chrome.tabs.get(tabId, callbackFromGet);
        });

        waitsFor(function() {
          return getCallbackCalled;
        }, "Callback for get was not called", 3000);

        runs(function(){
          expect(tab2).not.toBeDefined();
        });
    });

    it('Can query tabs', function(){
        var tabID = 2;
        var tabs;
        var callback2 = function(aTabs){tabs = aTabs;}
        chrome.tabs.query({'id':tabID}, callback2);

        expect(tabs).toBeDefined();
        expect(tabs.length).toBe(1);
        expect(tabs[0].id).toBe(tabID);
    });

    it('Active tab logic', function(){
        var tabs;
        var queryCallbackCalled = false;
        var queryCallback = function(aTabs){tabs = aTabs; queryCallbackCalled = true; }
        waits(1000);

        runs(function(){
          chrome.tabs.query({}, queryCallback);
        });

        waitsFor(function() {
          return queryCallbackCalled;
        }, "Callback for query was not called", 3000);

        runs(function(){
            var activeCount = 0;
            for(var i = 0; i < tabs.length; ++i) {
              if (tabs[i].active) ++activeCount;
            }
            expect(activeCount).toBe(1);
        });
    });


    it('Can update tabs', function(){
        chrome.tabs.update(1, {'url':'http://www.salsitasoft.com/'});
    });

    xit('Can close all tabs except one', function(){
        var tabs;
        var removeCallbackCalled = false;
        var queryCallbackCalled = false;
        var queryCallback = function(aTabs){tabs = aTabs; queryCallbackCalled = true; }
        chrome.tabs.query({}, queryCallback);
        tabIDs = []
        for(var i = 1; i < tabs.length; ++i) {
          tabIDs.push(tabs[i].id);
        }
        runs(function(){
            chrome.tabs.remove(tabIDs, function(){ removeCallbackCalled = true; });
        });

        waitsFor(function() {
          return removeCallbackCalled;
        }, "Callback for remove was not called", 3000);

        runs(function(){
          tabs = null;
          chrome.tabs.query({}, queryCallback);
        });

        waitsFor(function() {
          return queryCallbackCalled;
        }, "Callback for query was not called", 3000);

        runs(function(){
          expect(tabs.length).toBe(1);
        });
    });
  });

});



