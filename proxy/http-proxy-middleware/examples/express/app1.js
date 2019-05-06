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
  selfHandleResponse: true/*,

onProxyRes :function(proxyRes, req, res){
      var _write = res.write;

      var val;
      var body;
      var random = Math.floor(Math.random() * (9999999 - 1000000  + 1)) + 1000000;
      var convert = "/" + random.toString() + "/";
      
      if(map.has(req.ip))
      {
         val = map.get(req.ip)
     
         if(req.url.indexOf(val) >= 0)
         {
          convert = val;
         }
         else if(req.url.indexOf("/gallery/") >= 0)
         {
            console.log("Blacklisting Module Will Be Called Here ");
            return;
         }
         else if(req.url.match(/\/[0-9][0-9][0-9][0-9][0-9][0-9][0-9]\/+/g))
         {  
            console.log("Call Blacklisting Module");
            return;
         }

      }
      else
      {
          map.set(req.ip, convert);

      }   

      proxyRes.on('data', function(data) {
          body = data.toString('utf-8');
      });
      res.write = function (data) {
        try{
          console.log(convert);
          body = body.replace(/\/gallery\//g,convert);
          //console.log(body);
          _write.call(res,body.toString('utf-8'));
        } catch (err) {}
      }
    }*/
   
});


var app = express()

/**
 * Add the proxy to express
 */
app.use('/', jsonPlaceholderProxy)
app.use(morgan('combined', {immediate: accessLogStream}))
app.listen(port)


