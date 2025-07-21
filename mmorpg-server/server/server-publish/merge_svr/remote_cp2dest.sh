#!/bin/sh

src_user='aldzn'
src_pass='2AJfPNFQxH7HWDc!'


src_host='rm-wz951oaz476t71lq8.mysql.rds.aliyuncs.com'
src_db='merge_386'


dest_host='rm-bp1y0r2qdw7e07z4i.mysql.rds.aliyuncs.com'
dest_db='aldzn_816'
dest_replay_db='replay_816'

dest_user='aldzn'
dest_pass='2AJfPNFQxH7HWDc!'

dest_insname='rm-bp1y0r2qdw7e07z4i'


echo "start copy"
mysql -h ${dest_host} -u ${dest_user} -p${dest_pass} ${dest_db}</data/dnf/${src_db}_bk_db.sql
