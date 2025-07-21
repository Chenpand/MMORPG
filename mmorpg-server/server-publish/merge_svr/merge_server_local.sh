#!/bin/sh

src_user='aldzn'
src_pass='2AJfPNFQxH7HWDc!'

server_num=2
src_host=('rm-wz9k59i4t8c5mc417.mysql.rds.aliyuncs.com' 'rm-wz9k59i4t8c5mc417.mysql.rds.aliyuncs.com')
src_db=('aldzn_388' 'aldzn_389')

new_zid="817"
merge_to_zid='388'
dest_host='rm-wz951oaz476t71lq8.mysql.rds.aliyuncs.com'
dest_db=('merge_388' 'merge_389')

dest_user='aldzn'
dest_pass='2AJfPNFQxH7HWDc!'

function MergeTable()
{
     dest_db_1=$1
     dest_db_2=$2

     mysql -h ${dest_host} -u${dest_user} -p${dest_pass} ${dest_db[0]} -e "call merge_table('${dest_db_1}','${dest_db_2}')"
}

exeStartMerge="call start_merge(${merge_to_zid})"
updateZoneId="update t_player_info set zoneid=${new_zid}"

if [ ${server_num} -ne ${#src_host[@]} ]
then
	echo "src_host num error!"
	exit
fi

if [ ${server_num} -ne ${#src_db[@]} ]
then
	echo "src_db num error!"
	exit
fi

if [ ${server_num} -ne ${#dest_db[@]} ]
then
  	echo "dest_db num error!"
	exit
fi


echo "----------------start merge--------------------"
echo "src_user : $src_user"
echo "src_pw : $src_pass"
echo "dest_user : $dest_user"
echo "dest_pw : $dest_pass"
echo "new zoneid : $new_zid"
echo "merge to zoneid : $merge_to_zid"
echo "dest host : $dest_host"
for((i=1;i<=${server_num};i++)) do
	echo "src_host_$i : ${src_host[$i-1]}"
	echo "src_db_$i : ${src_db[$i-1]}"
	echo "dest_db_$i : ${dest_db[$i-1]}"
done
echo "-----------------------------------------------"

echo "start dump src db desc..."
for((i=0;i<${server_num};i++)) do
    mysqldump --set-gtid-purged=OFF -h ${src_host[$i]} -u ${src_user} -p${src_pass} -d ${src_db[$i]}>/data/dnf/merge/${src_db[$i]}_desc.sql
done



echo "compare db desc..."
for((i=1;i<${server_num};i++)) do
    diff ${src_db[0]}_desc.sql ${src_db[$i]}_desc.sql

    while true; do
     read -p"Do you want to continue? Y/N" yn
     case $yn in
     [Yy]* ) break;;
     [Nn]* ) exit;;
     * ) echo "Please answer yes or no.";;
     esac
    done
done

echo "start dump src db..."
for((i=0;i<${server_num};i++)) do
    mysqldump --set-gtid-purged=OFF -h ${src_host[$i]} -u ${src_user} -p${src_pass} ${src_db[$i]}>/data/dnf/merge/${src_db[$i]}_bk_db.sql
done



# 在中转rds上创建合并服得库
echo "create dest database"
for((i=0;i<${server_num};i++)) do
	#检查要创建得库是否存在
	is_exist=`mysql -h ${dest_host} -u${dest_user} -p${dest_pass} -e "show databases" | grep "${dest_db[$i]}" | wc -l`

	if [ $is_exist -ne 0 ]                                                                                
	then
	 	echo "create databases ${dest_db[$i]} failed ..."
	 	exit 1
	fi

	# 通过阿里云创建库等待60S
	node /data/ansible/mkServer/AliyunRdsTools/app.js rm-wz951oaz476t71lq8 ${dest_db[$i]} 60
done


sleep 10

echo "start import db to dest db..."
for((i=0;i<${server_num};i++)) do
mysql -h ${dest_host} -u ${dest_user} -p${dest_pass} ${dest_db[$i]}</data/dnf/merge/${src_db[$i]}_bk_db.sql
done


echo "import merge proc..."
mysql -h ${dest_host} -u ${dest_user} -p${dest_pass} ${dest_db[0]}</data/dnf/merge/create_merge_proc.sql




echo "merge db..."
for((i=1;i<${server_num};i++)) do
    MergeTable ${dest_db[0]} ${dest_db[$i]}
done


# echo "backup merged db.."
# mysqldump --set-gtid-purged=OFF -h ${dest_host} -u ${dest_user} -p${dest_pass} ${dest_db[0]}>/data/dnf/merge/${dest_db[0]}_bk_db.sql


echo "start merge..."
mysql -h ${dest_host} -u${dest_user} -p${dest_pass} ${dest_db[0]} -e "${exeStartMerge}"


echo "update new zoneid"
mysql -h ${dest_host} -u${dest_user} -p${dest_pass} ${dest_db[0]} -e "${updateZoneId}"


