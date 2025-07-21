var net = require('net');
//var Buffer = require('buffer');

function Server() {
    this.socket = new net.Socket();
    this.recvBuffer = new Buffer(1024);
    this.recvBufferIndex = 0;
    this.cb = null;
    this.finish = false;
};

module.exports = Server;

Server.prototype.connect = function(ip, port, cb) {
    this.socket.connect(port, ip, function() {
        cb();
    });

    var server = this;

    this.socket.on('data', function(data) {
        data.copy(server.recvBuffer, server.recvBufferIndex);
        server.recvBufferIndex += data.byteLength;
        checkMsg(server);
    });

    this.socket.on('error', function(err) {
        console.log(err);
    })
};

Server.prototype.close = function(data) {
    this.finish = true;
    this.socket.end();
}

Server.prototype.on = function(event, cb) {
    this.socket.on(event, cb);
}

Server.prototype.send = function(data, cb) {
    this.cb = cb;
    var str = JSON.stringify(data);
    var msgBuf = new Buffer(str, 'utf8');
    var buf = new Buffer(msgBuf.byteLength + 10);
    buf.writeUInt32LE(msgBuf.byteLength, 0);
    buf.writeUInt16BE(0, 4);
    buf.writeUInt32BE(0, 6);
    msgBuf.copy(buf, 10);
    
    this.socket.write(buf);
}

function checkMsg(server) {
    var len = server.recvBuffer.readUInt32LE(0);
    var needLen = len + 4;
    if(len == 0) {
        needLen += 6;
    }

    var msg = new Buffer(len);
    if(server.recvBufferIndex < needLen) {
        return;
    }

    server.recvBuffer.copy(msg, 0, 4, needLen);
    server.recvBuffer.copy(server.recvBuffer, 0, needLen, server.recvBufferIndex - needLen);
    server.recvBufferIndex -= needLen;

    if(len == 0) {
        return;
    }

    console.log(msg.toString());
    var res = JSON.parse(msg.toString());
    if(server.cb && !server.finish)
    {
        server.cb(null, res);
    }

    server.finish = true;
}
