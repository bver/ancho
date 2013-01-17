define(function() {
  describe('HTML document', function() {

    beforeEach(function() {
    });

    it('document.domain and document.location.href contain correct values', function(){
      var docDomain = document.domain,
        docUrl = document.location.href;

      var match = /\/\/([^\/]+)\//.exec(docUrl);

      expect(match).not.toBeNull();
      expect(match[1]).toEqual(docDomain);
    });
  });
});