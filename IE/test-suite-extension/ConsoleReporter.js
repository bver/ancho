(function() {

  var ConsoleReporter = function() {
    this.started = false;
    this.finished = false;
  };

  ConsoleReporter.prototype = {
    reportRunnerResults: function(runner) {
      var duration = (new Date()).getTime() - this.start_time;
      var failed = this.executed_specs - this.passed_specs;
      var spec_str = this.executed_specs + (this.executed_specs === 1 ? " spec, " : " specs, ");
      var fail_str = failed + (failed === 1 ? " failure in " : " failures in ");

      this.log("Runner Finished.");
      this.log(spec_str + fail_str + (duration/1000) + "s.");
      this.finished = true;
    },

    reportRunnerStarting: function(runner) {
      this.started = true;
      this.start_time = (new Date()).getTime();
      this.executed_specs = 0;
      this.passed_specs = 0;
      this.log("Runner Started.");
    },

    reportSpecResults: function(spec) {
      var resultText = "Failed.";

      if (spec.results().passed()) {
        this.passed_specs++;
        resultText = "Passed.";
      }

      this.log(resultText);
    },

    reportSpecStarting: function(spec) {
      this.executed_specs++;
      this.log(spec.suite.description + ' : ' + spec.description + ' ... ');
    },

    reportSuiteResults: function(suite) {
      var results = suite.results();
      this.log(suite.description + ": " + results.passedCount + " of " + results.totalCount + " passed.");
    },

    log: function(str) {
      console.log(str);
    }
  };

  jasmine.ConsoleReporter = ConsoleReporter;
})();