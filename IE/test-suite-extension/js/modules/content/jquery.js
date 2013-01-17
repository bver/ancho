define(function() {

  return describe('jQuery functionality', function() {

    beforeEach(function() {
    });

    it('document ready is triggered', function(){
      var ready = false;

      runs(function() {
        $(document).ready(function() {
          ready = true;
        });
      });

      waitsFor(function() {
        return ready;
      }, "$(document).ready never triggered", 3000);

      runs(function() {
        expect(ready).toEqual(true);
      });
    });

    it('possible to insert element into DOM', function(){
      var body = $('body');
      var div = $('<div>Hello world!</div>');

      body.append(div);

      expect(body.length).toBeGreaterThan(0);
      expect(div.length).toEqual(1);
    });

  });
});