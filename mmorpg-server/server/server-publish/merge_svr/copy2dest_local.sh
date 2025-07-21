#!/bin/sh

src_user='aldzn'
src_pass='2AJfPNFQxH7HWDc!'


src_host='rm-wz951oaz476t71lq8.mysql.rds.aliyuncs.com'
src_db='merge_388'


dest_host='rm-wz9k59i4t8c5mc417.mysql.rds.aliyuncs.com'
dest_db='aldzn_817'
dest_replay_db='replay_817'

dest_user='aldzn'
dest_pass='2AJfPNFQxH7HWDc!'

dest_insname='rm-wz9k59i4t8c5mc417'



echo "start backup"
mysqldump --set-gtid-purged=OFF -h ${src_host} -u ${src_user} -p${src_pass} ${src_db}>/data/dnf/merge/${src_db}_bk_db.sql

echo "create new database"
#检查要创建得库是否存在
is_exist=`mysql -h ${dest_host} -u${dest_user} -p${dest_pass} -e "show databases" | grep "${dest_db}" | wc -l`

if [ $is_exist -ne 0 ]                                                                                
then
	 echo "databases ${dest_db} is exist error..."
	 exit 1
fi

# 通过阿里云创建库等待60S
node /data/ansible/mkServer/AliyunRdsTools/app.js ${dest_insname} ${dest_db} 60
node /data/ansible/mkServer/AliyunRdsTools/app.js ${dest_insname} ${dest_replay_db} 60

#if [ $is_create_suc -ne 0 ]                                                                                
#then
#	 echo "create databases ${dest_db} failed ..."
#	 exit 1
#fi
sleep 10

echo "start copy"
mysql -h ${dest_host} -u ${dest_user} -p${dest_pass} ${dest_db}</data/dnf/merge/${src_db}_bk_db.sql
mysql -h ${dest_host} -u ${dest_user} -p${dest_pass} ${dest_replay_db}</data/ansible/mkServer/template/replay.sql
