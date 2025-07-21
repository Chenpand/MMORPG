var Server = require('./server');

function Message() {
    
};

module.exports = Message;

Message.prototype.send = function(node, cmd, subcmd, body, cb) {
    var msg = {
        "head" : {
            "cmd" : cmd,
            "subcmd" : subcmd,
            "timestamp" : "0"
        },
        "body" : body
    }

    var server = new Server();
    server.connect(node.ip, node.port, function() {
        server.send(msg, cb);
    });

    server.on('error', function(err) {
        server.close();
        if(cb)
        {
            cb(err, null);
        }
    });

    setTimeout(function() {
        if(server.finish == false)
        {
            cb('timeout', null);
        }
        server.close();
    }, 10000);
}