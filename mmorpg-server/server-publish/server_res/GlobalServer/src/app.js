var http = require('http');
var url = require('url');
var util = require('util');
var crypto = require('crypto');
var framework = require('./framework');
var sdk_xy = require('./sdk/sdk_xy');
var logger = require('./base/logger');
var router = require('./router');
var config = require('../config');

try { //异常捕获
    //var framework = global.framework;//new Framework();
    framework.use(sdk_xy);
    framework.init();

    var server = http.createServer(function(req, res) {
        var postData = "";
        var url_parse = url.parse(req.url, true);
        var pathname = url_parse.pathname;

        console.log("Request for " + pathname + " received.");

        req.addListener("data", function(postDataChunk) {
            postData += postDataChunk;
        });

        req.addListener("end", function() {
            console.log(postData);
            var body = null;
            try {
                body = JSON.parse(postData);
            } catch(err) {}
            if(body == null) {
                body = postData;
            }
            router(
                {
                    framework: framework,
                    url: req.url,
                    path: pathname,
                    query: url_parse.query,
                    body: body
                }, res
            );
        });
    });

    server.listen(config.listen_port);
} catch (err) {
    logger.error(err);
}
