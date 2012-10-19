
define(function() {


  describe('chrome.tabs API', function() {
    var callback = function(aTab){tab = aTab;}
    var address = 'http://www.google.cz';

    beforeEach(function() {

    });

    it('Has important methods defined', function(){
        expect(chrome.tabs.create).toBeDefined();
        //expect(event.hasListener).toBeDefined();
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
        var tabs;
        var callback2 = function(aTabs){tabs = aTabs;}
        chrome.tabs.query({'id':2}, callback2);

        expect(tabs).toBeDefined();
        expect(tabs.length).toBe(1);
    });


    it('Can update tabs', function(){
        chrome.tabs.update(2, {'active':true});
    });

    it('Can close all tabs', function(){
        var tabs;
        var callback2 = function(aTabs){tabs = aTabs;}
        chrome.tabs.query({}, callback2);
        tabIDs = []
        for(var i = 0; i < tabs.length; ++i) {
          tabIDs.push(tabs[i].id);
        }
        chrome.tabs.remove(tabIDs, function(){});
    });
  });

});



