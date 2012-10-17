
define(function() {
 
    
  describe('chrome.tabs API', function() {
    var tab;
    var callback = function(aTab){tab = aTab;}
    
    beforeEach(function() {
    
    });
    
    it('Has important methods defined', function(){
        expect(chrome.tabs.create).toBeDefined();
        //expect(event.hasListener).toBeDefined();
    });

    it('Can create tabs - callback invoked', function(){
        var address = 'http://www.google.cz';
        
        var tab;
        var callbackSpy = jasmine.createSpy("tab create callback"); //function(aTab){tab = aTab;}
        
        chrome.tabs.create({'url': address}, callbackSpy);
        
        expect(callbackSpy).toHaveBeenCalled();
        expect(callbackSpy.mostRecentCall.args.length).toBe(1);
    });
    
    it('Can create tabs - correct tab info passed to callback ', function(){
        var address = 'http://www.google.cz';
        
        chrome.tabs.create({'url': address}, callback);  
        
        expect(tab).toBeDefined();
        expect(tab.url).toBeDefined();
        expect(tab.url).toMatch(address);
    });
    
    it('Can remove tabs', function(){
        var tab;
        chrome.tabs.create({'url': address}, callback);
        var tabId = tab.id; 
        
        var tab1;
        var callback1 = function(aTab){tab1 = aTab;}
        
        chrome.tabs.get(tabId, callback1);
        expect(tab1).toBeDefined();
        chrome.tabs.remove(tabId);
        expect(tab1).not.toBeDefined();
    });
  });


});



