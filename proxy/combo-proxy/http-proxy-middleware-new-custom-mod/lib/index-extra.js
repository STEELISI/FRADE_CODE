//var map = require('/mnt/http-proxy-middleware/examples/express/app1').map
var _ = require('lodash')
var httpProxy = require('http-proxy')
var configFactory = require('./config-factory')
var handlers = require('./handlers')
var contextMatcher = require('./context-matcher')
var PathRewriter = require('./path-rewriter')
var Router = require('./router')
var logger = require('./logger').getInstance()
var getArrow = require('./logger').getArrow
var http = require('http');
var HashMap = require('hashmap');
var map = new HashMap();

module.exports = HttpProxyMiddleware
//New Code Start
var fs = require("fs");
var stream;
stream = fs.createWriteStream("/tmp/blacklistpipe1");

function ip2int(ip) {
    return ip.split('.').reduce(function(ipInt, octet) { return (ipInt<<8) + parseInt(octet, 10)}, 0) >>> 0;
}
function rev(string) {
     var str = string.split('.');
     var reversed =  str[3] + "." + str[2] + "." + str[1] + "." + str[0]
     return reversed
}

var stream = fs.createWriteStream("/tmp/blacklistpipe1",{'flags': 'a'});
var erstream = fs.createWriteStream("/tmp/errblacklistpipe1");
const myConsole = new console.Console(stream, erstream);
var Set = require("collections/set");
var embedded = new Set();
var blk = new Set();
//New Code End







function HttpProxyMiddleware (context, opts) {
  // https://github.com/chimurai/http-proxy-middleware/issues/57
  var wsUpgradeDebounced = _.debounce(handleUpgrade)
  var wsInitialized = false
  var config = configFactory.createConfig(context, opts)
  var proxyOptions = config.options
  var orig

  // create proxy
  require('http').globalAgent.maxSockets = Infinity
  var proxy = httpProxy.createProxyServer({agent: new http.Agent({ keepAlive: true }), timeout:500})
  //logger.info('[HPM] Proxy created:', config.context, ' -> ', proxyOptions.target)

  var pathRewriter = PathRewriter.create(proxyOptions.pathRewrite) // returns undefined when "pathRewrite" is not provided

  // attach handler to http-proxy events
  handlers.init(proxy, proxyOptions)

  // log errors for debug purpose
  proxy.on('error', logError)

  // https://github.com/chimurai/http-proxy-middleware/issues/19
  // expose function to upgrade externally
  middleware.upgrade = wsUpgradeDebounced

  return middleware

  function middleware (req, res, next) {
    if (shouldProxy(config.context, req)) {
      var activeProxyOptions = prepareProxyRequest(req)
      proxy.web(req, res, activeProxyOptions)
      req.url = orig
      //_write.call(res,"abc");
      //console.log(res.write)

/*onProxyRes :function(proxyRes, req, res){
console.log("here3"+req.ip);
         if(req.ip != null && req.url != null && (!(req.ip.indexOf("10.1.") >= 0)))
         {

        console.log("here4"+req.ip);
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
            console.log("Custom_Blacklist 3 ");
            var dec = ip2int(rev(req.ip))
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
            var dec = ip2int(rev(req.ip))
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
          map.set(req.ip, convert);

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

}*/

















    } else {
      next()
    }

    if (proxyOptions.ws === true) {
      // use initial request to access the server object to subscribe to http upgrade event
      catchUpgradeRequest(req.connection.server)
    }
  }

  function catchUpgradeRequest (server) {
    // subscribe once; don't subscribe on every request...
    // https://github.com/chimurai/http-proxy-middleware/issues/113
    if (!wsInitialized) {
      server.on('upgrade', wsUpgradeDebounced)
      wsInitialized = true
    }
  }

  function handleUpgrade (req, socket, head) {
    // set to initialized when used externally
    wsInitialized = true

    if (shouldProxy(config.context, req)) {
      var activeProxyOptions = prepareProxyRequest(req)
      proxy.ws(req, socket, head, activeProxyOptions)
      logger.info('[HPM] Upgrading to WebSocket')
    }
  }

  /**
   * Determine whether request should be proxied.
   *
   * @private
   * @param  {String} context [description]
   * @param  {Object} req     [description]
   * @return {Boolean}
   */
  function shouldProxy (context, req) {
    var path = (req.originalUrl || req.url)
    return contextMatcher.match(context, path, req)
  }
/**
 * Get request IP address.
 *
 * @private
 * @param {IncomingMessage} req
 * @return {string}
 */

function getip (req) {
  return req.ip
// ||
//    req._remoteAddress ||
//    (req.connection && req.connection.remoteAddress) ||
//    undefined
}

/**
 * Record the start time.
 * @private
 */

function recordStartTime () {
  this._startAt = process.hrtime()
  this._startTime = new Date()
}


  /**
   * Apply option.router and option.pathRewrite
   * Order matters:
   *    Router uses original path for routing;
   *    NOT the modified path, after it has been rewritten by pathRewrite
   * @param {Object} req
   * @return {Object} proxy options
   */
  function prepareProxyRequest (req) {
    // https://github.com/chimurai/http-proxy-middleware/issues/17
    // https://github.com/chimurai/http-proxy-middleware/issues/94

      var val;
      var body;

         console.log("here"+req.ip);
         if(req.ip != null && req.url != null && (!(req.ip.indexOf("10.1.") >= 0)))
         {
        console.log("here1"+req.ip);
        if(map.has(req.ip))
        {
         val = map.get(req.ip)

         if(req.url.indexOf(val) >= 0)
         {
          convert = val;
         }

         else if(req.url.indexOf("/gallery/") >= 0)
         {  
            console.log("Custom_Blacklist 1 ");
            var dec = ip2int(rev(req.ip))
            var ip = dec.toString()
            if(!blk.has(ip))
            {
            myConsole.log(ip)
            blk.add(ip)
            }
            res.destroy();
            return;
         }
         else if(req.url.match(/\/[0-9][0-9][0-9][0-9][0-9][0-9][0-9]\/+/g))
         {  
            console.log("Custom_Blacklist 2 ");
            var dec = ip2int(rev(req.ip))
            var ip = dec.toString()
            if(!blk.has(ip))
            {
            myConsole.log(ip)
            blk.add(ip)
            }
            res.destroy();
            return;
         }

      }
    }


    orig = req.url
    req.url = (req.originalUrl || req.url)

    // store uri before it gets rewritten for logging
    req.url = req.url.replace(/\/\d\d\d\d\d\d\d\//g,"/gallery/");
    var originalPath = req.url
    if (proxyOptions.logLevel === 'debug') {
      //var arrow = getArrow(originalPath, req.url, proxyOptions.target, newProxyOptions.target)
      req._startTime = undefined
      req._remoteAddress = getip(req)
      recordStartTime.call(req)
      //logger.debug('[HPM] %s %s %s %s', req.method, originalPath, arrow, newProxyOptions.target)
      logger.debug('%s %s %s %s', req._remoteAddress.split(":").pop(), req._startTime.getTime(), req.method, originalPath)
      //req.url = req.url.replace(/\/\d\d\d\d\d\d\d\//g,"/gallery/");
    }


    var newProxyOptions = _.assign({}, proxyOptions)

    // Apply in order:
    // 1. option.router
    // 2. option.pathRewrite
    __applyRouter(req, newProxyOptions)
    __applyPathRewrite(req, pathRewriter)

    // debug logging for both http(s) and websockets
    ////if (proxyOptions.logLevel === 'debug') {
      //var arrow = getArrow(originalPath, req.url, proxyOptions.target, newProxyOptions.target)
    ////  req._startTime = undefined
    ////  req._remoteAddress = getip(req)
    ////  recordStartTime.call(req)
      //logger.debug('[HPM] %s %s %s %s', req.method, originalPath, arrow, newProxyOptions.target)
    ////  logger.debug('%s %s %s %s', req._remoteAddress.split(":").pop(), req._startTime.getTime(), req.method, originalPath)
    ////}

    return newProxyOptions
  }

  // Modify option.target when router present.
  function __applyRouter (req, options) {
    var newTarget

    if (options.router) {
      newTarget = Router.getTarget(req, options)

      if (newTarget) {
        logger.debug('[HPM] Router new target: %s -> "%s"', options.target, newTarget)
        options.target = newTarget
      }
    }
  }

  // rewrite path
  function __applyPathRewrite (req, pathRewriter) {
    if (pathRewriter) {
      var path = pathRewriter(req.url, req)

      if (typeof path === 'string') {
        req.url = path
      } else {
        logger.info('[HPM] pathRewrite: No rewritten path found. (%s)', req.url)
      }
    }
  }

  function logError (err, req, res) {
    var hostname = (req.headers && req.headers.host) || (req.hostname || req.host) // (websocket) || (node0.10 || node 4/5)
    var target = proxyOptions.target.host || proxyOptions.target
    var errorMessage = '[HPM] Error occurred while trying to proxy request %s from %s to %s (%s) (%s)'
    var errReference = 'https://nodejs.org/api/errors.html#errors_common_system_errors' // link to Node Common Systems Errors page

    logger.error(errorMessage, req.url, hostname, target, err.code, errReference)
  }
}
