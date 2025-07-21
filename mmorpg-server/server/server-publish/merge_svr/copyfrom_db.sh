#!/bin/sh

src_user='aldzn'
src_pass='2AJfPNFQxH7HWDc!'

server_num=5
src_host=('rm-bp162ha669ym25k0q.mysql.rds.aliyuncs.com' 'rm-wz98l4dnt2uyqti1b.mysql.rds.aliyuncs.com' 'rm-wz98l4dnt2uyqti1b.mysql.rds.aliyuncs.com' 'rm-bp1kv9go922wezeok.mysql.rds.aliyuncs.com' 'rm-bp1kv9go922wezeok.mysql.rds.aliyuncs.com')
src_db=('aldzn_351' 'aldzn_352' 'aldzn_353' 'aldzn_354' 'aldzn_355')




echo "start dump src db..."
for((i=0;i<${server_num};i++)) do
	mysqldump --set-gtid-purged=OFF -h ${src_host[$i]} -u ${src_user} -p${src_pass} ${src_db[$i]}>/data/dnf/merge/${src_db[$i]}_bk_db.sql
done


echo "remote copy..."
for((i=0;i<${server_num};i++)) do
	scp /data/dnf/merge/${src_db[$i]}_bk_db.sql root@120.132.26.173:/data/dnf/merge
done







