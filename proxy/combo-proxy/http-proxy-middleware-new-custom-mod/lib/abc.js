var spawn = require('child_process').spawn,
    ls    = spawn('ipset list blacklist');

ls.stdout.on('data', function (data) {
  console.log('stdout: ' + data.toString());
});

ls.stderr.on('data', function (data) {
  console.log('stderr: ' + data.toString());
});

ls.on('exit', function (code) {
  console.log('child process exited with code ' + code.toString());
});
