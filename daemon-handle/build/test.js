var casper = require('casper').create({
    pageSettings: {
        webSecurityEnabled: false
    }
});

casper.start();

casper.then(function() {
    var current = 1;
    var end = 2;

    for (;current < end;) {

      (function(cntr) {
        casper.thenOpen('https://localhost/', function() {
			
			this.wait(750, function() {
              this.echo('casper.async: ' + this.getTitle() + ' ' + cntr);
			});
              // here we can download stuff
        });
      })(current);

      current++;

    }

});

casper.run(function() {
    this.echo('Done.').exit();
});