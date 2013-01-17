
define(function() {


  describe('chrome.extension API', function() {

    beforeEach(function() {

    });

    it('has basic API available', function() {
      expect(chrome.extension.sendMessage).toBeDefined();
      expect(chrome.extension.onMessage).toBeDefined();
      expect(chrome.extension.connect).toBeDefined();
      expect(chrome.extension.onConnect).toBeDefined();
      expect(chrome.extension.getURL).toBeDefined()
    });

    it('returns meaningfull URLs', function() {
        expect(chrome.extension.getURL('foo.js')).toMatch('chrome-extension://');
    });
  });


});


