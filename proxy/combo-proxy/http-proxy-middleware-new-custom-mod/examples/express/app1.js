var express = require('express')
var fs = require('fs')
var morgan = require('morgan')
var path = require('path')
var HashMap = require('hashmap');
var map = new HashMap();
var port = process.env.PORT || 80
var app = express()

// create a write stream (in append mode)
var accessLogStream = fs.createWriteStream(path.join(__dirname, 'access.log'), {flags: 'a'})
var proxy = require('../../index') // require('http-proxy-middleware');

var fs = require("fs");
var stream;
stream = fs.createWriteStream("/tmp/blacklistpipe");

function ip2int(ip) {
    return ip.split('.').reduce(function(ipInt, octet) { return (ipInt<<8) + parseInt(octet, 10)}, 0) >>> 0;
}
function rev(string) {
     var str = string.split('.');
     var reversed =  str[3] + "." + str[2] + "." + str[1] + "." + str[0]
     return reversed
}

var stream = fs.createWriteStream("/tmp/blacklistpipe",{'flags': 'a'});
var erstream = fs.createWriteStream("/tmp/errblacklistpipe");
const myConsole = new console.Console(stream, erstream);
var Set = require("collections/set");
var embedded = new Set();
var blk = new Set();


//var http = require('http');
/**
 * Configure proxy middleware
 */
var jsonPlaceholderProxy = proxy({
  target: 'http://10.1.1.4',
  //agent: new http.Agent({ keepAlive: true }), 
  changeOrigin: true, // for vhosted sites, changes host header to match to target's host
  logLevel: 'debug',
  changeOrigin: true,
  selfHandleResponse: true,
onProxyReq :function(proxyRes, req, res){

     var val;
      var body;
      var ip;
         //console.log("=========================")
         ip = req._remoteAddress.split(":").pop()
         //console.log(ip)
         //console.log(req);
         if(ip != null && req.url != null && (!(ip.indexOf("10.1.") >= 0)))
         {
        //console.log("here1"+ip);
	if(map.has(ip))
        {
         val = map.get(ip)

         if(req.url.indexOf(val) >= 0)
         {
          convert = val;
         }
        
         else if(req.url.indexOf("/gallery/") >= 0)
         {
            console.log("Custom_Blacklist 0 ");
            var dec = ip2int(rev(ip))
            var ip1 = dec.toString()
            if(!blk.has(ip1))
            {
            //myConsole.log(ip1);
            var s = ip + " 1 GET BLK"
            console.log(s);
            blk.add(ip1);
            }
            res.destroy();
            return;
         }
         else if(req.url.match(/\/[0-9][0-9][0-9][0-9][0-9][0-9][0-9]\/+/g))
         {
            console.log("Custom_Blacklist 2 ");
            var dec = ip2int(rev(ip))
            var ip1 = dec.toString()
            if(!blk.has(ip1))
            {
            myConsole.log(ip1)
            blk.add(ip1)
            }
            res.destroy();
            return;
         }

      }
    }

},


onProxyRes :function(proxyRes, req, res){
         var ipad
         ipad = req._remoteAddress.split(":").pop()
         //console.log("here3"+ipad);
         if(ipad != null && req.url != null && (!(ipad.indexOf("10.1.") >= 0)))
         {
    
        //console.log("here4"+ipad);
      var _write = res.write;
      var val;
      var body;
      var random = Math.floor(Math.random() * (9999999 - 1000000  + 1)) + 1000000;
      var convert = "/" + random.toString() + "/";
      if(map.has(ipad))
      {
         val = map.get(ipad)
     
         if(req.url.indexOf(val) >= 0)
         {
          convert = val;
         }
         else if(req.url.indexOf("/gallery/") >= 0)
         {
            console.log("Custom_Blacklist 3 ");
            var dec = ip2int(rev(ipad))
            //console.log(dec)
            //console.log(blacklist)
            //var blacklist = "\n" + rev(dec.toString())
            var ip = dec.toString()
            if(!blk.has(ip))
            {
            myConsole.log(ip)
            blk.add(ip)
            }
            //stream.write(  dec.toString() + "\n")
            //fs.appendFileSync('/tmp/blacklistpipe', dec.toString());
            return;
         }
         else if(req.url.match(/\/[0-9][0-9][0-9][0-9][0-9][0-9][0-9]\/+/g))
         {  
            console.log("Custom_Blacklist 4 ");
            var dec = ip2int(rev(ipad))
            //console.log(dec)
            var ip = dec.toString()
            if(!blk.has(ip))
            {
            myConsole.log(ip)
            blk.add(ip)
            }
            
            return;
         }

      }
      else
      {
          map.set(ipad, convert);

      }   

      proxyRes.on('data', function(data) {
          body = data.toString('utf-8');
      });
      res.write = function (data) {
        try{
          //console.log(convert);
          body = body.replace(/\/gallery\//g,convert);
          //console.log(body);
          _write.call(res,body.toString('utf-8'));
        } catch (err) {}
      }
    }
   }
});


var app = express()

/**
 * Add the proxy to express
 */
app.use('/', jsonPlaceholderProxy)
app.use(morgan('combined', {immediate: accessLogStream}))
app.listen(port)


