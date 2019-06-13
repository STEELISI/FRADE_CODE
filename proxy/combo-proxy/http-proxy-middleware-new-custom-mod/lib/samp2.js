var exec = require('child_process').exec;
function execute(command, callback){
    exec("command", function(error, stdout, stderr){ callback(stdout); });
};

var out = function(callback){
    execute("ipset list blacklist", function(name){});
};
console.log(out)
