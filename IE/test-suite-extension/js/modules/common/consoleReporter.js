define(function() {

  var ConsoleReporter = function(aConsole) {
    this.started = false;
    this.finished = false;
    this.console = aConsole;
  };

  ConsoleReporter.prototype = {
    reportRunnerResults: function(runner) {
      var duration = (new Date()).getTime() - this.start_time;
      var failed = this.executed_specs - this.passed_specs;
      var spec_str = this.executed_specs + (this.executed_specs === 1 ? " spec, " : " specs, ");
      var fail_str = failed + (failed === 1 ? " failure in " : " failures in ");

      this.passedLog("Runner Finished.");
      this.passedLog(spec_str + fail_str + (duration/1000) + "s.");
      this.finished = true;
    },

    reportRunnerStarting: function(runner) {
      this.started = true;
      this.start_time = (new Date()).getTime();
      this.executed_specs = 0;
      this.passed_specs = 0;
      this.passedLog("Runner Started.");
    },

    reportSpecResults: function(spec) {
      var resultText = spec.suite.description + ' : ' + spec.description + ' ... ';

      if (spec.results().passed()) {
        this.passed_specs++;
        this.passedLog(resultText + "Passed.");
      } else {
        this.failedLog(resultText + "Failed.");
        items = spec.results().getItems()
        for( i = 0; i < items.length; ++i ) {
          this.failedLog("--- " + items[i]);
        }
      }
    },

    reportSpecStarting: function(spec) {
      this.executed_specs++;
    },

    reportSuiteResults: function(suite) {
      var results = suite.results();
      this.passedLog(suite.description + ": " + results.passedCount + " of " + results.totalCount + " passed.");
    },

    passedLog: function(str) {
      this.console.log(str);
    },

    failedLog: function(str) {
      this.console.error(str);
    }
  };

  return ConsoleReporter;
});