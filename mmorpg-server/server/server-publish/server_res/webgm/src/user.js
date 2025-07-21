var mongodb = require('./db');
var crypto = require('crypto');

function User(user) {
    this.name = user.name;
    this.password = user.password;
}

module.exports = User;

User.get = function(name, callback) {
    mongodb.open(function(err, db) {
        if(err) {
            return callback(err);
        } 
        
        // 读取users集合
        db.collection('users', function(err, collection) {
            if(err) {
                mongodb.close();
                return callback(err);
            } 
            
            // 查找用户名(键为name)值为name的一个文档
            collection.findOne({
                name: name
            }, function(err, user) {
                mongodb.close();
                if(err) {
                    return callback(err);
                } 
                
                callback(null, user);
            });
        });
    });
};