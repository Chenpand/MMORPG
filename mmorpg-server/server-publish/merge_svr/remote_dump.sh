#!/bin/sh

src_user='aldzn'
src_pass='2AJfPNFQxH7HWDc!'

server_num=2
src_host=('rm-bp1y0r2qdw7e07z4i.mysql.rds.aliyuncs.com' 'rm-bp1y0r2qdw7e07z4i.mysql.rds.aliyuncs.com')
src_db=('aldzn_386' 'aldzn_387')

new_zid="816"
merge_to_zid='386'
dest_host='rm-wz951oaz476t71lq8.mysql.rds.aliyuncs.com'
dest_db=('merge_386' 're_merge_387')

dest_user='aldzn'
dest_pass='2AJfPNFQxH7HWDc!'



echo "start dump src db..."
for((i=0;i<${server_num};i++)) do
    mysqldump --set-gtid-purged=OFF -h ${src_host[$i]} -u ${src_user} -p${src_pass} ${src_db[$i]}>/data/dnf/${src_db[$i]}_bk_db.sql
done

echo "scp sql..."
for((i=0;i<${server_num};i++)) do
   scp /data/dnf/${src_db[$i]}_bk_db.sql root@39.108.116.227:/data/dnf/merge 
done
