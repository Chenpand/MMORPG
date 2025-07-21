var log4js = require('log4js');
log4js.configure({
  appenders: [
    { 
      type: 'console' 
    }, 
    {
      type: 'dateFile', 
      filename: 'logs/', 
      pattern: "yyyyMMdd.log",  // 占位符，紧跟在filename后面  
      alwaysIncludePattern: true,       // 文件名是否始终包含占位符  
      maxLogSize: 1024,
      backups:4,
      category: 'globalserver' 
    }
  ],
  replaceConsole: true
});

module.exports = log4js.getLogger('globalserver');