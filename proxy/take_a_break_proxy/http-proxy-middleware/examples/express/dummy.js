var Set = require("collections/set");
var embedded = new Set();
var fs = require('fs');
var array = fs.readFileSync('/users/rajat19/frade/brandon/FRADE/conf/wikipedia/embedded.txt').toString().split("\n");
for(i in array) {
    embedded.add(array[i]);
}


console.log("Hi")
console.log("Set\n" + embedded.has("/6TqxvGmb.jpg"))
if(embedded.has("/6TqxvGmb.jpg"))
{
  console.log("Found Embedded");
}
