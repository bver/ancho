desc('Install the extension dependencies.');
task('deps', [], function() {
  var current_dir = process.cwd();
  process.chdir(process.env.sourcedir);
  jake.exec('npm install', function() {
    process.chdir(current_dir);
    complete();
  }, { printStdout: true });
}, { async: true });
