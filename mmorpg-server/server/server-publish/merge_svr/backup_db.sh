#!/bin/sh

user='root'
pass='xnUn4g1v'

server_num=2
src_host=('127.0.0.1' '127.0.0.1')
src_db=('aldzn_1' 'aldzn_2')





echo "----------------start backup--------------------"
echo "user : $user"
echo "pw : $pass"
for((i=1;i<=${server_num};i++)) do
	echo "src_host_$i : ${src_host[$i-1]}"
	echo "src_db_$i : ${src_db[$i-1]}"
	echo "dest_db_$i : ${dest_db[$i-1]}"
done
echo "-----------------------------------------------"


echo "start dump src db desc..."

for((i=0;i<${server_num};i++)) do
	mysqldump --set-gtid-purged=OFF -h ${src_host[$i]} -u ${user} -p${pass} -d ${src_db[$i]}>/data/dnf/merge/${src_db[$i]}_desc.sql
done

echo "dump end!"


echo "start dump src db..."

for((i=0;i<${server_num};i++)) do
mysqldump --set-gtid-purged=OFF -h ${src_host[$i]} -u ${user} -p${pass} ${src_db[$i]}>/data/dnf/merge/${src_db[$i]}_bk_db.sql
done

echo "dump end!"

echo "start import db to dest db..."

for((i=0;i<${server_num};i++)) do
mysql -h ${dest_host} -u ${user} -p${pass} ${dest_db[$i]}</data/dnf/merge/${src_db[$i]}_bk_db.sql
done

echo "import end!"

echo "import merge proc..."
mysql -h ${dest_host} -u ${user} -p${pass} ${dest_db[0]}</data/dnf/merge/create_merge_proc.sql
echo "import merge proc end!"

echo "merge db..."
for((i=1;i<${server_num};i++)) do
	MergeTable ${dest_db[0]} ${dest_db[$i]}
done

echo "merge end!"

echo "backup merged db.."
mysqldump --set-gtid-purged=OFF -h ${dest_host} -u ${user} -p${pass} ${dest_db[0]}>/data/dnf/merge/${dest_db[0]}_bk_db.sql
echo "backup end!"

echo "start merge..."
mysql -h ${dest_host} -u${user} -p${pass} ${dest_db[0]} -e "${exeStartMerge}"
 
echo "merge end!"

echo "update new zoneid"
mysql -h ${dest_host} -u${user} -p${pass} ${dest_db[0]} -e "${updateZoneId}"
echo "update end!"




