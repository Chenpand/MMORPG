var Config = require('../config')
var crypto = require('crypto');
var util = require('util');
var mysql = require('mysql');
var Memcached = require('memcached');
var logger = require('./base/logger');
var Error = require('./base/error');

function Framework() {
    this.sdks = [];

    this.mysql_pool = mysql.createPool({
        host : Config.db.host,
        user : Config.db.user,
        password : Config.db.password,
        database : Config.db.database
    });
    this.memcached = new Memcached(Config.memcached);
}


var framework = null;
module.exports = function() {
    if(framework == null) {
        framework = new Framework();
    } 
    return framework;
}();

Framework.prototype.init = function() {
    var framework = this;
    var sql = util.format("select accid from t_account_map order by accid desc limit 1");
    framework.executeSql(sql, function(err, rows) {
        if(err) {
            logger.error(err);
            return;
        }

        if(rows.length == 0) {
            framework.accid_seed = 0;
        } else {
            framework.accid_seed = parseInt(rows[0].accid);
        }
    });
}

function mk_login_ret(config, error, openid, accid) {
    if(config == null) {
        var ret = {
            result : error.id,
            error: error.info,
        };

        return JSON.stringify(ret);
    }

    var timestamp = Math.round(new Date().getTime() / 1000);
    var seed = timestamp.toString() + config.key1 + config.key2;
    var key = crypto.createHash('md5').update(seed).digest('hex');
    var params = util.format("openid=%s&userid=%d&t=%d&pf=%s", openid, accid, timestamp, config.platform);
    var hashval = crypto.createHash('sha1').update(key + params).digest('hex');

    var ret = {
        result : error.id,
        error: error.info,
        params : params,
        hashval : hashval
    };

    return JSON.stringify(ret);
}

Framework.prototype.query_account_by_openid = function(platform, openid, cb) {
    var framework = this;
    var key = platform + ":" + openid;
    this.memcached.get(key, function(err, data) {
        if(err || data == null) {
            if(err) {
                logger.error(err);
            }
            // key不存在memcached中
            var sql = util.format("select accid from t_account_map where platform = '%s' and openid = '%s'", platform, openid);
            framework.executeSql(sql, function(err, rows) {
                if(err) {
                    logger.error(err);
                    return cb(err);
                }

                if(rows.length == 0) {
                    return cb(null, 0);
                }
                var accid = rows[0].accid;

                // 插入memcached中
                framework.memcached.set(key, accid, 0, function(err) {
                    if(err) {
                        logger.error('insert into ' + key + ' => ' + accid + ' failed, error: ' + err);
                    }
                });

                return cb(null, rows[0].accid);
            });
            return;
        }

        cb(null, data);
    });
}

Framework.prototype.use = function(sdk) {
    if(sdk.init() == false) {
        logger.error("init sdk(" + platform + ") failed.");
        return;
    }

    this.sdks.push(sdk);
}

Framework.prototype.executeSql = function(sql, cb) {
    this.mysql_pool.getConnection(function(err, connection) {
        connection.release();

        if(err) {
            return cb(err);
        }

        connection.query(sql, function(err, rows) {
            cb(err, rows);
        });
    });
}

// 生成账号ID方法
Framework.prototype._gen_accid = function() {
    this.accid_seed = this.accid_seed + 1;
    return this.accid_seed;
}

Framework.prototype.get_server_config = function(id) {
    for(var i = 0; i < Config.server_list.length; i++) {
        if(Config.server_list[i].id == id) {
            return Config.server_list[i];
        }
    }

    return null;
}

// 验证登录
Framework.prototype.checkLogin = function(req, cb) {
    var framework = this;
    var server = null;
    for(var i = 0; i < Config.server_list.length; i++) {
        if(Config.server_list[i].id == req.id) {
            server = Config.server_list[i];
        }
    }

    if(server == null) {
        return cb("invalid server", mk_login_ret(server, Error.InvalidServer, 0, 0));
    }

    var sdk = null;
    for(var i = 0; i < this.sdks.length; i++) {
        if(this.sdks[i].platform == server.platform) {
            sdk = this.sdks[i];
            break;
        }
    }

    if(sdk == null) {
        return cb("invalid platform", mk_login_ret(server, Error.InvalidServer, 0, 0));
    }

    logger.info('recv login request: ' + JSON.stringify(req));

    sdk.checkLogin(req, function(result, err, openid) {
        if(result != 0) {
            return cb(err, mk_login_ret(server, {id: result, info: err}, 0, 0));
        }

        // 查询account
        framework.query_account_by_openid(sdk.platform, openid, function(err, accid) {
            if(err) {
                return cb(err, mk_login_ret(server, Error.DBError, openid, accid));
            }

            if(accid == 0) {
                // 创建account
                accid = framework._gen_accid();
                
                logger.info(util.format('add account(%s, %s, %s)', sdk.platform, openid, accid));

                // 插入mysql中
                var timestamp = Math.round(new Date().getTime() / 1000);
                var sql = util.format("insert into t_account_map values('%s', '%s', %s, %d)", sdk.platform, openid, accid, timestamp);
                framework.executeSql(sql, function(err) {
                    if(err) {
                        logger.error('execute sql:' + sql + ' failed, error: ' + err);
                    }
                });
                
                // 插入memcached中
                var key = sdk.platform + ":" + openid.toString();
                framework.memcached.set(key, accid, 0, function(err) {
                    if(err) {
                        logger.error('insert into ' + key + ' => ' + accid + ' failed, error: ' + err);
                    }
                });
                return cb(err, mk_login_ret(server, Error.CloseLoginNewPlayer, openid, accid));
            }

            cb(null, mk_login_ret(server, Error.Success, openid, accid));
        });
    });
}