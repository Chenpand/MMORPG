#!/usr/bin/env bash

src_log_dir="/opt/app/LogServer/stat/"
desc_log_dir="/opt/app/LogServer/yunying_stat/"
if [ ! -d $desc_log_dir ];then
    mkdir $desc_log_dir;
fi

list=(login refer pay props currency feed guide act ser att inventory entry)
#now=`date -d "2 hour ago" +"%Y-%m-%d-%H"`
for((k=10;k<13;k++))
do
now="2013-02-18-${k}"
list_count=${#list[*]}
i=0
while [ $i -lt $list_count ]
do
        find $src_log_dir|grep ${list[$i]}|grep $now |xargs  cat >> $desc_log_dir/${list[$i]}_$now.txt
        md5sum $desc_log_dir/${list[$i]}_$now.txt >> $desc_log_dir/md5sum_$now.txt
        cd $desc_log_dir
        tar -zcf md5sum_$now.txt.tar.gz md5sum_$now.txt
#        rm -rf md5sum_$now.txt
        tar -zcf ${list[$i]}_$now.txt.tar.gz ${list[$i]}_$now.txt
#        rm -rf ${list[$i]}_$now.txt
        let i++
done
done
/usr/bin/find /opt/app/LogServer/yunying_stat/ -name "*.tar.gz" -mtime +6 -delete
#/usr/bin/find /opt/app/LogServer/yunying_stat/ -name "*.tar.gz" -size -200c -delete
/usr/bin/find /opt/app/LogServer/yunying_stat/ -type f -name "*.txt" -mtime +6 -delete
/usr/bin/find /opt/app/LogServer/stat/ -name "*.txt" -ctime +1 -delete
exit $?
