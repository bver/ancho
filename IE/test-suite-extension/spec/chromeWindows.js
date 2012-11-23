
define(function() {


  describe('chrome.windows API', function() {
    var callback = function(aTab){tab = aTab;}
    var address = 'http://www.google.cz';

    beforeEach(function() {

    });

    it('Can get all windows', function(){
      var windows;
      var callbackCalled = false;
      var callback = function(aWindows){ callbackCalled = true; windows = aWindows;}
      
      runs(function(){
        chrome.windows.getAll(callback);
      });
      
      waitsFor(function() {
        return callbackCalled;
      }, "Callback for windows query was not called", 3000);
      
      runs(function(){
        expect(windows).toBeDefined();
        /*for (var i = 0; i < windows.length; ++i) {
          console.info("Window " + i);
          for (j in windows[i]) {
            console.info("\t" + j + ": " + windows[i][j]);
          }
        }*/
      });
    });
    
    it('Can create browser window', function(){
      var win;
      var callbackCalled = false;
      var callback = function(aWindow){ callbackCalled = true; win = aWindow;}
      
      runs(function(){
          chrome.windows.create({type: 'normal', url :'http://www.wikipedia.org'}, callback);
      });
      
      waitsFor(function() {
        return callbackCalled;
      }, "Callback for window creation was not called", 3000);
      
      runs(function(){
        expect(win).toBeDefined();
      });
    });

  });

});



