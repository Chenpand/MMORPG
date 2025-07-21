/// <reference path="../../NodeSnippet/typings/index.d.ts" />

var express = require('express');
var moment = require('moment');
var crypto = require('crypto');
var router = express.Router();
var Server = require('../src/server');
var Message = require('../src/message');
var User = require('../src/user');
var formidable = require('formidable');
var fs = require('fs');

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', {
    success: req.flash('success'),
    error: req.flash('error')
  });
});

router.post('/login', function(req, res, next) {
  // 生成密码的md5
  var md5 = crypto.createHash('md5');
  var password = md5.update(req.body.password).digest('hex');
  User.get(req.body.username, function(err, user) {
    if (err) {
      console.log(err);
      req.flash('error', '系统错误，请联系管理员！');
      return res.redirect('/');
    }
    if(!user) {
      req.flash('error', '用户不存在！');
      return res.redirect('/');
    }
            
    // 检查密码是否一致
    if(user.password != password) {
      req.flash('error', '密码错误！');
      return res.redirect('/');
    }
            
    // 用户名和密码都正确，将用户信息存入session
    req.session.user = user;
    res.redirect('/main');
  });
});

router.get('/logout', function(req, res, next) {
  req.session.user = null;
  res.redirect('/');
});

router.get('/itemanage', function(req, res, next) {
  res.render('itemanage', { 
    title: '道具管理',
    success: req.flash('success'),
    error: req.flash('error'),
  });
});

router.post('/itemanage', function(req, res) {
  /*var msg = {
    "head":{
      "cmd":"itemManage",
      "subcmd":"giveItem",
      "timestamp":"888"
    },
    "body":req.body
  }

  Server.send(msg);*/
  //req.flash('success', '执行成功');
  /*var str = JSON.stringify(msg);
  var buf = new Buffer(str.length);
  var json = buf.toJSON();
  console.log(json);*/

  var message = new Message();
  message.send("itemManage", "giveItem", req.body, function(err, response) {
    if(err)
    {
      console.log(err);
      req.flash('error', '执行失败');
      res.redirect('/itemanage');
      return;
    }

    console.log(response);
    req.flash('success', '执行成功');
    res.redirect('/itemanage');
  });
});

router.get('/main', function(req, res, next) {
  res.render('main', {});
});

router.get('/query_people', function(req, res, next) {
  res.render('query_people', {
    playerInfoes:req.flash('playerInfoes'), 
    test : req.flash('test')
  });
});

function getYMD(sec) {
  var myDate=new Date(sec);
  myDate.setSeconds(sec);
  var year=myDate.getFullYear();
  var month=myDate.getMonth()+1;
  var date = myDate.getDate();
  var YMD=year + '-' + month + '-' + date + " " + myDate.getHours() + ":" + myDate.getMinutes() + ":" + myDate.getSeconds();
  return YMD;
}

router.post('/query_people', function(req, res) {
  var message = new Message();
  message.send("playerManage", "query", req.body, function(err, response) {
    if(err)
    {
      console.log(err);
      return res.redirect('/query_people');
    }

    console.log(response);
    console.log(response.body.info);
    req.flash('playerInfoes', response.body.info);
    for (var i = 0; i < response.body.info.length; i++)
    {
      response.body.info[i]['reg_time'] = getYMD(response.body.info[i]['reg_time']);
    }

    req.flash('playerInfoes', response.body.info);
    res.redirect('/query_people');
    //return res.render('query_people', {playerInfoes : response.body.info});
  });

  /*req.flash('test', 1);
  res.redirect('/query_people');*/
});

router.get('/giveitem', function(req, res, next) {
  res.render('giveitem',{
    success: req.flash('success'),
    error: req.flash('error'),
  });
});

router.post('/giveitem', function(req, res, next) {
  var form = new formidable.IncomingForm();   //创建上传表单
      form.encoding = 'utf-8';		//设置编辑
      form.uploadDir = 'public/upload/';	 //设置上传目录
      form.keepExtensions = true;	 //保留后缀
      form.maxFieldsSize = 2 * 1024 * 1024;   //文件大小

    form.parse(req, function(err, fields, files) {
      if (err) {
        req.flash('error', err);
        res.redirect('/giveitem');
        return;
      }

      fs.readFile(files.file.path, function(err, data) {
        if (err) {
          console.log(err);
          req.flash('error', err);
          res.redirect('/giveitem');
          return;		
        }

        var myDate=new Date();
        var year=myDate.getFullYear();
        var month=myDate.getMonth()+1;
        var date = myDate.getDate();
        var ymd = year.toString() + month + date + myDate.getHours() + myDate.getMinutes() + myDate.getSeconds();
        var newName = 'giveitem_' + ymd;

        fs.rename(files.file.path, form.uploadDir + newName, function (err) {
          if(err) {
            return console.log(err);
          }
        });  

        var body = {
          player_list: [],
          server_broadcast: fields.server_broadcast,
          items:[],
          content: fields.content
        }

        body.items.push({id:fields.itemid, num:fields.itemnum})

        if (body.server_broadcast == undefined) {
          body.server_broadcast = '0';
          if(data.length == 0) {
            body.player_list.push(fields.playerid);
          } else {
            var players = data.toString().split('\r\n');
            body.player_list = players;
          }
        } else {
          body.server_broadcast = '1';
        }

        var message = new Message();
        message.send("itemManage", "giveItem", body, function(err, response) {
          if(err)
          {
            console.log(err);
            req.flash('error', err);
            res.redirect('/giveitem');
            return;
          }

          console.log(response);
          req.flash('success', '执行成功');
          res.redirect('/giveitem');
        });
      });
    });
});

router.get('/reload_script', function(req, res, next) {
  res.render('reload_script');
});

router.post('/reload_script', function(req, res, next) {
  console.log(req.body);

  var message = new Message();
  message.send("gameControl", "reloadScripts", req.body, function(err, response) {
    if(err) {
      console.log(err);
      res.redirect('/reload_script');
      return;
    }

    console.log(response);
    res.redirect('/reload_script');
  });
});

router.get('/player_punish', function(req, res, next) {
  res.render('player_punish');
});

router.post('/player_punish', function(req, res, next) {
  console.log(req.body);

  var message = new Message();
  var body = {
    account_id: req.body.account_id,
    type: req.body.type,
    duration_time: req.body.time
  };

  message.send("playerManage", "punish", body, function(err, response) {
    if(err) {
      console.log(err);
      res.redirect('/player_punish');
      return;
    }

    console.log(response);
    res.redirect('/player_punish');
  });
});

router.post('/file-upload', function(req, res, next) {
  console.log(req.files);
});

router.get('/announcement', function(req, res, next) {
  res.render('announcement',{
    announcements: req.flash('announcements'),
    success: req.flash('success'),
    error: req.flash('error'),
  });
});

router.post('/query_announcement', function(req, res, next) {
  var message = new Message();
  message.send("gameControl", "showAnnouncements", {}, function(err, response) {
    if (err) {
      console.log(err);
      req.flash('error', err);
      res.redirect('/announcement');
      return;
    }

    var announcements = [];

    for (var i = 0; i < response.body.announcements.length; i++) {
      var announcement = response.body.announcements[i];
      announcement['time'] = getYMD(announcement['time']);
      announcements.push(announcement)
    }

    req.flash('announcements', announcements);
    res.redirect('/announcement');
  });
  /*var announcements = [];
  announcements.push({
    id:1,
    time:2,
    interval:3,
    repeattimes:4,
    content:'test'
  });
  req.flash('announcements', announcements);
  res.redirect('/announcement');*/
});

router.get('/publish_announcement', function(req, res, next) {
  res.render('publish_announcement',{
    success: req.flash('success'),
    error: req.flash('error'),
  });
});

router.post('/publish_announcement', function(req, res, next) {
  var date = new Date(req.body.fuck);
  var begintime = Date.parse(date) / 1000;
  var body = {
    begintime: begintime.toString(),
    interval: req.body.interval,
    repeattimes: req.body.repeattimes,
    content: req.body.content
  };

  var message = new Message();
  message.send("gameControl", "announce", body, function(err, response) {
    if (err) {
      console.log(err);
      req.flash('error', err);
      res.redirect('/publish_announcement');
      return;
    }

    req.flash('success', '发布成功');
    res.redirect('/announcement');
  });
});

router.post('/announcement_del', function(req, res, next) {
  var body = {
    id : req.body.id
  };

  var message = new Message();
  message.send("gameControl", "delAnnouncement", body, function(err, response) {
    var result = {error: null};
    if (err) {
      console.log(err);
      result.error = err;
      res.write(JSON.stringify(result));
      res.end();
      return;
    }

    res.end(JSON.stringify(result));
  });
});

module.exports = router;
