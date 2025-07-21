var settings = require('./settings');
var Db = require('mongodb').Db;
var Connection = require('mongodb').Connection;
var Server = require('mongodb').Server;

module.exports = new Db(settings.db.name, 
                        new Server(settings.db.host, settings.db.port),
                        {safe : true});