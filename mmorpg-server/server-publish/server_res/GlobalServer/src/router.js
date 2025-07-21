var url = require('url');
var querystring = require('querystring');
var logger = require('./base/logger');
var framework = require('./framework');

function onLogin(req, res) {
    framework.checkLogin(req.query, function(err, resp) {
        res.end(resp);
        if(err) {
            return logger.info(err);
        }
    });
}

module.exports = function(req, res) {
    logger.info('recv request: ' + req.url);
    
    if(req.path == '/login') {
        onLogin(req, res);
    } else if(req.path == '/test') {
        var query = {
            id: 101,
            openid: 47251115,
            token: '3a34ad44f7727bc004b093cde9ebfaa2'
        };

        var totalUsedTime = 0;
        var test = 1000;
        for(var i = 0; i < test; i++) {
            var startTime = new Date().getTime();
            framework.checkLogin(query, function(err, resp) {
                var endTime = new Date().getTime();
                var usedTime = endTime - startTime;
                totalUsedTime += usedTime;
                test = test - 1;

                if(test == 0) {
                    logger.debug('total used time ' + totalUsedTime + ' ms');
                }
            });
        }
    } else {
        logger.error('unknown request:' + req.url);
        res.end();
    }
}