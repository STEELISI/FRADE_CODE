const fs = require('graceful-fs');
var date = new Date(); 
var timestamp = date.getTime();

var Set = require("collections/set");
var ips = new Set();

 
console.log(timestamp)
 
var contents = fs.readFileSync('xx', 'utf8');
ips = contents.split("\n");
if( ips.has("10.2.194.555"))
{  
              console.log("Found 1")
 }

var i;
for(i=0; i < ips.length; i++)
{
   console.log(ips[i])
}


//console.log(contents);
           if( contents.indexOf("10.2.194.5\n") > -1)
           {  
              console.log("Found")
           }

var timestamp = date.getTime();
console.log(timestamp)
