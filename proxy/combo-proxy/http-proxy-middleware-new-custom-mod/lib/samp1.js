var shell = require('shelljs');
var version = exec('ipset list blacklist').output;
console.log(version)
