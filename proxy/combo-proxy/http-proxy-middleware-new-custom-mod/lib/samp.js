var sys = require('sys')
var exec = require('child_process').exec;
function puts(error, stdout, stderr) { sys.puts(stdout) }
var val = exec("ipset list blacklist", puts);
var p = 0

val.stdout.on('data', function(data) {
    if( data.indexOf("10.2.3.117") > -1)
{

console.log("================");
 console.log(data);
 console.log("Found");
 p=1;
}
else
{
console.log("================");
console.log(data);
console.log("Not Found");
}
});
console.log("P=")
console.log(p)

