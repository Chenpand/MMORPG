module.exports = {
    // 服务器监听的端口
    listen_port : 19268,
    server_list : [
        {
            id : 101,
            platform : "xy",
            ip : "192.168.2.22",
            port : 3021,
            key1 : "huDqrvKLuk69Ajdl",
            key2 : "lsnMS3SYrnQNkndf",
            billing_sig_key: "7ce54ae53973d0d504e67a6c7fd98c78"
        }
    ],
    // 账号数据库
    db : {
        host : "121.41.17.47",
        user : "root",
        password : "123456",
        database : "dnf-db-account"
    },
    memcached : "121.41.17.47:11211"
}