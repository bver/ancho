define(function() {

  describe('chrome.tabs API', function() {
    var address = 'http://www.google.cz';

    beforeEach(function() {
    });

    it('Has important methods defined', function(){
      expect(chrome.tabs.create).toBeDefined();
      expect(chrome.tabs.remove).toBeDefined();
      expect(chrome.tabs.query).toBeDefined();
      expect(chrome.tabs.get).toBeDefined();
      expect(chrome.tabs.update).toBeDefined();
    });

    it('Can create tabs - callback invoked', function(){
      var callback = jasmine.createSpy("tab create callback");

      runs( function() {
         chrome.tabs.create({'url': address}, callback);
      });

      waitsFor(function() {
        return callback.calls.length > 0;
      }, "Callback fot createTab was not called", 3000);

      runs( function(){
          expect(callback).toHaveBeenCalled();
          expect(callback.mostRecentCall.args.length).toBe(1);
      });
    });

    it('Can create tabs - correct tab info passed to callback ', function(){
      var tab;

      runs( function() {
        chrome.tabs.create({'url': address}, function(aTab) {tab = aTab;});
      });

      waitsFor(function() {
        return tab;
      }, "Callback fot createTab was not called", 3000);

      runs( function() {
        expect(tab).toBeDefined();
        expect(tab.id).toBeDefined();
        //expect(tab.url).toMatch(address); // currently not supported
      });
    });

    it('Can remove tabs', function(){
        var tab;
        var removeCallbackCalled = false, getCallbackCalled = false;

        runs( function() {
            chrome.tabs.create({'url': address}, function(aTab) {tab = aTab;});
        });

        waitsFor(function() {
          return tab;
        }, "Callback fot createTab was not called", 3000);

        runs(function(){
          expect(tab).toBeDefined();
          expect(tab.id).toBeDefined();
          chrome.tabs.remove(tab.id, function(){ removeCallbackCalled = true; });
        });

        waitsFor(function() {
          return removeCallbackCalled;
        }, "Callback for remove was not called", 3000);

        runs(function(){
          chrome.tabs.get(tab.id, function(aTab){ tab = aTab; getCallbackCalled = true; });
        });

        waitsFor(function() {
          return getCallbackCalled;
        }, "Callback for get was not called", 3000);

        runs(function(){
          expect(tab).not.toBeDefined();
        });
    });

    it('Can query tabs', function(){
      var tabs;
      runs(function(){
        chrome.tabs.query(
          {
            windowId: chrome.windows.WINDOW_ID_CURRENT,
            active: true
          },
          function(aTabs){
            tabs = aTabs;
          }
        );
      });

      waitsFor(function() {
        return tabs;
      }, "Callback for query was not called", 3000);

      runs(function(){
        expect(tabs).toBeDefined();
        expect(tabs.length).toBe(1);
      });
    });

    it('Active tab logic', function(){
        var tabs;
        var callback = function(aTabs){ tabs = aTabs; }
        waits(1000);

        runs(function(){
          chrome.tabs.query({}, callback);
        });

        waitsFor(function() {
          return tabs;
        }, "Callback for query was not called", 3000);

        runs(function(){
            var activeCount = 0;
            for(var i = 0; i < tabs.length; ++i) {
              if (tabs[i].active) ++activeCount;
            }
            expect(activeCount).toBeGreaterThan(0);
        });
    });

    it('Can update tabs', function(){
      var tabs;
      runs(function(){
        chrome.tabs.query({}, function(aTabs){ tabs = aTabs; });
      });

      waitsFor(function() {
        return tabs;
      }, "Callback for query was not called", 3000);

      runs(function(){
        expect(tabs).toBeDefined();
        expect(tabs.length).toBeGreaterThan(0);
        expect(tabs[0].id).toBeGreaterThan(0);
        chrome.tabs.update(tabs[0].id, {'url':'http://www.salsitasoft.com/'});
      });
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
