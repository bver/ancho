
define(function() {
 
    
  describe('chrome.tabs API', function() {
    var tab;
    var callback = function(aTab){tab = aTab;}
    var address = 'http://www.google.cz';
    
    beforeEach(function() {
        tab = undefined;
        
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
        var address = 'http://www.google.cz';
        
        chrome.tabs.create({'url': address}, callback);  
        
        expect(tab).toBeDefined();
        expect(tab.url).toBeDefined();
        expect(tab.url).toMatch(address);
    });
    
    it('Can remove tabs', function(){
        runs( function() {
          chrome.tabs.create({'url': address}, callback);
        });
        
        waitsFor(function() {
          return tab != undefined;
        }, "Callback for createTab was not called", 3000);

        var tabId = tab.id; 
        var tab1;
        var callback1 = function(aTab){tab1 = aTab;}
        runs(function(){
          chrome.tabs.get(tabId, callback1);
        });
        waitsFor(function() {
          return tab1;
        }, "Callback for get was not called", 3000);
        
        runs(function(){
          expect(tab1).toBeDefined();
        });
        var removeCallbackCalled = false;
        runs(function(){
          chrome.tabs.remove(tabId, function(){ removeCallbackCalled = true; });
        });
        
        waitsFor(function() {
          return removeCallbackCalled;
        }, "Callback for remove was not called", 3000);
        
        runs(function(){
          chrome.tabs.get(tabId, callback1);
        });
        
        waitsFor(function() {
          return tab1;
        }, "Callback for get was not called", 3000);
        
        runs(function(){
          expect(tab1).not.toBeDefined();
        });
    });
    
    it('Can query tabs', function(){
        var tabs;
        var callback2 = function(aTabs){tabs = aTabs;}
        chrome.tabs.query({}, callback2);  
        
        expect(tabs).toBeDefined();
    });
    
    it('Can update tabs', function(){
        chrome.tabs.update(2, {'active':true});  
    });
  });

});



