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
module.exports = HttpProxyMiddleware

function HttpProxyMiddleware (context, opts) {
  // https://github.com/chimurai/http-proxy-middleware/issues/57
  var wsUpgradeDebounced = _.debounce(handleUpgrade)
  var wsInitialized = false
  var config = configFactory.createConfig(context, opts)
  var proxyOptions = config.options

  // create proxy
  require('http').globalAgent.maxSockets = Infinity
  httpProxy.setMaxSockets = 600000;
  var proxy = httpProxy.createProxyServer({agent: new http.Agent({ keepAlive: false }), timeout:0.001,proxyTimeout:0.001})
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
    //req.socket.destroy()
    //res.destroy()
    if (shouldProxy(config.context, req)) {
      var activeProxyOptions = prepareProxyRequest(req)
         ///var proxyReq = ( http).request(common.setupOutgoing( {}, activeProxyOptions, req));
         ///server.emit('econnreset', err, req, res, url);
          ///proxyReq.abort();

      ///var host = (req.headers && req.headers.host)
       //req.connection.abort()        
       ////req.connection.destroy()
       ////req.socket.destroy()
       ////req.pipe().abort()
       ////res.connection.destroy()
       ////req.socket.destroy()
        ///res.end()
        ////-----  res.destroy()
	////stream.destroy()
       ///logger.info('Destroying')
       ///req.socket.destroy()
       //res.writeHead(504)
       //res.end('Error occured while trying to proxy to: ' + host + req.url)
      proxy.web(req, res, activeProxyOptions)
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
      //proxy.ws(req, socket, head, activeProxyOptions)
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
    req.url = (req.originalUrl || req.url)

    // store uri before it gets rewritten for logging
    var originalPath = req.url
    if (proxyOptions.logLevel === 'debug') {
      //var arrow = getArrow(originalPath, req.url, proxyOptions.target, newProxyOptions.target)
      req._startTime = undefined
      req._remoteAddress = getip(req)
      recordStartTime.call(req)
      //logger.debug('[HPM] %s %s %s %s', req.method, originalPath, arrow, newProxyOptions.target)
      logger.debug('%s %s %s %s', req._remoteAddress.split(":").pop(), req._startTime.getTime(), req.method, originalPath)
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
