var request = require('request');
var util = require('util');
var crypto = require('crypto');
var logger = require('../base/logger');
var framework = require('../framework');
var Message = require('../base/message')

var appid = 100031076;
// 签名密钥,请换成自己KEY(去dev后台查看key)
APPKEY = 'gOQuxDl7uY0k7t5OR1uU72Bu8f8JN5BJ';
PAYKEY = 'GPpvimdx4OTKUaQnWwwst3VCQtVYogdD';

var ChargeResult = {
    Success: {
        id: 0,
        msg: '回调成功'
    },
    InvalidParam: {
        id: 1,
        msg: '参数错误'
    },
    PlayerUnexist: {
        id: 2,
        msg: '玩家不存在'
    },
    InvalidServerId: {
        id: 3,
        msg: '游戏服不存在'
    },
    RepeatOrder: {
        id: 4,
        msg: '订单已存在'
    },
    InvalidExtraData: {
        id: 5,
        msg: '透传信息错误'
    },
    InvalidSign: {
        id: 6,
        msg: '签名校验错误'
    },
    DBError: {
        id: 7,
        msg: '数据库错误'
    },
    Other: {
        id: 8,
        msg: '其它错误'
    }
}

function Sdk() {
    this.platform = "xy";
    
    this.init = function() {
        params = {
            'orderid':'100001_10001_1407811507_9347',
            'uid':'10001',
            'amount':'1.00',
            'serverid':'23',
            'extra':'201408036987',
            'ts':'1407811565',
            'sign':'7e7fd393064f6a6b4e4bf09d07b2bd65',
            'sig':'eb072996de9449f1d66fafb903896857'
        };
        //this.charge(params, null, function() {});
    }

    this.checkLogin = function(req, cb) {
        var url = 'http://passport.xyzs.com/checkLogin.php';
        /*var loginReq = { 
            uid : parseInt(req.openid), 
            appid : appid, 
            token : req.token
        };*/
        var loginReq = util.format("uid=%s&appid=%d&token=%s", req.openid, appid, req.token);
        var options = {
            url: 'http://passport.xyzs.com/checkLogin.php',
            method: 'post',
            body: loginReq,
            headers: {
                 'Content-type': 'application/x-www-form-urlencoded'
            }
        };

        request(options, function(err, res, body) {
            if(err) {
                logger.error(err);
                return cb(1001, '验证服务器异常', req.openid);
            }
            try {
                var obj = JSON.parse(body);
            } catch(e) {
                logger.error(e);
                return cb(1001, '验证服务器异常', req.openid);
            }
        
            return cb(obj.ret, obj.error, req.openid);
        });

        //cb(0, null, req.openid);
    }

    this.genAccByOpenid = function(openid) {
        return openid;
    }

    // 签名方法
    function _gen_safe_sign(params, appkey) {
        if(params.length == 0 || appkey == '') {
            return "";
        }

        var queryStr = "";
        var key_list = [];
        for(var key in params) {
            if(key == 'sign' || key == 'sig') {
                continue;
            }
            key_list.push(key);
        }
        key_list.sort();
        for(var i = 0; i < key_list.length; i++) {
            var key = key_list[i];
            if(queryStr != "") {
                queryStr += '&';
            }
            queryStr = queryStr + key + '=' + params[key];
        }
		
        var md5sum = crypto.createHash('md5');
        md5sum.update(appkey+queryStr);
        var str = md5sum.digest('hex');
        return str;
    }

    function make_charge_ret(code) {
        return JSON.stringify({
            ret: code.id,
            msg: code.msg
        });
    }

    function make_charge_ret_by_id(id) {
        for(var key in ChargeResult) {
            if(ChargeResult[key].id == id) {
                return ChargeResult[key];
            }
        }

        return {id: id, msg: 'Unknown'};
    }

    function chargeReturn(res, ret, cb) {
        res.end(make_charge_ret(ret));
        cb(ret.id);
    }

    function _gen_token(params, key) {
        var str = "";
        for(var key in params) {
            str += params[key].toString();
        }
        str += key;

        var md5sum = crypto.createHash('md5');
        md5sum.update(str);
        return md5sum.digest('hex');
    }

    this.charge = function(req, res, cb) {
        var safeSign = _gen_safe_sign(req, APPKEY)
	
        if(safeSign != req.sign) {
            return chargeReturn(res, ChargeResult.InvalidSign, cb);
        }
            
        if(req.sig != '') {
            safeSign = _gen_safe_sign(req, PAYKEY)
            if(safeSign != req.sig) {
                return chargeReturn(res, ChargeResult.InvalidSign, cb);
            }
        }

        var serverConfig = framework.get_server_config(req.serverid);
        if(serverConfig == null) {
            return chargeReturn(res, ChargeResult.InvalidServerId, cb);
        }

        // 获取账号ID
        framework.query_account_by_openid("xy", request.uid, function(err, accid) {
            if(err) {
                return chargeReturn(res, ChargeResult.DBError, cb);
            }

            /*if(accid == 0) {
                return chargeReturn(res, ChargeResult.PlayerUnexist, cb);
            }*/

            var message = new Message();
            var body = {
                orderid: req.orderid,
                accid: '654',
                roleid: '417726457624678327',
                goodsid: '1',
                money: '10',
                ts: 'req.ts'
            };
            var token = _gen_token(body);
            body.token = token;

            message.send(serverConfig, "chargeManage", "charge", body, function(err, response) {
                if(err) {
                    logger.error(err);
                    
                    return chargeReturn(res, ChargeResult.Other, cb);
                }

                return chargeReturn(res, make_charge_ret_by_id(response.result), cb);
            });
        });
    }
}

module.exports = new Sdk();