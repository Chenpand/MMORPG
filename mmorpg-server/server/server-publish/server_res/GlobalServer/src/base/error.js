module.exports = {
    Success: {
        id: 0,
        info: ''
    },
    InvalidServer: {
        id: 1000,
        info: '无效的服务器'
    },
    DBError: {
        id: 1001,
        info: '服务器异常'
    },
    AuthError: {
        id: 1002,
        info: '登录验证异常'
    },
    DuplicateAccount: {
        id: 2000,
        info: '账号已被使用'
    },
    AccountOrPasswdError: {
        id: 2001,
        info: '账号密码错误'
    },
    CloseLoginNewPlayer: {
        id: 2002,
        info: '无法登录'
    }
}